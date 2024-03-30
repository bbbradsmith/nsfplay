# NSFPlay 3

This is a work in progress total rewrite of NSFPlay. Goals include:

* Clean separation of sound emulation as a library that can be reused in other projects.
* Cross platform command line player.
* Cross platform GUI player.
* Winamp plugin.
* Rewrite to change the emulation architecture, hopefully more efficient and easier to maintain.

Make targets:
* `make` - same as make `core` 
* `make core` - build core library `nsfplaycore` to `output/make`
* `make cmd` - build `nsfplaycmd` to `output/make`
* `make gui` - build `nsfplay` to `output/make`
* `make winamp` - build `nsfplay.dll` winamp plugin to `output/make`
* `make install` - copy `nsfplaycmd` to `/usr/local/bin`
* `make uninstall` - delete `nsfplaycmd` from `/usr/local/bin`
* `make install prefix=/my/directory` - copy `nsfplaycmd` to custom directory.
* `make uninstall prefix=/my/directory` - delete `nsfplaycmd` from custom directory.
