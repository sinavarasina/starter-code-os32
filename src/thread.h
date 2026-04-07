/*
 * thread.h - Definisi Thread Control Block (TCB) dan prototipe fungsi thread
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * Kalian hanya perlu mengimplementasikan fungsi-fungsi yang
 * dideklarasikan di sini, di dalam file thread.c.
 */
#ifndef THREAD_H
#define THREAD_H

#include "types.h"

#define MAX_THREADS  8
#define STACK_SIZE   4096   /* 4 KB per thread */

typedef enum {
    THREAD_READY   = 0,
    THREAD_RUNNING = 1,
    THREAD_DEAD    = 2,
} thread_state_t;

/*
 * Thread Control Block (TCB)
 * --------------------------
 * Setiap thread memiliki satu TCB yang menyimpan:
 *   - id            : nomor unik thread
 *   - state         : kondisi thread saat ini
 *   - stack_pointer : nilai ESP yang disimpan saat thread di-suspend
 *   - stack[]       : memori stack milik thread ini (4 KB)
 *   - name          : nama thread untuk keperluan debug
 */
typedef struct {
    uint32_t        id;
    thread_state_t  state;
    uint32_t        stack_pointer;
    uint8_t         stack[STACK_SIZE];
    char            name[32];
} tcb_t;

/*
 * ================================================================
 * TUGAS MAHASISWA – implementasikan kedua fungsi ini di thread.c
 * ================================================================
 *
 * thread_create()
 * ---------------
 * Inisialisasi TCB dan bentuk stack awal agar saat
 * context_switch() pertama kali dijalankan, eksekusi langsung
 * melompat ke fungsi 'func'.
 *
 * Langkah:
 *   1. Isi field TCB: id, name, state = THREAD_READY
 *   2. Hitung puncak stack:
 *        uint32_t *sp = (uint32_t *)(tcb->stack + STACK_SIZE);
 *   3. Susun stack frame awal (push dari atas ke bawah):
 *        *(--sp) = (uint32_t)func;  <- "return address" untuk ret
 *        *(--sp) = 0;               <- ebp (nilai dummy)
 *        *(--sp) = 0;               <- ebx (nilai dummy)
 *        *(--sp) = 0;               <- esi (nilai dummy)
 *        *(--sp) = 0;               <- edi (nilai dummy)
 *   4. Simpan ESP baru: tcb->stack_pointer = (uint32_t)sp;
 *   5. return 0;
 *
 * thread_yield()
 * ---------------
 * Serahkan CPU ke thread berikutnya dalam antrian scheduler.
 *
 * Langkah:
 *   1. Ambil thread berikutnya: tcb_t *next = scheduler_next();
 *   2. Simpan pointer thread sekarang: tcb_t *prev = current_thread;
 *   3. Perbarui current_thread = next  (lakukan SEBELUM switch)
 *   4. Update state: prev->state = THREAD_READY
 *                    next->state = THREAD_RUNNING
 *   5. Panggil context_switch(&prev->stack_pointer, next->stack_pointer)
 * ================================================================
 */
int  thread_create(tcb_t *tcb, uint32_t id, const char *name, void (*func)(void));
void thread_yield(void);

/* Disediakan di context_switch.asm – JANGAN diubah */
void context_switch(uint32_t *old_esp, uint32_t new_esp);

#endif /* THREAD_H */
