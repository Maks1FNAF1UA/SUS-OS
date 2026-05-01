/* Linker script for kernel */

ENTRY(_start)

SECTIONS
{
    . = 0x10000;  /* Kernel starts at 0x10000 (64KB) as per bootloader */

    .text.startup ALIGN(4K) : {
        *(.text.startup)
    }

    .text ALIGN(4K) : {
        *(.text*)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data*)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss*)
    }
}
