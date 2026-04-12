/*
 * thread.h - Definisi Thread dan API Manajemen Thread (Pintos Style)
 *
 * Diadaptasi dari Pintos (Stanford) untuk IF-OS Praktikum SisOP.
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * Implementasikan fungsi-fungsi yang ditandai TUGAS MAHASISWA
 * di dalam file thread.c.
 */
#ifndef THREAD_H
#define THREAD_H

#include "types.h"

/* Jumlah thread maksimum yang bisa dibuat */
#define MAX_THREADS 8

/* Nilai prioritas thread */
#define PRI_MIN 0      /* Prioritas terendah */
#define PRI_DEFAULT 31 /* Prioritas default */
#define PRI_MAX 63     /* Prioritas tertinggi */

/* Magic number untuk deteksi stack overflow.
 * Jika thread->magic berubah dari nilai ini, stack sudah overflow! */
#define THREAD_MAGIC 0xcd6abf4b

/* Status thread selama siklus hidupnya */
enum thread_status {
  THREAD_RUNNING, /* Sedang berjalan di CPU */
  THREAD_READY,   /* Siap jalan, menunggu giliran */
  THREAD_BLOCKED, /* Menunggu event (sinkronisasi) */
  THREAD_DYING,   /* Akan segera dihentikan */
};

/* ================================================================
 *  Struct argumen thread generic
 *
 *  Dipakai oleh thread_func_generic() agar satu fungsi bisa
 *  digunakan oleh semua thread — nama diambil dari thread_name().
 *
 *  Fields:
 *    color     : warna teks VGA (lihat konstanta di vga.h)
 *    max_ticks : jumlah tick sebelum thread exit (0 = infinite)
 * ================================================================ */
struct thread_args {
  uint8_t color;
  uint32_t max_ticks;
};

/*
 * Thread Control Block (TCB) — struct thread
 * -------------------------------------------
 * Setiap thread memiliki satu halaman 4 KB. Struct ini ada di
 * BAGIAN BAWAH halaman (offset 0). Stack tumbuh DARI ATAS ke bawah.
 *
 *   4 KB +-----------------------------+
 *        |       kernel stack          |
 *        |           ↓ grows down      |
 *        |                             |
 *        |   [switch_threads_frame]    | ← thread->stack
 *        |   [switch_entry_frame  ]    |
 *        |   [kernel_thread_frame ]    |
 *        |                             |
 *        +-----------------------------+
 *        |       struct thread         | ← thread (page boundary)
 *   0 KB +-----------------------------+
 *
 * running_thread() mendapat pointer thread saat ini hanya dari ESP:
 *   current = (struct thread *)(esp & ~(PGSIZE - 1))
 */
struct thread {
  tid_t tid;                 /* Nomor unik thread */
  enum thread_status status; /* Status saat ini */
  char name[16];             /* Nama thread (untuk debug) */
  uint8_t *stack;            /* Saved ESP (diperbarui saat context switch) */
  int priority;              /* Prioritas (Minggu 6: priority scheduling) */
  unsigned magic;            /* Harus == THREAD_MAGIC; deteksi overflow */
};

/* Tipe fungsi yang dijalankan thread */
typedef void thread_func(void *aux);

/* ================================================================
 * FUNGSI YANG SUDAH DISEDIAKAN ASISTEN — JANGAN diubah
 * ================================================================ */

/* Inisialisasi sistem thread — panggil SEKALI di kernel_main */
void thread_init(void);

/* Kembalikan pointer ke thread yang sedang berjalan */
struct thread *thread_current(void);
struct thread *running_thread(void);

/* Nama dan TID thread yang sedang berjalan */
const char *thread_name(void);
tid_t thread_tid(void);

/* Hentikan thread saat ini, beralih ke thread lain */
void thread_exit(void);

/* Blokir thread saat ini (dipakai oleh synch.c Minggu 5) */
void thread_block(void);

/* Pindahkan thread yang sedang blocked ke state READY */
void thread_unblock(struct thread *t);

/* Getter/setter prioritas */
int thread_get_priority(void);
void thread_set_priority(int new_priority);

/* Dipanggil setelah context switch selesai */
void thread_schedule_tail(struct thread *prev);

/* ================================================================
 * TUGAS MAHASISWA MINGGU 4 — implementasikan di thread.c
 * ================================================================
 *
 * thread_create()
 * ---------------
 * Buat thread baru dengan nama, prioritas, fungsi, dan argumen.
 * Kembalikan TID thread baru, atau TID_ERROR jika gagal.
 *
 * Langkah:
 *   1. Alokasi page baru: struct thread *t = allocate_thread_page()
 *   2. Inisialisasi: init_thread(t, name, priority)
 *   3. Beri TID: t->tid = allocate_tid()
 *   4. Setup 3 stack frame (lihat petunjuk di thread.c)
 *   5. Masukkan ke ready list: thread_unblock(t)
 *   6. return t->tid
 *
 * thread_yield()
 * ---------------
 * Thread yang sedang berjalan secara sukarela melepas CPU.
 * Mahasiswa menentukan mekanisme Round-Robin di sini.
 *
 * Langkah:
 *   1. cur = thread_current()
 *   2. cur->status = THREAD_READY
 *   3. Masukkan cur ke ready queue
 *   4. schedule()
 *
 * next_thread_to_run()
 * ---------------------
 * Kembalikan thread berikutnya yang akan dijadwalkan.
 * Minggu 4: Round-Robin (FIFO dari ready queue)
 * Minggu 6: modifikasi untuk Priority Scheduling
 * ================================================================
 */
tid_t thread_create(const char *name, int priority, thread_func *function,
                    void *aux);
void thread_yield(void);
struct thread *next_thread_to_run(void);

/* ================================================================
 * FUNGSI ASSEMBLY (switch.asm) — JANGAN diubah
 * ================================================================ */

/* Lakukan context switch dari cur ke next, kembalikan prev */
struct thread *switch_threads(struct thread *cur, struct thread *next);

/* Titik masuk pertama untuk thread baru (dipanggil dari switch.asm) */
void switch_entry(void);

/* Offset field 'stack' dalam struct thread — dipakai switch.asm */
extern uint32_t thread_stack_ofs;

#endif /* THREAD_H */
