# NSFPlay 3

This is a work in progress total rewrite of NSFPlay. Goals include:

* Clean separation of sound emulation as a library that can be reused in other projects.
* Cross platform command line player.
* Cross platform GUI player.
* Winamp plugin.
* Rewrite to change the emulation architecture, hopefully more efficient and easier to maintain.

Make targets:
* `make` - builds `cmd`, `gui`, and on windows: `winamp` .
* `make clean` - deletes `intermediate` and `output` directories.
* `make core` - build core library `nsfplaycore` to `output/make`.
* `make cmd` - build `nsfplaycmd` to `output/make`.
* `make gui` - build `nsfplay` to `output/make`.
* `make winamp` - (windows only) build `nsfplug.dll` winamp plugin to `output/make`.
* `make install` - copy `nsfplay`/`nsfplaycmd` to `/usr/local/bin`.
* `make uninstall` - delete install from `/usr/local/bin`.
* `make install prefix=~/my/directory` - install to custom directory.
* `make uninstall prefix=~/my/directory` - uninstall from custom directory.

See `makefile.common` for various potential environment overrides.