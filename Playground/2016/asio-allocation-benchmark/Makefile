all:
	[ -d build ] || mkdir build
	[ -d build/Makefile ] || (cd build && cmake ..)
	(cd build && make -j$(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 3))

clean:
	[ -d build ] && (cd build && make clean)

distclean:
	rm -rf build
