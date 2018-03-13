cat stats \
    | perl -pe 's,\n,__NEWLINE__,g' \
    | perl -pe 's,\s+, ,g' \
    | perl -pe 's,__NEWLINE__,\n,g' \
    | perl -pe 's,[()],,g' \
    | perl -pe 's,\band\b,&&,g' \
    | perl -pe 's,\bor\b,||,g' \
    | perl -pe 's,\blen\s*=\s*(\d+),len=\1,g' \
    | perl -pe 's,vlan (\d+),vlan=\1,g' \
    | perl -pe 's,dst host (\d+\.\d+\.\d+\.\d+),ip.dst=\1,g'  \
    | perl -pe 's,src host (\d+\.\d+\.\d+\.\d+),ip.src=\1,g'  \
    | perl -pe 's,dst host (\S),ip6.dst=\1,g'  \
    | perl -pe 's,src host (\S),ip6.src=\1,g'  \
    | perl -pe 's,ip src (\d+\.\d+\.\d+\.\d+),ip.src=\1,g'  \
    | perl -pe 's,ip dst (\d+\.\d+\.\d+\.\d+),ip.dst=\1,g'  \
    | perl -pe 's,ip6 src (\S+),ip6.src=\1,g' \
    | perl -pe 's,ip6 dst (\S+),ip6.dst=\1,g' \
    | perl -pe 's,ether src (\S+),ether.src=\1,g' \
    | perl -pe 's,ether dst (\S+),ether.dst=\1,g' \
    | perl -pe 's,(\w+) src port (\d+),\1.src_port=\2,g' \
    | perl -pe 's,(\w+) dst port (\d+),\1.dst_port=\2,g' \
    | perl -pe 's,(\w+) port (\d+),\1.port=\2,g' \
    | perl -pe 's,(\w+) port (\d+),\1.port=\2,g' \
    >stats.preprocessed
