# OS Kernel Builder

Simple x86 bootloader and kernel with build system for QEMU.

## Setup

First, install the cross-compiler toolchain:

```bash
chmod +x setup.sh
./setup.sh
```

Or manually install:
- Ubuntu/Debian: `sudo apt-get install build-essential nasm qemu qemu-system-x86 binutils-i686-elf gcc-i686-elf`
- Fedora/RHEL: `sudo dnf install gcc make nasm qemu qemu-system-x86 binutils-i686-elf gcc-i686-elf`

## Building

```bash
make
```

This will:
1. Compile `boot.asm` with NASM
2. Compile `kernel.c` with i686-elf-gcc
3. Link kernel with custom linker script
4. Create bootable disk image (`os.img`)

## Running

```bash
make run
```

Launches QEMU with your OS image.

Debug mode (with GDB support):
```bash
make debug
```

## Cleaning

```bash
make clean
```

## Project Structure

- `boot.asm` - x86 bootloader (loads kernel and switches to protected mode)
- `kernel.c` - Kernel main code
- `kernel.h` - Kernel header with graphics/text functions
- `link.ld` - Linker script (places kernel at 0x10000)
- `Makefile` - Build automation

## Available Functions

### Text Mode
- `print_char(char c, uint8_t fg, uint8_t bg)` - Print single character
- `print_string(const char* str, uint8_t fg, uint8_t bg)` - Print string
- `print_clear_screen()` - Clear screen
- `print_newline()` - Move to next line

### Graphics Mode
- `draw_pixel(int x, int y, uint8_t color)` - Draw pixel
- `draw_filled_rectangle(int x, int y, int w, int h, uint8_t color)` - Draw rectangle

## Notes

- Kernel runs in 32-bit protected mode
- Uses VGA for output
- Bootloader at 0x7C00, Kernel at 0x10000
