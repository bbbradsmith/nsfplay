include ../makefile.common

# TODO windows icons should have 16, 32, 48, 256
#      make a 48 version

ICON_INTDIR ?= ../$(INTDIR)/icon
MAC_INTDIR ?= $(ICON_INTDIR)/nsfplay.iconset

.PHONY: default rebuild mac
default: rebuild

ifneq ("$(wildcard magick)","")
	$error("magick utility not found, please install imagemagick")
endif

ICOS = nsfplay.ico famicom.ico
ICOS_TEMP = $(addprefix $(ICON_INTDIR)/,$(ICOS))

rebuild: $(ICOS_TEMP)
	cp $(ICOS_TEMP) .

$(ICON_INTDIR)/nsfplay.ico: nsfplay16.png nsfplay32.png | $(ICON_INTDIR)/
	magick nsfplay32.png -scale 256 $(ICON_INTDIR)/nsfplay256.png
	magick convert nsfplay16.png nsfplay32.png $(ICON_INTDIR)/nsfplay256.png -colors 256 $@

$(ICON_INTDIR)/famicom.ico: famicom16.png famicom32.png | $(ICON_INTDIR)/
	magick famicom32.png -scale 256 $(ICON_INTDIR)/famicom256.png
	magick convert famicom16.png famicom32.png $(ICON_INTDIR)/famicom256.png -colors 256 $@

mac: nsfplay16.png nsfplay32.png
	$(MKDIR) $(MAC_INTDIR)
	cp nsfplay16.png $(MAC_INTDIR)/icon_16x16.png
	cp nsfplay32.png $(MAC_INTDIR)/icon_16x16@2x.png
	cp nsfplay32.png $(MAC_INTDIR)/icon_32x32.png
	magick nsfplay32.png -scale 64   $(MAC_INTDIR)/icon_32x32@2x.png
	magick nsfplay32.png -scale 128  $(MAC_INTDIR)/icon_128x128.png
	magick nsfplay32.png -scale 256  $(MAC_INTDIR)/icon_256x256.png
	magick nsfplay32.png -scale 512  $(MAC_INTDIR)/icon_512x512.png
	magick nsfplay32.png -scale 1024 $(MAC_INTDIR)/icon_512x512@2x.png
	cp $(MAC_INTDIR)/icon_512x512.png $(MAC_INTDIR)/icon_256x256@2x.png
	cp $(MAC_INTDIR)/icon_256x256.png $(MAC_INTDIR)/icon_128x128@2x.png
	iconutil -c icns -o $(ICON_INTDIR)/nsfplay.icns $(MAC_INTDIR)

$(ICON_INTDIR)/:
	$(MKDIR) $@

clean:
	rm -rf $(ICON_INTDIR)
