#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

/* VGA graphics mode constants (320x200x256 colors, mode 13h) */
#define VGA_WIDTH  320
#define VGA_HEIGHT 200
#define VGA_MEMORY ((uint8_t*)0xA0000)

/* Color codes for VGA graphics mode (palette colors) */
#define VGA_COLOR_BLACK        0
#define VGA_COLOR_BLUE         1
#define VGA_COLOR_GREEN        2
#define VGA_COLOR_CYAN         3
#define VGA_COLOR_RED          4
#define VGA_COLOR_MAGENTA      5
#define VGA_COLOR_BROWN        6
#define VGA_COLOR_LIGHT_GREY   7
#define VGA_COLOR_DARK_GREY    8
#define VGA_COLOR_LIGHT_BLUE   9
#define VGA_COLOR_LIGHT_GREEN  10
#define VGA_COLOR_LIGHT_CYAN   11
#define VGA_COLOR_LIGHT_RED    12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN  14
#define VGA_COLOR_YELLOW       14
#define VGA_COLOR_WHITE        15

/* Print functions */
void print_char(char c, uint8_t fg_color, uint8_t bg_color);
void print_string(const char* str, uint8_t fg_color, uint8_t bg_color);
void print_at(size_t row, size_t col, const char* str, uint8_t fg_color, uint8_t bg_color);
void print_clear_screen(uint8_t bg_color);
void print_newline(void);

/* Graphics functions */
void draw_pixel(int x, int y, uint8_t color);
void draw_filled_rectangle(int x, int y, int width, int height, uint8_t color);

/* Utility functions */
void wait_ms(uint32_t milliseconds);
uint8_t input_char(void);
void input_string(char* buffer, size_t max_length);
int strcmp(const char* s1, const char* s2);
void real_hardware_shutdown(void);

/* Kernel entry point */
void kernel_main(void);
void start_shell(void);
void start_desktop(void);


#endif
