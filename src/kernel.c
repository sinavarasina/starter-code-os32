/*
 * kernel.c - Entry point kernel IF-OS
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * ── Minggu 4: Thread ─────────────────────────────────────
 * Tiga thread (A, B, C) berjalan selang-seling secara kooperatif.
 * Jika thread_create(), thread_yield(), dan next_thread_to_run()
 * diimplementasikan dengan benar, output akan terlihat:
 *
 *   [Thread A] tick 0
 *   [Thread B] tick 0
 *   [Thread C] tick 0
 *   [Thread A] tick 1
 *   ...
 *
 * ── Minggu 5: Sinkronisasi ───────────────────────────────
 * Producer-consumer dengan semaphore dan lock.
 * Aktifkan demo dengan mendefinisikan DEMO_WEEK5.
 *
 * ── Minggu 6: Scheduling ─────────────────────────────────
 * Thread dengan prioritas berbeda. Thread priority tinggi
 * harus jalan lebih dulu.
 * Aktifkan demo dengan mendefinisikan DEMO_WEEK6.
 */
#include "thread.h"
#include "types.h"
#include "vga.h"

/* ── Demo Minggu 4: Tiga thread kooperatif ─────────────── */

static void func_a(void *aux) {
  (void)aux;
  uint32_t tick = 0;
  while (1) {
    vga_set_color(0x0A); /* Bright green */
    vga_puts("[Thread A] tick ");
    vga_putint((int32_t)tick++);
    vga_putchar('\n');
    thread_yield();
  }
}

static void func_b(void *aux) {
  (void)aux;
  uint32_t tick = 0;
  while (1) {
    vga_set_color(0x0B); /* Bright cyan */
    vga_puts("[Thread B] tick ");
    vga_putint((int32_t)tick++);
    vga_putchar('\n');
    thread_yield();
  }
}

static void func_c(void *aux) {
  (void)aux;
  uint32_t tick = 0;
  while (1) {
    vga_set_color(0x0E); /* Yellow */
    vga_puts("[Thread C] tick ");
    vga_putint((int32_t)tick++);
    vga_putchar('\n');
    thread_yield();
  }
}

/* ── Entry point ────────────────────────────────────────── */

void kernel_main(uint32_t magic, uint32_t mb_info) {
  (void)magic;
  (void)mb_info;

  vga_init();
  vga_set_color(VGA_COLOR_BRIGHT_WHITE);
  vga_puts("===================================\n");
  vga_puts("  IF-OS  |  Praktikum Sistem Operasi\n");
  vga_puts("===================================\n\n");

  vga_set_color(VGA_COLOR_WHITE);
  vga_puts("Inisialisasi sistem thread...\n");
  thread_init();

  vga_puts("Membuat thread A, B, C...\n");
  thread_create("Thread-A", PRI_DEFAULT, func_a, NULL);
  thread_create("Thread-B", PRI_DEFAULT, func_b, NULL);
  thread_create("Thread-C", PRI_DEFAULT, func_c, NULL);

  vga_puts("Memulai scheduling...\n");
  vga_set_color(VGA_COLOR_DARK_GRAY);
  vga_puts(
      "(Jika layar berhenti di sini, implementasi Minggu 4 belum selesai)\n\n");

  /* kernel_main menyerahkan CPU secara permanen.
   * thread_block() = masuk BLOCKED, tidak kembali ke ready queue.
   * Thread A, B, C akan berjalan terus (round-robin). */
  thread_block();

  /* Jika sampai di sini: seharusnya tidak terjadi */
  vga_set_color(VGA_COLOR_BRIGHT_RED);
  vga_puts("\nERROR: kernel_main() kembali dari thread_block()!\n");
  while (1)
    __asm__ volatile("hlt");
}
