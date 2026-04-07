/*
 * thread.c - Implementasi manajemen thread kernel
 *
 * ================================================================
 *  TUGAS MAHASISWA
 *  Implementasikan fungsi thread_create() dan thread_yield()
 *  di bawah ini. Baca petunjuk di thread.h sebelum mulai.
 * ================================================================
 */
#include "thread.h"
#include "scheduler.h"
#include "vga.h"

/* Salin string tanpa menggunakan <string.h> */
static void str_copy(char *dst, const char *src, uint32_t max) {
    uint32_t i = 0;
    while (i < max - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

/*
 * thread_create()
 *
 * Inisialisasi TCB dan bentuk stack awal thread.
 * Kembalikan 0 jika berhasil.
 *
 * Hapus baris (void)... dan return -1 setelah kalian
 * selesai mengimplementasikan fungsi ini.
 */
int thread_create(tcb_t *tcb, uint32_t id, const char *name, void (*func)(void)) {
    /* ── Implementasi kalian di sini ────────────────────────── */

    (void)tcb; (void)id; (void)name; (void)func; (void)str_copy;
    return -1;
}

/*
 * thread_yield()
 *
 * Serahkan CPU ke thread berikutnya.
 * Fungsi ini dipanggil dari dalam thread yang sedang berjalan.
 */
void thread_yield(void) {
    /* ── Implementasi kalian di sini ────────────────────────── */
}
