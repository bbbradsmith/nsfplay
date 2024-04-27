include ../makefile.common

TARGETFILE ?= $(WINAMP_NAME)$(DLL_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
WINAMP_INTDIR ?= ../$(INTDIR)/winamp

.PHONY: default clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(WINAMP_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(WINAMP_INTDIR)/,$(SRCS:.cpp=.d))
CORE = ../$(OUTDIR)/$(CORE_NAME)$(LIB_SUFFIX)
GUI = ../$(OUTDIR)/$(GUI_NAME)$(LIB_SUFFIX)
CXXFLAGS_ALL = $(CXXFLAGS) $(CXXFLAGS_EXTRA) $(INC_COMMON) $(WXL_CXXFLAGS)
LDFLAGS_ALL = $(LDFLAGS) $(LDFLAGS_EXTRA) $(LDFLAGS_DLL)
LIBS = $(CORE) $(GUI) $(WXL_LIBS)

$(TARGET): $(OBJS) $(CORE) $(GUI) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS_ALL) $(OBJS) $(LIBS)
	$(STRIP_DEBUG)

$(WINAMP_INTDIR)/%.d: %.cpp | $(WINAMP_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(WINAMP_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(WINAMP_INTDIR)/%.o: %.cpp $(WINAMP_INTDIR)/%.d | $(WINAMP_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS_ALL) -c $<

$(WINAMP_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

$(CORE):
	$(MAKE) -C ../core

$(GUI):
	$(MAKE) -C ../gui

clean:
	rm -rf $(WINAMP_INTDIR)
	rm -rf $(TARGET)

-include $(DEPS)
