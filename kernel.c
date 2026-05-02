#include "kernel.h"

void _start(void) __attribute__((section(".text.startup")));

void _start(void)
{
    kernel_main();
    for (;;) {
        __asm__("hlt");
    }
}

/* Cursor position */
static size_t cursor_row = 0;
static size_t cursor_col = 0;
static uint8_t current_fg = VGA_COLOR_WHITE;
static uint8_t current_bg = VGA_COLOR_BLACK;

/* Simple 8x8 font for graphics mode */
static const uint8_t font[128][8] = {
    [0] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // space
    ['!'] = {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00},
    ['"'] = {0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00},
    ['#'] = {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},
    ['$'] = {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00},
    ['%'] = {0x00,0x66,0x6C,0x18,0x30,0x66,0x46,0x00},
    ['&'] = {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00},
    ['\''] = {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00},
    ['('] = {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00},
    [')'] = {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},
    ['*'] = {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},
    ['+'] = {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    [','] = {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30},
    ['-'] = {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},
    ['.'] = {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00},
    ['/'] = {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},
    ['0'] = {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00},
    ['1'] = {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},
    ['2'] = {0x3C,0x66,0x06,0x1C,0x30,0x60,0x7E,0x00},
    ['3'] = {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00},
    ['4'] = {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00},
    ['5'] = {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00},
    ['6'] = {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00},
    ['7'] = {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    ['8'] = {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00},
    ['9'] = {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00},
    [':'] = {0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00},
    [';'] = {0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x00},
    ['<'] = {0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00},
    ['='] = {0x00,0x7E,0x00,0x7E,0x00,0x00,0x00,0x00},
    ['>'] = {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00},
    ['?'] = {0x3C,0x66,0x06,0x1C,0x18,0x00,0x18,0x00},
    ['@'] = {0x3C,0x66,0x6E,0x6E,0x60,0x66,0x3C,0x00},
    ['A'] = {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00},
    ['B'] = {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00},
    ['C'] = {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00},
    ['D'] = {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00},
    ['E'] = {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00},
    ['F'] = {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00},
    ['G'] = {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00},
    ['H'] = {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00},
    ['I'] = {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},
    ['J'] = {0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00},
    ['K'] = {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},
    ['L'] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00},
    ['M'] = {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},
    ['N'] = {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00},
    ['O'] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    ['P'] = {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00},
    ['Q'] = {0x3C,0x66,0x66,0x66,0x6E,0x66,0x3E,0x00},
    ['R'] = {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00},
    ['S'] = {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00},
    ['T'] = {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00},
    ['U'] = {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    ['V'] = {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00},
    ['W'] = {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    ['X'] = {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00},
    ['Y'] = {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},
    ['Z'] = {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00},
    ['['] = {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},
    ['\\'] = {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00},
    [']'] = {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00},
    ['^'] = {0x18,0x3C,0x66,0x00,0x00,0x00,0x00,0x00},
    ['_'] = {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00},
    ['`'] = {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00},
    ['a'] = {0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00},
    ['b'] = {0x60,0x60,0x7C,0x66,0x66,0x66,0x7C,0x00},
    ['c'] = {0x00,0x00,0x3C,0x60,0x60,0x60,0x3C,0x00},
    ['d'] = {0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00},
    ['e'] = {0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00},
    ['f'] = {0x1C,0x30,0x30,0x7C,0x30,0x30,0x30,0x00},
    ['g'] = {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x3C},
    ['h'] = {0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00},
    ['i'] = {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00},
    ['j'] = {0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x6C,0x38},
    ['k'] = {0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00},
    ['l'] = {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},
    ['m'] = {0x00,0x00,0x76,0x7E,0x6E,0x66,0x66,0x00},
    ['n'] = {0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00},
    ['o'] = {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00},
    ['p'] = {0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60},
    ['q'] = {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06},
    ['r'] = {0x00,0x00,0x7C,0x66,0x60,0x60,0x60,0x00},
    ['s'] = {0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00},
    ['t'] = {0x30,0x30,0x7C,0x30,0x30,0x30,0x1C,0x00},
    ['u'] = {0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00},
    ['v'] = {0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00},
    ['w'] = {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00},
    ['x'] = {0x00,0x00,0x66,0x3C,0x18,0x3C,0x66,0x00},
    ['y'] = {0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x3C},
    ['z'] = {0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00},
    ['{'] = {0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00},
    ['|'] = {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00},
    ['}'] = {0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00},
    ['~'] = {0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00},
};

/* Draw a character at pixel position (x, y) */
static void draw_char(int x, int y, char c, uint8_t fg_color, uint8_t bg_color) {
    if (c < 0 || c >= 128) c = '?'; // Unknown character
    
    const uint8_t* char_data = font[(uint8_t)c];
    
    for (int row = 0; row < 8; row++) {
        uint8_t row_data = char_data[row];
        for (int col = 0; col < 8; col++) {
            uint8_t color = (row_data & (1 << (7 - col))) ? fg_color : bg_color;
            draw_pixel(x + col, y + row, color);
        }
    }
}

/* Clear the screen */
void print_clear_screen(uint8_t bg_color) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            draw_pixel(x, y, bg_color);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}


/* Print a single character */
void print_char(char c, uint8_t fg_color, uint8_t bg_color)
{
    if (c == '\n') {
        print_newline();
        return;
    }

    if (cursor_row >= VGA_HEIGHT / 8) {
        cursor_row = 0;
    }

    draw_char(cursor_col * 8, cursor_row * 8, c, fg_color, bg_color);

    cursor_col++;
    if (cursor_col >= VGA_WIDTH / 8) {
        cursor_col = 0;
        cursor_row++;
    }
}

/* Helper: set the current cursor position */
static void set_cursor_position(size_t row, size_t col)
{
    if (row >= VGA_HEIGHT / 8) {
        row = (VGA_HEIGHT  / 8);
        row = (VGA_HEIGHT / 8) - 1;
    }
    if (col >= VGA_WIDTH / 8) {
        col = (VGA_WIDTH / 8);
    }
    cursor_row = row;
    cursor_col = col;
}

/* Print a null-terminated string */
void print_string(const char* str, uint8_t fg_color, uint8_t bg_color)
{
    while (*str) {
        print_char(*str, fg_color, bg_color);
        str++;
    }
}

/* Print a string at a specific row and column */
void print_at(size_t row, size_t col, const char* str, uint8_t fg_color, uint8_t bg_color)
{
    set_cursor_position(row, col);
    print_string(str, fg_color, bg_color);
}

/* Print newline */
void print_newline(void)
{
    cursor_col = 0;
    cursor_row++;
    if (cursor_row >= VGA_HEIGHT / 8) {
        cursor_row = 0;
    }
}

/* Draw a single pixel at (x, y) with given color */
void draw_pixel(int x, int y, uint8_t color)
{
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        const size_t index = y * VGA_WIDTH + x;
        VGA_MEMORY[index] = color;
    }
}

/* Draw a filled rectangle */
void draw_filled_rectangle(int x, int y, int width, int height, uint8_t color) {
    // Базова перевірка меж (clipping)
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > VGA_WIDTH) width = VGA_WIDTH - x;
    if (y + height > VGA_HEIGHT) height = VGA_HEIGHT - y;

    if (width <= 0 || height <= 0) return;

    for (int dy = 0; dy < height; dy++) {
        // Знаходимо початок рядка в пам'яті
        uint8_t* row = &VGA_MEMORY[(y + dy) * VGA_WIDTH + x];
        // Заповнюємо рядок
        for (int dx = 0; dx < width; dx++) {
            row[dx] = color;
        }
    }
}


/* String comparison function for freestanding environment */
int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/* Utility: simple millisecond wait (busy-wait) */
void wait_ms(uint32_t milliseconds)
{
    /* Assuming ~1GHz processor, rough approximation */
    uint32_t iterations = milliseconds * 1000000;
    while (iterations--) {
        __asm__("nop");
    }
}

/* Helper: read byte from I/O port */
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Helper: write byte to I/O port */
static inline void outb(uint8_t value, uint16_t port)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Keyboard input: read a single character from PS/2 keyboard */
uint8_t input_char(void)
{
    uint8_t status, scancode;
    
    /* Wait for keyboard data available */
    while (1) {
        status = inb(0x64);  /* Read status port */
        if (status & 0x01) { /* Data available */
            break;
        }
    }
    
    scancode = inb(0x60);   /* Read data port */
    return scancode;
}

// Функції для роботи з портами
static inline void outw(unsigned short port, unsigned short val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned short inw(unsigned short port) {
    unsigned short ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void emergency(const char* error_code) {
    // 1. Очищуємо та малюємо "Червоний екран смерті"
    print_clear_screen(VGA_COLOR_RED);
    // Використовуємо реальні межі екрана замість 10000 для коректності

    // 2. Виводимо текст
    print_at(10, 25, "!!!!!!!!!!!!!!!!!!!!!!", VGA_COLOR_WHITE, VGA_COLOR_RED);
    print_at(11, 25, "!!!!!! EMERGENCY !!!!!", VGA_COLOR_WHITE, VGA_COLOR_RED);
    print_at(12, 25, "!!!!!!!!!!!!!!!!!!!!!!", VGA_COLOR_WHITE, VGA_COLOR_RED);
    print_at(14, 15, "Error code: ", VGA_COLOR_WHITE, VGA_COLOR_RED);
    print_at(14, 27, error_code, VGA_COLOR_WHITE, VGA_COLOR_RED);
    print_at(16, 10, "System halted, please restart!", VGA_COLOR_WHITE, VGA_COLOR_RED);

    // 3. ЗУПИНКА СИСТЕМИ
    // Ми вимикаємо переривання, щоб клавіатура чи таймер не "розбудили" процесор
    __asm__ volatile ("cli"); 

    while(1) {
        // Зупиняємо виконання (процесор спить до наступного рестарту)
        __asm__ volatile ("hlt");
    }
}


typedef struct __attribute__((packed)) {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} acpi_rsdp_t;

typedef struct __attribute__((packed)) {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemid[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} acpi_sdt_header_t;

typedef struct __attribute__((packed)) {
    acpi_sdt_header_t h;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t reserved;
    uint8_t preferred_pm_profile;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t pm1_evt_len;
    uint8_t pm1_cnt_len;
    uint8_t pm2_cnt_len;
    uint8_t pm_tmr_len;
    uint8_t gpe0_blk_len;
    uint8_t gpe1_blk_len;
    uint8_t gpe1_base;
    uint8_t cst_cnt;
    uint16_t p_lvl2_lat;
    uint16_t p_lvl3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alrm;
    uint8_t mon_alrm;
    uint8_t century;
    uint16_t iapc_boot_arch;
    uint8_t reserved2;
    uint32_t flags;
    uint64_t reset_reg;
    uint8_t reset_value;
    uint8_t reserved3;
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;
    uint32_t x_pm1a_evt_blk;
    uint32_t x_pm1b_evt_blk;
    uint32_t x_pm1a_cnt_blk;
    uint32_t x_pm1b_cnt_blk;
    uint32_t x_pm2_cnt_blk;
    uint32_t x_pm_tmr_blk;
    uint32_t x_gpe0_blk;
    uint32_t x_gpe1_blk;
    uint8_t x_pm1_evt_len;
    uint8_t x_pm1_cnt_len;
    uint8_t x_pm2_cnt_len;
    uint8_t x_pm_tmr_len;
    uint8_t x_gpe0_blk_len;
    uint8_t x_gpe1_blk_len;
    uint8_t x_gpe1_base;
    uint8_t reserved4;
} acpi_fadt_t;

static int acpi_memcmp(const void* a, const void* b, size_t count)
{
    const uint8_t* pa = (const uint8_t*)a;
    const uint8_t* pb = (const uint8_t*)b;
    while (count--) {
        if (*pa != *pb) {
            return *pa - *pb;
        }
        pa++;
        pb++;
    }
    return 0;
}

static uint8_t acpi_checksum(const void* addr, size_t len)
{
    const uint8_t* bytes = (const uint8_t*)addr;
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += bytes[i];
    }
    return sum;
}

static const acpi_rsdp_t* acpi_find_rsdp(void)
{
    const char rsdp_sig[8] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};
    const uint16_t* ebda_ptr = (const uint16_t*)0x040E;
    uint16_t ebda_seg;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    __asm__ volatile ("movw %1, %0" : "=r" (ebda_seg) : "m" (*ebda_ptr));
#pragma GCC diagnostic pop
    uint32_t ebda = (uint32_t)ebda_seg << 4;

    if (ebda >= 0x00080000 && ebda < 0x000A0000) {
        for (uint32_t addr = ebda; addr < ebda + 1024; addr += 16) {
            const acpi_rsdp_t* rsdp = (const acpi_rsdp_t*)(uintptr_t)addr;
            if (acpi_memcmp(rsdp->signature, rsdp_sig, 8) == 0 && acpi_checksum(rsdp, 20) == 0) {
                return rsdp;
            }
        }
    }

    for (uint32_t addr = 0x000E0000; addr < 0x00100000; addr += 16) {
        const acpi_rsdp_t* rsdp = (const acpi_rsdp_t*)(uintptr_t)addr;
        if (acpi_memcmp(rsdp->signature, rsdp_sig, 8) == 0 && acpi_checksum(rsdp, 20) == 0) {
            return rsdp;
        }
    }

    return NULL;
}

static const acpi_sdt_header_t* acpi_find_table(const char* signature)
{
    const acpi_rsdp_t* rsdp = acpi_find_rsdp();
    if (!rsdp) {
        return NULL;
    }

    if (rsdp->revision < 2 || rsdp->xsdt_address == 0) {
        const acpi_sdt_header_t* rsdt = (const acpi_sdt_header_t*)(uintptr_t)rsdp->rsdt_address;
        if (!rsdt) {
            return NULL;
        }
        size_t entries = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
        const uint32_t* table_entries = (const uint32_t*)((uintptr_t)rsdt + sizeof(acpi_sdt_header_t));
        for (size_t i = 0; i < entries; i++) {
            const acpi_sdt_header_t* table = (const acpi_sdt_header_t*)(uintptr_t)table_entries[i];
            if (acpi_memcmp(table->signature, signature, 4) == 0) {
                return table;
            }
        }
    } else {
        const acpi_sdt_header_t* xsdt = (const acpi_sdt_header_t*)(uintptr_t)rsdp->xsdt_address;
        if (!xsdt) {
            return NULL;
        }
        size_t entries = (xsdt->length - sizeof(acpi_sdt_header_t)) / 8;
        const uint64_t* table_entries = (const uint64_t*)((uintptr_t)xsdt + sizeof(acpi_sdt_header_t));
        for (size_t i = 0; i < entries; i++) {
            const acpi_sdt_header_t* table = (const acpi_sdt_header_t*)(uintptr_t)table_entries[i];
            if (acpi_memcmp(table->signature, signature, 4) == 0) {
                return table;
            }
        }
    }

    return NULL;
}

static int acpi_parse_s5(const acpi_sdt_header_t* dsdt, uint8_t* slp_typa, uint8_t* slp_typb)
{
    const uint8_t* data = (const uint8_t*)dsdt;
    const uint8_t* end = data + dsdt->length;

    for (const uint8_t* ptr = data; ptr + 4 < end; ptr++) {
        if (ptr[0] == '_' && ptr[1] == 'S' && ptr[2] == '5' && ptr[3] == '_') {
            const uint8_t* p = ptr + 4;
            while (p < end && *p != 0x12) {
                p++;
            }
            if (p >= end) {
                continue;
            }
            p++;
            if (p >= end) {
                continue;
            }

            uint8_t length_byte = *p++;
            if (length_byte & 0x80) {
                uint32_t byte_count = (length_byte >> 6) & 0x03;
                for (uint32_t i = 0; i < byte_count && p < end; i++) {
                    p++;
                }
            }

            if (p >= end) {
                continue;
            }

            if (*p == 0x0A) {
                p++;
                if (p >= end) {
                    continue;
                }
                *slp_typa = *p++;
            } else if (*p == 0x0B) {
                p++;
                if (p + 1 >= end) {
                    continue;
                }
                *slp_typa = (uint8_t)(p[0] | (p[1] << 8));
                p += 2;
            } else {
                continue;
            }

            if (p >= end) {
                continue;
            }

            if (*p == 0x0A) {
                p++;
                if (p >= end) {
                    continue;
                }
                *slp_typb = *p;
            } else if (*p == 0x0B) {
                p++;
                if (p + 1 >= end) {
                    continue;
                }
                *slp_typb = (uint8_t)(p[0] | (p[1] << 8));
            } else {
                continue;
            }

            return 0;
        }
    }

    return -1;
}

static int acpi_do_shutdown(void)
{
    const acpi_fadt_t* fadt = (const acpi_fadt_t*)acpi_find_table("FACP");
    if (!fadt) {
        return -1;
    }

    const acpi_sdt_header_t* dsdt = NULL;
    if (fadt->x_dsdt) {
        dsdt = (const acpi_sdt_header_t*)(uintptr_t)fadt->x_dsdt;
    } else {
        dsdt = (const acpi_sdt_header_t*)(uintptr_t)fadt->dsdt;
    }
    if (!dsdt) {
        return -1;
    }

    uint8_t slp_typa = 0;
    uint8_t slp_typb = 0;
    if (acpi_parse_s5(dsdt, &slp_typa, &slp_typb) != 0) {
        return -1;
    }

    uint16_t pm1a = (uint16_t)fadt->pm1a_cnt_blk;
    uint16_t pm1b = (uint16_t)fadt->pm1b_cnt_blk;
    uint16_t sleep_value_a = (uint16_t)((slp_typa << 10) | (1 << 13));
    uint16_t sleep_value_b = (uint16_t)((slp_typb << 10) | (1 << 13));

    if (pm1a) {
        outw(pm1a, sleep_value_a);
    }
    if (pm1b) {
        outw(pm1b, sleep_value_b);
    }

    return 0;
}

void real_hardware_shutdown(void) {
    __asm__ volatile ("cli");

    if (acpi_do_shutdown() == 0) {
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    outw(0xB004, 0x2000);
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
    outb(0x02, 0xCF9);

    for (;;) {
        __asm__ volatile ("hlt");
    }
}



/* Keyboard input: read a string until Enter or max length */
void input_string(char* buffer, size_t max_length)
{
    size_t index = 0;
    uint8_t scancode;
    
    while (index < max_length - 1) {
        scancode = input_char();
        
        /* Enter key (0x1C) */
        if (scancode == 0x1C) {
            buffer[index] = '\0';
            print_newline();
            break;
        }
        /* Backspace (0x0E) */
        else if (scancode == 0x0E && index > 0) {
            index--;
            print_char('\b', VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
        /* Regular printable keys */
        else if (scancode < 0x3B) {
            /* Simple scancode to ASCII mapping for alphanumeric */
            char ch = 0;
            if (scancode >= 0x02 && scancode <= 0x0A) {
                ch = '1' + (scancode - 0x02);  /* 1-9 */
            } else if (scancode == 0x0B) {
                ch = '0';  /* 0 key */
            } else if (scancode >= 0x10 && scancode <= 0x19) {
                ch = "qwertyuiop"[scancode - 0x10];
            } else if (scancode >= 0x1E && scancode <= 0x28) {
                ch = "asdfghjkl"[scancode - 0x1E];
            } else if (scancode >= 0x2C && scancode <= 0x32) {
                ch = "zxcvbnm"[scancode - 0x2C];
            } else if (scancode == 0x39) {
                ch = ' ';  /* Spacebar */
            }
            
            if (ch != 0) {
                buffer[index++] = ch;
                print_char(ch, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
        }
    }
    buffer[index] = '\0';
}

/* Kernel main entry point - called by bootloader */
void kernel_main(void)
{
    /* Kernel initialization and loading messages */
    print_at(0, 0, "SUS OS Kernel Loading...", VGA_COLOR_RED, VGA_COLOR_GREEN);
    wait_ms(1000);
    print_at(1, 0, "Initializing VGA graphics mode...", VGA_COLOR_YELLOW, VGA_COLOR_BLUE);
    wait_ms(1000);
    print_at(2, 0, "Setting up keyboard input...", VGA_COLOR_WHITE, VGA_COLOR_CYAN);
    wait_ms(1000);
    print_at(3, 0, "!!!ALL LOADED SUCCESSFULLY!!!", VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    print_at(4, 0, "Clearing screen...", VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    wait_ms(2000);
    print_clear_screen(VGA_COLOR_BLACK);

    /* Boot menu - choose what to run */
    print_at(10, 30 - 17, "SUS OS Boot Menu", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    print_at(12, 35 - 17, "1. Boot", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    print_at(13, 35 - 17, "2. Shell", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    print_at(14, 35 - 17, "3. Shutdown", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);        print_at(20, 35 - 17, "ENTER OPTION: ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);

    uint8_t key = input_char();           /* Get one keystroke */
    while (key == 0x02 || key == 0x03 || key == 0x04) {
        if (key == 0x02) { /* 1 key - Boot GUI */
            print_clear_screen(VGA_COLOR_BLACK);
            start_desktop();
            key = 0;
        } else if (key == 0x03) { /* 2 key - Shell */
            print_clear_screen(VGA_COLOR_BLACK);
            start_shell();
        } else if (key == 0x04) { /* 3 key - Shutdown */
            print_clear_screen(VGA_COLOR_BLACK);
            print_at(10, 30 - 10, "Shutting down...", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            wait_ms(2000);
            real_hardware_shutdown();
        } else {
            print_clear_screen(VGA_COLOR_BLACK);
            print_at(10, 30 - 10, "Invalid input!", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            wait_ms(1000);
            print_clear_screen(VGA_COLOR_BLACK);
        }
    }

    /* Infinite loop */
    while (1) {
        __asm__("hlt");
    }
}
