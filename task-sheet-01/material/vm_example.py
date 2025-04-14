#!/bin/python
# 
# This script demonstrates basic virtualization-based obfuscation. It includes
# - a function to be virtualized
# - a basic VM interpreter
# - a basic VM disassembler
# 
# (c) Tim Blazytko, 2022.

def to_virtualize(p0):
    """Function to be virtualized"""
    result = 0
    for index in range(100):
        result += p0
        index += 1

    return result


def vm_disassembler(bytecode):
    """Disassembler for the designed VM"""
    index = 0
    while index < len(bytecode):
        opcode = bytecode[index]

        if opcode == 0:
            print(f"{hex(index)}: mov r{bytecode[index + 1]}, {hex(bytecode[index + 2])}")
            index += 3
        elif opcode == 1:
            print(f"{hex(index)}: mov r{bytecode[index + 1]}, param0")
            index += 2
        elif opcode == 2:
            print(f"{hex(index)}: cmp r{bytecode[index + 1]}, {hex(bytecode[index + 2])}")
            index += 3
        elif opcode == 3:
            print(f"{hex(index)}: jae {hex(bytecode[index + 1])}")
            index += 2
        elif opcode == 4:
            print(f"{hex(index)}: add r{bytecode[index + 1]}, r{bytecode[index + 2]}")
            index += 3
        elif opcode == 5:
            print(f"{hex(index)}: jmp {hex(bytecode[index + 1])}")
            index += 2
        elif opcode == 6:
            print(f"{hex(index)}: vmexit")
            index += 1


def vm_execute(bytecode, param0):
    """
    VM interpreter for the example VM.

    VM Design:
    - register-based architecture (256 registers + flag register + virtual instruction pointer)
    - returns r0 in vmexit
    - variable-length instruction encoding
    - each bytecode field is a single byte (for opcode, register/address/constant).
    """
    # init vm register
    vip = 0
    vm_regs = [0] * 256
    flag_greater_equal = 0

    # fetch-decode-execute loop
    while True:
        opcode = bytecode[vip]
        
        # mov reg, constant
        if opcode == 0:
            reg = bytecode[vip + 1]
            constant = bytecode[vip + 2]
            vm_regs[reg] = constant
            vip += 3
        # mov reg, param0
        elif opcode == 1:
            reg = bytecode[vip + 1]
            vm_regs[reg] = param0
            vip += 2
        # cmp reg, constant
        elif opcode == 2:
            reg = bytecode[vip + 1]
            constant = bytecode[vip + 2]
            flag_greater_equal = vm_regs[reg] >= constant
            vip += 3
        # jae <address>
        elif opcode == 3:
            if flag_greater_equal:
                vip = bytecode[vip + 1]
            else:
                vip += 2
        # add reg, reg
        elif opcode == 4:
            reg0 = bytecode[vip + 1]
            reg1 = bytecode[vip + 2]
            vm_regs[reg0] = vm_regs[reg0] + vm_regs[reg1]
            vip += 3
        # jmp address
        elif opcode == 5:
            address = bytecode[vip + 1]
            vip = address
        # vmexit
        elif opcode == 6:
            # return r0
            value = vm_regs[0]
            return value
        else:
            print(f"invalid opcode {opcode}")
            exit()

# bytecode of virtualzied function
bytecode = [
    # mov r0, 0 ; result := 0
    0x00, 0x00, 0x00,
    # mov r3, 1 ; set intermediate value for increment
    0x00, 0x03, 0x01,
    # mov r1, param0 ; r1 := param0
    0x01, 0x01,
    # mov r2, 0 ; index := 0
    0x00, 0x02, 0x00,
    # LOOP_CHECK:
    # cmp r2, 100 ; index >= 100
    0x02, 0x02, 0x64,
    # jae EXIT ; vip = EXIT if index >= 100
    0x03, 0x18,
    # add r0, r1 ; result := param0
    0x04, 0x00, 0x01,
    # add r2, r3 ; result += 1
    0x04, 0x02, 0x03,
    # jmp LOOP_CHECK ; vip = LOOP_CHECK
    0x05, 0x0b, 
    # EXIT:
    # VM_EXIT ; return result
    0x06
]

print("disassembly:")
vm_disassembler(bytecode)

print(f"output of original function: {to_virtualize(100)}")
print(f"output of virtualized function: {vm_execute(bytecode, 100)}")


