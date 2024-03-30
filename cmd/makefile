include ../makefile.common

CMD_INTDIR ?= ../$(INTDIR)/cmd
CMD_OUTDIR ?= ../$(OUTDIR)
prefix ?= /usr/local/bin
TARGETFILE ?= nsfplaycmd$(EXE_SUFFIX)
TARGET ?= $(CMD_OUTDIR)/$(TARGETFILE)
CMD_INSTALL = $(prefix)/$(TARGETFILE)

.PHONY: default install uninstall clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(CMD_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(CMD_INTDIR)/,$(SRCS:.cpp=.d))
CORE = $(CMD_OUTDIR)/nsfplaycore$(LIB_SUFFIX)

$(TARGET): $(OBJS) $(CORE) | $(CMD_OUTDIR)/
	$(CXX) -o $(TARGET) $(LDFLAGS) $(OBJS) $(CORE)

$(CMD_INTDIR)/%.d: %.cpp | $(CMD_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CMD_INTDIR)/$(basename $<).o $(CXXFLAGS) -c $<

$(CMD_INTDIR)/%.o: %.cpp $(CMD_INTDIR)/%.d | $(CMD_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS) -c $<

$(CMD_INTDIR)/:
	$(MKDIR) $@

$(CMD_OUTDIR)/:
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
	rm -rf $(CMD_OUTDIR)

include $(DEPS)
