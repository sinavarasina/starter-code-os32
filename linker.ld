/* linker.ld - Linker script kernel 32-bit
 *
 * File ini sudah disiapkan oleh asisten praktikum.
 * JANGAN diubah.
 *
 * Kernel dimuat pada alamat fisik 0x100000 (1 MB)
 * sesuai spesifikasi Multiboot.
 */

OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    . = 0x100000;

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : ALIGN(4096) {
        *(.rodata)
    }

    .data : ALIGN(4096) {
        *(.data)
    }

    .bss : ALIGN(4096) {
        *(COMMON)
        *(.bss)
    }
}
