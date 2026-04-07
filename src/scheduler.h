/*
 * scheduler.h - Antarmuka Round-Robin Scheduler
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * Implementasikan semua fungsi yang dideklarasikan di sini
 * di dalam file scheduler.c.
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread.h"

/*
 * ================================================================
 * TUGAS MAHASISWA – implementasikan semua fungsi ini di scheduler.c
 * ================================================================
 *
 * scheduler_init()
 *   Inisialisasi struktur data antrian.
 *   Setel count = 0 dan current_idx = 0.
 *
 * scheduler_add(tcb)
 *   Tambahkan pointer TCB ke antrian.
 *   Contoh: queue[count++] = tcb;
 *
 * scheduler_next()
 *   Kembalikan thread berikutnya secara Round-Robin.
 *   Maju current_idx secara melingkar:
 *     current_idx = (current_idx + 1) % count;
 *     return queue[current_idx];
 *
 * scheduler_run()
 *   Bootstrap scheduler – hanya dipanggil SEKALI dari kernel_main.
 *   Fungsi ini tidak boleh return.
 *   Cara kerja:
 *     1. Set current_thread = queue[0], state = THREAD_RUNNING
 *     2. "Lompat" ke thread pertama:
 *          uint32_t dummy;
 *          context_switch(&dummy, current_thread->stack_pointer);
 * ================================================================
 */
void   scheduler_init(void);
void   scheduler_add(tcb_t *tcb);
tcb_t *scheduler_next(void);
void   scheduler_run(void);

/* Pointer ke thread yang sedang aktif berjalan.
 * Wajib memperbarui variabel ini setiap kali terjadi
 * pergantian thread. */
extern tcb_t *current_thread;

#endif /* SCHEDULER_H */
