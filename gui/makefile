include ../makefile.common

TARGETFILE ?= $(GUI_NAME)$(LIB_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
GUI_INTDIR ?= ../$(INTDIR)/gui

.PHONY: default clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(GUI_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(GUI_INTDIR)/,$(SRCS:.cpp=.d))
INCS = -I$(WX_INCLUDE) -I$(WX_LIB_INCLUDE)

$(TARGET): $(OBJS) | $(dir $(TARGET))
	rm -f $(TARGET)
	$(AR) rcs $(TARGET) $(OBJS)

$(GUI_INTDIR)/%.d: %.cpp | $(GUI_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(GUI_INTDIR)/$(basename $<).o $(INCS) $(CXXFLAGS) -c $<

$(GUI_INTDIR)/%.o: %.cpp $(GUI_INTDIR)/%.d | $(GUI_INTDIR)/
	$(CXX) -o $@ $(INCS) $(CXXFLAGS) -c $<

$(GUI_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

clean:
	rm -rf $(GUI_INTDIR)
	rm -rf $(dir $(TARGET))

include $(DEPS)
