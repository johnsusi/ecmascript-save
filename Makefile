preset := debug
out_dir := out
build_dir := $(out_dir)/build/$(preset)
toolchain_file := ./vcpkg/scripts/buildsystems/vcpkg.cmake
libraries := catch2
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))

VCPKG_ROOT ?= $(current_dir)/vcpkg

@PHONY: all init configure dependencies build test publish clean
all: build

init: | $(VCPKG_ROOT)
	echo "root is $(VCPKG_ROOT)"

$(VCPKG_ROOT):
	git clone https://github.com/microsoft/vcpkg $(VCPKG_ROOT)

$(build_dir)/CMakeCache.txt: CMakeLists.txt
	CXX=clang++ VCPKG_ROOT=$(VCPKG_ROOT) cmake --preset=$(preset) .

configure: $(build_dir)/CMakeCache.txt

build: configure
	cmake --build $(build_dir)

test: build
	cmake --build $(build_dir) --target test

publish: build
	cpack $(build_dir) -GZIP

clean:
	rm -Rf $(out_dir)
