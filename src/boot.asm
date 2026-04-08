; boot.asm - Multiboot header + kernel entry point
;
; File ini sudah disiapkan oleh asisten praktikum.
; JANGAN diubah.
;
; PENTING: initial_thread_page HARUS page-aligned (align 4096).
; Struct thread untuk kernel_main ada di BAGIAN BAWAH halaman ini.
; Stack kernel_main ada di BAGIAN ATAS halaman ini.
; Ini memungkinkan running_thread() menemukan struct thread
; hanya dari nilai ESP:
;   current_thread = (struct thread *)(esp & ~(PGSIZE - 1))

MBOOT_MAGIC  equ 0x1BADB002
MBOOT_FLAGS  equ 0x3
MBOOT_CKSUM  equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CKSUM

section .bss
; KRITIS: harus page-aligned agar running_thread() berfungsi.
; struct thread ada di offset 0 dari halaman ini.
; Stack kernel_main tumbuh dari atas halaman ini ke bawah.
align 4096
global initial_thread_page
initial_thread_page:
    resb 4096          ; Satu page: struct thread (bawah) + stack (atas)

section .text
global _start
extern kernel_main

_start:
    ; ESP = puncak halaman initial_thread_page
    ; (stack tumbuh ke bawah, tidak boleh menimpa struct thread di offset 0)
    mov  esp, initial_thread_page + 4096

    push 0             ; reset EFLAGS
    popf

    push ebx           ; arg2: pointer Multiboot info
    push eax           ; arg1: Multiboot magic number
    call kernel_main

    ; kernel_main tidak seharusnya return
    cli
.halt:
    hlt
    jmp .halt

; Mark stack as non-executable (suppress linker warning)
section .note.GNU-stack noalloc noexec nowrite progbits
