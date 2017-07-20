FROM ubuntu:latest

RUN \
  apt-get update && \
  apt-get install -y \
    build-essential software-properties-common wget && \
  add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
  add-apt-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" && \
  add-apt-repository "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" && \
  add-apt-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main" && \
  add-apt-repository "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main" && \
  add-apt-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main" && \
  add-apt-repository "deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-4.0 main" && \
  wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add - && \
  apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y \
    ninja-build g++-6 g++-7 libicu-dev libboost-all-dev \
    clang-3.9 clang-4.0 clang-5.0 libc++-dev libc++abi-dev

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
        -DSCU=YES \
        && \
    ninja && ./UnitTest && ./ECMAScript -c -v /usr/project/dist/test.min.js

RUN mkdir /build-gcc7-release && \
    export CC=gcc-7 CXX=g++-7 && \
    cd /build-gcc7-release && \
    cmake /usr/project \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=/usr/project/cmake/Toolchain-linux-static.cmake \
        -DTESTING=YES \
        -DSCU=YES \
        -DCMAKE_AR="/usr/bin/gcc-ar" \
        -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>" \
        -DCMAKE_CXX_ARCHIVE_FINISH="true" \
        && \
    ninja && ./UnitTest && ./ECMAScript -c -v /usr/project/dist/test.min.js

RUN mkdir /build-gcc7-debug-usan && \
    export CC=gcc-7 CXX=g++-7 && \
    cd /build-gcc7-debug-usan && \
    cmake /usr/project \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTESTING=YES \
        -DSCU=YES \
        -DCMAKE_CXX_FLAGS="-g -O1 -fsanitize=undefined -fno-omit-frame-pointer -fPIC" \
        && \
    ninja && ./UnitTest && ./ECMAScript -c -v /usr/project/dist/test.min.js

RUN mkdir /build-gcc7-debug-asan && \
    export CC=gcc-7 CXX=g++-7 && \
    cd /build-gcc7-debug-asan && \
    cmake /usr/project \
        -GNinja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTESTING=YES \
        -DSCU=YES \
        -DCMAKE_CXX_FLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer -fPIC" \
        && \
    export LSAN_OPTIONS=verbosity=1:log_threads=1 && \
    export ASAN_OPTIONS=verbosity=1,coverage=1,detect_leaks=1 && \
    ninja
