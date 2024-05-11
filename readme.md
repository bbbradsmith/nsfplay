# NSFPlay 3

This is a work in progress total rewrite of NSFPlay. Goals include:

* Clean separation of sound emulation as a library that can be reused in other projects.
* Cross platform command line player.
* Cross platform GUI player.
* Winamp plugin.
* Rewrite to change the emulation architecture, hopefully more efficient and easier to maintain.

### Unstable Build
  * [Download](https://nightly.link/bbbradsmith/nsfplay/workflows/build_nsfplay3/nsfplay3)

## Components

Products:
* `core` - A platform-agnostic NSF player library `nsfcore` suitable for integration into other programs.
* `cmd` - A stand-alone command line player `nsfplac`.
* `nsfplay` - A stand-alone GUI player.
* `winamp` - A winamp plugin (Windows 32-bit only).

Support:
* `gui` - A library that provides a common user interface, allowing a plugin to share the same UI as the stand alone player.
* `include` - Public interfaces for the `core` and `gui` libraries.
* `icons` - Icons used for the GUI, edit the PNG files, and rebuild the ICO copies using the makefile ([imagemagick](https://imagemagick.org) required).
* `enum` - Definitions of all settings and text used by NSFPlay, allowing localized text in multiple languages ([python](https://www.python.org/) required).
* `shared` - Miscellaneous shared internal code.
* `wx` - [wxWidgets v3.2.4](https://github.com/wxWidgets/wxWidgets/tree/v3.2.4) cross platform GUI library.
* `portaudio` - [PortAudio v19.7.0](https://github.com/PortAudio/portaudio/tree/v19.7.0) cross platform audio library.

## Core Library

The NSFPlay core library is intended to be easy to include in and project that needs NES/Famicom sound emulation. Here are some notes on its goals and useful properties:

* The code is C++11 with no external dependencies.
* Interface is `extern "C"`, to allow integration into both C and C++ projects.
* [Very permissively licensed](license.txt).
* All state is stored in an `NSFCore` [PIMPL](https://en.cppreference.com/w/cpp/language/pimpl) structure. Multiple core instances can be used simultaneously.
* Can be used with no NSF file, and direct register writes can be used to manipulate the emulated sound devices directly. This may be suitable for use as an NES sound engine for a game.
* All strings are UTF-8 char.
* The core makes no access to the file system. You load files and provide the data to the core.
* The interfaces are designed so that new settings and properties can be easily added in future versions easily through new enumerations, rather than requiring new interface members. Use the constants provided in `nsfplayenums.h`, and they should still work in future versions.
* Some features like the expansion audio, or the text data for user interfaces, can be defined out to reduce the library footprint.
* To avoid name collision, all public interface functions are prefixed with `nsfplay_`. All public interface structs and enumerations are prefixed with `NSF`. All internal non-static functions and variables are kept within an `nsf` namespace.
* Allocations are kept to a minimum, and you can provide your own allocator instead of the default `malloc`/`free`:
  * 1 allocation from `nsfplay_create` for a new `NSFCore` state structure, this will always be the same size for any given build.
  * 1 allocation for internal rendering buffers, made when emulation begins (`nsfplay_song`/`nsfplay_song_play`/`nsfplay_render`/`nsfplay_emu_init`/`nsfplay_emu_run`), or can be manually triggered with `nsfplay_ready`. These will not be reallocated unless a change in settings or selected audio expansions require a larger total allocation. In particular, samplerate and downsampling settings will affect the amount needed.
  * 1 (optional) allocation from `nsfplay_load` to make an internal copy of the NSF file. The `assume` option will instead use the read only file contents given directly, as it will assume the pointer will be valid until it is unloaded.
  * 1 (optional) small allocation for every string setting that is changed from the default (there are very few of these). This can happen in `nsfplay_set_str` or indirectly through `nsfplay_set_ini` or `nsfplay_set_init` if they include string settings. An `assume` option is provided which will use the passed string settings directly instead of making internal copies.
* Interaction with the core has no internal thread-safety. There are a few recommended ways to use the library with multi-threading:
  1. Run all core library functions from one thread. This is suitable for rendering an entire song at once, perhaps for WAV output, or later playback.
  2. Run all core library functions from one thread. Keep 2 or more render buffers full in your main loop, and use a mutex to exchange finished buffer pointers with the audio thread for consumption. Some audio interface libraries may already provide a ready-made thread-safe consumption interface like this. The disadvantage is that if your buffers are too short, or the main loop takes too long before filling new buffers, the audio queue could starve.
  3. Run the core entirely in your audio callback thread. Use a small mutexed command state to pass control messages to the core's thread.
  4. Run `nsfplay_render` render in your audio callback thread, but stop the audio thread (or block the render call) whenever you need to call core functions from the main thread.
  5. Create your own mutex to use for all core library calls. This can be done by building the core with `NSF_MUTEX=1` defined.
* `NSFCore` instances do not have any shared state and two cores can be safely used in different threads, with the following caveats:
  * If you use `nsfplay_set_alloc` to provide a custom allocator, this allocator should have its own thread safety. The default is `std::malloc`/`std::free` which are usually already thread safe.
  * If you use `nsfplay_set_error_log` to set a global error log callback, this callback should have its own thread safety.
  * With a `DEBUG` build, if you use `nsfplay_set_debug_print` to replace the default debug log, this callback should have its own thread safety. The default uses `std::fprintf(stdout,...)` which is usually already thread safe.

## Build

Building the GUI components `nsfplay`/`winamp` requires first setting up the wxWidgets libraries. If you only need to use the command-line `nsfplac`, then you can skip this step.

If you need an example for how to build, you might look at the [build_nsfplay3.yml](.github/workflows/build_nsfplay3.yml) script used for the automated CI builds.

### Windows

[Visual Studio 2019](https://visualstudio.microsoft.com/vs/older-downloads/) is the target build tool for Windows. Alternatively, MSYS2 can be used to build with make instead (see below).

  * Initialize the git submodules if it wasn't done when cloning:
    * `git submodule init`
    * `git submodule update --depth 1`
    * `cd wx`
    * `git submodule init`
    * `git submodule update --depth 1`
  * Run `wxlib.bat` to build the wxWidgets libraries.
  * Run `portaudiolib.bat` to build the PortAudio libraries.
  * Use `nsfplay.sln` to build.

### Make

Other target platforms can be built with `make`. [MSYS2](https://www.msys2.org/) can also be used to do this on Windows.

  * Run `make wxlib` to prepare the wxWidgets libraries.
  * Run `make portaudio` to prepare the PortAudio libraries.
  * Use `make` to build.

Requirements:
  * `make` to run the build.
  * `gcc` the chosen compiler.
  * `git` (optional) to download the repository and its submodules.
  * `cmake` (optional) to build wxWidgets libraries.
  * `libgtk-3-dev` (optional) used by wxWidgets on Ubuntu.

### wxWidgets Library

You could potentially [download wxWidgets binaries](https://www.wxwidgets.org/downloads/)
and use them instead of having to build them here,
but there is some additional setup required:
* In `nsfplay.sln` open the properties for each project and change `C/C++ > Code Generation > Runtime Library` to `Multi Threaded DLL /MD` or the debug version for all projects.
* Put the `include` folder from the downloaded headers in `wxlib\include`.
* Put the `lib\vc14x_x64_dll` folder from the 64-bit downloaded libraries in `wxlib\lib\vc_x64_lib`.
* Put the `lib\vc14x_dll` folder from the 32-bit downloaded libraries in `wxlib\lib\vc_lib`.

It's probably easier, however, to just use the `wxlib.bat` file to build it from the submodule source code in `wx`.

Otherwise on other platforms there is probably some wxWidgets library package you can install and use. In theory could could use the `WXL_CONFIG` environment variable to tell the make build how to use it instead.
  * `export WXL_CONFIG=/path/to/wx-config`
  * `make`

On Windows (including MSYS2), you don't need to keep the cmake generated directories in wxlib once you've built the libraries. Everything needed will be in `wxlib\include` and `wxlib\lib`, and the other folders can be deleted, if you'd like to recover some space (wxWidgets is unfortunately large). On other platforms, the `wx-config` utility will be contained there, so you should at least keep that and whatever other files it uses to determine its build configuration. (`wx-config` does not appear to be aware of `cmake --install` and will reference the source includes, and uninstalled library binaries.)

### PortAudio Library

You could [download PortAudio binaries](https://files.portaudio.com/download.html)
and use them instead of having to build them here, but see the note about the wxWidgets Library above for project settings that must be changed.

On other platforms, you can use `PAL_DIR` to tell the make build to use a different installation of PortAudio.
  * `export PAL_DIR=/path/to/portaudio`
  * `make`

Like with the wxWidgets library build, you can delete the cmake directories in `portaudiolib` after building the libraries if you wish to save space.

Note: the MSVC build currently does not support WDM/KS drivers, because there is some conflict involving a multiple definition of `_GUID_NULL` which I could not find a resolution for. Hopefully this will be fixed in the next release of PortAudio.

### Make Targets

* `make` - builds `cmd`, `gui`
* `make clean` - deletes `intermediate/` and `output/` directories.
* `make core` - build core library `nsfcore` to `output/make/`.
* `make core_mutex` - (build test) rebuild core with blanket mutex around public interface.
* `make core_notext` - (build test) rebuild core with all text data defined out.
* `make core_minimal` - (build test) rebuild core with all optional features defined out.
* `make cmd` - build `nsfplac` to `output/make/`.
* `make cmd_nosound` - (build test) rebuild cmd without PortAudio.
* `make gui` - build gui library `nsfgui` to `output/make/`.
* `make nsfplay` - build `nsfplay` to `output/make/`.
* `make winamp` - (windows 32-bit only) build `nsfplay.dll` winamp plugin to `output/make/`.
* `make install` - copy `nsfplay`/`nsfplac` to `/usr/local/bin/`.
* `make uninstall` - delete install from `/usr/local/bin/`.
* `make install prefix=~/my/directory` - install to custom directory.
* `make uninstall prefix=~/my/directory` - uninstall from custom directory.
* `make icons` - rebuild windows icons from PNG source.
* `make enums` - generate enums and text data.
* `make enums_verify` - (build test) verify that current generated enums/text data match their source.
* `make mac` - packages `nsfplay` into `nsfplay.app` (build nsfplay first).
* `make wxlib` - fetches wxWidgets submodule in `wx/` and builds libraries to `wxlib/`.
* `make portaudio` - fetches PortAudio submodule in `portaudio/` and builds libraries to `portaudiolib/`.

See `makefile.common` for various potential environment overrides.
