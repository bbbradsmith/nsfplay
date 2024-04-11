include makefile.common

.PHONY: default core cmd gui nsfplay winamp install uninstall clean

default: core cmd nsfplay

ifeq ($(OS),Windows_NT)
default: winamp
endif

core:
	$(MAKE) -C core

cmd: core
	$(MAKE) -C cmd

gui: core
	$(MAKE) -C gui

nsfplay: core
	$(MAKE) -C nsfplay

winamp: core
	$(MAKE) -C winamp

install: cmd
	$(MAKE) -C cmd install
	$(MAKE) -C nsfplay install

uninstall:
	$(MAKE) -C cmd uninstall
	$(MAKE) -C nsfplay uninstall

clean:
	rm -rf $(INTDIR)
	rm -rf $(OUTDIR)
