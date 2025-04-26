# Task 2: Virtualization-based Obfuscation

The VM under consideration will utilize a register-based architecture, incorporating 10 64-bit general-purpose registers (r0 - r9) and 6 parameter registers (r10 - r15). The parameter registers are used to handle in- and output of function calls. The input values of a function are expected to be in r11 to r15 starting with the first input variable in r11. The return value(s) are expected to be in the same. The register r10 holds the number of input or output variables. There is no stack or anything similiar and the current instruction is pointed by the write-protected 32-bit register ip. In addition, it will be equipped with a three-bit flag register, which is employed for the purpose of indicating comparison operations, specifically identifying whether the value is smaller, equal or greater. The following tables present the instruction set:

### Custom Instruction Set for Fibonacci VM

| Operation | Opcode | Operands                                    | Total Size | Description                      | Example                                         |
| --------- | ------ | ------------------------------------------- | ---------- | -------------------------------- | ----------------------------------------------- |
| MOV       | 0x10   | reg (1B), value (8B)                        | 6B         | Move immediate value to register | 0x10 0x02 0x000000000000000A: Move 10 to r2     |
| COPY      | 0x20   | dest_reg (1B), src_reg (1B)                 | 3B         | Copy value between registers     | 0x20 0x03 0x04: Copy from r4 to r3              |
| ADD       | 0x30   | dest_reg (1B), src1_reg (1B), src2_reg (1B) | 4B         | Add two registers                | 0x30 0x01 0x02 0x00: r1 = r2 + r0               |
| INC       | 0x31   | reg (1B)                                    | 2B         | Increment register by 1          | 0x31 0x03: Increment r3                         |
| DEC       | 0x32   | reg (1B)                                    | 2B         | Decrement register by 1          | 0x32 0x03: Decrement r3                         |
| SUB       | 0x33   | dest_reg (1B), src1_reg (1B), src2_reg (1B) | 4B         | Sub two registers                | 0x33 0x01 0x02 0x00: r1 = r2 - r0               |
| CMP       | 0x40   | reg1 (1B), reg2 (1B)                        | 3B         | Compare registers, set flags     | 0x40 0x03 0x04: Compare r3 and r4               |
| CMPI      | 0x41   | reg (1B), value (8B)                        | 6B         | Compare reg with immediate       | 0x41 0x00 0x0000000000000001: Compare r0 with 1 |
| JMP       | 0x50   | addr (4B)                                   | 5B         | Unconditional jump               | 0x50 0x00000010: Jump to addr 0x10              |
| JEQ       | 0x51   | addr (4B)                                   | 5B         | Jump if equal                    | 0x51 0x00000020: Jump to 0x20 if equal          |
| JLE       | 0x52   | addr (4B)                                   | 5B         | Jump if less                     | 0x52 0x00000030: Jump if less                   |
| JGE       | 0x53   | addr (4B)                                   | 5B         | Jump if greater                  | 0x53 0x00000030: Jump if greater                |
| JNEQ      | 0x54   | addr (4B)                                   | 5B         | Jump if not equal                | 0x54 0x00000020: Jump to 0x20 if not equal      |
| RET       | 0x99   | -                                           | 1B         | Return function call             | 0x99                                            |

### Flags Register

- Bit 0: Less than (LT) - Set when first operand < second operand
- Bit 1: Equal (EQ) - Set when first operand = second operand
- Bit 2: Greater than (GT) - Set when first operand > second operand
