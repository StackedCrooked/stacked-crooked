for i in `seq 0 3` ; do echo "With -O$i: $(g++ -o test -O$i main.cpp && ./test)" ; done
