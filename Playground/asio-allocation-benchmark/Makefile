all:
	[ -d ./build ] || mkdir build
	[ -f build/Makefile ] || (cd build && cmake ..)
	(cd build && make -j$(shell nproc) "$@")

clean:
	rm -rf build
