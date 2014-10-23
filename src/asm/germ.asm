;------------------------------------------------------------------------------
; Name:
;   germ x64
;
; in:
;   edi-address to callback function
;
; Description:
;   Code which will scan for free space in memory and copy itself into it.
;
[section .text]

[BITS 64]

%include "germ.mac"

global _start

_start:
    nop
    
    ; prolog - stack frame creation
    push        rbp
    mov         rbp, rsp
    sub         rsp, 0x20
    ; stack frame:
    ;   - 0x8  start address
    ;   - 0x10 reg_cb address
    ;   - 0x18 copy address
    ;   - 0x20 debug_cb address

    call        delta
delta:
    pop         r15
    sub         r15, delta          ; our start address
    mov         [rbp-0x08], r15     ; store start address
    mov         [rbp-0x10], rdi     ; store reg_cb address
    mov         [rbp-0x20], rsi     ; store debug_cb address

    ; print out the begin execution message
    _print      0x8, exe_str, exe_str_len

    ; carry out the search for free space to replicate into
    call        search

    debug

    ; if no free space found we exit out without the copy
    test        rax, rax
    jz          complete

    ; store the copy dest address
    mov         [rbp-0x18], rax

    ; carry out the copy of this germ
    mov         rdi, [rbp-0x18]     ; copy dest address
    mov         rsi, [rbp-0x08]     ; germ start address
    mov         rcx, germ_len       ; copy length
    rep         movsb               ; copy those bytes

    ; print copy message
    _print      0x8, cpy_str, cpy_str_len

    ; make call to the registration cb
    xor         rsi, rsi
    xor         rdx, rdx
    mov         rdi, [rbp-0x18]     ; arg0: replicant address
    mov         rsi, germ_len
;    _get_var    0x08, germ_len, si  ; arg1: length
    _get_var    0x08, gen, dl       ; arg2: generation
    call        [rbp-0x10]          ; reg_cb address

complete:
    ; print completion message
    _print      0x8, comp_str, comp_str_len

exit:
    ; epilog - stack frame cleanup
    mov         rsp, rbp
    pop         rbp
    xor         rax, rax
    ret

;------------------------------------------------------------------------------
; print:
;   use the linux write syscall to output text string to stdout
; in:
;   rcx-address of string
;   rdx-string length
; returns:
;   rax-#bytes written
;
print:
    xor         rax, rax
    mov         al,  0x1            ; sys_write
    mov         rdi, 0x1            ; stdout fd
    syscall
    ret

;------------------------------------------------------------------------------
; search:
;   look for enough free space to replicate into.  We can only search as far
;   as our 'reach' will allow.
; in:
; returns:
;   rax-address found
;
search:
    xor         rax, rax
    xor         rdi, rdi
    mov         rdi, [rbp-0x8]      ; start address
    add         rdi, germ_len       ; move pointer to beyond end
;    _get_var    0x8, germ_len, di
    xor         rcx, rcx
    _get_var    0x8, reach, cx
.find_null:
    repne       scasw               ; repeat scasw as long as [rdi] is not NULL (rax)

    ; exit if we out of reach (rcx zero)
    test        rcx, rcx
    jz          .fail

    ; hold the copy dest address
    mov         r12, rdi

    ; hold the remaining reach
    mov         r13, rcx

.find_space:
    ; find enough free space for replication
    xor         rcx, rcx
    _get_var    0x8, reach, cx
    repe        scasw               ; repeat scasw as long as [rdi] is NULL (rax)

    ; if rcx is not zero we do not have enough space
    test        rcx, rcx
    mov         rcx, r13            ; remaining reach
    jnz         .find_null          ; restart search

    ; return the copy dest address
    mov         rax, r12
    jmp         .exit

.fail:
    xor         rax, rax
.exit:
    ret

;------------------------------------------------------------------------------
; fail:
;
; in:
; returns:
;
failure:
    ; print out failure message
    _print      0x8, fail_str, fail_str_len

    mov         rbx, 0xDEADBEEF
    mov         rax, 0x1

    pop         r15                 ; get rid of previous return address
    jmp         exit


;------------------------------------------------------------------------------
; debug string
;
exe_str:        db "[x] germ executing",10,0
exe_str_len:    equ $-exe_str

cpy_str:        db "[x] copied",10,0
cpy_str_len:    equ $-cpy_str

comp_str:       db "[x] complete",10,0
comp_str_len:   equ $-comp_str

fail_str:       db "[x] failure",10,0
fail_str_len:   equ $-fail_str

;------------------------------------------------------------------------------
; germ info
;
stack_sz:       db 020h
reach:          dw 0FFh             ; how many bytes we can search
gen:            db  01h             ; generation
life:           db  05h

germ_len:       equ end-_start       ; size of germ
end:
