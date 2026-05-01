; boot.asm - Simple x86 bootloader that loads and runs the kernel
; This bootloader runs in 16-bit real mode

BITS 16
ORG 0x7C00                          ; BIOS loads bootloader at 0x7C00

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00                  ; Set stack pointer below bootloader

    ; Set graphics mode (320x200x256 colors, mode 13h)
    mov ax, 0x0013
    int 0x10



    ; Load kernel from disk using BIOS extended LBA read
    mov si, disk_packet
    mov byte [si], 0x10              ; packet size
    mov byte [si+1], 0x00            ; reserved
    mov word [si+2], 30               ; sectors to read
    mov word [si+4], 0x0000          ; buffer offset
    mov word [si+6], 0x1000          ; buffer segment (0x10000 physical)
    mov word [si+8], 1                ; starting LBA low
    mov word [si+10], 0x0000
    mov word [si+12], 0x0000
    mov word [si+14], 0x0000

    mov ah, 0x42                     ; extended read
    mov dl, 0x80                     ; first hard disk
    int 0x13
    jc disk_error

    ; Print success message
    mov si, loaded_msg
    call print_string

    ; Switch to protected mode
    cli                             ; Clear interrupts
    lgdt [gdt_descriptor]           ; Load GDT

    ; Set PE (Protection Enable) bit in CR0
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    ; Far jump to 32-bit code
    jmp 0x08:protected_mode

; Print string (SI points to string, null-terminated)
print_string:
    push ax
    push bx
.loop:
    lodsb                           ; Load byte from [DS:SI]
    or al, al                       ; Check for null terminator
    jz .done
    mov ah, 0x0E                    ; BIOS print character
    mov bx, 0x0007                  ; Page 0, color
    int 0x10
    jmp .loop
.done:
    pop bx
    pop ax
    ret

disk_error:
    mov si, error_msg
    call print_string
    hlt

; GDT (Global Descriptor Table)
gdt_start:
    ; Null descriptor
    dq 0x0000000000000000

    ; Code descriptor (0x08)
    dq 0x00cf9a000000ffff

    ; Data descriptor (0x10)
    dq 0x00cf92000000ffff

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; 32-bit protected mode code
BITS 32
protected_mode:
    mov ax, 0x10                    ; Data segment selector
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000               ; Set new stack

    ; Jump to kernel entry point at 0x10000
    jmp 0x10000

; Messages
BITS 16
loading_msg:    db 'Loading kernel...', 0x0D, 0x0A, 0
loaded_msg:     db 'Kernel loaded!', 0x0D, 0x0A, 0
error_msg:      db 'Disk error!', 0x0D, 0x0A, 0

disk_packet:
    times 16 db 0

; Bootloader signature (required by BIOS)
times 510 - ($ - $$) db 0
dw 0xAA55
