#pragma once


#define FEATURES_ENABLE_STATS       1
#define FEATURES_ENABLE_MAC_CHECK   1
#define FEATURES_ENABLE_IP_CHECK    1
#define FEATURES_ENABLE_UDP         1
#define FEATURES_ENABLE_TCP         1


namespace Features {


enum
{
    enable_stats     = (1 << 0) * FEATURES_ENABLE_STATS,
    enable_mac_check = (1 << 1) * FEATURES_ENABLE_MAC_CHECK,
    enable_ip_check  = (1 << 2) * FEATURES_ENABLE_IP_CHECK,
    enable_udp       = (1 << 3) * FEATURES_ENABLE_UDP,
    enable_tcp       = (1 << 4) * FEATURES_ENABLE_TCP
};


} // namespace Features
