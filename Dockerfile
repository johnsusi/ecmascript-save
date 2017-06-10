FROM ubuntu:latest

RUN \
  apt-get update && \
  apt-get install -y build-essential software-properties-common && \
  add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
  apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y \
    cmake ninja-build g++-7 libicu-dev libboost-all-dev

ENV CC=gcc-7 CXX=g++-7

COPY . /usr/project

RUN mkdir /build && \
    cd /build && \
    cmake /usr/project \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DTESTING=NO \
        -DSTATIC=YES \
        -DLTO=YES \
        -DCMAKE_AR="/usr/bin/gcc-ar" \
        -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>" \
        -DCMAKE_CXX_ARCHIVE_FINISH="true" \
        && \
    ninja



