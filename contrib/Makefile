.PHONY: all clean debug release release_debug demo

STATIC_PREFIX=lib
DYNLIB_PREFIX=lib
STATIC_EXT=.a
DYNLIB_EXT=.so
EXE_EXT=

OBJDIR := .objs
DEPDIR := .deps

LIB_STATIC=$(STATIC_PREFIX)nsfplay$(STATIC_EXT)
LIB_DYNLIB=$(DYNLIB_PREFIX)nsfplay$(DYNLIB_EXT)

CXXFLAGS_DEBUG := -g -O0 -Wall -fPIC -std=c++11
CFLAGS_DEBUG := -g -O0 -Wall -fPIC

CXXFLAGS_RELEASE := -g0 -O2 -Wall -fPIC -std=c++11 -DNDEBUG
CFLAGS_RELEASE := -g0 -O2 -Wall -fPIC -DNDEBUG

CXXFLAGS_RELEASE_DEBUG := -g -O2 -Wall -fPIC -std=c++11 -DNDEBUG
CFLAGS_RELEASE_DEBUG := -g -O2 -Wall -fPIC -DNDEBUG

DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

LIBXGM_CPP_SRCS = \
	../xgm/devices/Audio/MedianFilter.cpp \
	../xgm/devices/Audio/echo.cpp \
	../xgm/devices/Audio/filter.cpp \
	../xgm/devices/Audio/rconv.cpp \
	../xgm/devices/CPU/nes_cpu.cpp \
	../xgm/devices/Memory/nes_bank.cpp \
	../xgm/devices/Memory/nes_mem.cpp \
	../xgm/devices/Memory/nsf2_vectors.cpp \
	../xgm/devices/Memory/ram64k.cpp \
	../xgm/devices/Misc/detect.cpp \
	../xgm/devices/Misc/log_cpu.cpp \
	../xgm/devices/Misc/nes_detect.cpp \
	../xgm/devices/Misc/nsf2_irq.cpp \
	../xgm/devices/Sound/nes_apu.cpp \
	../xgm/devices/Sound/nes_dmc.cpp \
	../xgm/devices/Sound/nes_fds.cpp \
	../xgm/devices/Sound/nes_fme7.cpp \
	../xgm/devices/Sound/nes_mmc5.cpp \
	../xgm/devices/Sound/nes_n106.cpp \
	../xgm/devices/Sound/nes_vrc6.cpp \
	../xgm/devices/Sound/nes_vrc7.cpp \
	../xgm/player/nsf/nsf.cpp \
	../xgm/player/nsf/nsfconfig.cpp \
	../xgm/player/nsf/nsfplay.cpp \
	../xgm/player/nsf/pls/ppls.cpp \
	../xgm/player/nsf/pls/sstream.cpp

LIBXGM_C_SRCS = \
	../xgm/devices/Sound/legacy/emu2149.c \
	../xgm/devices/Sound/legacy/emu2212.c \
	../xgm/devices/Sound/legacy/emu2413.c

LIBVCM_CPP_SRCS = \
	../vcm/group.cpp \
	../vcm/value.cpp

LIBXGM_OBJS = $(patsubst %,$(OBJDIR)/%.o,$(basename $(subst ../,,$(LIBXGM_CPP_SRCS)))) $(patsubst %,$(OBJDIR)/%.o,$(basename $(subst ../,,$(LIBXGM_C_SRCS))))
LIBVCM_OBJS = $(patsubst %,$(OBJDIR)/%.o,$(basename $(subst ../,,$(LIBVCM_CPP_SRCS))))

LIBXGM_DEPS = $(patsubst %,$(DEPDIR)/%.d,$(basename $(subst ../,,$(LIBXGM_CPP_SRCS)))) $(patsubst %,$(DEPDIR)/%.d,$(basename $(subst ../,,$(LIBXGM_C_SRCS))))
LIBVCM_DEPS = $(patsubst %,$(DEPDIR)/%.d,$(basename $(subst ../,,$(LIBVCM_CPP_SRCS))))

OBJS = $(LIBXGM_OBJS) $(LIBVCM_OBJS)
DEPS = $(LIBXGM_DEPS) $(LIBVCM_DEPS)

$(shell mkdir -p $(dir $(OBJS)) > /dev/null)
$(shell mkdir -p $(dir $(DEPS)) > /dev/null)

COMPILE.c = $(CC) -c -o $@ $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(CFLAGS_EXTRA)
COMPILE.cc = $(CXX) -c -o $@ $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS_EXTRA)
LINK.o = $(CXX) -shared -o $@ $(LDFLAGS) $(LDLIBS)
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d
VPATH = ../

all: debug

show:
	echo $(LIBXGM_DEPS)

debug:
	"$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "STATIC_PREFIX=$(STATIC_PREFIX)" "DYNLIB_PREFIX=$(DYNLIB_PREFIX)" "STATIC_EXT=$(STATIC_EXT)" "DYNLIB_EXT=$(DYNLIB_EXT)" "CFLAGS=$(CFLAGS_DEBUG)" "CXXFLAGS=$(CXXFLAGS_DEBUG)" "$(LIB_STATIC)" "$(LIB_DYNLIB)"

release:
	"$(MAKE)" clean
	"$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "STATIC_PREFIX=$(STATIC_PREFIX)" "DYNLIB_PREFIX=$(DYNLIB_PREFIX)" "STATIC_EXT=$(STATIC_EXT)" "DYNLIB_EXT=$(DYNLIB_EXT)" "CFLAGS=$(CFLAGS_RELEASE)" "CXXFLAGS=$(CXXFLAGS_RELEASE)" "$(LIB_STATIC)" "$(LIB_DYNLIB)"

release_debug:
	"$(MAKE)" clean
	"$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "STATIC_PREFIX=$(STATIC_PREFIX)" "DYNLIB_PREFIX=$(DYNLIB_PREFIX)" "STATIC_EXT=$(STATIC_EXT)" "DYNLIB_EXT=$(DYNLIB_EXT)" "CFLAGS=$(CFLAGS_RELEASE_DEBUG)" "CXXFLAGS=$(CXXFLAGS_RELEASE_DEBUG)" "$(LIB_STATIC)" "$(LIB_DYNLIB)"

demo: nsf2wav$(EXE_EXT)

nsf2wav$(EXE_EXT): $(OBJDIR)/nsf2wav.o $(LIB_STATIC)
	$(CXX) -o $@ $^

$(LIB_STATIC): $(OBJS)
	$(AR) rcs $@ $^

$(LIB_DYNLIB): $(OBJS)
	$(LINK.o) $^

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)

clean:
	$(RM) $(DEPS)
	$(RM) $(OBJS)
	$(RM) $(LIB_STATIC) $(LIB_DYNLIB)

