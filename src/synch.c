/*
 * synch.c - Implementasi Sinkronisasi: Semaphore, Lock, Condition Variable
 *
 * ================================================================
 *  TUGAS MAHASISWA MINGGU 5
 *  Implementasikan fungsi yang ditandai TODO di bawah ini.
 *  Baca petunjuk di synch.h sebelum mulai.
 * ================================================================
 */
#include "synch.h"
#include "thread.h"

/* ── Semaphore ───────────────────────────────────────────── */

/*
 * sema_init() — Sudah disediakan, JANGAN diubah.
 */
void sema_init(struct semaphore *sema, unsigned value) {
    sema->value        = value;
    sema->waiter_count = 0;
}

/*
 * sema_down() — Operasi P()
 *
 * Kurangi nilai semaphore. Jika nilai sudah 0, thread saat ini
 * diblokir hingga sema_up() dipanggil.
 *
 * Petunjuk:
 *   while (sema->value == 0) {
 *       // Tambah thread_current() ke sema->waiters
 *       sema->waiters[sema->waiter_count++] = thread_current();
 *       thread_block();  // <- thread tidur di sini
 *   }
 *   sema->value--;
 */
void sema_down(struct semaphore *sema) {
    /* ── Implementasikan di sini ─────────────────────────── */
    (void)sema;
}

/*
 * sema_up() — Operasi V()
 *
 * Tambah nilai semaphore. Jika ada thread yang menunggu,
 * bangunkan satu di antaranya.
 *
 * Petunjuk:
 *   sema->value++;
 *   if (sema->waiter_count > 0) {
 *       struct thread *t = sema->waiters[0];
 *       // geser sisa waiters ke kiri
 *       for (uint32_t i = 0; i < sema->waiter_count - 1; i++)
 *           sema->waiters[i] = sema->waiters[i + 1];
 *       sema->waiter_count--;
 *       thread_unblock(t);
 *   }
 */
void sema_up(struct semaphore *sema) {
    /* ── Implementasikan di sini ─────────────────────────── */
    (void)sema;
}

/* ── Lock ────────────────────────────────────────────────── */

/*
 * lock_init() — Sudah disediakan, JANGAN diubah.
 */
void lock_init(struct lock *lock) {
    lock->holder = NULL;
    sema_init(&lock->semaphore, 1);  /* Binary semaphore: value 1 = bebas */
}

/*
 * lock_acquire()
 *
 * Ambil lock. Jika lock sedang dipegang thread lain, blokir sampai bebas.
 *
 * Petunjuk:
 *   sema_down(&lock->semaphore);
 *   lock->holder = thread_current();
 */
void lock_acquire(struct lock *lock) {
    /* ── Implementasikan di sini ─────────────────────────── */
    (void)lock;
}

/*
 * lock_release()
 *
 * Lepaskan lock yang sedang dipegang. Hanya boleh dipanggil
 * oleh thread yang memegang lock.
 *
 * Petunjuk:
 *   lock->holder = NULL;
 *   sema_up(&lock->semaphore);
 */
void lock_release(struct lock *lock) {
    /* ── Implementasikan di sini ─────────────────────────── */
    (void)lock;
}

/*
 * lock_held_by_current_thread() — Sudah disediakan, JANGAN diubah.
 */
int lock_held_by_current_thread(const struct lock *lock) {
    return lock->holder == thread_current();
}

/* ── Condition Variable ──────────────────────────────────── */

/*
 * cond_init() — Sudah disediakan, JANGAN diubah.
 */
void cond_init(struct condition *cond) {
    cond->waiter_count = 0;
}

/*
 * cond_wait() — BONUS Minggu 5
 *
 * Tunggu kondisi terpenuhi. Lock harus dipegang saat dipanggil.
 *
 * Petunjuk:
 *   // Buat semaphore lokal dengan value 0 untuk thread ini
 *   struct semaphore waiter_sema;
 *   sema_init(&waiter_sema, 0);
 *   cond->waiters[cond->waiter_count++] = &waiter_sema;
 *   lock_release(lock);
 *   sema_down(&waiter_sema);  // tidur sampai di-signal
 *   lock_acquire(lock);
 */
void cond_wait(struct condition *cond, struct lock *lock) {
    /* ── Implementasikan di sini (BONUS) ────────────────── */
    (void)cond; (void)lock;
}

/*
 * cond_signal() — BONUS Minggu 5
 *
 * Bangunkan SATU thread yang menunggu kondisi ini.
 *
 * Petunjuk:
 *   if (cond->waiter_count > 0) {
 *       sema_up(cond->waiters[0]);
 *       // geser sisa waiters
 *       cond->waiter_count--;
 *   }
 */
void cond_signal(struct condition *cond, struct lock *lock) {
    /* ── Implementasikan di sini (BONUS) ────────────────── */
    (void)cond; (void)lock;
}

/*
 * cond_broadcast() — BONUS Minggu 5
 *
 * Bangunkan SEMUA thread yang menunggu kondisi ini.
 *
 * Petunjuk:
 *   while (cond->waiter_count > 0)
 *       cond_signal(cond, lock);
 */
void cond_broadcast(struct condition *cond, struct lock *lock) {
    /* ── Implementasikan di sini (BONUS) ────────────────── */
    (void)cond; (void)lock;
}
