#!/bin/bash
set -e

[ -z "$RTE_SDK" ] && { echo "RTE_SDK not set." >&2; exit 1; }
[ -z "$RTE_TARGET" ] && { echo "RTE_TARGET not set." >&2; exit 1; }

echo 4    > /sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages
echo 4    > /sys/devices/system/node/node1/hugepages/hugepages-1048576kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages


[ -d /dev/hugepages1G ] || {
    mkdir /dev/hugepages1G
    mount -t hugetlbfs -o pagesize=1G none /dev/hugepages1G
}

[ -d /dev/hugepages2M ] || {
    mkdir /dev/hugepages2M
    mount -t hugetlbfs -o pagesize=2M none /dev/hugepages2M
}

rm -f Transmit .main.o.d.tmp main.o
make

#./Transmit -m 4096 "$@"


# Mellanox
./Transmit -m 4096  -w 0000:83:00.0 -w 0000:84:00.0 -c 0xffffffff  -n 4
