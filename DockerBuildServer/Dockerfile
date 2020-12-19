FROM debian:buster

RUN export DEBIAN_FRONTEND=noninteractive \
    && apt update \
    && apt install -y \
        bison \
        build-essential \
        curl \
        flex \
        git \
        libfl-dev \
        libgmp-dev \
        libmpc-dev \
        libmpfr-dev \
        libz3-dev \
        python3-dev \
        python3-pip \
        sshfs \
        subversion \
        sudo \
        unzip \
        vim \
        wget


RUN pip3 install conan cmake ninja
