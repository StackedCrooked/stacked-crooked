for i in `seq 0 3` ; do echo "With optimization level $i: $(g++ -o test -fstrict-aliasing -O$i main.cpp && ./test)" ; done
