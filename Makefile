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
CXFLAGS   = $(INCLUDES)\
            -D__$(MACHINE_BASE)__=$(MACHINE_VERSION)\
			-ffreestanding -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector\
			-std=c++17 -Wall -Wextra
LD        = $(PREFIX)-ld
LDFLAGS   = -Ttools/ld/$(MACHINE).ld 
LIBRARIES = $(shell $(CXX) $(CXFLAGS) --print-file-name=libgcc.a)
OBJCOPY  = $(PREFIX)-objcopy

EMU      = qemu-system-aarch64
EMUFLAGS = -M $(MACHINE),usb=on -smp 4 -m 1G -d guest_errors -display none

OBJECTS = $(patsubst src/%.S,obj/%.o,$(patsubst src/%.cpp,obj/%.o,$(shell find src/shared \( -name '*.cpp' -o -name '*.S' \))))\
          $(patsubst src/%.S,obj/%.o,$(patsubst src/%.cpp,obj/%.o,$(shell find src/$(MACHINE_BASE) \( -name '*.cpp' -o -name '*.S' \))))

ifeq ($(DEBUG),)
DEBUG=1
endif

ifeq ($(DEBUG),1)
CXFLAGS += -ggdb -O0
else
CXFLAGS += -g0 -O3
endif

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

run-qemu: $(KERNEL_BINARY)
	$(EMU) $(EMUFLAGS) -serial stdio -kernel $(KERNEL_ELF)

mon-qemu: $(KERNEL_BINARY)
	$(EMU) $(EMUFLAGS) -monitor stdio -kernel $(KERNEL_ELF)

debug-qemu: $(KERNEL_BINARY)
	$(EMU) $(EMUFLAGS) -kernel $(KERNEL_ELF) -s -S -monitor stdio

check:
	@cppcheck --quiet --enable=all $(INCLUDES) `find src -name "*.cpp"` `find include -name "*.hpp"` 2>&1 | grep -v "never used"

.PHONY: run-qemu debug-qemu $(KERNEL_BINARY)
