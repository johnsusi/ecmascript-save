FROM ubuntu:22.04 AS base

RUN apt-get update -y && apt-get install -y build-essential git make cmake ninja-build curl tar zip lcov pkg-config

WORKDIR /ecmascript

COPY . ./

FROM base AS build

RUN make build preset=release CXX=g++-11

FROM base AS test

RUN make build preset=debug CXX=g++-11 CXXFLAGS="--coverage"
RUN lcov --rc lcov_branch_coverage=1 -d out/build/debug --zerocounters && \
	lcov --rc lcov_branch_coverage=1 -d out/build/debug -o out/build/debug/coverage.base -c -i && \
 	make test preset=debug && \
 	lcov --rc lcov_branch_coverage=1 -d out/build/debug -o out/build/debug/coverage.collected -c && \
 	lcov --rc lcov_branch_coverage=1 -a out/build/debug/coverage.base -a out/build/debug/coverage.collected -o out/build/debug/coverage.total && \
 	lcov --rc lcov_branch_coverage=1 -r out/build/debug/coverage.total '/usr/include/*' '*/tests/*' '*/vcpkg_installed/*' '*/CMakeFiles/*' -o out/build/debug/coverage.info && \
	genhtml --legend --branch-coverage -o out/build/debug/report out/build/debug/coverage.info

FROM scratch AS test-report

COPY --from=test /ecmascript/out/build/debug/report report
COPY --from=test /ecmascript/out/build/debug/coverage.info info.lcov

# RUN git submodule update --init --recursive
# RUN make configure build prefix=debug CXX=g++-11 CXXFLAGS="--coverage" && \
#     lcov --rc lcov_branch_coverage=1 -d out/build/debug --zerocounters && \
# 	lcov --rc lcov_branch_coverage=1 -d out/build/debug -c -i -o out/build/debug/coverage.base && \
# 	make test prefix=debug && \
# 	lcov --rc lcov_branch_coverage=1 -d out/build/debug --capture -o out/build/debug/coverage.info && \
# 	lcov --rc lcov_branch_coverage=1 -a out/build/debug/coverage.base -a out/build/debug/coverage.info -o out/build/debug/coverage.total && \
# 	lcov --rc lcov_branch_coverage=1 --remove out/build/debug/coverage.total '/usr/include/*' '*/tests/*' '*/vcpkg_installed/*' -o out/build/debug/coverage.cleaned && \
# 	genhtml --legend --branch-coverage -o out/build/debug/report out/build/debug/coverage.cleaned

