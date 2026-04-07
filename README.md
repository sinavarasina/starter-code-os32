# Toy OS 32-bit — Starter Code Praktikum Sistem Operasi

Basis kode OS 32-bit berbasis x86 yang berjalan di QEMU menggunakan protokol **Multiboot (GRUB)**.
Digunakan pada **Minggu 4: Framework & Threads**.

---

## Prasyarat

Semua langkah dilakukan di dalam **Ubuntu (VirtualBox)**. Install dependensi berikut:

```bash
sudo apt update
sudo apt install -y build-essential gcc-multilib nasm \
                    qemu-system-x86 grub-pc-bin xorriso
```

---

## Struktur Direktori

```
starter-code/
├── Makefile                     # Build system (JANGAN DIUBAH)
├── linker.ld                    # Linker script (JANGAN DIUBAH)
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg         # Konfigurasi GRUB (JANGAN DIUBAH)
└── src/
    ├── boot.asm                 # Multiboot entry point (JANGAN DIUBAH)
    ├── context_switch.asm       # Context switch assembly (JANGAN DIUBAH)
    ├── types.h                  # Tipe dasar uint32_t dll (JANGAN DIUBAH)
    ├── vga.h / vga.c            # Driver teks VGA 80x25 (JANGAN DIUBAH)
    ├── kernel.c                 # Entry point kernel (JANGAN DIUBAH)
    ├── thread.h                 # Definisi TCB + prototipe (JANGAN DIUBAH)
    ├── thread.c                 # ★ TUGAS MAHASISWA ★
    ├── scheduler.h              # Antarmuka scheduler (JANGAN DIUBAH)
    └── scheduler.c              # ★ TUGAS MAHASISWA ★
```

---

## Cara Penggunaan

### 1. Compile & buat ISO
```bash
make
```

### 2. Jalankan di QEMU
```bash
make run
```

### 3. Bersihkan build
```bash
make clean
```

---

## Tugas Mahasiswa

Implementasikan fungsi-fungsi yang ditandai `/* TODO */` di dua file berikut:

### `src/thread.c`

| Fungsi | Deskripsi |
|--------|-----------|
| `thread_create()` | Inisialisasi TCB dan siapkan stack awal |
| `thread_yield()` | Serahkan CPU ke thread berikutnya |

### `src/scheduler.c`

| Fungsi | Deskripsi |
|--------|-----------|
| `scheduler_init()` | Inisialisasi antrian scheduler |
| `scheduler_add()` | Tambahkan thread ke antrian |
| `scheduler_next()` | Kembalikan thread berikutnya (Round-Robin) |
| `scheduler_run()` | Bootstrap: mulai jalankan thread pertama |

---

## Output yang Diharapkan

### Sebelum diimplementasikan (state awal)

```
===================================
  Toy OS 32-bit  |  Praktikum SisOP
===================================
Inisialisasi scheduler...
Membuat thread...
Menjalankan scheduler...
(Jika layar berhenti di sini, implementasi kalian belum benar)

ERROR: scheduler_run() kembali – ada yang salah!
```

Kernel tetap booting dan menampilkan pesan — ini **normal** karena
thread dan scheduler belum diisi.

### Setelah diimplementasikan (goal akhir)

```
===================================
  Toy OS 32-bit  |  Praktikum SisOP
===================================
Inisialisasi scheduler...
Membuat thread...
Menjalankan scheduler...

[Thread A] tick 0
[Thread B] tick 0
[Thread C] tick 0
[Thread A] tick 1
[Thread B] tick 1
[Thread C] tick 1
...
```

Tiga thread berjalan **selang-seling** tanpa preemption (cooperative multithreading).

---

## Petunjuk Implementasi

### `thread_create()` — Menyusun Stack Awal

Stack awal harus disusun seperti ini (dari alamat tinggi ke rendah):

```
+-------------------------+  ← tcb->stack + STACK_SIZE (puncak)
|  [func] ← ret address  |
|  [0]    ← ebp (dummy)  |
|  [0]    ← ebx (dummy)  |
|  [0]    ← esi (dummy)  |
|  [0]    ← edi (dummy)  |  ← tcb->stack_pointer menunjuk sini
+-------------------------+
```

Saat `context_switch` pertama kali dijalankan pada thread ini,
ia akan `pop edi, esi, ebx, ebp` lalu `ret` ke `func`.

### `context_switch()` (sudah disediakan)

```c
void context_switch(uint32_t *old_esp, uint32_t new_esp);
```

- Menyimpan `ebp, ebx, esi, edi` ke stack lama
- Menyimpan ESP ke `*old_esp`
- Mengganti ESP ke `new_esp`
- Me-restore register dari stack baru lalu `ret`

---

## Troubleshooting

| Masalah | Kemungkinan Penyebab |
|---------|----------------------|
| QEMU langsung mati / reboot | Kernel panic / triple fault — cek linker & boot.asm |
| Layar berhenti setelah "Menjalankan scheduler..." | `scheduler_run()` belum diimplementasi |
| Output acak / crash | Stack alignment salah di `thread_create()` |
| Thread tidak bergantian | `scheduler_next()` tidak memutar index |

---

## Referensi

- [OSDev Wiki – Bare Bones](https://wiki.osdev.org/Bare_Bones)
- [OSDev Wiki – Scheduling Algorithms](https://wiki.osdev.org/Scheduling_Algorithms)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [x86 Calling Convention](https://wiki.osdev.org/Calling_Conventions)
