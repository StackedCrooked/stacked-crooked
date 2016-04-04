#include <array>
#include <cstdint>


struct Packet
{

};


using MACAddress = std::array<uint8_t, 6>;
using RemoteMAC = MACAddress;
using LocalMAC = MACAddress;


using IPv4Address = std::array<uint8_t, 4>;
using LocalIP = IPv4Address;
using RemoteIP = IPv4Address;


struct EthernetHeader
{
    MACAddress mDestination;
    MACAddress mSource;
    uint16_t mEtherType;
};


struct EthernetDecoder
{

};


struct IPv4Header
{
};


struct IPv4Decoder
{
};


struct IPv4Stack
{
    void send_raw(Packet& packet);

    RemoteMAC resolve(RemoteIP remote_ip)
    {
        Packet packet;
        make_arp_request(packet, remote_ip, mLocalMAC);
        send_raw(packet);
    }

    void receive(EthernetDecoder dec)
    {
        if (!dec.validate())
        {
            return;
        }

        if (dec.is_ipv4())
        {
            handle_ip(dec);
        }
        else if (dec.is_arp())
        {
            handle_arp(dec);
        }
    }

private:
    bool handle_ip(EthernetDecoder dec)
    {
        IPv4Decoder ipdec(dec.payload());

        if (!ipdec.validate())
        {
            return;
        }

        if (dec.is_unicast(mLocalMAC))
        {
            handle_ip_unicast(ipdec);
        }
        else if (dec.is_multicast())
        {
            handle_ip_multicast(ipdec);
        }
        else if (dec.is_broadcast())
        {
            handle_ip_broadcast(ipdec);
        }
    }

    void handle_ip_unicast(const IPv4Decoder& dec)
    {
        if (!dec.is_unicast())
        {
            // IPv4 broadcast/multicast over Ethernet unicast is not allowed.
            return;
        }

        if (dec.is_tcp())
        {
            mTCPHandler.handle(dec);
        }
        else if (dec.is_udp())
        {
            mUDPHandler.handle_unicast(dec);
        }
        else if (dec.is_icmp())
        {
            mICMPHandler.handle_unicast(dec);
        }
    }

    void handle_ip_broadcast(const IPv4Decoder& ipdec)
    {
        if (!ipdec.is_broadcast())
        {
            // TODO: Check RFC if we may accept this.
            return;
        }

        if (ipdec.is_udp())
        {
            mUDPHandler.handle_broadcast(ipdec);
        }
        else if (ipdec.is_icmp())
        {
            mICMPHandler.handle_broadcast(ipdec);
        }
    }

    void handle_ip_unicast(const EthernetDecoder& dec);
    {
        // Should be ARP REPLY
    }

    void handle_ip_multicast(const IPv4Decoder& dec)
    {
        // UDP/ICMP/IGMP are possible destinations here.
    }

    void handle_ip_broadcast(const ARPDecoder& dec)
    {
        // Should be ARP REQUEST
    }

    LocalMAC mLocalMAC;
    LocalIP  mLocalIP;
    Gateway  mGateway;
    Netmask  mNetmask;

    TCPHandler mTCPHandler;
    UDPHandler mUDPHandler;
    ICMPHandler mICMPHandler;
    IGMPHandler mIGMPHandler;
};



struct IPv6Stack
{
    void send_raw(Packet& packet);

    RemoteMAC resolve(RemoteIP remote_ip)
    {
        Packet packet;
        MakeNeightDiscoveryRequest(packet, remote_ip, mLocalMAC);
        send_raw(packet);
    }

    void receive(EthernetDecoder dec)
    {
        if (!dec.validate())
        {
            return;
        }

        if (!dec.is_ipv6())
        {
            return;
        }

        if (dec.isUnicast(mLocalMAC))
        {
            handle_unicast(dec);
        }
        else if (dec.isMulticast(mLocalMAC))
        {
            handle_multicast(dec);
        }
        else if (dec.isBroadcast(mLocalMAC))
        {
            handle_broadcast(dec);
        }
    }

private:
    LocalMAC mLocalMAC;
    LinkLocalIP mLinkLocalIP;

    // Other IP addresses (DHCP, SLAAC, ...)

    NeighborCache mNeighborCache;

    TCPHandler mTCPHandler;
    UDPHandler mUDPHandler;
    // etc..
};




struct TCPHandler
{
    void handle(TCPDecoder tcpdec, IPv4Decoder ipv4dec)
    {
        if (!validate(tcpdec, ipv4dec)) return;
        do_handle(tcpdec);
    }

    void handle(TCPDecoder tcpdec, IPv6Decoder ipdec)
    {
        if (!validate(tcpdec, ipv6dec)) return;
        do_handle(tcpdec);
    }

private:
    bool validate(TCPDecoder tcpdec, IPv4Decoder ipdec);
    bool validate(TCPDecoder tcpdec, IPv6Decoder ipdec);

    void do_handle(TCPDecoder tcpdec)
    {
        switch (tcpdec.getFlags() & (SYN|ACK|FIN|RST))
        {
            case ACK:
            {
                // Dispatch to an existing connection.
                break;
            }
            case SYN:
            {
                // If there is an existing connection in syn_received state.
                    // Retransmit our syn_ack.
                    // Don't allow this to repeat infinitely.

                // Otherwise create a new connection in syn_received state.
                break;
            }
            case SYN|ACK:
            {
                // If we have an existing connection in syn_sent state
                    // Change state to established.
                    // Let the TCP begin.
                break;
            }
            case FIN:
            {
                // FIN without ACK is not allowed.
                // RFC does not say if we should send RST.
                // Sending an RST in response to incorrect packet seems wrong.
                // We should discard which is the default thing to do with invalid packet.
                return;
            }
            case FIN|ACK:
            {
                // If no matching connection found then check RFC if we need to send RST.
                break;
            }
            case RST:
            {
                // RST without ACK is less common but valid.
                break;
            }
            case RST|ACK:
            {
                // We receive this because we tried to connect to invalid port.
                // If we have a connection in syn_sent state and the incoming ack
                // matches the sent sequence number then we destroy this sesssion.
                break;
            }
            default:
            {
                // Invalid combination.
                break;
            }
        }
    }
};
