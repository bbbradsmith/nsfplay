include makefile.common

.PHONY: default core cmd gui nsfplay mac winamp icons enums enums_verify wxlib install uninstall clean

default: core cmd nsfplay

core:
	$(MAKE) -C core

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
