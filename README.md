ECMAScript
==========

This is an implementation of ECMA-262 5.1 edition.

![Build Status](https://github.com/johnsusi/ecmascript/actions/workflows/ci.yml/badge.svg)
![Coverage Status](https://coveralls.io/repos/github/johnsusi/ecmascript/badge.svg?branch=main)

## Dependencies

This project uses [vcpkg](https://github.com/microsoft/vcpkg).

Either have Vcpkg installed locally and set `VCPKG_ROOT` to the absolute path of
the installation directory or use `make vcpkg` to get a local copy in the
workspace.

## Configuring

```
make configure
```

To select a compiler:

```
make configure CXX=clang++ 
```

To use a custom vcpkg installation:

```
make configure VCPKG_ROOT=<vcpkg install directory>
```

To reconfigure clean the build first:

```
make clean configure CXX=... VCPKG_ROOT=...
```

## Building

```
make build
```

Note: `make build` will catch configuration changes to `CMakeLists.txt`.

## Testing

```
make test
```

## Documentation

- Install mkdocs
    # pip install mkdocs
    # pip install mkdocs-material



