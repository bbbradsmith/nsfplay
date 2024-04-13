# NSFPlay 3

This is a work in progress total rewrite of NSFPlay. Goals include:

* Clean separation of sound emulation as a library that can be reused in other projects.
* Cross platform command line player.
* Cross platform GUI player.
* Winamp plugin.
* Rewrite to change the emulation architecture, hopefully more efficient and easier to maintain.

### Unstable Build
  * [Download](https://nightly.link/bbbradsmith/nsfplay/workflows/build/nsfplay3)

## Components

* `core` - A platform-agnostic NSF player library suitable for integration into other programs.
* `gui` - A cross platform GUI library that works with the core, allowing plugins to share the same UI as the stand alone player.
* `cmd` - A stand-alone command line player.
* `nsfplay` - A stand-alone GUI player.
* `winamp` - A winamp plugin (Windows 32-bit only).
* `wx` - [wxWidgets v3.2.4](https://github.com/wxWidgets/wxWidgets/tree/v3.2.4) cross platform GUI library.

## Build

### Visual Studio 2019
  * Initialize the git submodules if it wasn't done when cloning:
    * `git submodule init`
    * `git submodule update --depth 1`
    * `cd wx`
    * `git submodule init`
    * `git submodule update --depth 1`
  * Run `wxlib.bat` to build the wxWidgets libraries.
  * Use `nsfplay.sln` to build.

[Visual Studio 2019](https://visualstudio.microsoft.com/vs/older-downloads/) is the target build tool. Alternatively, [MSYS2](https://www.msys2.org/) can be used to
build with make instead. If you want to use Visual Studio 2022, see the note below about the wxWidgets Library.

### Make

  * Run `make wxlib` to prepare the wxWidgets libraries.
  * Use `make` to build.

Requirements:
  * `make` to run the build.
  * `gcc` the chosen compiler.
  * `git` (optional) to download the repository and its submodules.
  * `cmake` (optional) to build wxWidgets libraries.
  * `libgtk-3-dev` (optional) used by wxWidgets on Ubuntu.

If using MSYS2 you'll need a specific version of `gcc` and `cmake`. You might look at the [build.yml](.github/workflows/build.yml) script to help understand what you need.

### wxWidgets Library

You could potentially [download wxWidgets binaries](https://www.wxwidgets.org/downloads/)
and use them instead of having to build them here,
but there is some additional setup required:
* In `nsfplay.sln` open the properties for each project and change `C/C++ > Code Generation > Runtime Library` to `Multi Threaded DLL /MD` or the debug version for all projects.
* Put the `include` folder from the downloaded headers in `wxlib\include`.
* Put the `lib\vc14x_x64_dll` folder from the 64-bit downloaded libraries in `wxlib\lib\vc_x64_lib`.
* Put the `lib\vc14x_dll` folder from the 32-bit downloaded libraries in `wxlib\lib\vc_lib`.

It's probably easier, however, to just use the `wxlib.bat` file to build it from the submodule source code in `wx`. If you want to try with Visual Studio 2022, you can edit the batch file or set an environment variable:
  * `set WXB_GENERATOR="Visual Studio 17 2022"`
  * `wxlib.bat`

Otherwise on other platforms there is probably some wxWidgets library package you can install and use. In theory could could use the `WXL_CONFIG` environment variable to tell the make build how to use it instead.
  * `export WXL_CONFIG=/path/to/wx-config`
  * `make`

On Windows (including MSYS2), you don't need to keep the cmake generated directories in wxlib once you've built the libraries. Everything needed will be in `wxlib\include` and `wxlib\lib`, and the other folders can be deleted, if you'd like to recover some space (wxWidgets is unfortunately large). On other platforms, the `wx-config` utility will be contained there, so you should at least keep that and whatever other files it uses to determine its build configuration.

### Make Targets

* `make` - builds `cmd`, `gui`
* `make clean` - deletes `intermediate/` and `output/` directories.
* `make core` - build core library `nsfcore` to `output/make/`.
* `make cmd` - build `nsfplac` to `output/make/`.
* `make gui` - build gui library `nsfgui` to `output/make/`.
* `make nsfplay` - build `nsfplay` to `output/make/`.
* `make winamp` - (windows 32-bit only) build `nsfplay.dll` winamp plugin to `output/make/`.
* `make install` - copy `nsfplay`/`nsfplac` to `/usr/local/bin/`.
* `make uninstall` - delete install from `/usr/local/bin/`.
* `make install prefix=~/my/directory` - install to custom directory.
* `make uninstall prefix=~/my/directory` - uninstall from custom directory.
* `make wxlib` - fetches wxWidgets submodule in `wx/` and builds libraries to `wxlib/`.

See `makefile.common` for various potential environment overrides.
