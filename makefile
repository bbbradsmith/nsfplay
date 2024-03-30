include makefile.common

.PHONY: default core cmd gui winamp clean

default: core cmd gui

core:
	make -C core

cmd: core
	make -C cmd

gui: core
	make -C gui

winamp: core
	make -C winamp

clean:
	rm -rf $(INTDIR)
	rm -rf $(OUTDIR)
