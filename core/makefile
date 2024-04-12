include ../makefile.common

TARGETFILE ?= $(CORE_NAME)$(LIB_SUFFIX)
TARGET ?= ../$(OUTDIR)/$(TARGETFILE)
CORE_INTDIR ?= ../$(INTDIR)/core

.PHONY: default clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(CORE_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(CORE_INTDIR)/,$(SRCS:.cpp=.d))
CXXFLAGS_ALL = $(CXXFLAGS) $(CXXFLAGS_EXTRA) $(INC_COMMON)

$(TARGET): $(OBJS) | $(dir $(TARGET))
	rm -f $(TARGET)
	$(AR) rcs $(TARGET) $(OBJS)

$(CORE_INTDIR)/%.d: %.cpp | $(CORE_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CORE_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(CORE_INTDIR)/%.o: %.cpp $(CORE_INTDIR)/%.d | $(CORE_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS_ALL) -c $<

$(CORE_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

clean:
	rm -rf $(CORE_INTDIR)
	rm -rf $(dir $(TARGET))

include $(DEPS)
