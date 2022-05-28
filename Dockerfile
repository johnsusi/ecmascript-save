FROM ubuntu:22.04

RUN apt-get update -y && apt-get install -y build-essential git make cmake ninja-build curl tar zip lcov pkg-config

WORKDIR /usr/src/app

COPY . ./

RUN git submodule update --init --recursive
RUN make configure build prefix=debug CXX=g++-11 CXXFLAGS="--coverage" && \
    lcov --rc lcov_branch_coverage=1 -d out/build/debug --zerocounters && \
	lcov --rc lcov_branch_coverage=1 -d out/build/debug -c -i -o out/build/debug/coverage.base && \
	make test prefix=debug && \
	lcov --rc lcov_branch_coverage=1 -d out/build/debug --capture -o out/build/debug/coverage.info && \
	lcov --rc lcov_branch_coverage=1 -a out/build/debug/coverage.base -a out/build/debug/coverage.info -o out/build/debug/coverage.total && \
	lcov --rc lcov_branch_coverage=1 --remove out/build/debug/coverage.total '/usr/include/*' '*/tests/*' '*/vcpkg_installed/*' -o out/build/debug/coverage.cleaned && \
	genhtml --legend --branch-coverage -o out/build/debug/report out/build/debug/coverage.cleaned

