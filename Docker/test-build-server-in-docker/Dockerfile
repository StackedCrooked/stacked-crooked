# FILE: Dockerfile

FROM debian:stretch

# Install a bunch of build shit.
RUN apt-get update \
	&& apt-get install -y \
	apt-utils \
	apt-transport-https \
	aria2 \
	bison \
	build-essential \
	cmake \
	curl \
	flex \
	git \
	libfl-dev \
	libgmp-dev \
	libmpc-dev \
	libmpfr-dev \
	libz3-dev \
	subversion \
	sudo \
	unzip \
	vim \
	wget

# Update APT sources from our local copy.
COPY sources.list /etc/apt/sources.list

# Run updates again.
RUN aria2c -o /etc/apt/trusted.gpg.d/php.gpg -x1 https://packages.sury.org/php/apt.gpg \
	&& apt-get update \
	&& apt-get dist-upgrade -y \
	&& apt-get build-dep -y gcc \
	&& apt-get build-dep -y clang
 
RUN aria2c -o ninja.zip -x2 https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip \
	&& unzip ninja.zip \
	&& rm ninja.zip \
	&& mv ninja /usr/bin/ninja


RUN rm -rf /tmp/gcc-build \
	&& mkdir -p /tmp/gcc-build \
	&& cd /tmp/gcc-build \
	&& wget https://ftp.gnu.org/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.xz \
	&& tar xf gcc-9.3.0.tar.xz  \
	&& (cd gcc-9.3.0 && ./contrib/download_prerequisites) \
	&& (mkdir build && cd build && ../gcc-9.3.0/configure --prefix=/opt/gcc-9.3.0 --program-suffix=-9.3.0 --disable-multilib --enable-languages=c,c++ && make -j$(nproc) && make install-strip) \
	&& cd /tmp \
	&& rm -rf /tmp/gcc-build


# Use our gcc as THE gcc.
RUN update-alternatives --install /usr/bin/gcc gcc /opt/gcc-9.3.0/bin/g++-9.3.0 100 --slave /usr/bin/g++ g++ /opt/gcc-9.3.0/bin/g++-9.3.0

