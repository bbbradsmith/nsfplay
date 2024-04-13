include makefile.common

.PHONY: default core cmd gui wxlib nsfplay winamp install uninstall clean

default: core cmd nsfplay

core:
	$(MAKE) -C core

cmd: core
	$(MAKE) -C cmd

gui: core
	$(MAKE) -C gui

wxlib:
	$(MAKE) -f makefile.wx

nsfplay: core gui
	$(MAKE) -C nsfplay

winamp: core gui
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
