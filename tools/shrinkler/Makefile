
ifndef PLATFORM
PLATFORM := native
endif

ifneq ($(PLATFORM),$(filter $(PLATFORM),amiga windows-32 windows-64 native native-32 native-64 mac))
DUMMY := $(error Unsupported platform $(PLATFORM))
endif

BUILD_DIR     := build/$(PLATFORM)
INCLUDE       := -I decrunchers_bin
MKDIR_DUMMY   := $(shell mkdir -p $(BUILD_DIR))

all: $(BUILD_DIR)/Shrinkler

# Common flags
CFLAGS := -Wall -Wno-sign-compare
LFLAGS :=

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -O3
ifneq ($(PLATFORM),mac)
LFLAGS += -s
endif
endif

ifdef PROFILE
CFLAGS += -fno-inline -fno-inline-functions
LFLAGS :=
endif

ifeq ($(PLATFORM),amiga)

# Amiga build, using Amiga-GCC

CC       := m68k-amigaos-g++
LINK     := m68k-amigaos-g++
CFLAGS   += -m68000
LFLAGS   += -noixemul

else
ifeq ($(PLATFORM),windows-32)

# 32-bit MinGW build

CC       := i686-w64-mingw32-g++
LINK     := i686-w64-mingw32-g++
LFLAGS   += -static -static-libgcc -static-libstdc++

else
ifeq ($(PLATFORM),windows-64)

# 64-bit MinGW build

CC       := x86_64-w64-mingw32-g++
LINK     := x86_64-w64-mingw32-g++
LFLAGS   += -static -static-libgcc -static-libstdc++

else

# Native build

CC       := g++
LINK     := g++

ifeq ($(PLATFORM),native-32)
CFLAGS   += -m32
LFLAGS   += -m32
endif

ifeq ($(PLATFORM),native-64)
CFLAGS   += -m64
LFLAGS   += -m64
endif

ifeq ($(PLATFORM),mac)
CFLAGS   += -mmacosx-version-min=10.9
LFLAGS   += -mmacosx-version-min=10.9
endif

endif
endif
endif

$(BUILD_DIR)/%.o: cruncher/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@


HEADERS  := Header1.dat Header1C.dat Header1T.dat Header1CT.dat Header2.dat Header2C.dat
HEADERS  += OverlapHeader.dat OverlapHeaderC.dat OverlapHeaderT.dat OverlapHeaderCT.dat
HEADERS  += MiniHeader.dat MiniHeaderC.dat

$(BUILD_DIR)/Shrinkler.o: cruncher/*.h $(patsubst %,decrunchers_bin/%,$(HEADERS))

%.dat: %.bin
	python3 -c 'print(", ".join("0x%02X" % b for b in open("$^", "rb").read()))' > $@

$(BUILD_DIR)/Shrinkler: $(BUILD_DIR)/Shrinkler.o
	$(LINK) $(LFLAGS) $< -o $@

clean:
	rm -rf build decrunchers_bin/*.dat
