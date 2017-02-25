#pragma once


#define ENABLE_MAC_CHECK 1
#define ENABLE_IP_CHECK  1

#define ENABLE_STAT 1
#define ENABLE_UDP  1
#define ENABLE_TCP  1


namespace Features {


enum
{
    enable_stats     = ENABLE_STAT      * (1 << 0),
    enable_mac_check = ENABLE_MAC_CHECK * (1 << 1),
    enable_ip_check  = ENABLE_IP_CHECK  * (1 << 2),
    enable_udp       = ENABLE_UDP       * (1 << 3),
    enable_tcp       = ENABLE_UDP       * (1 << 4),
};


static_assert(enable_stats, "");
static_assert(enable_mac_check, "");
static_assert(enable_ip_check, "");
static_assert(enable_udp, "");
static_assert(enable_tcp, "");


} // namespace Features
