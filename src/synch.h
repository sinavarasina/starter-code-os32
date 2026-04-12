/*
 * synch.h - Sinkronisasi: Semaphore, Lock, Condition Variable
 *
 * Diadaptasi dari Pintos src/threads/synch.h (Stanford)
 * untuk IF-OS Praktikum SisOP — Minggu 5.
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * Implementasikan fungsi-fungsi stub di synch.c.
 */
#ifndef SYNCH_H
#define SYNCH_H

#include "thread.h"

/* ── Semaphore ───────────────────────────────────────────── */

/*
 * Semaphore — penghitung non-negatif dengan dua operasi atomik:
 *   sema_down() : P() — tunggu hingga value > 0, lalu kurangi
 *   sema_up()   : V() — tambah value, bangunkan waiter jika ada
 */
struct semaphore {
	unsigned value;			     /* Nilai saat ini (>= 0) */
	struct thread *waiters[MAX_THREADS]; /* Thread yang sedang menunggu */
	uint32_t waiter_count;		     /* Jumlah waiter */
};

void sema_init(struct semaphore *, unsigned value);

/*
 * TUGAS MAHASISWA MINGGU 5:
 *   sema_down() dan sema_up()
 */
void sema_down(struct semaphore *); /* P() — block jika value == 0 */
void sema_up(struct semaphore *);   /* V() — wake waiter, tambah value */

/* ── Lock (Mutex) ────────────────────────────────────────── */

/*
 * Lock — mutex sederhana, maksimal satu thread memegang lock.
 * Diimplementasikan di atas semaphore dengan value awal 1.
 */
struct lock {
	struct thread *holder;	    /* Thread yang memegang lock (atau NULL) */
	struct semaphore semaphore; /* Binary semaphore (value 0 atau 1) */
};

void lock_init(struct lock *);

/*
 * TUGAS MAHASISWA MINGGU 5:
 *   lock_acquire() dan lock_release()
 */
void lock_acquire(struct lock *);
void lock_release(struct lock *);

/* Kembalikan 1 jika thread saat ini memegang lock, 0 jika tidak */
int lock_held_by_current_thread(const struct lock *);

/* ── Condition Variable ──────────────────────────────────── */

/*
 * Condition Variable — tempat thread menunggu suatu kondisi terpenuhi.
 * Selalu digunakan bersama lock (monitor pattern).
 */
struct condition {
	struct semaphore *waiters[MAX_THREADS]; /* Semaphore per waiter */
	uint32_t waiter_count;
};

void cond_init(struct condition *);

/*
 * TUGAS MAHASISWA MINGGU 5 (bonus):
 *   cond_wait(), cond_signal(), cond_broadcast()
 */
void cond_wait(struct condition *, struct lock *);
void cond_signal(struct condition *, struct lock *);
void cond_broadcast(struct condition *, struct lock *);

#endif /* SYNCH_H */
