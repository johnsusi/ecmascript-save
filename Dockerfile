FROM ubuntu:22.04 AS build

RUN apt-get update -y && apt-get install -y build-essential git make cmake ninja-build curl tar zip

WORKDIR /
RUN git clone https://github.com/microsoft/vcpkg && /vcpkg/bootstrap-vcpkg.sh



