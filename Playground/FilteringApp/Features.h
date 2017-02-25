#pragma once


#ifndef FEATURES_ENABLE_STATS
#define FEATURES_ENABLE_STATISTIC 1
#endif


#ifndef FEATURES_ENABLE_MAC_CHECK
#define FEATURES_ENABLE_MAC_CHECK 1
#endif


#ifndef FEATURES_ENABLE_IP4_CHECK
#define FEATURES_ENABLE_IP4_CHECK 1
#endif


#ifndef FEATURES_ENABLE_UDP_FLOWS
#define FEATURES_ENABLE_UDP_FLOWS 1
#endif


#ifndef FEATURES_ENABLE_TCP_FLOWS
#define FEATURES_ENABLE_TCP_FLOWS 1
#endif


namespace Features {


enum
{
    enable_stats     = FEATURES_ENABLE_STATISTIC,
    enable_mac_check = FEATURES_ENABLE_MAC_CHECK,
    enable_ip_check  = FEATURES_ENABLE_IP4_CHECK,
    enable_udp       = FEATURES_ENABLE_UDP_FLOWS,
    enable_tcp       = FEATURES_ENABLE_TCP_FLOWS
};


} // namespace Features
