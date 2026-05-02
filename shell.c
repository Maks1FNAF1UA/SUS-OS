#include "kernel.h"

char shell_buffer[64];

void setup_shell(void) {
    print_clear_screen(VGA_COLOR_BLACK);
    print_at(10, 0, "SUS OS Shell", VGA_COLOR_CYAN, VGA_COLOR_RED);
    print_at(12, 0, "Type 'help' for commands.", VGA_COLOR_CYAN, VGA_COLOR_RED);
    print_at(14, 0, "SUS> ", VGA_COLOR_RED, VGA_COLOR_BLACK);
    input_string(shell_buffer, sizeof(shell_buffer));
}

void start_shell(void) {
    setup_shell(); /* Read line from keyboard */
    if (strcmp(shell_buffer, "help") == 0) {
        print_at(16, 0, "Available commands: help, clear, shutdown", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
        wait_ms(1000);
        start_shell();
    } else if (strcmp(shell_buffer, "clear") == 0) {
        print_clear_screen(VGA_COLOR_BLACK);
        start_shell();
    } else if (strcmp(shell_buffer, "shutdown") == 0) {
        print_clear_screen(VGA_COLOR_BLACK);
        print_at(22, 30 - 10, "Shutting down", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        wait_ms(2000);
        real_hardware_shutdown();
    } else {
        print_at(16, 35 - 10, "Unknown command!", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        wait_ms(1000);
        start_shell();
    }
}

