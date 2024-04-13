include ../makefile.common

TARGETFILE ?= $(NSFPLAY_NAME)$(EXE_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
NSFPLAY_INTDIR ?= ../$(INTDIR)/nsfplay
NSFPLAY_INSTALL = $(prefix)/$(TARGETFILE)

.PHONY: default install uninstall clean

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

$(TARGET): $(OBJS) $(CORE) $(GUI) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS_ALL) $(OBJS) $(LIBS)
	$(STRIP_DEBUG)

$(NSFPLAY_INTDIR)/%.d: %.cpp | $(NSFPLAY_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(NSFPLAY_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(NSFPLAY_INTDIR)/%.o: %.cpp $(NSFPLAY_INTDIR)/%.d | $(NSFPLAY_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS_ALL) -c $<

$(NSFPLAY_INTDIR)/nsfplay_rc.o: nsfplay.rc *.ico | $(NSFPLAY_INTDIR)/
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
	rm -rf $(dir $(TARGET))

include $(DEPS)
