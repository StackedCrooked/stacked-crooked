#include <array>
#include <chrono>
#include <iostream>


using Clock = std::chrono::system_clock;


#include <stdint.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>


#define RX_RING_SIZE 64
#define TX_RING_SIZE 64

#define NUM_MBUFS 511
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 4


static rte_eth_conf get_default_por_config()
{
    auto result = rte_eth_conf();
    result.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
    return result;
}


static auto port_conf_default = get_default_por_config();


static unsigned nb_ports;


std::array<uint64_t, 1024> tx_timestamps;
std::array<uint64_t, 1024> rx_timestamps;
auto num_latencies = 0ul;


inline int64_t get_timestamp()
{
    //return rte_rdtsc_precise() / 2.6;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now().time_since_epoch()).count();
}


static uint16_t add_timestamps(uint8_t /*port*/, uint16_t /*qidx*/, rte_mbuf **packets, uint16_t num_packets, void*)
{
    uint64_t now = get_timestamp();

    for (auto i = 0u; i < num_packets; i++)
    {
        *reinterpret_cast<uint64_t*>(static_cast<uint8_t*>(packets[i]->buf_addr) + packets[i]->data_off) = now;

    }

    return num_packets;
}



static uint16_t calc_latency(uint8_t /*port*/, uint16_t /*qidx*/, rte_mbuf **packets, uint16_t num_packets, uint16_t /*max_pkts*/, void*)
{
    //std::cout << __FUNCTION__ << std::endl;
    uint64_t now = get_timestamp();

    for (auto i = 0; i != num_packets; ++i)
    {
        tx_timestamps[num_latencies] = *reinterpret_cast<uint64_t*>(static_cast<uint8_t*>(packets[i]->buf_addr) + packets[i]->data_off);
        rx_timestamps[num_latencies] = now;
        num_latencies++;
    }


    if (num_latencies == tx_timestamps.size())
    {
        std::cout << "F=" << rx_timestamps.front() << '-' << tx_timestamps.front() << '=' << rx_timestamps.front() - tx_timestamps.front() << '\n';
        std::cout << "B=" << rx_timestamps.back () << '-' << tx_timestamps.back () << '=' << rx_timestamps.back () - tx_timestamps.back () << '\n';
        num_latencies = 0;
	}
    return num_packets;
}

/*
 * Initialises a given port using global settings and with the rx buffers
 * coming from the mbuf_pool passed as parameter
 */
static inline int port_init(uint8_t port, rte_mempool *mbuf_pool)
{
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;

	if (port >= rte_eth_dev_count())
		return -1;

    auto retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0)
    {
		return retval;
    }

    for (auto q = 0; q < rx_rings; q++)
    {
        retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE, rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0)
        {
			return retval;
        }
	}

    for (auto q = 0; q < tx_rings; q++)
    {
        retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE, rte_eth_dev_socket_id(port), NULL);
		if (retval < 0)
        {
			return retval;
        }
	}

    retval = rte_eth_dev_start(port);

	if (retval < 0)
    {
		return retval;
    }


    ether_addr addr;
	rte_eth_macaddr_get(port, &addr);

    printf("Port %u MAC: %02"PRIx8" %02"PRIx8" %02"PRIx8 " %02"PRIx8" %02"PRIx8" %02"PRIx8"\n",
           (unsigned)port,
           addr.addr_bytes[0], addr.addr_bytes[1],
           addr.addr_bytes[2], addr.addr_bytes[3],
           addr.addr_bytes[4], addr.addr_bytes[5]);

	rte_eth_promiscuous_enable(port);

    rte_eth_add_rx_callback(port, 0, calc_latency, NULL);
    rte_eth_add_tx_callback(port, 0, add_timestamps, NULL);

	return 0;
}

/*
 * Main thread that does the work, reading from INPUT_PORT
 * and writing to OUTPUT_PORT
 */
static  __attribute__((noreturn)) void
lcore_main(void)
{
	uint8_t port;

	for (port = 0; port < nb_ports; port++)
    {
        if (rte_eth_dev_socket_id(port) > 0 && rte_eth_dev_socket_id(port) != (int)rte_socket_id())
        {
            printf("WARNING, port %u is on remote NUMA node to polling thread.\n\tPerformance will not be optimal.\n", port);
        }
    }

    printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n", rte_lcore_id());

    for (;;)
    {
        //for (port = 0; port < nb_ports; port++)
        {
            std::array<rte_mbuf*, BURST_SIZE> bufs;
            auto rx_burst_size = rte_eth_rx_burst(0, 0, bufs.data(), bufs.size());

            if (unlikely(rx_burst_size == 0))
            {
				continue;
            }

            auto tx_burst_size = rte_eth_tx_burst(1, 0, bufs.data(), rx_burst_size);
            if (unlikely(tx_burst_size < rx_burst_size))
            {
                for (auto buf = tx_burst_size; buf < rx_burst_size; buf++)
                {
					rte_pktmbuf_free(bufs[buf]);
                }
			}
		}
	}
}

/* Main function, does initialisation and calls the per-lcore functions */
int main(int argc, char *argv[])
{
	/* init EAL */
	int ret = rte_eal_init(argc, argv);

	if (ret < 0)
    {
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    }

	argc -= ret;
	argv += ret;

	nb_ports = rte_eth_dev_count();

	if (nb_ports < 2 || (nb_ports & 1))
    {
		rte_exit(EXIT_FAILURE, "Error: number of ports must be even\n");
    }

    auto mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL)
    {
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
    }

    // initialize all ports
    for (auto portid = 0u; portid < nb_ports; portid++)
    {
        if (port_init(portid, mbuf_pool) != 0)
        {
            rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8"\n", portid);
        }
    }

	if (rte_lcore_count() > 1)
    {
        printf("\nWARNING: Too much enabled lcores - App uses only 1 lcore\n");
    }

    uint8_t udp_packet[] = { 0x00, 0xff, 0x23, 0x00, 0x00, 0x20, 0x00, 0xff, 0x23, 0x00, 0x00, 0x10, 0x08, 0x00, 0x45, 0x00, 0x00, 0x32, 0x5f, 0x8f, 0x00, 0x00, 0x40, 0x11, 0x17, 0x0d, 0x01, 0x01, 0x01, 0x0a, 0x01, 0x01, 0x01, 0x14, 0x03, 0xf2, 0x03, 0xfc, 0x00, 0x1e, 0xf3, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    std::array<rte_mbuf*, 4> mbufs;
    for (auto& mbuf : mbufs)
    {
        mbuf = rte_pktmbuf_alloc(mbuf_pool);
        auto payload_buffer = rte_pktmbuf_append(mbuf, sizeof(udp_packet));
        memcpy(payload_buffer, udp_packet, sizeof(udp_packet));
    }
    rte_eth_tx_burst(1, 0, mbufs.data(), mbufs.size());

    // call lcore_main on master core only
	lcore_main();
	return 0;
}
