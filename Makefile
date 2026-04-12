# =============================================================
#  Makefile - IF-OS 32-bit
#  Praktikum Sistem Operasi
#
#  File ini sudah disiapkan oleh asisten praktikum.
#  JANGAN diubah.
#
#  Perintah:
#    make        → compile + buat ISO
#    make run    → jalankan di QEMU
#    make clean  → hapus hasil build
# =============================================================

ASM := nasm

# ── Flag demo (kosong = Minggu 4 Round-Robin) ────────────────
# Contoh penggunaan:
#   make DEMO=-DSYNCHRONIZATION
#   make DEMO=-DPRIORITY_SCHEDULING
DEMO ?=

# ── Deteksi arsitektur host ──────────────────────────────────
# Di dalam Docker: IFOS_ARCH di-set oleh Dockerfile.
# Di luar Docker (Ubuntu native): pakai uname -m.
ARCH := $(shell echo $${IFOS_ARCH:-$(shell uname -m)})

ifeq ($(ARCH),arm64)
    CC := i686-linux-gnu-gcc
    LD := i686-linux-gnu-ld
    CFLAGS := -ffreestanding -fno-stack-protector -nostdlib -nostdinc \
              -fno-builtin -fno-pic -Wall -Wextra -std=c99 -O0 -Isrc $(DEMO)
else
    CC := gcc
    LD := ld
    CFLAGS := -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc \
              -fno-builtin -fno-pic -Wall -Wextra -std=c99 -O0 -Isrc $(DEMO)
endif

ASMFLAGS := -f elf32
LDFLAGS  := -m elf_i386 -T linker.ld

SRC_DIR   := src
BUILD_DIR := build
ISO_DIR   := iso

OBJS := $(BUILD_DIR)/boot.o     \
        $(BUILD_DIR)/switch.o   \
        $(BUILD_DIR)/vga.o      \
        $(BUILD_DIR)/thread.o   \
        $(BUILD_DIR)/synch.o    \
        $(BUILD_DIR)/kernel.o

KERNEL := $(BUILD_DIR)/kernel.elf
ISO    := $(BUILD_DIR)/os.iso

.PHONY: all run clean format

all: $(ISO)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	@echo ""
	@echo ">>> Kernel berhasil di-link: $@  [arch=$(ARCH)]"
	@echo ""

$(ISO): $(KERNEL)
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o $@ $(ISO_DIR) 2>/dev/null
	@echo ">>> ISO siap: $@"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(ISO)
	@echo ">>> Menjalankan QEMU (tekan q untuk keluar)..."
	qemu-system-i386 -cdrom $(ISO) -m 32M -display curses 2>/dev/null

# Alternatif: jalankan di VNC (connect ke localhost:5900 dengan VNC viewer)
run-vnc: $(ISO)
	@echo ">>> Menjalankan QEMU (VNC di port 5900)..."
	qemu-system-i386 -cdrom $(ISO) -m 32M -vnc :0

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(ISO_DIR)/boot/kernel.elf
	@echo ">>> Build directory dihapus."

format:
	@echo ">>> Memformat source code..."
	clang-format -i -style=file src/*.c src/*.h
	@echo ">>> Selesai!"
