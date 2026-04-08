; switch.asm - Context Switch antar Thread (Pintos Style, NASM)
;
; Diadaptasi dari Pintos src/threads/switch.S (Stanford)
; untuk IF-OS Praktikum SisOP.
;
; File ini sudah disiapkan oleh asisten praktikum.
; JANGAN diubah.
;
; Referensi Pintos:
;   struct thread *switch_threads(struct thread *cur, struct thread *next);
;
; Cara kerja switch_threads:
;   1. Push register callee-saved ke stack CUR (ebx, ebp, esi, edi).
;   2. Simpan ESP ke cur->stack  (field 'stack' di struct thread).
;   3. Ganti ESP ke next->stack  (switch ke stack thread NEXT).
;   4. Pop register callee-saved dari stack NEXT.
;   5. ret → lompat ke alamat yang ada di puncak stack NEXT:
;        - Thread BARU   : switch_entry (setup pertama kali)
;        - Thread LAMA   : kembali ke schedule() yang memanggilnya
;
; Layout stack setelah 4x push di dalam switch_threads:
;   [esp +  0] = edi (tersimpan)
;   [esp +  4] = esi
;   [esp +  8] = ebp
;   [esp + 12] = ebx
;   [esp + 16] = return address (ke schedule)
;   [esp + 20] = arg cur    (SWITCH_CUR)
;   [esp + 24] = arg next   (SWITCH_NEXT)

SWITCH_CUR  equ 20
SWITCH_NEXT equ 24

global switch_threads
extern thread_stack_ofs
extern thread_schedule_tail

; ── switch_threads ─────────────────────────────────────────
switch_threads:
    ; Simpan callee-saved registers thread CUR
    push ebx
    push ebp
    push esi
    push edi

    ; edx = offset field 'stack' di dalam struct thread
    mov  edx, [thread_stack_ofs]

    ; Simpan ESP thread CUR ke cur->stack
    mov  eax, [esp + SWITCH_CUR]     ; eax = cur
    mov  [eax + edx], esp            ; cur->stack = esp

    ; Ganti ESP ke stack thread NEXT
    mov  ecx, [esp + SWITCH_NEXT]    ; ecx = next
    mov  esp, [ecx + edx]            ; esp = next->stack

    ; Pulihkan callee-saved registers thread NEXT
    pop  edi
    pop  esi
    pop  ebp
    pop  ebx

    ret          ; eax masih berisi 'cur' lama = nilai kembalian switch_threads

; ── switch_entry ───────────────────────────────────────────
; Titik masuk PERTAMA untuk thread yang baru dibuat.
; Dipanggil dari switch_threads saat thread baru pertama kali dijadwalkan.
;
; Saat switch_entry dipanggil:
;   eax = thread sebelumnya (prev) — akan diteruskan ke thread_schedule_tail
;   Stack (dari bawah ke atas):
;     [esp +  0] = sf->cur  (dummy, dibuang)
;     [esp +  4] = sf->next (dummy, dibuang)
;     [esp +  8] = ef->eip  = kernel_thread
;     [esp + 12] = kf->eip  = NULL (return addr dummy)
;     [esp + 16] = kf->function
;     [esp + 20] = kf->aux

global switch_entry

switch_entry:
    add  esp, 8          ; buang sf->cur dan sf->next (arg dummy)
    push eax             ; push prev → arg untuk thread_schedule_tail(prev)
    call thread_schedule_tail
    add  esp, 4          ; bersihkan arg
    ret                  ; lompat ke kernel_thread

; ── Mark stack as non-executable ───────────────────────────
; Menghilangkan warning linker:
;   "missing .note.GNU-stack section implies executable stack"
section .note.GNU-stack noalloc noexec nowrite progbits
