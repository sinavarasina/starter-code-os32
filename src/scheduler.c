/*
 * scheduler.c - Implementasi Round-Robin Scheduler
 *
 * ================================================================
 *  TUGAS KALIAN
 *  Implementasikan keempat fungsi scheduler di bawah ini.
 *  Baca petunjuk di scheduler.h sebelum mulai.
 * ================================================================
 */
#include "scheduler.h"
#include "thread.h"

/* Struktur data antrian – sudah disiapkan, tidak perlu diubah */
static tcb_t  *queue[MAX_THREADS];
static uint32_t count       = 0;
static uint32_t current_idx = 0;

/* Pointer ke thread yang sedang berjalan */
tcb_t *current_thread = (tcb_t *)0;

/*
 * scheduler_init()
 * Inisialisasi scheduler sebelum thread-thread dibuat.
 */
void scheduler_init(void) {
    /* ── Implementasi kalian di sini ────────────────────────── */
}

/*
 * scheduler_add()
 * Daftarkan satu thread ke antrian scheduler.
 */
void scheduler_add(tcb_t *tcb) {
    /* ── Implementasi kalian di sini ────────────────────────── */
    (void)tcb;
}

/*
 * scheduler_next()
 * Kembalikan thread berikutnya (Round-Robin).
 * Dipanggil oleh thread_yield().
 */
tcb_t *scheduler_next(void) {
    /* ── Implementasi kalian di sini ────────────────────────── */
    return (tcb_t *)0;
}

/*
 * scheduler_run()
 * Mulai menjalankan thread pertama. Tidak boleh return.
 */
void scheduler_run(void) {
    /* ── Implementasi kalian di sini ────────────────────────── */
}
