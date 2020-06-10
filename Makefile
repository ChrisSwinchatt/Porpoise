MACHINE_BASE    = raspi
MACHINE_VERSION = 3
MACHINE         = $(MACHINE_BASE)$(MACHINE_VERSION)

KERNEL_ELF    = bin/porpoise.elf
KERNEL_BINARY = bin/sdcard/boot/kernel8.img

PREFIX := aarch64-linux-gnu

CXX       = $(PREFIX)-g++
CXFLAGS   = -Iinclude/shared -Iinclude/$(MACHINE_BASE) -Iinclude/shared/lib -Iinclude/$(MACHINE_BASE)/lib\
            -D__$(MACHINE_BASE)__=$(MACHINE_VERSION)\
			-ffreestanding -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector\
			-std=c++17 -Wall -Wextra
LDFLAGS   = -Ttools/ld/$(MACHINE).ld 
LIBRARIES = -lgcc
OBJCOPY  = $(PREFIX)-objcopy

EMU      = qemu-system-aarch64
EMUFLAGS = -M $(MACHINE),usb=on -smp 4 -m 512 -d guest_errors -display none

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
	@echo "Compiling `basename $^`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -c -o $@ $^

obj/%.o: src/%.S
	@echo "Assembling `basename $^`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -D__ASM_SOURCE__ -c -o $@ $^

all: $(KERNEL_BINARY)

clean:
	@rm -rf obj/* $(KERNEL_BINARY) $(KERNEL_ELF)

$(KERNEL_BINARY): $(KERNEL_ELF)
	@echo "Converting to binary `basename $@`"
	@mkdir -p `dirname $@`
	@$(OBJCOPY) -O binary $< $@

$(KERNEL_ELF): $(OBJECTS)
	@echo "Linking `basename $@`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBRARIES)

run-qemu: $(KERNEL_ELF)
	$(EMU) $(EMUFLAGS) -serial stdio -kernel $(KERNEL_ELF)

mon-qemu: $(KERNEL_ELF)
	$(EMU) $(EMUFLAGS) -monitor stdio -kernel $(KERNEL_ELF)

debug-qemu: $(KERNEL_ELF)
	$(EMU) $(EMUFLAGS) -kernel $(KERNEL_ELF) -s -S &
	gdb-multiarch -s $(KERNEL_ELF) -q -ex "set architecture aarch64" -ex "b _start" -ex "target remote :1234"

.PHONY: run-qemu debug-qemu $(KERNEL_BINARY) $(KERNEL_ELF)
