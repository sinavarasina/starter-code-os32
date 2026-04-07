; boot.asm - Multiboot header + kernel entry point
;
; File ini sudah disiapkan oleh asisten praktikum.
; JANGAN diubah.

MBOOT_MAGIC  equ 0x1BADB002
MBOOT_FLAGS  equ 0x3
MBOOT_CKSUM  equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CKSUM

section .bss
align 16
stack_bottom:
    resb 16384      ; 16 KB stack kernel
stack_top:

section .text
global _start
extern kernel_main

_start:
    mov  esp, stack_top

    push 0
    popf

    push ebx        ; pointer ke struktur Multiboot info
    push eax        ; magic number
    call kernel_main

    cli
.halt:
    hlt
    jmp .halt
