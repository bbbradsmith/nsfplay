include ../makefile.common

TARGETFILE ?= $(CMD_NAME)$(EXE_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
CMD_INTDIR ?= ../$(INTDIR)/cmd
CMD_INSTALL = $(prefix)/$(TARGETFILE)

.PHONY: default install uninstall clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(CMD_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(CMD_INTDIR)/,$(SRCS:.cpp=.d))
CORE = ../$(OUTDIR)/$(CORE_NAME)$(LIB_SUFFIX)
CXXFLAGS_ALL = $(CXXFLAGS) $(CXXFLAGS_EXTRA) $(INC_COMMON)
LDFLAGS_ALL = $(LDFLAGS) $(LDFLAGS_EXTRA) $(LDFLAGS_CMD)
LIBS =

# NOSOUND=1 builds with no audio output (WAV out only)

NOSOUND ?= 0

ifeq ($(NOSOUND),1)
	CXXFLAGS_ALL += -DNSF_NOSOUND=1
else
	CXXFLAGS_ALL += $(PAL_CXXFLAGS)
	LIBS += $(PAL_LIBS)
endif

# target build

$(TARGET): $(OBJS) $(CORE) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS_ALL) $(OBJS) $(CORE) $(LIBS)
	$(STRIP_DEBUG)

$(CMD_INTDIR)/%.d: %.cpp | $(CMD_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CMD_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(CMD_INTDIR)/%.o: %.cpp $(CMD_INTDIR)/%.d | $(CMD_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS_ALL) -c $<

$(CMD_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

$(CORE):
	$(MAKE) -C ../core

install: $(TARGET)
	$(MKDIR) $(dir $(CMD_INSTALL))
	cp $(TARGET) $(CMD_INSTALL)

uninstall:
	rm -f $(CMD_INSTALL)

clean:
	rm -rf $(CMD_INTDIR)
	rm -rf $(TARGET)

-include $(DEPS)
