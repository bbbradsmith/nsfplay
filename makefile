include makefile.common

.PHONY: default core core_mutex core_notext core_minimal cmd cmd_nosound gui nsfplay mac winamp icons enums enums_verify portaudiolib wxlib install uninstall clean

default: core cmd nsfplay

core:
	$(MAKE) -C core

core_mutex:
	$(MAKE) -C core clean
	$(MAKE) -C core MUTEX=1

core_notext:
	$(MAKE) -C core clean
	$(MAKE) -C core NOTEXT=1

core_minimal:
	$(MAKE) -C core clean
	$(MAKE) -C core MINIMAL=1

cmd: core
	$(MAKE) -C cmd

cmd_nosound: core
	$(MAKE) -C cmd clean
	$(MAKE) -C cmd NOSOUND=1

gui: core
	$(MAKE) -C gui

nsfplay: core gui
	$(MAKE) -C nsfplay

nsfplay_nosound: core gui
	$(MAKE) -C nsfplay clean
	$(MAKE) -C nsfplay NOSOUND=1

mac:
	$(MAKE) -C nsfplay mac

winamp: core gui
	$(MAKE) -C winamp

icons:
	$(MAKE) -C icons

enums:
	$(MAKE) -C enums

enums_verify:
	$(MAKE) -C enums verify

portaudiolib:
	$(MAKE) -f makefile.portaudio

wxlib:
	$(MAKE) -f makefile.wx

install: cmd
	$(MAKE) -C cmd install
	$(MAKE) -C nsfplay install

uninstall:
	$(MAKE) -C cmd uninstall
	$(MAKE) -C nsfplay uninstall

clean:
	rm -rf $(INTDIR)
	rm -rf $(OUTDIR)
