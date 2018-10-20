all:
	[ -d ./build ] || mkdir build
	[ -f build/Makefile ] || (cd build && cmake ..)
	(cd build && make "$@")
