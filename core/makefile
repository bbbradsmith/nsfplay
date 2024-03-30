include ../makefile.common

CORE_INTDIR ?= ../$(INTDIR)/core
CORE_OUTDIR ?= ../$(OUTDIR)
TARGET ?= $(CORE_OUTDIR)/nsfplaycore$(LIB_SUFFIX)

.PHONY: default clean

default: $(TARGET)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(CORE_INTDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(CORE_INTDIR)/,$(SRCS:.cpp=.d))

$(TARGET): $(OBJS) | $(CORE_OUTDIR)/
	rm -f $(TARGET)
	$(AR) rcs $(TARGET) $(OBJS)

$(CORE_INTDIR)/%.d: %.cpp | $(CORE_INTDIR)/
	$(CXX) -M -MM -MF $@ -MT $(CORE_INTDIR)/$(basename $<).o $(CXXFLAGS) -c $<

$(CORE_INTDIR)/%.o: %.cpp $(CORE_INTDIR)/%.d | $(CORE_INTDIR)/
	$(CXX) -o $@ $(CXXFLAGS) -c $<

$(CORE_INTDIR)/:
	$(MKDIR) $@

$(CORE_OUTDIR)/:
	$(MKDIR) $@

clean:
	rm -rf $(CORE_INTDIR)
	rm -rf $(CORE_OUTDIR)

include $(DEPS)
