/*
 * kernel.c - Entry point kernel IF-OS
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * ── Minggu 4: Round-Robin ────────────────────────────────
 * Tiga thread (A, B, C) berjalan selang-seling secara kooperatif.
 * Compile default (tanpa flag tambahan).
 *
 * Output yang diharapkan:
 *   [Thread-A] tick 0
 *   [Thread-B] tick 0
 *   [Thread-C] tick 0
 *   [Thread-A] tick 1
 *   ...
 *
 * ── Minggu 5: Sinkronisasi ───────────────────────────────
 * Producer-consumer dengan semaphore dan lock.
 * Aktifkan dengan:
 *   make DEMO=-DSYNCHRONIZATION
 *
 * Jika sema_down/sema_up belum diimplementasikan:
 *   → layar freeze, Producer terus jalan tanpa Consumer
 * Jika sudah benar:
 *   → Producer dan Consumer bergantian rapi
 *
 * ── Minggu 6: Priority Scheduling ───────────────────────
 * Thread dengan prioritas berbeda. Thread prioritas tinggi
 * harus jalan lebih dulu.
 * Aktifkan dengan:
 *   make DEMO=-DPRIORITY_SCHEDULING
 *
 * Jika next_thread_to_run() masih FIFO:
 *   → Low/Med/High muncul bergantian tidak terurut
 * Jika sudah benar:
 *   → High jalan lebih dulu (N tick), lalu Med, lalu Low
 */

#include "thread.h"
#include "types.h"
#include "vga.h"

/* ── Fungsi thread generic ───────────────────────────────────────
 *
 * Output: "[NamaThread] tick N"
 * Jika max_ticks > 0, thread exit setelah tick ke-N.
 * Jika max_ticks == 0, thread berjalan selamanya.
 * ─────────────────────────────────────────────────────────────── */
static void thread_func_generic(void *aux)
{
	struct thread_args *args = (struct thread_args *)aux;
	uint32_t tick = 0;

	while (1) {
		vga_set_color(args->color);
		vga_puts("[");
		vga_puts(thread_name());
		vga_puts("] tick ");
		vga_putint((int32_t)tick);
		vga_putchar('\n');

		tick++;

		if (args->max_ticks > 0 && tick >= args->max_ticks) {
			vga_set_color(args->color);
			vga_puts("[");
			vga_puts(thread_name());
			vga_puts("] selesai (");
			vga_putint((int32_t)args->max_ticks);
			vga_puts(" tick)\n");
			thread_exit();
		}

		thread_yield();
	}
}

/* ── Argumen per thread (static agar tidak hilang dari stack) ─── */
static struct thread_args args_a = {VGA_COLOR_BRIGHT_GREEN, 0};
static struct thread_args args_b = {VGA_COLOR_BRIGHT_CYAN, 0};
static struct thread_args args_c = {VGA_COLOR_YELLOW, 0};
static struct thread_args args_low = {VGA_COLOR_DARK_GRAY, 0};
static struct thread_args args_med = {VGA_COLOR_BRIGHT_CYAN, 0};
static struct thread_args args_high = {VGA_COLOR_BRIGHT_WHITE,
				       5}; /* exit setelah 5 tick */

/* ================================================================
 *  Demo Minggu 5: Producer-Consumer
 * ================================================================ */
#ifdef SYNCHRONIZATION
#include "synch.h"

static struct semaphore sema_empty;
static struct semaphore sema_full;
static struct lock buf_lock;
static int shared_buffer = -1;

static void producer(void *aux)
{
	(void)aux;
	int item = 0;
	while (1) {
		sema_down(&sema_empty);
		lock_acquire(&buf_lock);

		shared_buffer = item;
		vga_set_color(VGA_COLOR_BRIGHT_GREEN);
		vga_puts("[Producer] kirim item ");
		vga_putint(item++);
		vga_putchar('\n');

		lock_release(&buf_lock);
		sema_up(&sema_full);
		thread_yield();
	}
}

static void consumer(void *aux)
{
	(void)aux;
	while (1) {
		sema_down(&sema_full);
		lock_acquire(&buf_lock);

		vga_set_color(VGA_COLOR_YELLOW);
		vga_puts("[Consumer] terima item ");
		vga_putint(shared_buffer);
		vga_putchar('\n');

		lock_release(&buf_lock);
		sema_up(&sema_empty);
		thread_yield();
	}
}
#endif /* SYNCHRONIZATION */

/* ================================================================
 *  Entry point
 * ================================================================ */
void kernel_main(uint32_t magic, uint32_t mb_info)
{
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

/* ── Minggu 5: Sinkronisasi ──────────────────────────────── */
#ifdef SYNCHRONIZATION

	vga_puts("Demo Minggu 5: Sinkronisasi (Producer-Consumer)\n");
	vga_set_color(VGA_COLOR_DARK_GRAY);
	vga_puts("(Jika layar freeze, sema_down/sema_up belum "
		 "diimplementasikan)\n\n");
	vga_set_color(VGA_COLOR_WHITE);

	sema_init(&sema_empty, 1);
	sema_init(&sema_full, 0);
	lock_init(&buf_lock);

	thread_create("Producer", PRI_DEFAULT, producer, NULL);
	thread_create("Consumer", PRI_DEFAULT, consumer, NULL);

/* ── Minggu 6: Priority Scheduling ───────────────────────── */
#elif defined(PRIORITY_SCHEDULING)

	vga_puts("Demo Minggu 6: Priority Scheduling\n");
	vga_set_color(VGA_COLOR_DARK_GRAY);
	vga_puts("(Jika masih Round-Robin, next_thread_to_run() belum "
		 "dimodifikasi)\n");
	vga_puts(
	    "(High=50 jalan 5 tick lalu exit, Med=31 mengambil alih, Low=10 "
	    "terakhir)\n\n");
	vga_set_color(VGA_COLOR_WHITE);

	/* Dibuat urutan Low → Med → High (bukan urutan prioritas)
	 * agar efek priority scheduling terlihat jelas */
	thread_create("Low", 10, thread_func_generic, &args_low);
	thread_create("Med", PRI_DEFAULT, thread_func_generic, &args_med);
	thread_create("High", 50, thread_func_generic, &args_high);

/* ── Minggu 4: Round-Robin (default) ─────────────────────── */
#else

	vga_puts("Membuat thread A, B, C...\n");
	thread_create("Thread-A", PRI_DEFAULT, thread_func_generic, &args_a);
	thread_create("Thread-B", PRI_DEFAULT, thread_func_generic, &args_b);
	thread_create("Thread-C", PRI_DEFAULT, thread_func_generic, &args_c);

	vga_puts("Memulai scheduling...\n");
	vga_set_color(VGA_COLOR_DARK_GRAY);
	vga_puts("(Jika layar berhenti di sini, implementasi Minggu 4 belum "
		 "selesai)\n\n");

#endif

	/* kernel_main menyerahkan CPU secara permanen */
	thread_block();

	/* Seharusnya tidak pernah sampai di sini */
	vga_set_color(VGA_COLOR_BRIGHT_RED);
	vga_puts("\nERROR: kernel_main() kembali dari thread_block()!\n");
	while (1)
		__asm__ volatile("hlt");
}
