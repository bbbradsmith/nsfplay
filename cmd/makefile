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

$(TARGET): $(OBJS) $(CORE) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS) $(LDFLAGS_CMD) $(OBJS) $(CORE)
	$(STRIP_DEBUG)

$(CMD_INTDIR)/%.d: %.cpp | $(CMD_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CMD_INTDIR)/$(basename $<).o $(CXXFLAGS) -c $<

$(CMD_INTDIR)/%.o: %.cpp $(CMD_INTDIR)/%.d | $(CMD_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS) -c $<

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
	rm -rf $(dir $(TARGET))

include $(DEPS)
