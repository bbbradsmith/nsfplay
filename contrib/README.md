# NSFPlay Library

This directory contains a `Makefile` for building an NSFPlay library.

The library is just the core NSF/NSFe/M3U parser/emulator/renderer.
It does not have routines/methods for creating a GUI, playing audio to
devices, saving configuration options to files, and so on.

Currently experimental, your mileage may vary.

Also contains an example application for converting NSF(e) tracks
to WAV files.

## Building

cd into this directory and run make:

```bash
cd contrib
make
```

By default, it builds `libnsfplay.a` and `libnsfplay.so`, using
your default C and C++ compilers, in a debug mode.

Run `make release` to make an optimized build, and `make release_debug`
to perform an optimized build with debug info.

`release` and `release_debug` builds are always from-scratch, `debug`
builds use the usual Make semantics for determining what needs building.

## Customization

To pass additional `CFLAGS` and `CXXFLAGS`, use `CFLAGS_EXTRA` and
`CXXFLAGS_EXTRA`, for example:

```bash
make CFLAGS_EXTRA="-Wextra" CXXFLAGS_EXTRA="-Wextra"
```

To build libraries using different extensions, use
`STATIC_EXT` and `DYNLIB_EXT`. Change prefixes with
`STATIC_PREFIX` and `DYNLIB_PREFIX`.

Example: To cross-compile a Windows DLL just named `nsfplay.dll`, in the release config:

```bash
make CC=i686-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ DYNLIB_PREFIX= DYNLIB_EXT=.dll release
```

## Installation

This Makefile supports installing the library and header files via `make install`

`install` uses the following variables:

* `DESTDIR` to install to a staging directory.
* `PREFIX` to change installation prefix
    * default: `/usr/local`
* `LIBDIR` to set the library installation directory
    * default: `$(PREFIX)/lib`
    * setting `LIBDIR` directly ignores `DESTDIR` and `PREFIX`
* `INCDIR` to set the header installation directory
    * default: `$(PREFIX)/include/nsfplay`
    * setting `INCDIR` directly ignores `DESTDIR` and `PREFIX`
