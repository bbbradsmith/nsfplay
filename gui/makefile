include ../makefile.common

TARGETFILE ?= $(GUI_NAME)$(LIB_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
GUI_INTDIR ?= ../$(INTDIR)/gui

.PHONY: default clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(GUI_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(GUI_INTDIR)/,$(SRCS:.cpp=.d))
CXXFLAGS_ALL = $(CXXFLAGS) $(CXXFLAGS_EXTRA) $(INC_COMMON) $(WXL_CXXFLAGS)

$(TARGET): $(OBJS) | $(dir $(TARGET))
	rm -f $(TARGET)
	$(AR) rcs $(TARGET) $(OBJS)

$(GUI_INTDIR)/%.d: %.cpp | $(GUI_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(GUI_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(GUI_INTDIR)/%.o: %.cpp $(GUI_INTDIR)/%.d | $(GUI_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS_ALL) -c $<

$(GUI_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

clean:
	rm -rf $(GUI_INTDIR)
	rm -rf $(dir $(TARGET))

-include $(DEPS)
