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

# minimal build test to make sure the core still compiles with as much defined out as possible

MINIMAL ?= 0
ifeq ($(MINIMAL),1)
	CXXFLAGS_ALL += -DNSF_NOTEXT
endif

# precompiled header

PCH_H = core.h
PCH_BUILD = pch.cpp
PCH_DECOY = $(CORE_INTDIR)/$(CORE_NAME).h
PCH = $(PCH_DECOY)$(PCH_SUFFIX)
PCHFLAGS = $(PCH_USE) $(PCH_DECOY)$(PCH_USE_SUFFIX)

$(PCH_DECOY): | $(CORE_INTDIR)/
	echo "#error \"Precompiled header decoy $(notdir $(PCH_DECOY)) used instead of $(notdir $(PCH))\"" > $@

$(PCH).d: $(PCH_BUILD) | $(CORE_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(PCH) $(CXXFLAGS_ALL) -c $<

$(PCH): $(PCH_BUILD) $(PCH_DECOY) $(PCH).d | $(CORE_INTDIR)/
	$(CXX) $(PCH_COMPILE) -o $(PCH) $(CXXFLAGS_ALL) -c $(PCH_BUILD)

# target build

$(TARGET): $(OBJS) | $(dir $(TARGET))
	rm -f $(TARGET)
	$(AR) rcs $(TARGET) $(OBJS)

# want $(PCHFLAGS) here but gcc does not appear to be able to use precompiled headers for -MF
$(CORE_INTDIR)/%.d: %.cpp $(PCH) | $(CORE_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CORE_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(CORE_INTDIR)/%.o: %.cpp $(CORE_INTDIR)/%.d $(PCH) | $(CORE_INTDIR)/
	$(CXX) -o $@ $(PCHFLAGS) $(CXXFLAGS_ALL) -c $<

$(CORE_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

clean:
	rm -rf $(CORE_INTDIR)
	rm -rf $(dir $(TARGET))

-include $(PCH).d $(DEPS)
