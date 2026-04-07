; context_switch.asm - Pergantian konteks antar thread (level Assembly)
;
; File ini sudah disiapkan oleh asisten praktikum.
; JANGAN diubah.
;
; Prototype C:
;   void context_switch(uint32_t *old_esp, uint32_t new_esp);
;
; Cara kerja:
;   1. Push register callee-saved (ebp, ebx, esi, edi) ke stack lama.
;   2. Simpan ESP saat ini ke *old_esp.
;   3. Ganti ESP ke nilai new_esp (stack thread baru).
;   4. Pop register callee-saved dari stack baru.
;   5. ret → eksekusi lompat ke thread baru.
;
; Tata letak stack saat context_switch dipanggil:
;   [esp+0 ] = return address
;   [esp+4 ] = old_esp  (pointer)
;   [esp+8 ] = new_esp  (nilai)
;
; Setelah 4x push, offset bergeser +16:
;   [esp+16] = return address
;   [esp+20] = old_esp
;   [esp+24] = new_esp

global context_switch

context_switch:
    push ebp
    push ebx
    push esi
    push edi

    mov  eax, [esp + 20]
    mov  [eax], esp

    mov  ecx, [esp + 24]
    mov  esp, ecx

    pop  edi
    pop  esi
    pop  ebx
    pop  ebp

    ret
