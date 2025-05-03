#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_REGS 16
#define NUM_FLAGS 3
#define DEBUG 0

uint8_t fib_bytecode[] = {
// fib:
    // mov     r0, 0x00    ; a = 0
    0x10, 0x00, 0x00,
    // mov     r1, 0x01    ; b = 1
    0x10, 0x01, 0x01,
    // copy    r2, r1      ; s = b
    0x20, 0x02, 0x01,
    // mov     r3, 0x02    ; i = 2
    0x10, 0x03, 0x02,
    // cmpi    r11, 0x00   ; if (n != 0)
    0x41, 0x0B, 0x00,
    // jneq    .Loop       ; goto .Loop
    0x54, 0x12,
    // ret                 ; r11 = 0 -> return 0
    0x99,
// .Loop: 
    // cmp     r3, r11     ; if (i <= n)
    0x40, 0x03, 0x0B,
    // jge     .Leave      ; goto .Leave
    0x53, 0x25,
    // add     r2, r0, r1  ; s = a + b
    0x30, 0x02, 0x00, 0x01,
    // copy    r0, r1      ; a = b
    0x20, 0x00, 0x01,
    // copy    r1, r2      ; b = s
    0x20, 0x1, 0x02,
    // inc     r3          ; i += 1
    0x31, 0x03,
    // jmp     .Loop       ; goto .Loop
    0x50, 0x12,
// .Leave:
    // ; return s
    // copy     r11, r2     
    0x20, 0x0B, 0x02,
    // ret      
    0x99
};

typedef struct {
    uint32_t ip;                // instruction pointer
    uint64_t r[NUM_REGS];       // registers
    uint8_t  flag[NUM_FLAGS];   // flag register
} VM;

// Prints the VM state
void print_vm(VM* vm, const char* label) {
    printf("\n==== VM State: %s ====\n", label);
    printf("Instruction Pointer: %u (0x%016X)\n", vm->ip, vm->ip);
    
    // Print registers
    printf("Registers:\n");
    for (int i = 0; i < NUM_REGS; i++) {
        printf("  r%2d: %20lu (0x%016lx)\n", i, vm->r[i], vm->r[i]);
    }
    
    // Print flags
    printf("Flags: [L:%d E:%d G:%d]\n", 
           vm->flag[0], vm->flag[1], vm->flag[2]);
    
    // Print current instruction if within bounds
    if (vm->ip < sizeof(fib_bytecode)) {
        printf("Next Operation: 0x%02X\n", fib_bytecode[vm->ip]);
    } else {
        printf("IP out of bounds\n");
    }
    printf("============================\n\n");
}

// Execute a single instruction / handles opcode
int exec_instruction(VM* vm, uint8_t op) {
    
    // store old instruction pointer
    uint32_t old_ip;
    old_ip = vm->ip;
    
    switch(op) {
        // MOV
        case 0x10: {
            uint8_t reg = fib_bytecode[old_ip + 1];
            uint8_t value = fib_bytecode[old_ip + 2];
            vm->r[reg] = value;
            
            vm->ip += 3;
            break;
        }
        // COPY
        case 0x20: {
            uint8_t dest_reg = fib_bytecode[old_ip + 1];
            uint8_t src_reg = fib_bytecode[old_ip + 2];
            vm->r[dest_reg] = vm->r[src_reg];
            
            vm->ip += 3;
            break;
        }
        // ADD
        case 0x30: {
            uint8_t dest_reg = fib_bytecode[old_ip + 1];
            uint8_t src1_reg = fib_bytecode[old_ip + 2];
            uint8_t src2_reg = fib_bytecode[old_ip + 3];
            
            // MBA Transformation for addition: a + b = (a ^ b) + 2*(a & b)
            uint32_t xor_result = vm->r[src1_reg] ^ vm->r[src2_reg];
            uint32_t and_result = vm->r[src1_reg] & vm->r[src2_reg];
            vm->r[dest_reg] = xor_result + (and_result << 1);
            // x^2 - x is always even for any integer x
            uint32_t x = (vm->r[dest_reg] * 0x10325476 + 0x98BADCFE) & 0xFF;
            uint32_t opaque_result = (x * x - x) & 0x1;
            
            if (opaque_result == 0) {
                // Always true, but hard for compiler to prove
                vm->ip += 4;
            } else {
                // Dead code that will never execute but compiler can't easily determine this
                // and using volatile to prevent optimization.
                volatile uint32_t temp = vm->r[dest_reg];
                vm->r[dest_reg] = ~temp;
                vm->ip += 4;
            }
            
            break;
        }
        // INC
        case 0x31: {
            uint8_t reg = fib_bytecode[old_ip + 1];
            vm->r[reg] += 1;
            
            vm->ip += 2;
            break;
        }
        // DEC
        case 0x32: {
            uint8_t reg = fib_bytecode[old_ip + 1];
            vm->r[reg] -= 1;
            
            vm->ip += 2;
            break;
        }
        // SUB
        case 0x33: {
            uint8_t dest_reg = fib_bytecode[old_ip + 1];
            uint8_t src1_reg = fib_bytecode[old_ip + 2];
            uint8_t src2_reg = fib_bytecode[old_ip + 3];
            
            // MBA Transformation for subtraction: a - b = a + ~b + 1 = a ^ ~b ^ (~(a & ~b))
            uint32_t not_b = ~vm->r[src2_reg];
            uint32_t a_xor_notb = vm->r[src1_reg] ^ not_b;
            uint32_t a_and_notb = vm->r[src1_reg] & not_b;
            vm->r[dest_reg] = a_xor_notb ^ (~a_and_notb);
            
            // Complex opaque predicate using Collatz conjecture properties
            // For any positive integer, applying steps of the Collatz sequence
            // will eventually reach 1 (unproven but believed to be true)
            // 
            // See:
            //  - https://stackoverflow.com/questions/33223289/collatz-predicate-in-prolog
            //  - https://www.geeksforgeeks.org/c-program-implement-collatz-conjecture/
            //  - https://www.geeksforgeeks.org/program-to-print-collatz-sequence/
            uint32_t n = ((vm->r[dest_reg] ^ 0xDEADBEEF) & 0xFF) | 1; // Ensure odd starting value
            uint32_t steps = 0;
            // Limited iterations to avoid excessive computation
            for (int i = 0; i < 10 && n != 1; i++) {
                if (n % 2 == 0) {
                    n = n / 2;
                } else {
                    n = 3 * n + 1;
                }
                steps++;
            }
            
            // Every number will reach either 1 or a cycle including 1
            if ((steps & 0x7) != 0x7) {
                // Always true but hard to prove statically
                vm->ip += 4;
            } else {
                // Dead code path with side effects to prevent elimination
                volatile uint32_t temp = vm->r[dest_reg];
                vm->r[dest_reg] = temp ^ 0xFFFFFFFF;
                vm->ip += 4;
            }
            
            break;
        }
        // CMP
        case 0x40: {
            uint8_t reg1 = fib_bytecode[old_ip + 1];
            uint8_t reg2 = fib_bytecode[old_ip + 2];
            uint64_t first_r = vm->r[reg1];
            uint64_t second_r = vm->r[reg2];

            if (first_r < second_r) {
                vm->flag[0] = 1; // set less flag
            } else {
                vm->flag[0] = 0; // reset less flag (could be set from last comparison)
            }

            if (first_r == second_r) {
                vm->flag[1] = 1; // set equal flag
            } else {
                vm->flag[1] = 0; // reset equal flag (could be set from last comparison)
            }

            if (first_r > second_r) {
                vm->flag[2] = 1; // set greater flag
            } else {
                vm->flag[2] = 0; // reset greater flag (could be set from last comparison)
            }
            
            vm->ip += 3;
            break;
        }
        // CMPI 
        case 0x41: {
            uint8_t reg = fib_bytecode[old_ip + 1];
            uint8_t value = fib_bytecode[old_ip + 2];
            uint64_t first_r = vm->r[reg];
            uint64_t second_v = value;

            if (first_r < second_v) {
                vm->flag[0] = 1; // set less flag
            } else {
                vm->flag[0] = 0; // reset less flag (could be set from last comparison)
            }

            if (first_r == second_v) {
                vm->flag[1] = 1; // set equal flag
            } else {
                vm->flag[1] = 0; // reset equal flag (could be set from last comparison)
            }

            if (first_r > second_v) {
                vm->flag[2] = 1; // set greater flag
            } else {
                vm->flag[2] = 0; // reset greater flag (could be set from last comparison)
            }
            
            vm->ip += 3;
            break;
        }
        // JMP
        case 0x50: {
            uint8_t addr = fib_bytecode[old_ip + 1];
            vm->ip = addr;
            break;
        }
        // JEQ
        case 0x51: {
            uint8_t addr = fib_bytecode[old_ip + 1];
            if (vm->flag[1] == 1) {
                vm->ip = addr;
            } else {
                vm->ip += 2;
            }
            break;
        }
        // JLE
        case 0x52: {
            uint8_t addr = fib_bytecode[old_ip + 1];
            if (vm->flag[0] == 1 && vm->flag[1] == 0) {
                vm->ip = addr;
            } else {
                vm->ip += 2;
            }
            break;
        }
        // JGE
        case 0x53: {
            uint8_t addr = fib_bytecode[old_ip + 1];
            if (vm->flag[2] == 1 && vm->flag[1] == 0) {
                vm->ip = addr;
            } else {
                vm->ip += 2;
            }
            break;
        }
        // JNEQ
        case 0x54: {
            uint8_t addr = fib_bytecode[old_ip + 1];
            if (vm->flag[1] == 0) {
                vm->ip = addr;
            } else {
                vm->ip += 2;
            }
            break;
        }
        // RET
        case 0x99:
            return 1;
        // Any undefinied
        default:
            fprintf(stderr, "Unknown opcode 0x%02X at vip=%u\n", op, vm->ip - 1);
            exit(EXIT_FAILURE);
    }

    if (DEBUG == 2) {
        char label[64];
        sprintf(label, "After execution of instruction at 0x%02X", old_ip);
        print_vm(vm, label);
    }    
    return 0;
}

// Fetch, decode, execute logic
uint64_t run_vm(VM *vm) {

    // Store old "i" to avoid redudant output
    uint64_t old_r3 = 0;

    while (vm->ip < sizeof(fib_bytecode)) {
        
        if (DEBUG) {
            if (vm->r[3] != old_r3) {
                // The result is calculated within "s" that is stored in r2
                printf("Result-step for i=%ld: %ld\n", vm->r[3], vm->r[2]);
                old_r3 = vm->r[3];
            }
        } 
        
        uint8_t op = fib_bytecode[vm->ip];
        if (exec_instruction(vm, op)) {
            return vm->r[11];
        }
    }
    
    // Should never reach here
    return 0;
}

// Initilize VM state
void init_vm(VM* vm) {
    // All registers should be empty on start
    for (int i = 0; i < NUM_REGS; i++) {
        vm->r[i] = 0;
    }
    // All flags shouldn't set
    for (int i = 0; i < NUM_FLAGS; i++) {
        vm->flag[i] = 0;
    }
    // Instruction pointer points to first index
    vm->ip = 0;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse the first argument as a 64-bit value
    char *end;
    uint64_t n = strtoull(argv[1], &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Invalid number: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    // setup VM
    VM vm;
    init_vm(&vm);

    // Prepare fib-function call:
    //  - r10 holds the number of arguments, in this case 1
    //  - r11 holds the first argument, in this case "n"
    vm.r[10] = 1;
    vm.r[11] = n;

    if (DEBUG) {
        print_vm(&vm, "At Function-Call");
    }   
    
    // Calculate fib within the software VM
    uint64_t result = run_vm(&vm);
    printf("Result: %lu\n", result);

    if (DEBUG) {
        print_vm(&vm, "After Function-Call");
    }   

    return EXIT_SUCCESS;
}