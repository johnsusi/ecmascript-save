include build/make/defaults.mk

preset := release
out_dir := out
current_dir := $(dir $(mkfile_path))
build_dir := $(out_dir)/build/$(preset)
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
VCPKG_ROOT ?= $(current_dir)vcpkg
vcpkg_dir := $(abspath $(VCPKG_ROOT))

.PHONY: all configure build test package documentation clean ci

all:
	@echo "Usage:                                                              "
	@echo $(vcpkg_dir)
	@echo $(build_dir)
	@echo $(SHELL)

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

$(build_dir): $(vcpkg_dir)
	@VCPKG_ROOT=$(vcpkg_dir) cmake --preset=$(preset) .

configure: | $(build_dir)

build: configure
	@cmake --build $(build_dir)

test: build
	@ctest --test-dir $(build_dir) || ctest --test-dir $(build_dir) --rerun-failed --output-on-failure

test-report: build
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

documentation:
	@mkdocs build

book:
	@pandoc docs/book/index.md -o out/book.pdf \
		--table-of-contents \
		--number-sections \
		-V mainfont="Palatino" \
		-V documentclass=report \
		-V papersize=A5 \
		-V geometry:margin=1in

clean:
	@rm -Rf $(out_dir)

ci: build test package
