preset := debug
out_dir := out
build_dir := $(out_dir)/build/$(preset)
toolchain_file := ./vcpkg/scripts/buildsystems/vcpkg.cmake
libraries := catch2
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))

VCPKG_ROOT ?= $(current_dir)vcpkg
vcpkg_dir := $(abspath $(VCPKG_ROOT))

.PHONY: all configure build test package clean
all:
	@echo "Usage:                                                              "

$(vcpkg_dir):
	@echo "Missing vcpkg at $(vcpkg_dir)! Install vcpkg using:                 "
	@echo "                                                                    "
	@echo "  git clone https://github.com/microsoft/vcpkg                      "
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
	@VCPKG_ROOT=$(vcpkg_dir) VERBOSE=1 cmake --preset=$(preset) .

configure: $(build_dir)/CMakeCache.txt

build:
	@cmake --build $(build_dir)

test: build
	@ctest --test-dir $(build_dir)

test-report: test
	@lcov --rc lcov_branch_coverage=1 -d $(build_dir) -z
	@lcov --rc lcov_branch_coverage=1 -d $(build_dir) -c -i -o $(build_dir)/coverage.base
	@ctest --test-dir $(build_dir)
	@lcov --rc lcov_branch_coverage=1 -d $(build_dir) -c -o $(build_dir)/coverage.collect
	@lcov --rc lcov_branch_coverage=1 -a $(build_dir)/coverage.base -a $(build_dir)/coverage.collect -o $(build_dir)/coverage.total
	@lcov --rc lcov_branch_coverage=1 -r $(build_dir)/coverage.total '/usr/*' '*/test/*' '*/vcpkg_installed/*' '*/CMakeFiles/*' -o $(build_dir)/lcov.info
	@genhtml --legend --branch-coverage --highlight -o $(build_dir)/report $(build_dir)/lcov.info
	@rm $(build_dir)/coverage.*

package: build
	@cpack -B $(build_dir) -G ZIP

clean:
	@rm -Rf $(out_dir)

ci: configure build test

cd: configure build test package
