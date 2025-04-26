fib:
    mov     r0, 0x00    ; a = 0
    mov     r1, 0x01    ; b = 1
    copy    r2, r1      ; s = b
    mov     r3, 0x02    ; i = 2
    cmpi    r11, 0x00   ; if (n != 0)
    jneq    .Loop       ; goto .Loop
    ret                 ; r11 = 0 -> return 0
.Loop:
    cmp     r3, r11     ; if (i <= n)
    jge     .Leave      ; goto .Leave
    add     r2, r0, r1  ; s = a + b
    copy    r0, r1      ; a = b
    copy    r1, r2      ; b = s
    inc     r3          ; i += 1
    jmp     .Loop       ; goto .Loop
.Leave:
    ; return s
    copy     r11, r2     
    ret                 