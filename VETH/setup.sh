#!/bin/bash

# Setup network namespace named "blue"
ip netns add blue


# Create veth pair "blue.out" "blue.in"
ip link add blue.out type veth peer name blue.in


# Add blue.in to blue (while leaving blue.out in main namespace)
ip link set blue.in netns blue


# blue.in becomes DHCP server and will need a static IP
ip netns exec blue ip addr add 10.0.0.1 dev blue.in

# blue.in up
ip netns exec blue ip link set blue.in up


# The subnet is 10.0.0.0/24 (range from 10.0.0.1 to 10.0.0.255)
ip netns exec blue ip route add 10.0.0.0/24 dev blue.in


# Run DHCP server inside blue
ip netns exec blue dnsmasq --dhcp-range=10.0.0.2,10.0.0.254,255.255.255.0 --interface=blue.in --no-daemon


# Run DHCP client
#dhclient -d blue.out


