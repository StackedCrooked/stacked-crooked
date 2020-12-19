FROM debian:buster

RUN export DEBIAN_FRONTEND=noninteractive \
    && apt update \
    && apt -y install python3-dev python3-pip subversion git sshfs

RUN pip3 install conan cmake ninja

# Build gcc
RUN export GCC_VERSION=9.3.0 \
    && cd /tmp \
    && wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz \
    && tar xf gcc-${GCC_VERSION}.tar.gz \
    && cd /tmp/gcc-${GCC_VERSION} \
    && contrib/download_prerequisites \
    && mkdir build \
    && cd build \
    && ../configure -v --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --prefix=/opt/gcc-${GCC_VERSION} --enable-checking=release --enable-languages=c,c++ --disable-multilib --program-suffix=-${GCC_VERSION} \
    && make -j$(nproc) \
    && make install \
    && cd /tmp \
    && rm -rf /tmp/gcc-${GCC_VERSION} /tmp/gcc-${GCC_VERSION}.tar.gz \
    && ln -fs /opt/gcc-${GCC_VERSION} /opt/gcc


