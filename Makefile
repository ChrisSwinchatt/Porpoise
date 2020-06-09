MACHINE = raspi2

KERNEL_ELF    = bin/porpoise.elf
KERNEL_BINARY = bin/sdcard/boot/kernel8.img

PREFIX := aarch64-linux-gnu

CXX       = $(PREFIX)-g++
CXFLAGS   = -Iinclude/shared -Iinclude/$(MACHINE)\
			-ffreestanding -nostdlib -nodefaultlibs -nostartfiles -fno-exceptions -fno-rtti -fno-stack-protector \
			-O2\
			-std=c++17 -Wall -Wextra

LDFLAGS   = -Ttools/ld/$(MACHINE).ld 

LIBRARIES = -lgcc

OBJCOPY  = $(PREFIX)-objcopy

EMU      = qemu-system-aarch64
EMUFLAGS = -M raspi2,usb=on -smp 4 -m 512 -display sdl -sdl -d guest_errors

OBJECTS = $(patsubst src/%.S,obj/%.o,$(patsubst src/%.cpp,obj/%.o,$(shell find src/shared \( -name '*.cpp' -o -name '*.S' \))))\
          $(patsubst src/%.S,obj/%.o,$(patsubst src/%.cpp,obj/%.o,$(shell find src/$(MACHINE) \( -name '*.cpp' -o -name '*.S' \))))

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
	@rm -rf bin/* obj/*

$(KERNEL_BINARY): $(KERNEL_ELF)
	@echo "Converting to binary `basename $@`"
	@mkdir -p `dirname $@`
	@$(OBJCOPY) -O binary $< $@

$(KERNEL_ELF): $(OBJECTS)
	@echo "Linking `basename $@`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBRARIES)

run-qemu: $(KERNEL_ELF)
	@$(EMU) $(EMUFLAGS) -serial stdio -kernel $(KERNEL_ELF)

debug-qemu: $(KERNEL_ELF)
	@$(EMU) $(EMUFLAGS) -S -s &
	gdb $(KERNEL_BINARY) -ex "target remote :1234" -kernel $(KERNEL_ELF)

.PHONY: run-qemu debug-qemu
