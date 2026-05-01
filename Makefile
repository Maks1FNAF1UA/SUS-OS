# Makefile for OS kernel compilation and QEMU execution
# For Linux Mint (Debian-based systems)

# Compiler and tools
CC = gcc
LD = ld
AS = nasm
QEMU = qemu-system-i386

# Flags for 32-bit compilation
CFLAGS = -ffreestanding -fno-pie -m32 -g -O2 -Wall -Wextra
LDFLAGS = -m elf_i386 -T link.ld -nostdlib
ASFLAGS = -f bin

# Output files
BOOT_BIN = boot.bin
KERNEL_OBJ = kernel.o
KERNEL_ELF = kernel.elf
KERNEL_BIN = kernel.bin
OS_IMG = os.img

# Build targets
.PHONY: all clean run

all: $(OS_IMG)

# Compile bootloader
$(BOOT_BIN): boot.asm
	$(AS) $(ASFLAGS) $< -o $@

# Compile kernel C code
$(KERNEL_OBJ): kernel.c kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel ELF
$(KERNEL_ELF): $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) $< -o $@

# Convert kernel ELF to raw binary
$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $< $@

# Create disk image with bootloader and kernel
$(OS_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(OS_IMG) bs=1M count=10
	dd if=$(BOOT_BIN) of=$(OS_IMG) bs=512 count=1 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(OS_IMG) bs=512 seek=1 conv=notrunc

# Run in QEMU
run: $(OS_IMG)
	$(QEMU) -drive format=raw,file=$(OS_IMG) -m 512

# Run in QEMU with debugging
debug: $(OS_IMG)
	$(QEMU) -drive format=raw,file=$(OS_IMG) -m 512 -s -S

# Clean build artifacts
clean:
	rm -f $(BOOT_BIN) $(KERNEL_OBJ) $(KERNEL_ELF) $(KERNEL_BIN) $(OS_IMG)
