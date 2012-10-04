for i in `seq 0 3` ; do echo "O$i $(g++ -o test -O$i main.cpp && ./test)" ; done
