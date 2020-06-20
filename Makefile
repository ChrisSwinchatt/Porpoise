DEBUG := 1

MACHINE_BASE    = raspi
MACHINE_VERSION = 3
MACHINE         = $(MACHINE_BASE)$(MACHINE_VERSION)

KERNEL_ELF    = bin/porpoise.elf
KERNEL_DEBUG  = bin/porpoise.debug
KERNEL_BINARY = bin/sdcard/boot/kernel8.img

PREFIX := aarch64-linux-gnu

CXX       = $(PREFIX)-g++
INCLUDES  = -Iinclude/shared -Iinclude/$(MACHINE_BASE) -Iinclude/shared/lib -Iinclude/$(MACHINE_BASE)/lib
DEFINES   = -D__$(MACHINE_BASE)__=$(MACHINE_VERSION) $(shell tools/cat-config)
CXFLAGS   = $(INCLUDES)\
            $(DEFINES)\
			-ffreestanding -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector\
			-std=c++17 -Wall -Wextra -Werror
LD        = $(PREFIX)-ld
LDFLAGS   = -Ttools/ld/$(MACHINE).ld 
LIBRARIES = $(shell $(CXX) $(CXFLAGS) --print-file-name=libgcc.a)
OBJCOPY  = $(PREFIX)-objcopy

EMU      = qemu-system-aarch64
EMUFLAGS = -M $(MACHINE),usb=on -smp 4 -m 1G -d guest_errors -display none

ASM_OBJECTS = $(patsubst src/%.S,obj/%.o,$(shell find src/shared -name '*.S' ))            \
              $(patsubst src/%.S,obj/%.o,$(shell find src/$(MACHINE_BASE) -name '*.S' ))

CXX_OBJECTS = $(patsubst src/%.cpp,obj/%.o,$(shell find src/shared -name '*.cpp' ))          \
              $(patsubst src/%.cpp,obj/%.o,$(shell find src/$(MACHINE_BASE) -name '*.cpp' ))

OBJECTS = $(ASM_OBJECTS) $(CXX_OBJECTS)

ifeq ($(DEBUG),)
DEBUG=1
endif

ifeq ($(DEBUG),1)
CXFLAGS += -ggdb -O0
else
CXFLAGS += -g0 -O3
endif

export PWD
export DEFINES

obj/%.o: src/%.cpp
	@echo "<<< `basename $^`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -S -o $@.S $^
	@$(CXX) $(CXFLAGS) -c -o $@ $@.S

obj/%.o: src/%.S
	@echo "<<< `basename $^`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -D__ASM_SOURCE__ -c -o $@ $^

all: $(KERNEL_BINARY)

clean:
	@rm -rf obj/* $(KERNEL_BINARY) $(KERNEL_ELF) $(KERNEL_DEBUG)

cleancpp:
	@rm -rf $(CXX_OBJECTS) 2>/dev/null

$(KERNEL_BINARY): $(OBJECTS)
	@mkdir -p `dirname $@`
	@echo ">>> `basename $(KERNEL_ELF)`"
	@$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $^ $(LIBRARIES)
	@
	@echo ">>> `basename $(KERNEL_DEBUG)`"
	@cp $(KERNEL_ELF) $(KERNEL_DEBUG)
	@$(OBJCOPY) --strip-all $(KERNEL_ELF)
	@
	@echo ">>> `basename $@`"
	@$(OBJCOPY) -O binary $(KERNEL_ELF) $@

cxflags:
	@echo $(CXX) $(CXFLAGS)

run-qemu: $(KERNEL_BINARY)
	$(EMU) $(EMUFLAGS) -kernel $(KERNEL_ELF) -serial stdio

mon-qemu: $(KERNEL_BINARY)
	$(EMU) $(EMUFLAGS) -kernel $(KERNEL_ELF) -monitor stdio

debug-qemu: $(KERNEL_BINARY)
	$(EMU) $(EMUFLAGS) -kernel $(KERNEL_ELF) -s -S

check:
	@cppcheck --quiet --enable=all $(INCLUDES) $(DEFINES) `find src -name "*.cpp"` `find include -name "*.hpp"` 2>&1 | less

test:
	@$(MAKE) -s -j1 -C test
test-array:
	@$(MAKE) -s -j1 -C test test-array
test-algorithm:
	@$(MAKE) -s -j1 -C test test-algorithm
test-bitarray:
	@$(MAKE) -s -j1 -C test test-bitarray
test-vector:
	@$(MAKE) -s -j1 -C test test-vector

.PHONY: run-qemu debug-qemu test $(KERNEL_BINARY)
