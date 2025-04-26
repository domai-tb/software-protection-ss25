fib_bytecode = [
# fib:
    # mov     r0, 0x00    ; a = 0
    0x10, 0x00, 0x00,
    # mov     r1, 0x01    ; b = 1
    0x10, 0x01, 0x01,
    # copy    r2, r1      ; s = b
    0x20, 0x02, 0x01,
    # cmpi    r11, 0x00   ; if (n != 0)
    0x41, 0x0B, 0x00,
    # jneq    .Loop       ; goto .Loop
    0x54, 0x0F,
    # ret                 ; r11 = 0 -> return 0
    0x99,
# .Loop: 
    # mov     r3, 0x02    ; i = 2
    0x10, 0x03, 0x02,
    # cmp     r3, r11     ; if (i <= n)
    0x40, 0x03, 0x0B,
    # jge     .Leave      ; goto .Leave
    0x53, 0x25,
    # add     r2, r0, r1  ; s = a + b
    0x30, 0x02, 0x00, 0x01,
    # copy    r0, r1      ; a = b
    0x20, 0x00, 0x01,
    # copy    r1, r2      ; b = s
    0x20, 0x1, 0x02,
    # inc     r3          ; i += 1
    0x31, 0x03,
    # jmp     .Loop       ; goto .Loop
    0x50, 0x0F,
# .Leave:
    # ; return s
    # mov     r11, r2     
    0x10, 0x0B, 0x02,
    # ret      
    0x99
]

def vm_disassembler(bytecode) -> None:
    ip = 0

    while ip < len(bytecode):
        opcode = bytecode[ip]

        if opcode == 16:
            print(f'{hex(ip)}: mov {bytecode[ip+1]}, {bytecode[ip+2]}')
            ip += 3
        elif opcode == 32:
            print(f'{hex(ip)}: copy {bytecode[ip+1]}, {bytecode[ip+2]}')
            ip += 3
        elif opcode == 48:
            print(f'{hex(ip)}: add {bytecode[ip+1]}, {bytecode[ip+2]}, {bytecode[ip+3]}')
            ip += 4
        elif opcode == 49:
            print(f'{hex(ip)}: inc {bytecode[ip+1]}')
            ip += 2
        elif opcode == 50:
            print(f'{hex(ip)}: dec {bytecode[ip+1]}')
            ip += 2
        elif opcode == 51:
            print(f'{hex(ip)}: sub {bytecode[ip+1]}, {bytecode[ip+2]}, {bytecode[ip+3]}')
            ip += 4
        elif opcode == 64:
            print(f'{hex(ip)}: cmp {bytecode[ip+1]}, {bytecode[ip+2]}')
            ip += 3
        elif opcode == 65:
            print(f'{hex(ip)}: cmpi {bytecode[ip+1]}, {bytecode[ip+2]}')
            ip += 3
        elif opcode == 80:
            print(f'{hex(ip)}: jmp {hex(bytecode[ip+1])}')
            ip += 2
        elif opcode == 81:
            print(f'{hex(ip)}: jeq {hex(bytecode[ip+1])}')
            ip += 2
        elif opcode == 82:
            print(f'{hex(ip)}: jle {hex(bytecode[ip+1])}')
            ip += 2
        elif opcode == 83:
            print(f'{hex(ip)}: jge {hex(bytecode[ip+1])}')
            ip += 2
        elif opcode == 84:
            print(f'{hex(ip)}: jneq {hex(bytecode[ip+1])}')
            ip += 2
        elif opcode == 153:
            print(f'{hex(ip)}: ret')
            ip += 1
        else:
            print(f'{hex(ip)}: UNKNOWN INSTRUCTION / INVALID BYTECODE')


vm_disassembler(fib_bytecode)        