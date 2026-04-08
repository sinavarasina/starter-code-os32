/*
 * scheduler.h — DEPRECATED
 *
 * File ini tidak lagi digunakan dalam IF-OS (Pintos Style).
 * Fungsionalitas scheduler sudah dipindahkan ke thread.h/thread.c:
 *
 *   scheduler_init()  → thread_init()
 *   scheduler_add()   → thread_unblock()
 *   scheduler_next()  → next_thread_to_run()   [TUGAS MAHASISWA]
 *   scheduler_run()   → thread_yield() dari kernel_main
 *
 * File ini dibiarkan ada agar tidak membingungkan jika ada referensi
 * lama, tapi tidak dikompilasi oleh Makefile.
 */
