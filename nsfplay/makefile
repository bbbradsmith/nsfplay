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
CORE = ../$(OUTDIR)/$(LIB_NAME)$(LIB_SUFFIX)

ifeq ($(OS),Windows_NT)
OBJS += $(NSFPLAY_INTDIR)/nsfplay_rc.o
endif

$(TARGET): $(OBJS) $(CORE) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS) $(LDFLAGS_NSFPLAY) $(OBJS) $(CORE)
	$(STRIP_DEBUG)

$(NSFPLAY_INTDIR)/%.d: %.cpp | $(NSFPLAY_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(NSFPLAY_INTDIR)/$(basename $<).o $(CXXFLAGS) -c $<

$(NSFPLAY_INTDIR)/%.o: %.cpp $(NSFPLAY_INTDIR)/%.d | $(NSFPLAY_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS) -c $<

$(NSFPLAY_INTDIR)/nsfplay_rc.o: nsfplay.rc *.ico | $(NSFPLAY_INTDIR)/
	windres -i $< -o $@

$(NSFPLAY_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

$(CORE):
	$(MAKE) -C ../core

install: $(TARGET)
	$(MKDIR) $(dir $(NSFPLAY_INSTALL))
	cp $(TARGET) $(NSFPLAY_INSTALL)

uninstall:
	rm -f $(NSFPLAY_INSTALL)

clean:
	rm -rf $(NSFPLAY_INTDIR)
	rm -rf $(dir $(TARGET))

include $(DEPS)
