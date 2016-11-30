#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <thread>
#include <future>
#include <stdint.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_errno.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>


// Thread-safe printing to std::cout
// Usage example: Log() << "message";
struct Log : std::ostringstream {
    ~Log() {
        static std::mutex mutex;
        auto s = str();
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << s << std::endl;
    }
};


using Clock = std::chrono::system_clock;


enum { packet_size = 1024 + 256 }; // includes the CRC


#define RX_RING_SIZE 128
#define TX_RING_SIZE 512


enum
{
    enable_tx = true,
    enable_rx = false,
    num_ports = 2,
    num_rx_queues = 1,
    num_tx_queues = 4,
    num_mbufs = 1024,
    mbuf_cache_size = 512
};


/*
 * RX and TX Prefetch, Host, and Write-back threshold values should be
 * carefully set for optimal performance. Consult the network
 * controller's datasheet and supporting DPDK documentation for guidance
 * on how these parameters should be set.
 */
#define RX_PTHRESH 8 /**< Default values of RX prefetch threshold reg. */
#define RX_HTHRESH 8 /**< Default values of RX host threshold reg. */
#define RX_WTHRESH 0 /**< Default values of RX write-back threshold reg. */


/*
 * These default values are optimized for use with the Intel(R) 82599 10 GbE
 * Controller and the DPDK ixgbe PMD. Consider using other values for other
 * network controllers and/or network drivers.
 */
#define TX_PTHRESH 32 /**< Default values of TX prefetch threshold reg. */
#define TX_HTHRESH 0  /**< Default values of TX host threshold reg. */
#define TX_WTHRESH 0  /**< Default values of TX write-back threshold reg. */



struct Counters
{
    __attribute__((aligned(128))) std::atomic<uint64_t> rx;
    __attribute__((aligned(128))) std::atomic<uint64_t> tx;
};


Counters counters[num_ports][num_tx_queues];



struct core_conf
{
    core_conf(int port_id, int queue_id, rte_mempool* pool) :
        port_id(port_id),
        queue_id(queue_id),
        pool(pool)
    {
    }

    int port_id;
    int queue_id;
    rte_mempool* pool;
};


static inline int local_rte_pktmbuf_alloc_bulk(struct rte_mempool *pool, struct rte_mbuf **mbufs, unsigned count)
{
    unsigned idx = 0;
    int rc;

    rc = rte_mempool_get_bulk(pool, (void **)mbufs, count);
    if (unlikely(rc))
        return rc;

    /* To understand duff's device on loop unwinding optimization, see
     * https://en.wikipedia.org/wiki/Duff's_device.
     * Here while() loop is used rather than do() while{} to avoid extra
     * check if count is zero.
     */
    switch (count % 4) {
    case 0:
        while (idx != count) {
            RTE_MBUF_ASSERT(rte_mbuf_refcnt_read(mbufs[idx]) == 0);
            rte_mbuf_refcnt_set(mbufs[idx], 1);
            rte_pktmbuf_reset(mbufs[idx]);
            idx++;
    case 3:
            RTE_MBUF_ASSERT(rte_mbuf_refcnt_read(mbufs[idx]) == 0);
            rte_mbuf_refcnt_set(mbufs[idx], 1);
            rte_pktmbuf_reset(mbufs[idx]);
            idx++;
    case 2:
            RTE_MBUF_ASSERT(rte_mbuf_refcnt_read(mbufs[idx]) == 0);
            rte_mbuf_refcnt_set(mbufs[idx], 1);
            rte_pktmbuf_reset(mbufs[idx]);
            idx++;
    case 1:
            RTE_MBUF_ASSERT(rte_mbuf_refcnt_read(mbufs[idx]) == 0);
            rte_mbuf_refcnt_set(mbufs[idx], 1);
            rte_pktmbuf_reset(mbufs[idx]);
            idx++;
        }
    }
    return 0;
}



/*
 * Main thread that does the work, reading from INPUT_PORT
 * and writing to OUTPUT_PORT
 */
std::atomic<int> tx_cores_started{0};
static int tx_core(void* p)
{
    auto conf = *static_cast<core_conf*>(p);
    for (auto i = 0u; i < num_ports; i++)
    {
        if (rte_eth_dev_socket_id(i) > 0 && rte_eth_dev_socket_id(i) != (int)rte_socket_id())
        {
            Log() << "WARNING, port " << i << " is on remote NUMA node to polling thread.\n\tPerformance will not be optimal.";
        }
    }

    Log() << "TX Core: Port=" << conf.port_id << " Queue=" << conf.queue_id << " Core=" << rte_lcore_id() << ": Forwarding packets.";

    std::vector<uint8_t> udp_packet = { 0x00, 0xff, 0x23, 0x00, 0x00, 0x20, 0x00, 0xff, 0x23, 0x00, 0x00, 0x10, 0x08, 0x00, 0x45, 0x00, 0x00, 0x32, 0x5f, 0x8f, 0x00, 0x00, 0x40, 0x11, 0x17, 0x0d, 0x01, 0x01, 0x01, 0x0a, 0x01, 0x01, 0x01, 0x14, 0x03, 0xf2, 0x03, 0xfc, 0x00, 0x1e, 0xf3, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    enum {  udp_packet_size = packet_size - 4 };
    udp_packet.resize(udp_packet_size);


    tx_cores_started++;
    while (tx_cores_started < num_tx_queues) {}

    rte_mempool* pool = conf.pool;

    for (;;)
    {
        std::array<rte_mbuf*, TX_RING_SIZE> mbufs;
        auto size = mbufs.size();

        while (0 != local_rte_pktmbuf_alloc_bulk(pool, mbufs.data(), size))
        {
            if (size > 32)
            {
                size = size / 2;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            }
        }

        auto data = mbufs.data();

        while (size != 0)
        {
            assert(size <= mbufs.size());
            for (auto i = 0; i != 32; ++i)
            {
                auto& mbuf = data[i];
                auto payload_buffer = rte_pktmbuf_append(mbuf, udp_packet_size);
                memcpy(payload_buffer, udp_packet.data(), udp_packet_size);
            }

            auto burst_size = 32;
            while (burst_size > 0)
            {
                auto n = rte_eth_tx_burst(conf.port_id, conf.queue_id, data, burst_size);
                burst_size -= n;
                data += n;
                counters[conf.port_id][conf.queue_id].tx += n;
            }

            size -= 32;
        }
    }

    return 0;
}


/*
 * Main thread that does the work, reading from INPUT_PORT
 * and writing to OUTPUT_PORT
 */
static int rx_core(void* p)
{
    auto conf = *static_cast<core_conf*>(p);

    for (auto i = 0u; i < num_ports; i++)
    {
        if (rte_eth_dev_socket_id(i) > 0 && rte_eth_dev_socket_id(i) != (int)rte_socket_id())
        {
            Log() << "WARNING, port " << i << " is on remote NUMA node to polling thread.\n\tPerformance will not be optimal.";
        }
    }

    Log() << "RX Core: Port=" << conf.port_id << " Queue=" << conf.queue_id << " Core=" << rte_lcore_id() << ": Receiving packets.";

    std::array<rte_mbuf*, 32> mbufs;

    for (;;)
    {
        if (auto n = rte_eth_rx_burst(conf.port_id, conf.queue_id, mbufs.data(), mbufs.size()))
        {
            counters[conf.port_id][conf.queue_id].rx += n;

            for (auto i = 0; i != n; ++i)
            {
                rte_pktmbuf_free(mbufs[i]);
            }
        }
    }

    return 0;
}


static rte_eth_conf get_default_port_conf()
{
    auto result = rte_eth_conf();
    result.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
    return result;
}


static auto port_conf_default = get_default_port_conf();


static rte_eth_rxconf get_rx_conf()
{
    auto result = rte_eth_rxconf();
    result.rx_thresh.pthresh = RX_PTHRESH;
    result.rx_thresh.hthresh = RX_HTHRESH;
    result.rx_thresh.wthresh = RX_WTHRESH;
    result.rx_free_thresh = 32;
    return result;
}

static rte_eth_txconf get_tx_conf()
{
    auto result = rte_eth_txconf();
    result.tx_thresh.pthresh = TX_PTHRESH;
    result.tx_thresh.hthresh = TX_HTHRESH;
    result.tx_thresh.wthresh = TX_WTHRESH;
    result.tx_free_thresh = 32;
    result.tx_rs_thresh = 32;
    result.txq_flags = 0;//(ETH_TXQ_FLAGS_NOMULTSEGS | ETH_TXQ_FLAGS_NOVLANOFFL | ETH_TXQ_FLAGS_NOXSUMSCTP | ETH_TXQ_FLAGS_NOXSUMUDP | ETH_TXQ_FLAGS_NOXSUMTCP);
    return result;
}


static auto rx_conf = get_rx_conf();

static auto tx_conf = get_tx_conf();


/*
 * Initialises a given port using global settings and with the rx buffers
 * coming from the mbuf_pool passed as parameter
 */
static inline int port_init(uint8_t port, rte_mempool* mbuf_pool)
{
    struct rte_eth_conf port_conf = port_conf_default;

    if (port >= rte_eth_dev_count())
        return -1;

    auto retval = rte_eth_dev_configure(port, num_rx_queues, num_tx_queues, &port_conf);
    if (retval != 0)
    {
        return retval;
    }

    for (auto q = 0; q < num_rx_queues; q++)
    {
        retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE, rte_eth_dev_socket_id(port), &rx_conf, mbuf_pool);
        if (retval < 0)
        {
            return retval;
        }
    }

    for (auto q = 0; q < num_tx_queues; q++)
    {
        retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE, rte_eth_dev_socket_id(port), &tx_conf);
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

    return 0;
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

    Log() << "rte_eth_dev_count=" << (int)rte_eth_dev_count();

    assert(num_ports == rte_eth_dev_count());

    if (num_ports < 2 || (num_ports & 1))
    {
        rte_exit(EXIT_FAILURE, "Error: number of ports must be even\n");
    }

    // initialize all ports
    for (auto portid = 0u; portid < num_ports; portid++)
    {
        std::stringstream ss;
        ss << portid << ".rx";
        auto pool = rte_pktmbuf_pool_create(ss.str().c_str(), num_mbufs, mbuf_cache_size, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
        if (pool == NULL)
        {
            auto e = rte_errno;
            Log() << "rte_pktmbuf_pool_create failed with error: e=" << e << " s=" << rte_strerror(e);
            rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
        }

        if (port_init(portid, pool) != 0)
        {
            auto e = rte_errno;
            Log() << "port_init failed with error: e=" << e << " s=" << rte_strerror(e);
            rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8"\n", portid);
        }
    }


    auto core_id = 8;
    if (enable_rx)
    {
        for (auto queue_id = 0; queue_id != num_rx_queues; ++queue_id)
        {
            rte_eal_remote_launch(&rx_core, new core_conf(num_ports - 1, queue_id, nullptr), core_id++);
        }
    }



    if (enable_tx)
    {
        for (auto queue_id = 0; queue_id != num_tx_queues; ++queue_id)
        {
            auto pool = rte_pktmbuf_pool_create(std::to_string(queue_id).c_str(), num_mbufs, mbuf_cache_size, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
            rte_eal_remote_launch(&tx_core, new core_conf(0, queue_id, pool), core_id++);
        }
    }

    (void)&rx_core;

    while (tx_cores_started < num_tx_queues) {}

    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::seconds(4));
        for (auto port_id = 0u; port_id != num_ports; ++port_id)
        {
            uint64_t rx = 0;
            uint64_t tx = 0;

            for (auto queue_id = 0u; queue_id != num_tx_queues; ++queue_id)
            {
                rx += counters[port_id][queue_id].tx.exchange(0);
                tx += counters[port_id][queue_id].tx.exchange(0);
            }
            rx /= 4;
            tx /= 4;

            if (rx + tx > 0)
            {
                std::cout << "Size+CRC=" << packet_size;
                std::cout << " PortId=" << port_id;
                if (tx > 0)
                {
                    std::cout << " TX: PPS=" << std::left << tx << " " << (tx * 8 * (packet_size + 20) / 1e9) << "Gbps";
                }
                if (rx > 0)
                {
                    std::cout << " RX: PPS=" << std::left << rx << " " << (rx * 8 * (packet_size + 20) / 1e9) << "Gbps";
                }
                std::cout << std::endl;
            }
        }
    }
}
