FROM debian:buster

RUN export DEBIAN_FRONTEND=noninteractive \
    && apt update 

RUN apt -y upgrade 
RUN apt -y install python3-dev python3-pip
RUN pip3 install conan cmake ninja
RUN apt -y install subversion git
