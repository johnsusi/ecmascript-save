preset := debug
out_dir := out
build_dir := $(out_dir)/build/$(preset)
toolchain_file := ./vcpkg/scripts/buildsystems/vcpkg.cmake
libraries := catch2
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))

VCPKG_ROOT ?= $(current_dir)vcpkg
vcpkg_dir := $(abspath $(VCPKG_ROOT))

.PHONY: all configure build test publish clean vcpkg
all: configure build test publish

$(vcpkg_dir):
	@echo "Missing vcpkg at $(vcpkg_dir)! Install vcpkg using:                 "
	@echo "                                                                    "
	@echo "  make vcpkg                                                        "
	@echo "                                                                    "
	@echo "For more information on how to install vcpkg go to                  "
	@echo "                                                                    "
	@echo "  https://github.com/microsoft/vcpkg                                "
	@echo "                                                                    "
	@echo "If you have vcpkg installed somewhere else, tell make about it using"
	@echo "either VCPKG_ROOT environment variable or calling make with:        "
	@echo "                                                                    "
	@echo "  make configure VCPKG_ROOT=<vcpkg install directory>               "
	@echo "                                                                    "
	@false

$(build_dir)/CMakeCache.txt: CMakeLists.txt | $(vcpkg_dir)
	@VCPKG_ROOT=$(vcpkg_dir) cmake --preset=$(preset) .

configure: $(build_dir)/CMakeCache.txt

build:
	@cmake --build $(build_dir)

test: build
	@cmake --build $(build_dir) --target test

publish: build
	@cpack $(build_dir) -GZIP

clean:
	@rm -Rf $(out_dir)

vcpkg:
	@git clone https://github.com/microsoft/vcpkg