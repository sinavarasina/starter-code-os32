/*
 * vga.c - Implementasi driver teks VGA 80x25
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 */
#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t *vga_buf = (uint16_t *)0xB8000;
static uint32_t vga_row = 0;
static uint32_t vga_col = 0;
static uint8_t vga_color = 0x0A; /* Bright green on black */

void vga_set_color(uint8_t color) { vga_color = color; }

static void vga_scroll(void)
{
	for (uint32_t r = 1; r < VGA_HEIGHT; r++)
		for (uint32_t c = 0; c < VGA_WIDTH; c++)
			vga_buf[(r - 1) * VGA_WIDTH + c] =
			    vga_buf[r * VGA_WIDTH + c];

	for (uint32_t c = 0; c < VGA_WIDTH; c++)
		vga_buf[(VGA_HEIGHT - 1) * VGA_WIDTH + c] =
		    (uint16_t)((vga_color << 8) | ' ');

	vga_row = VGA_HEIGHT - 1;
}

void vga_init(void)
{
	for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		vga_buf[i] = (uint16_t)((vga_color << 8) | ' ');
	vga_row = 0;
	vga_col = 0;
}

void vga_putchar(char c)
{
	if (c == '\n') {
		vga_col = 0;
		if (++vga_row >= VGA_HEIGHT)
			vga_scroll();
		return;
	}
	vga_buf[vga_row * VGA_WIDTH + vga_col] =
	    (uint16_t)((vga_color << 8) | (uint8_t)c);
	if (++vga_col >= VGA_WIDTH) {
		vga_col = 0;
		if (++vga_row >= VGA_HEIGHT)
			vga_scroll();
	}
}

void vga_puts(const char *str)
{
	while (*str)
		vga_putchar(*str++);
}

void vga_putint(int32_t n)
{
	if (n < 0) {
		vga_putchar('-');
		n = -n;
	}
	if (n >= 10)
		vga_putint(n / 10);
	vga_putchar('0' + (char)(n % 10));
}
