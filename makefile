include makefile.common

.PHONY: default core core_mutex core_notext core_minimal cmd gui nsfplay mac winamp icons enums enums_verify wxlib install uninstall clean

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

gui: core
	$(MAKE) -C gui

nsfplay: core gui
	$(MAKE) -C nsfplay

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
