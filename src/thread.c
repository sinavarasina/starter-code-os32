/*
 * thread.c - Manajemen Thread Kernel (Pintos Style)
 *
 * Diadaptasi dari Pintos src/threads/thread.c (Stanford)
 * untuk IF-OS Praktikum SisOP.
 *
 * ================================================================
 *  TUGAS MAHASISWA MINGGU 4
 *  Implementasikan tiga fungsi di bagian bawah file ini:
 *    1. thread_create()
 *    2. thread_yield()
 *    3. next_thread_to_run()
 *  Baca petunjuk di thread.h dan komentar di setiap fungsi.
 * ================================================================
 */
#include "thread.h"
#include "vga.h"
#include "types.h"

/* Forward declaration */
static void schedule(void);

/* ── Stack frame structs (internal) ──────────────────────── */

/* Frame paling ATAS stack: argumen untuk kernel_thread() */
struct kernel_thread_frame {
    void        *eip;       /* NULL — dummy return address */
    thread_func *function;  /* Fungsi yang akan dijalankan thread */
    void        *aux;       /* Argumen fungsi */
};

/* Frame tengah: return address menuju kernel_thread */
struct switch_entry_frame {
    void (*eip)(void);      /* Alamat kernel_thread */
};

/* Frame paling BAWAH stack: register callee-saved + return ke switch_entry */
struct switch_threads_frame {
    uint32_t   edi;         /* Callee-saved register (nilai dummy = 0) */
    uint32_t   esi;
    uint32_t   ebp;
    uint32_t   ebx;
    void      (*eip)(void); /* Alamat switch_entry */
    /* Dua word berikut adalah arg dummy cur & next — dibuang switch_entry */
    void      *cur_dummy;
    void      *next_dummy;
};

/* ── Variabel internal ───────────────────────────────────── */

/* Pool halaman 4 KB untuk thread-thread yang dibuat mahasiswa */
static uint8_t thread_pool[MAX_THREADS][PGSIZE] __attribute__((aligned(PGSIZE)));
static uint32_t thread_pool_idx = 0;

/* Antrian thread READY (circular array)
 * Diisi oleh thread_unblock(), dikuras oleh next_thread_to_run() */
static struct thread *ready_queue[MAX_THREADS * 2];
static uint32_t ready_head  = 0;
static uint32_t ready_tail  = 0;
static uint32_t ready_count = 0;

/* Thread pertama (kernel_main) */
static struct thread *initial_thread;

/* Penghitung TID */
static tid_t next_tid_counter = 1;

/* ── Offset field 'stack' — dipakai switch.asm ─────────
   Nilai ini dihitung saat compile dengan offsetof() */
uint32_t thread_stack_ofs = offsetof(struct thread, stack);

/* ── Fungsi internal ─────────────────────────────────────── */

static tid_t allocate_tid(void) {
    return next_tid_counter++;
}

/* Alokasi satu page dari pool untuk thread baru */
static struct thread *allocate_thread_page(void) {
    if (thread_pool_idx >= MAX_THREADS) return NULL;
    return (struct thread *)thread_pool[thread_pool_idx++];
}

/* Pindahkan stack pointer thread t ke bawah sebesar 'size' byte.
 * Kembalikan pointer ke area yang baru dialokasikan. */
static void *alloc_frame(struct thread *t, uint32_t size) {
    t->stack -= size;
    return (void *)t->stack;
}

/* Periksa apakah t adalah thread yang valid (magic number OK) */
static int is_thread(struct thread *t) {
    return t != NULL && t->magic == THREAD_MAGIC;
}

/* Inisialisasi struct thread dengan nilai awal */
static void init_thread(struct thread *t, const char *name, int priority) {
    /* Zero-fill struct thread */
    uint8_t *p = (uint8_t *)t;
    for (uint32_t i = 0; i < sizeof(struct thread); i++) p[i] = 0;

    t->status   = THREAD_BLOCKED;
    t->stack    = (uint8_t *)t + PGSIZE;  /* Puncak halaman */
    t->priority = priority;
    t->magic    = THREAD_MAGIC;

    /* Salin nama (maks 15 karakter) */
    uint32_t i = 0;
    while (i < 15 && name[i]) { t->name[i] = name[i]; i++; }
    t->name[i] = '\0';
}

/* Wrapper yang menjalankan fungsi thread, lalu membersihkan */
static void kernel_thread(thread_func *function, void *aux) {
    function(aux);
    thread_exit();
}

/* ── Fungsi PROVIDED (sudah jadi) ────────────────────────── */

/*
 * thread_init()
 * Inisialisasi sistem thread. Panggil sekali dari kernel_main
 * SEBELUM thread_create() manapun.
 *
 * Kernel_main sudah berjalan di initial_thread_page (boot.asm).
 * running_thread() akan menemukan struct thread-nya dari nilai ESP.
 */
void thread_init(void) {
    initial_thread = running_thread();
    init_thread(initial_thread, "main", PRI_DEFAULT);
    initial_thread->status = THREAD_RUNNING;
    initial_thread->tid    = allocate_tid();
}

/*
 * running_thread()
 * Kembalikan pointer ke thread yang sedang jalan
 * dengan membulatkan ESP ke page boundary.
 * (Ini berfungsi karena struct thread ada di BAWAH page-nya.)
 */
struct thread *running_thread(void) {
    uint32_t esp;
    __asm__ volatile ("mov %%esp, %0" : "=g" (esp));
    return (struct thread *)(void *)(esp & ~(uint32_t)(PGSIZE - 1));
}

/*
 * thread_current()
 * Sama dengan running_thread() + pemeriksaan keamanan.
 * Jika magic rusak → stack overflow terdeteksi.
 */
struct thread *thread_current(void) {
    struct thread *t = running_thread();
    if (!is_thread(t)) {
        vga_set_color(0x0C);
        vga_puts("\nKERNEL PANIC: stack overflow atau korupsi TCB!\n");
        while (1) __asm__ volatile ("hlt");
    }
    return t;
}

const char *thread_name(void) { return thread_current()->name; }
tid_t       thread_tid(void)  { return thread_current()->tid;  }

/*
 * thread_exit()
 * Hentikan thread yang sedang berjalan dan jadwalkan thread lain.
 */
void thread_exit(void) {
    thread_current()->status = THREAD_DYING;
    schedule();
    while (1) __asm__ volatile ("hlt");  /* Tidak seharusnya sampai sini */
}

/*
 * thread_block()
 * Blokir thread saat ini hingga di-unblock (untuk sinkronisasi Minggu 5).
 */
void thread_block(void) {
    thread_current()->status = THREAD_BLOCKED;
    schedule();
}

/*
 * thread_unblock()
 * Pindahkan thread t yang sedang BLOCKED ke state READY
 * dan masukkan ke antrian penjadwalan.
 */
void thread_unblock(struct thread *t) {
    t->status = THREAD_READY;
    if (ready_count < MAX_THREADS * 2) {
        ready_queue[ready_tail] = t;
        ready_tail  = (ready_tail + 1) % (MAX_THREADS * 2);
        ready_count++;
    }
}

int  thread_get_priority(void)           { return thread_current()->priority; }
void thread_set_priority(int new_priority){ thread_current()->priority = new_priority; }

/*
 * thread_schedule_tail()
 * Dipanggil oleh switch_entry (thread baru) dan schedule() (thread lama).
 * Tandai thread yang sekarang berjalan sebagai RUNNING.
 */
void thread_schedule_tail(struct thread *prev) {
    running_thread()->status = THREAD_RUNNING;
    (void)prev;
}

/*
 * schedule()
 * Fungsi internal: ambil thread berikutnya, lakukan context switch.
 */
static void schedule(void) {
    struct thread *cur  = running_thread();
    struct thread *next = next_thread_to_run();
    struct thread *prev = NULL;

    if (next == NULL) {
        vga_set_color(0x0C);
        vga_puts("\nKERNEL PANIC: next_thread_to_run() mengembalikan NULL!\n");
        while (1) __asm__ volatile ("hlt");
    }

    if (cur != next)
        prev = switch_threads(cur, next);

    thread_schedule_tail(prev);
}

/* ================================================================
 *  TUGAS MAHASISWA MINGGU 4
 * ================================================================ */

/*
 * thread_create()
 *
 * Buat thread baru yang akan menjalankan function(aux).
 * Kembalikan TID thread baru, atau TID_ERROR jika gagal.
 *
 * Petunjuk — setup 3 stack frame (dari atas ke bawah):
 *
 *  LANGKAH 1: Alokasi page dan inisialisasi TCB
 *    struct thread *t = allocate_thread_page();
 *    if (!t) return TID_ERROR;
 *    init_thread(t, name, priority);
 *    t->tid = allocate_tid();
 *
 *  LANGKAH 2: Frame 1 — kernel_thread_frame (paling atas, push pertama)
 *    struct kernel_thread_frame *kf = alloc_frame(t, sizeof *kf);
 *    kf->eip      = NULL;        <- dummy return address
 *    kf->function = function;    <- fungsi yang akan dijalankan
 *    kf->aux      = aux;         <- argumennya
 *
 *  LANGKAH 3: Frame 2 — switch_entry_frame
 *    struct switch_entry_frame *ef = alloc_frame(t, sizeof *ef);
 *    ef->eip = (void (*)(void))kernel_thread;
 *
 *  LANGKAH 4: Frame 3 — switch_threads_frame (paling bawah, push terakhir)
 *    struct switch_threads_frame *sf = alloc_frame(t, sizeof *sf);
 *    sf->eip       = switch_entry;   <- ret pertama dari switch_threads
 *    sf->ebp = sf->ebx = sf->esi = sf->edi = 0;
 *    sf->cur_dummy = sf->next_dummy  = NULL;
 *
 *  LANGKAH 5: Masukkan ke antrian
 *    thread_unblock(t);
 *    return t->tid;
 */
tid_t thread_create(const char *name, int priority,
                    thread_func *function, void *aux) {
    /* ── Implementasikan di sini ─────────────────────────── */
    (void)name; (void)priority; (void)function; (void)aux;
    return TID_ERROR;
}

/*
 * thread_yield()
 *
 * Thread yang sedang berjalan secara sukarela melepas CPU.
 * Thread ini kembali ke antrian READY dan thread lain dijadwalkan.
 *
 * Petunjuk:
 *   struct thread *cur = thread_current();
 *   cur->status = THREAD_READY;
 *   // masukkan cur ke ready_queue (gunakan ready_tail & ready_count)
 *   ready_queue[ready_tail] = cur;
 *   ready_tail  = (ready_tail + 1) % (MAX_THREADS * 2);
 *   ready_count++;
 *   schedule();
 */
void thread_yield(void) {
    /* ── Implementasikan di sini ─────────────────────────── */
}

/*
 * next_thread_to_run()
 *
 * Kembalikan thread berikutnya yang akan dijadwalkan.
 *
 * Minggu 4 — Round-Robin (FIFO):
 *   Ambil thread dari kepala antrian (ready_head).
 *   Jika antrian kosong, kembalikan initial_thread.
 *
 * Petunjuk:
 *   if (ready_count == 0) return initial_thread;
 *   struct thread *next = ready_queue[ready_head];
 *   ready_head  = (ready_head + 1) % (MAX_THREADS * 2);
 *   ready_count--;
 *   return next;
 *
 * Minggu 6 — Priority Scheduling:
 *   Cari thread dengan priority tertinggi dari antrian,
 *   keluarkan dari antrian, dan kembalikan.
 */
struct thread *next_thread_to_run(void) {
    /* ── Implementasikan di sini ─────────────────────────── */
    return initial_thread;   /* Stub: selalu kembalikan initial thread */
}
