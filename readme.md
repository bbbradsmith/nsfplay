# NSFPlay 3

This is a work in progress total rewrite of NSFPlay. Goals include:

* Clean separation of sound emulation as a library that can be reused in other projects.
* Cross platform command line player.
* Cross platform GUI player.
* Winamp plugin.
* Rewrite to change the emulation architecture, hopefully more efficient and easier to maintain.

Components:
* `core` - A platform-agnostic NSF player library suitable for integration into other programs.
* `gui` - A cross platform GUI library that works with the core, allowing plugins to share the same UI as the stand alone player.
* `cmd` - A stand-alone command line player.
* `nsfplay` - A stand-alone GUI player.
* `winamp` - A winamp plugin.
* `wx` - [wxWidgets v3.2.4](https://github.com/wxWidgets/wxWidgets/tree/v3.2.4)

Build Setup:
* Windows: run `wxlib.bat` to prepare libraries, then use `nsfplay.sln` to build.
* Other: run `make wx` to prepare libraries, then use `make` to build.

Make targets:
* `make` - builds `cmd`, `gui`
* `make clean` - deletes `intermediate` and `output` directories.
* `make core` - build core library `nsfcore` to `output/make`.
* `make cmd` - build `nsfplac` to `output/make`.
* `make gui` - build gui library `nsfgui` to `output/make`.
* `make nsfplay` - build `nsfplay` to `output/make`.
* `make winamp` - (windows 32-bit only) build `nsfplay.dll` winamp plugin to `output/make`.
* `make install` - copy `nsfplay`/`nsfplaycmd` to `/usr/local/bin`.
* `make uninstall` - delete install from `/usr/local/bin`.
* `make install prefix=~/my/directory` - install to custom directory.
* `make uninstall prefix=~/my/directory` - uninstall from custom directory.
* `make wx` - fetches wxWidgets submodule in `wx/` and builds libraries to `wxlib/`.

See `makefile.common` for various potential environment overrides.
