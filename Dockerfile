FROM ubuntu:latest

RUN \
  apt-get update && \
  apt-get install -y build-essential software-properties-common && \
  add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
  apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y \
    ninja-build g++-6 g++-7 libicu-dev libboost-all-dev wget

RUN wget --no-check-certificate --quiet -O - http://www.cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.tar.gz \
    | tar --strip-components=1 -xz -C /usr/local

COPY . /usr/project

# RUN mkdir /build-gcc6 && \
#     export CC=gcc-6 CXX=g++-6 && \
#     cd /build-gcc6 && \
#     cmake /usr/project \
#         -GNinja \
#         -DCMAKE_BUILD_TYPE=Release \
#         -DTESTING=YES \
#         && \
#     ninja && ./UnitTest

# RUN mkdir /build-gcc6-lto && \
#     export CC=gcc-6 CXX=g++-6 && \
#     cd /build-gcc6-lto && \
#     cmake /usr/project \
#         -GNinja \
#         -DCMAKE_BUILD_TYPE=Release \
#         -DTESTING=YES \
#         -DSTATIC=YES \
#         -DLTO=YES \
#         -DCMAKE_AR="/usr/bin/gcc-ar" \
#         -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>" \
#         -DCMAKE_CXX_ARCHIVE_FINISH="true" \
#         && \
#     ninja && ./UnitTest

RUN mkdir /build-gcc7-debug && \
    export CC=gcc-7 CXX=g++-7 && \
    cd /build-gcc7-debug && \
    cmake /usr/project \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTESTING=YES \
        && \
    ninja && ./UnitTest

RUN mkdir /build-gcc7-release && \
    export CC=gcc-7 CXX=g++-7 && \
    cd /build-gcc7-release && \
    cmake /usr/project \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DTESTING=YES \
        -DSTATIC=YES \
        -DLTO=YES \
        -DUNITY=YES \
        -DCMAKE_AR="/usr/bin/gcc-ar" \
        -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>" \
        -DCMAKE_CXX_ARCHIVE_FINISH="true" \
        && \
    ninja && ./UnitTest
