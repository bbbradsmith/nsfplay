include ../makefile.common

TARGETFILE ?= $(GUI_NAME)$(EXE_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
GUI_INTDIR ?= ../$(INTDIR)/gui
GUI_INSTALL = $(prefix)/$(TARGETFILE)

.PHONY: default install uninstall clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(GUI_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(GUI_INTDIR)/,$(SRCS:.cpp=.d))
CORE = ../$(OUTDIR)/$(LIB_NAME)$(LIB_SUFFIX)

ifeq ($(OS),Windows_NT)
OBJS += $(GUI_INTDIR)/gui_rc.o
endif

$(TARGET): $(OBJS) $(CORE) | $(dir $(TARGET))
	$(CXX) -o $(TARGET) $(LDFLAGS) $(LDFLAGS_GUI) $(OBJS) $(CORE)

$(GUI_INTDIR)/%.d: %.cpp | $(GUI_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(GUI_INTDIR)/$(basename $<).o $(CXXFLAGS) -c $<

$(GUI_INTDIR)/%.o: %.cpp $(GUI_INTDIR)/%.d | $(GUI_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS) -c $<

$(GUI_INTDIR)/gui_rc.o: gui.rc *.ico | $(GUI_INTDIR)/
	windres -i $< -o $@

$(GUI_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

$(CORE):
	$(MAKE) -C ../core

install: $(TARGET)
	$(MKDIR) $(dir $(GUI_INSTALL))
	cp $(TARGET) $(GUI_INSTALL)

uninstall:
	rm -f $(GUI_INSTALL)

clean:
	rm -rf $(GUI_INTDIR)
	rm -rf $(dir $(TARGET))

include $(DEPS)
