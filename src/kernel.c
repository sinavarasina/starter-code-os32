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
 * ── Minggu 5 Sesi 1: Analisis Race Condition ─────────────
 * Producer-Consumer TANPA sinkronisasi. Amati output yang kacau.
 * Aktifkan dengan:
 *   make DEMO=-DSYNCHRONIZATION
 *
 * Output yang diharapkan (RUSAK karena race condition):
 *   [Producer] menulis item 0
 *   [Consumer] membaca item        ← terpotong! Producer menyela
 *   [Producer] menulis item 1
 *   2                              ← angka salah / terlambat tercetak
 *   ...
 *
 * ── Minggu 5 Sesi 2: Implementasi Mutex/Lock ─────────────
 * Mahasiswa mengimplementasikan sema_down/sema_up/lock di synch.c,
 * lalu uncomment blok TODO di producer() dan consumer() di bawah.
 * Tetap gunakan:
 *   make DEMO=-DSYNCHRONIZATION
 *
 * Output yang diharapkan (BENAR setelah fix):
 *   [Producer] menulis item 0
 *   [Consumer] membaca item 0
 *   [Producer] menulis item 1
 *   [Consumer] membaca item 1
 *   ...
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
static struct thread_args args_high = {VGA_COLOR_BRIGHT_WHITE, 5};

/* ================================================================
 *  Demo Minggu 5: Producer-Consumer
 *
 *  SESI 1 — Modul 9 (Analisis Race Condition):
 *    Jalankan apa adanya. Amati output yang kacau.
 *    Blok TODO di bawah masih di-comment → TIDAK ada sinkronisasi.
 *
 *  SESI 2 — Modul 10 (Implementasi Mutex/Lock):
 *    1. Implementasikan sema_down, sema_up, lock_acquire, lock_release
 *       di dalam synch.c.
 *    2. Uncomment keempat blok TODO di producer() dan consumer().
 *    3. Jalankan ulang → output harus rapi dan berurutan.
 * ================================================================ */
#ifdef SYNCHRONIZATION
#include "synch.h"

static struct semaphore sema_empty;
static struct semaphore sema_full;
static struct lock buf_lock;
static int shared_buffer = -1;

/* ── Producer ────────────────────────────────────────────────── */
static void producer(void *aux)
{
	(void)aux;
	int item = 0;

	while (1) {
		/* ======================================================
		 * TODO MODUL 10 — Uncomment baris berikut setelah
		 * mengimplementasikan sema_down() di synch.c:
		 *
		 * sema_down(&sema_empty);   <- tunggu slot kosong
		 * lock_acquire(&buf_lock);  <- kunci buffer
		 * ====================================================== */

		/* ── CRITICAL SECTION ─────────────────────────────── */

		/* Tulis ke buffer */
		shared_buffer = item;

		/*
		 * JEBAKAN RACE CONDITION:
		 * thread_yield() di sini mensimulasikan preemption
		 * di tengah critical section — Consumer bisa masuk
		 * sebelum Producer selesai mencetak log!
		 */
		thread_yield();

		vga_set_color(VGA_COLOR_BRIGHT_GREEN);
		vga_puts("[Producer] menulis item ");
		vga_putint(item);
		vga_putchar('\n');
		item++;

		/* ── END CRITICAL SECTION ─────────────────────────── */

		/* ======================================================
		 * TODO MODUL 10 — Uncomment baris berikut setelah
		 * mengimplementasikan lock_release() dan sema_up():
		 *
		 * lock_release(&buf_lock);  <- lepas kunci buffer
		 * sema_up(&sema_full);      <- beri tahu Consumer ada item
		 * ====================================================== */

		thread_yield();
	}
}

/* ── Consumer ────────────────────────────────────────────────── */
static void consumer(void *aux)
{
	(void)aux;

	while (1) {
		/* ======================================================
		 * TODO MODUL 10 — Uncomment baris berikut setelah
		 * mengimplementasikan sema_down() di synch.c:
		 *
		 * sema_down(&sema_full);    <- tunggu sampai ada item
		 * lock_acquire(&buf_lock);  <- kunci buffer
		 * ====================================================== */

		/* ── CRITICAL SECTION ─────────────────────────────── */

		vga_set_color(VGA_COLOR_YELLOW);
		vga_puts("[Consumer] membaca item ");

		/*
		 * JEBAKAN RACE CONDITION:
		 * thread_yield() di sini mensimulasikan preemption
		 * di tengah mencetak log — Producer bisa menulis nilai
		 * baru ke shared_buffer sebelum Consumer sempat
		 * mencetak angkanya!
		 */
		thread_yield();

		vga_putint(shared_buffer);
		vga_putchar('\n');

		/* ── END CRITICAL SECTION ─────────────────────────── */

		/* ======================================================
		 * TODO MODUL 10 — Uncomment baris berikut setelah
		 * mengimplementasikan lock_release() dan sema_up():
		 *
		 * lock_release(&buf_lock);  <- lepas kunci buffer
		 * sema_up(&sema_empty);     <- beri tahu Producer slot kosong
		 * ====================================================== */

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

/* ── Minggu 5: Sinkronisasi (Sesi 1 & 2) ────────────────── */
#ifdef SYNCHRONIZATION

	vga_puts("Demo Minggu 5: Producer-Consumer\n");
	vga_set_color(VGA_COLOR_DARK_GRAY);
	vga_puts("Modul 9 : Amati race condition pada output di bawah.\n");
	vga_puts("Modul 10: Uncomment blok TODO, implementasikan synch.c,\n");
	vga_puts(
	    "          lalu jalankan ulang untuk melihat output rapi.\n\n");
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
	vga_puts("(High=50 jalan 5 tick lalu exit, Med=31 mengambil alih, "
		 "Low=10 terakhir)\n\n");
	vga_set_color(VGA_COLOR_WHITE);

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
