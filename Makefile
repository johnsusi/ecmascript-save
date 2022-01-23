
build_dir := build/debug

$(build_dir):
	mkdir -p $@

@PHONY: all
all: build

@PHONY: configure
configure: $(build_dir)
	CXX=clang++ cmake -S . -B $(build_dir) -GNinja -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

@PHONY: build
build: configure
	cmake --build $(build_dir)

@PHONY: publish
publish: build
	cpack $(build_dir) -GZIP

@PHONY: clean
clean:
	rm -Rf $(build_dir)