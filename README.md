# IF-OS — Starter Code Praktikum Sistem Operasi

OS 32-bit berbasis x86 yang berjalan di QEMU menggunakan protokol **Multiboot (GRUB)**.
Diadaptasi dari [Pintos (Stanford)](https://pintos-os.org) untuk praktikum IF-OS.

---

## Setup Environment

### Opsi A — Docker (Direkomendasikan untuk semua platform)

Install [Docker Desktop](https://www.docker.com/products/docker-desktop/), lalu:

```bash
chmod +x run-docker.sh
./run-docker.sh

# Di dalam container:
make          # compile + buat ISO
make run      # jalankan di QEMU
make clean    # hapus hasil build
```

> Docker mendukung Windows (Intel), macOS (Intel & M-series/ARM), dan Linux.

### Opsi B — Ubuntu Native (Intel/AMD)

```bash
sudo apt update
sudo apt install -y build-essential gcc-multilib nasm \
                    qemu-system-x86 grub-pc-bin xorriso
make
make run
```

---

## Struktur Direktori

```
starter-code/
├── Dockerfile              # Image Docker untuk build environment
├── run-docker.sh           # Script untuk masuk ke Docker
├── Makefile                # Build system (JANGAN DIUBAH)
├── linker.ld               # Linker script kernel (JANGAN DIUBAH)
├── iso/
│   └── boot/
│       ├── grub/
│       │   └── grub.cfg   # Konfigurasi GRUB (JANGAN DIUBAH)
│       └── kernel.elf      # Hasil build (di-generate otomatis)
├── src/                    # Source code starter
│   ├── boot.asm            # Multiboot entry point (JANGAN DIUBAH)
│   ├── switch.asm          # Context switch assembly (JANGAN DIUBAH)
│   ├── types.h             # Tipe dasar uint8_t, uint32_t, dll (JANGAN DIUBAH)
│   ├── vga.h / vga.c       # Driver teks VGA 80×25 (JANGAN DIUBAH)
│   ├── thread.h            # Definisi TCB + prototipe (JANGAN DIUBAH)
│   ├── synch.h             # Definisi Semaphore, Lock, Cond (JANGAN DIUBAH)
│   ├── scheduler.h         # Deprecated — lihat thread.h/thread.c
│   ├── kernel.c            # Entry point kernel (JANGAN DIUBAH)
│   ├── thread.c            # ★ TUGAS MAHASISWA Minggu 4 & 6 ★
│   └── synch.c             # ★ TUGAS MAHASISWA Minggu 5 ★
└── solutions/              # Golden solution (RAHASIA — tidak dibagikan ke mahasiswa)
    ├── week4/thread.c      # Solusi Minggu 4: threading
    ├── week5/synch.c       # Solusi Minggu 5: sinkronisasi
    └── week6/thread.c      # Solusi Minggu 6: priority scheduling
```

---

## Tugas per Minggu

### Minggu 4 — Thread (src/thread.c)

Implementasikan tiga fungsi berikut di `src/thread.c`:

| Fungsi | Deskripsi |
|--------|-----------|
| `thread_create(name, priority, function, aux)` | Alokasi TCB, setup 3-frame stack, masuk ke ready queue |
| `thread_yield()` | Thread aktif melepas CPU secara sukarela (Round-Robin) |
| `next_thread_to_run()` | Pilih thread berikutnya dari ready queue (FIFO Minggu 4) |

### Minggu 5 — Sinkronisasi (src/synch.c)

Implementasikan fungsi-fungsi berikut di `src/synch.c`:

| Fungsi | Deskripsi |
|--------|-----------|
| `sema_down(sema)` | P() — blokir thread jika value == 0 |
| `sema_up(sema)` | V() — naikkan value, bangunkan waiter |
| `lock_acquire(lock)` | Ambil mutex lock |
| `lock_release(lock)` | Lepaskan mutex lock |
| `cond_wait(cond, lock)` | Tunggu kondisi (BONUS) |
| `cond_signal(cond, lock)` | Bangunkan satu waiter (BONUS) |
| `cond_broadcast(cond, lock)` | Bangunkan semua waiter (BONUS) |

### Minggu 6 — Priority Scheduling (src/thread.c)

Modifikasi `next_thread_to_run()` agar memilih thread dengan **prioritas tertinggi**,
bukan sekadar FIFO. Fungsi lain tidak perlu diubah.

---

## Output yang Diharapkan

### Sebelum diimplementasikan (stub awal)

```
===================================
  IF-OS  |  Praktikum Sistem Operasi
===================================

Inisialisasi sistem thread...
Membuat thread A, B, C...
Memulai scheduling...
(Jika layar berhenti di sini, implementasi Minggu 4 belum selesai)

ERROR: kernel_main() kembali dari thread_block()!
```

Kernel tetap booting dan menampilkan pesan — ini **normal** sebelum tugas diimplementasikan.

### Setelah diimplementasikan (Minggu 4)

```
===================================
  IF-OS  |  Praktikum Sistem Operasi
===================================

Inisialisasi sistem thread...
Membuat thread A, B, C...
Memulai scheduling...

[Thread A] tick 0
[Thread B] tick 0
[Thread C] tick 0
[Thread A] tick 1
[Thread B] tick 1
[Thread C] tick 1
...
```

Tiga thread berjalan **selang-seling** (cooperative Round-Robin).

---

## Petunjuk Implementasi Minggu 4

### `thread_create()` — Setup 3 Stack Frame

Stack awal thread disusun dari **atas ke bawah** (push pertama = posisi paling atas):

```
high addr +-----------------------------+  ← t->stack awal (= t + PGSIZE)
          |  kf->eip  = NULL           |  ← kernel_thread_frame (push ke-1)
          |  kf->function              |
          |  kf->aux                   |
          |-----------------------------|
          |  ef->eip = kernel_thread   |  ← switch_entry_frame (push ke-2)
          |-----------------------------|
          |  sf->edi = 0               |  ← switch_threads_frame (push ke-3)
          |  sf->esi = 0               |
          |  sf->ebp = 0               |
          |  sf->ebx = 0               |
          |  sf->eip = switch_entry    |
          |  sf->cur_dummy  = NULL     |
          |  sf->next_dummy = NULL     |
low addr  +-----------------------------+  ← t->stack setelah setup
```

Saat pertama kali dijadwalkan, `switch_threads` me-pop register dan `ret` ke `switch_entry`,
yang kemudian memanggil `kernel_thread(function, aux)`.

### `thread_yield()` — Round-Robin

```
1. cur = thread_current()
2. cur->status = THREAD_READY
3. Masukkan cur ke ready_queue (gunakan ready_tail & ready_count)
4. schedule()     ← fungsi internal sudah disediakan
```

### `next_thread_to_run()` — FIFO

```
1. Jika ready_count == 0, return initial_thread
2. next = ready_queue[ready_head]
3. ready_head = (ready_head + 1) % (MAX_THREADS * 2)
4. ready_count--
5. return next
```

---

## Troubleshooting

| Masalah | Kemungkinan Penyebab |
|---------|----------------------|
| QEMU langsung reboot/blank | Triple fault — cek stack setup di `thread_create()` |
| Layar freeze setelah "Memulai scheduling..." | `next_thread_to_run()` atau `thread_yield()` belum benar |
| Hanya satu thread yang jalan | `thread_yield()` tidak memasukkan thread ke queue |
| Thread tidak bergantian | `next_thread_to_run()` tidak menggeser `ready_head` |
| `KERNEL PANIC: stack overflow` | `thread_create()` salah menghitung ukuran frame |
| Warning `unused-function` saat compile | Normal pada stub — hilang setelah fungsi diimplementasi |

---

## Referensi

- [Pintos Threads Documentation](https://www.cs.jhu.edu/~huang/cs318/fall21/project/pintos_2.html)
- [OSDev Wiki – Bare Bones](https://wiki.osdev.org/Bare_Bones)
- [OSDev Wiki – Scheduling Algorithms](https://wiki.osdev.org/Scheduling_Algorithms)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
