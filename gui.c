#include "kernel.h"

void show_desktop(void) {
    print_clear_screen(VGA_COLOR_RED);
    draw_filled_rectangle(0, 0, VGA_WIDTH, 20, VGA_COLOR_BLACK);
}

void start_desktop(void) {
    show_desktop();
}
