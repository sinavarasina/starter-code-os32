/*
 * kernel.c - Entry point kernel utama
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * Kernel ini membuat 3 thread yang masing-masing mencetak
 * pesan ke layar lalu memanggil thread_yield() agar thread
 * lain bisa berjalan.
 *
 * Jika thread_create(), thread_yield(), dan scheduler sudah
 * kalian implementasikan dengan benar, output di QEMU akan
 * terlihat seperti ini:
 *
 *   [Thread A] tick 0
 *   [Thread B] tick 0
 *   [Thread C] tick 0
 *   [Thread A] tick 1
 *   ...
 */
#include "vga.h"
#include "thread.h"
#include "scheduler.h"
#include "types.h"

static tcb_t thread_a, thread_b, thread_c;

static void func_a(void) {
    uint32_t tick = 0;
    while (1) {
        vga_set_color(0x0A);
        vga_puts("[Thread A] tick ");
        vga_putint((int32_t)tick++);
        vga_putchar('\n');
        thread_yield();
    }
}

static void func_b(void) {
    uint32_t tick = 0;
    while (1) {
        vga_set_color(0x0B);
        vga_puts("[Thread B] tick ");
        vga_putint((int32_t)tick++);
        vga_putchar('\n');
        thread_yield();
    }
}

static void func_c(void) {
    uint32_t tick = 0;
    while (1) {
        vga_set_color(0x0E);
        vga_puts("[Thread C] tick ");
        vga_putint((int32_t)tick++);
        vga_putchar('\n');
        thread_yield();
    }
}

void kernel_main(uint32_t magic, uint32_t mb_info) {
    (void)magic; (void)mb_info;

    vga_init();
    vga_set_color(0x0F);
    vga_puts("===================================\n");
    vga_puts("  Toy OS 32-bit  |  Praktikum SisOP\n");
    vga_puts("===================================\n");

    vga_set_color(0x07);
    vga_puts("Inisialisasi scheduler...\n");
    scheduler_init();

    vga_puts("Membuat thread...\n");
    thread_create(&thread_a, 1, "Thread-A", func_a);
    thread_create(&thread_b, 2, "Thread-B", func_b);
    thread_create(&thread_c, 3, "Thread-C", func_c);

    scheduler_add(&thread_a);
    scheduler_add(&thread_b);
    scheduler_add(&thread_c);

    vga_puts("Menjalankan scheduler...\n");
    vga_set_color(0x08);
    vga_puts("(Jika layar berhenti di sini, implementasi kalian belum benar)\n\n");

    scheduler_run();

    /* Tidak seharusnya sampai di sini */
    vga_set_color(0x0C);
    vga_puts("\nERROR: scheduler_run() kembali – ada yang salah!\n");
    while (1) { __asm__ volatile ("hlt"); }
}
