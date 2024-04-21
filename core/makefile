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

PCH_H = core.h
PCH_BUILD = pch.cpp
PCH_DECOY = $(CORE_INTDIR)/$(CORE_NAME).h
PCH = $(PCH_DECOY).gch
PCHFLAGS = -Winvalid-pch -include $(PCH_DECOY)

$(TARGET): $(OBJS) | $(dir $(TARGET))
	rm -f $(TARGET)
	$(AR) rcs $(TARGET) $(OBJS)

$(PCH_DECOY): | $(CORE_INTDIR)/
	echo "#error \"Precompiled header decoy $(notdir $(PCH_DECOY)) used instead of $(notdir $(PCH))\"" > $@

$(PCH): $(PCH_BUILD) $(PCH_DECOY) $(PCH_H) | $(CORE_INTDIR)/
	$(CXX) -x c++-header -o $(PCH) $(CXXFLAGS_ALL) -c $(PCH_BUILD)

$(CORE_INTDIR)/%.d: %.cpp $(PCH) | $(CORE_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CORE_INTDIR)/$(basename $<).o $(CXXFLAGS_ALL) -c $<

$(CORE_INTDIR)/%.o: %.cpp $(CORE_INTDIR)/%.d | $(CORE_INTDIR)/
	$(CXX) -o $@ $(PCHFLAGS) $(CXXFLAGS_ALL) -c $<

$(CORE_INTDIR)/:
	$(MKDIR) $@

$(dir $(TARGET)):
	$(MKDIR) $@

clean:
	rm -rf $(CORE_INTDIR)
	rm -rf $(dir $(TARGET))

include $(DEPS)
