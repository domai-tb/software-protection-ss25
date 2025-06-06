#!/usr/bin/env python3

import sys
from miasm.analysis.binary import Container
from miasm.analysis.machine import Machine
from miasm.core.locationdb import LocationDB
from miasm.expression.expression import *
from miasm.expression.simplifications import expr_simp
from miasm.ir.symbexec import SymbolicExecutionEngine


# hardcoded list of VM handlers taken from the binary
VM_HANDLERS = set([
    0x1390, # MOV
    0x1380, # JMP
    0x131c, # ADD
    0x1368, # JNZ
    0x12e6, # CMP
    0x126b, # ADDI
    0x13d8, # RET
    0x13f0, # CMPLE
    0x1350, # MOVI
    0x13b0, # XOR
    0x12c2, # LEA
    0x1297, # MOVC
])

def constraint_memory(address, num_of_bytes):
    """
    Reads `num_of_bytes` from the binary at a given address
    and builds symbolic formulas to pre-configure the symbolic
    execution engine for concolinc execution.
    """
    global container
    # read bytes from binary
    byte_stream = container.bin_stream.getbytes(address, num_of_bytes)
    # build symbolic memory address
    sym_address = ExprMem(ExprInt(address, 64), num_of_bytes * 8)
    # build symbolic memory value
    sym_value = ExprInt(int.from_bytes(
        byte_stream, byteorder='little'), num_of_bytes * 8)

    return sym_address, sym_value

def disassemble(sb, address):
    """
    Callback to dump individual VM handler information,
    execution context etc.
    """
    # fetch concrete value of current virtual instruction pointer
    vip = sb.symbols[ExprId("RDX", 64)]
    opcode = sb.symbols[ExprId("RAX",64)]
    rsp = sb.symbols[ExprId("RSP",64)]
    # catch the individual handlers and print execution context
    if int(address) == 0x1390:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - MOV:  reg[{param1}] = reg[{param2}]")
    if int(address) == 0x1380:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - JMP: jump to vip + 1 + {param1}")
    if int(address) == 0x131c:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(9, 64)).signExtend(64))
        param3 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - ADD: reg[{param3}] = reg[{param1}] + reg[{param2}]")
    if int(address) == 0x1368:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - JNZ: if {param2} != 0: jump to vip + 1 + {param1} ")
    if int(address) == 0x12e6:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(9, 64)).signExtend(64))
        param3 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - CMP: reg[{param3}] = reg[{param1}] < reg[{param2}]")
    if int(address) == 0x126b:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 16)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 16)])
        print(f"{opcode} - ADDI: reg[{param2}] = local_var + {param1}")
    if int(address) == 0x13d8:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - RET: return reg[{param1}]")
    if int(address) == 0x13f0:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(9, 64)).signExtend(64))
        param3 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - CMPLE: reg[{param1}] = reg[{param2}] <= reg[{param3}]")
    if int(address) == 0x1350:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - MOVI:  reg[{param1}] = {param2}")
    if int(address) == 0x13b0:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(9, 64)).signExtend(64))
        param3 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - XOR: reg[{param3}] = reg[{param2}] ^ reg[{param1}]")
    if int(address) == 0x12c2:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        param_address = expr_simp((vip + ExprInt(5, 64)).signExtend(64))
        param2 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - MOV:  reg[{param2}] = {param1}")
    if int(address) == 0x1297:
        param_address = expr_simp((vip + ExprInt(1, 64)).signExtend(64))
        param1 = expr_simp(sb.symbols[ExprMem(param_address, 32)])
        print(f"{opcode} - MOVC:  reg[{param1}] = local_var")


# check arguments
if len(sys.argv) != 3:
    print(f"[*] Syntax: {sys.argv[0]} <file> <input number>")
    exit()

# parse file path
file_path = sys.argv[1]

# address of vm entry
start_addr = 0x11a0

# init symbol table
loc_db = LocationDB()
# read binary file
container = Container.from_stream(open(file_path, 'rb'), loc_db)
# get CPU abstraction
machine = Machine(container.arch)
# disassembly engine
mdis = machine.dis_engine(container.bin_stream, loc_db=loc_db)

# initialize lifter to intermediate representation
lifter = machine.lifter_model_call(mdis.loc_db)

# disassemble the function at address
asm_cfg = mdis.dis_multiblock(start_addr)

# translate asm_cfg into ira_cfg
ira_cfg = lifter.new_ircfg_from_asmcfg(asm_cfg)

# init SE engine
sb = SymbolicExecutionEngine(lifter)

# constraint bytecode -- start address and size (highest address - lowest address)
length =  0x4327 - 0x4060
sym_address, sym_value = constraint_memory(0x4060, length)
sb.symbols[sym_address] = sym_value
# constraint VM input (rdi, first function argument). The value in `ExprInt` rerpesents the function's input value.
rdi = ExprId("RDI", 64)
sb.symbols[rdi] = ExprInt(int(sys.argv[2]), 64)

# init worklist
basic_block_worklist = [ExprInt(start_addr, 64)]

# worklist algorithm
while basic_block_worklist:
    # get current block
    current_block = basic_block_worklist.pop()

    # print(f"current block: {current_block}")

    # if current block is a VM handler, dump handler-specific knowledge
    if current_block.is_int() and int(current_block) in VM_HANDLERS:
        disassemble(sb, current_block)
    #else:
    #    print(f"other handler: {current_block}")

    # symbolical execute block -> next_block: symbolic value/address to execute
    next_block = sb.run_block_at(ira_cfg, current_block, step=False)

    # print(f"next block: {next_block}")

    # is next block is integer or label, continue execution
    if next_block.is_int() or next_block.is_loc():
        basic_block_worklist.append(next_block)

# dump symbolic state
#sb.dump()

# dump VMs/functions' return value -- only works if SE runs until the end
rax = ExprId("RAX", 64)
value = sb.symbols[rax]
print(f"VM return value: {value}")
