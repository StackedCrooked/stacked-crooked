[ -d Build ] || {
    mkdir Build
    (cd Build && cmake ..)
}

(cd Build && make -j8)
