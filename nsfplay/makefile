include ../makefile.common

TARGETFILE ?= $(NSFPLAY_NAME)$(EXE_SUFFIX)
TARGET_DIR ?= ../$(OUTDIR)
TARGET ?= $(TARGET_DIR)/$(TARGETFILE)
NSFPLAY_INTDIR ?= ../$(INTDIR)/nsfplay
NSFPLAY_INSTALL = $(prefix)/$(TARGETFILE)

.PHONY: default mac install uninstall clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(NSFPLAY_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(NSFPLAY_INTDIR)/,$(SRCS:.cpp=.d))
CORE = ../$(OUTDIR)/$(CORE_NAME)$(LIB_SUFFIX)
GUI = ../$(OUTDIR)/$(GUI_NAME)$(LIB_SUFFIX)
CXXFLAGS_ALL = $(CXXFLAGS) $(CXXFLAGS_EXTRA) $(INC_COMMON) $(WXL_CXXFLAGS)
LDFLAGS_ALL = $(LDFLAGS) $(LDFLAGS_EXTRA) $(LDFLAGS_NSFPLAY)
LIBS = $(CORE) $(GUI) $(WXL_LIBS)

ifeq ($(OS),Windows_NT)
OBJS += $(NSFPLAY_INTDIR)/nsfplay_rc.o
endif

# NOSOUND=1 builds with no audio output (WAV out only)

NOSOUND ?= 0

ifeq ($(NOSOUND),1)
	CXXFLAGS_ALL += -DNSF_NOSOUND=1
else
	CXXFLAGS_ALL += $(PAL_CXXFLAGS)
	LIBS += $(PAL_LIBS)
endif

# target build

$(TARGET): $(OBJS) $(CORE) $(GUI) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS_ALL) $(OBJS) $(LIBS)
	$(STRIP_DEBUG)

$(NSFPLAY_INTDIR)/%.d: %.cpp | $(NSFPLAY_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(NSFPLAY_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(NSFPLAY_INTDIR)/%.o: %.cpp $(NSFPLAY_INTDIR)/%.d | $(NSFPLAY_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS_ALL) -c $<

$(NSFPLAY_INTDIR)/nsfplay_rc.o: nsfplay.rc $(wildcard ../icons/*.ico) | $(NSFPLAY_INTDIR)/
	windres -i $< -o $@

$(NSFPLAY_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

$(CORE):
	$(MAKE) -C ../core

$(GUI):
	$(MAKE) -C ../gui

install: $(TARGET)
	$(MKDIR) $(dir $(NSFPLAY_INSTALL))
	cp $(TARGET) $(NSFPLAY_INSTALL)

uninstall:
	rm -f $(NSFPLAY_INSTALL)

clean:
	rm -rf $(NSFPLAY_INTDIR)
	rm -rf $(TARGET)

# mac app build
# intentionally does not depend on $(TARGET) to allow modifying it for fat builds before creating the app

mac: $(ICON_INTDIR)/nsfplay.icns Info.plist ../version.txt
	$(MKDIR) $(TARGET_DIR)/$(NSFPLAY_NAME).app
	$(MKDIR) $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents
	$(MKDIR) $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents/MacOS
	$(MKDIR) $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents/Resources
	cp ../$(INTDIR)/icon/nsfplay.icns $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents/Resources
	cp $(TARGET) $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents/MacOS
	cp Info.plist $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents
	plutil -insert CFBundleShortVersionString -string $(shell cat ../version.txt) $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents/Info.plist
	plutil -insert CFBundleVersion -string $(shell cat ../version.txt) $(TARGET_DIR)/$(NSFPLAY_NAME).app/Contents/Info.plist

$(ICON_INTDIR)/nsfplay.icns:
	make -C ../icons mac

-include $(DEPS)
