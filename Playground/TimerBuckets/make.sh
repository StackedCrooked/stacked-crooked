[ -d Build ] || {
	./rebuild.sh
	exit
}

(cd Build && make -j10)
	
