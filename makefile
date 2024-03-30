include makefile.common

.PHONY: default core cmd gui winamp install uninstall clean

default: core cmd gui

core:
	$(MAKE) -C core

cmd: core
	$(MAKE) -C cmd

gui: core
	$(MAKE) -C gui

winamp: core
	$(MAKE) -C winamp

install: cmd
	$(MAKE) -C cmd install

uninstall:
	$(MAKE) -C cmd uninstall

clean:
	rm -rf $(INTDIR)
	rm -rf $(OUTDIR)
