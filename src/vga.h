/*
 * vga.h - Antarmuka driver teks VGA 80x25
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 */
#ifndef VGA_H
#define VGA_H

#include "types.h"

void vga_init(void);
void vga_putchar(char c);
void vga_puts(const char *str);
void vga_putint(int32_t n);
void vga_set_color(uint8_t color);

/* Kode warna VGA (foreground | background << 4) */
#define VGA_COLOR_BLACK 0x0
#define VGA_COLOR_BLUE 0x1
#define VGA_COLOR_GREEN 0x2
#define VGA_COLOR_CYAN 0x3
#define VGA_COLOR_RED 0x4
#define VGA_COLOR_MAGENTA 0x5
#define VGA_COLOR_BROWN 0x6
#define VGA_COLOR_WHITE 0x7
#define VGA_COLOR_DARK_GRAY 0x8
#define VGA_COLOR_BRIGHT_BLUE 0x9
#define VGA_COLOR_BRIGHT_GREEN 0xA
#define VGA_COLOR_BRIGHT_CYAN 0xB
#define VGA_COLOR_BRIGHT_RED 0xC
#define VGA_COLOR_BRIGHT_MAGENTA 0xD
#define VGA_COLOR_YELLOW 0xE
#define VGA_COLOR_BRIGHT_WHITE 0xF

#endif /* VGA_H */
