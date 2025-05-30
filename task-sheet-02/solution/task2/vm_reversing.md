# Task 2: VM Disassembler

>Any names or variables correspond with the (renamed) property within the C-like pseudo code in `vm_routine.pseudo.c` or the Binary Ninja project file `vm.bin.bndb`.

This virtual machine is **register-based** and operates on a manually constructed **virtual instruction set**. It begins by **decrypting its bytecode** using a hardcoded XOR key. The VM sets up its **registers**, then enters a loop that interprets instructions from the decrypted bytecode.

Instructions are fetched from memory via a **virtual instruction pointer (`vip`)**. Each instruction operates on a **register array** simulating CPU-like behavior. Control flow instructions and arithmetic / logical operations are implemented using conditional logic and pointer arithmetic.

## VM Architecture Type

**Register-Based VM**

* The VM uses a set of virtual registers to store values.
* Most operations involve explicit register reads and writes** (e.g., `reg[dst] = reg[Y] ^ reg[Z]`), not a stack.

## Key VM Components

| Component                               | Description                                                                                                                                                                                  |
| --------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Virtual Instruction Pointer (`vip`)** | Pointer to current instruction in the decrypted bytecode. It is incremented or modified after each instruction execution.                                                                    |
| **Bytecode Location**                   | The encrypted bytecode resides at `0x404060`, decrypted using byte-qise XOR with the constant `0xcbcbcbcbcbcbcbcb`.                                                                                   |
| **Instruction Handlers**                | Instruction logic is implemented in the `while(true)` loop and dispatched based on the `op` (opcode) value. Each handler checks for specific opcode values and executes corresponding logic. |
| **Register File (`registers_array`)**   | Acts as the VM's general-purpose register bank. Registers are accessed using indices and hold 32-bit or 64-bit values.                                                                       |
| **Input Argument (`input_num`)**        | Initial argument passed into the VM via registers.                                                                           |

## Opcode Dispatch Mechanism

* Instruction execution begins by reading an `opcode` byte (`op = *(uint8_t*)vip;`).
* There is a nested dispatch structure:

  * If `op > 0x80`, it's in the high opcode group.
  * Specific opcodes (e.g., `0xdd`, `0xf1`, `0xc2`, `0xd5`, etc.) map to VM instructions like return, XOR, CMPLE, load immediate.
  * Lower opcodes handle other operations like memory dereferencing, jumps, comparisons, etc.