OBJ := \
	cart.o \
	cartbuf.o \
	sd.o \
	sdcrc16.o \
	cartinit.o \
	cartexit.o \
	cartcardinit.o \
	cartcardrddram.o \
	cartcardrdcart.o \
	cartcardwrdram.o \
	cartcardwrcart.o \
	ci.o \
	ciinit.o \
	ciexit.o \
	cicardinit.o \
	cicardrddram.o \
	cicardrdcart.o \
	cicardwrdram.o \
	cicardwrcart.o \
	edxinit.o \
	edxexit.o \
	edxcard.o \
	edxcardinit.o \
	edxcardrddram.o \
	edxcardrdcart.o \
	edxcardwrdram.o \
	edxcardwrcart.o \
	edinit.o \
	edexit.o \
	edcard.o \
	edcardinit.o \
	edcardrddram.o \
	edcardrdcart.o \
	edcardwrdram.o \
	edcardwrcart.o \
	sc.o \
	scinit.o \
	scexit.o \
	sccardinit.o \
	sccardrddram.o \
	sccardrdcart.o \
	sccardwrdram.o \
	sccardwrcart.o

U64_PREFIX  := mips-linux-gnu-
U64_CC      := $(U64_PREFIX)gcc
U64_AR      := $(U64_PREFIX)ar
U64_ARCH    := -mabi=32 -march=vr4300 -mfix4300 -mno-abicalls -fno-PIC -G 0
U64_FLAG    := -Ilibultra/include -Iinclude -D_ULTRA64
U64_OPT     := -Os
U64_WARN    := -Wall -Wextra -Wpedantic
U64_CCFLAG  := $(U64_ARCH) -mno-check-zero-division -ffreestanding
U64_CCFLAG  += -fno-common -fno-toplevel-reorder
U64_CCFLAG  += $(U64_FLAG) $(U64_OPT) $(U64_WARN)
U64_ASFLAG  := $(U64_ARCH) $(U64_FLAG) $(U64_OPT)

.PHONY: default
default: lib/libcart.a

.PHONY: clean
clean:
	rm -f -r build lib

lib/libcart.a: $(addprefix build/,$(OBJ))
	@mkdir -p $(dir $@)
	$(U64_AR) rc $@ $^

build/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(U64_CC) $(U64_ASFLAG) -c -o $@ $<

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(U64_CC) $(U64_CCFLAG) -c -o $@ $<
