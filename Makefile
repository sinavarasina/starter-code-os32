# =============================================================
#  Makefile - Toy OS 32-bit
#  Praktikum Sistem Operasi
#
#  File ini sudah disiapkan oleh asisten praktikum.
#  JANGAN diubah.
#
#  Perintah yang tersedia:
#    make        → compile + buat ISO
#    make run    → jalankan di QEMU
#    make clean  → hapus hasil build
#
#  Prasyarat (jalankan sekali di Ubuntu):
#    sudo apt install build-essential gcc-multilib nasm \
#                     qemu-system-x86 grub-pc-bin xorriso
# =============================================================

ASM      := nasm
CC       := gcc
LD       := ld

ASMFLAGS := -f elf32
CFLAGS   := -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc \
            -fno-builtin -fno-pic -Wall -Wextra -std=c99 -O2 -Isrc
LDFLAGS  := -m elf_i386 -T linker.ld

SRC_DIR   := src
BUILD_DIR := build
ISO_DIR   := iso

OBJS := $(BUILD_DIR)/boot.o          \
        $(BUILD_DIR)/context_switch.o \
        $(BUILD_DIR)/vga.o            \
        $(BUILD_DIR)/thread.o         \
        $(BUILD_DIR)/scheduler.o      \
        $(BUILD_DIR)/kernel.o

KERNEL := $(BUILD_DIR)/kernel.elf
ISO    := $(BUILD_DIR)/os.iso

.PHONY: all run clean

all: $(ISO)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	@echo ""
	@echo ">>> Kernel berhasil di-link: $@"
	@echo ""

$(ISO): $(KERNEL)
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o $@ $(ISO_DIR) 2>/dev/null
	@echo ">>> ISO siap: $@"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(ISO)
	@echo ">>> Menjalankan QEMU..."
	qemu-system-i386 -cdrom $(ISO) -m 32M

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(ISO_DIR)/boot/kernel.elf
	@echo ">>> Build directory dihapus."
