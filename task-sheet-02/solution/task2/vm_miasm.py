#
# References:
# - https://sudhackar.github.io/blog/learning-symbolic-execution-with-miasm
# - https://miasm.re/blog/2017/10/05/playing_with_dynamic_symbolic_execution.html
# - https://github.com/cea-sec/miasm/issues/163
# - https://github.com/mrphrazer/r2con2021_deobfuscation/blob/main/symbolic_execution.py
#

from argparse import ArgumentParser
from miasm.analysis.binary import Container
from miasm.analysis.machine import Machine
from miasm.core.locationdb import LocationDB
from miasm.ir.symbexec import SymbolicExecutionEngine
from miasm.expression.expression import *
from miasm.expression.simplifications import expr_simp

# ---- Arg Parse ----

parser = ArgumentParser("Simple SymbolicExecution demonstrator")
parser.add_argument("target_binary", help="Target binary path")
parser.add_argument("--address", help="Starting address for emulation. If not set, use the entrypoint")
options = parser.parse_args()

# ---- Setup Miasm ----

loc_db = LocationDB()
cont = Container.from_stream(open(options.target_binary, 'rb'), loc_db)
machine = Machine(cont.arch)
mdis = machine.dis_engine(cont.bin_stream, loc_db=cont.loc_db)

# no address -> entry point
if options.address is None:
    addr = cont.entry_point
else:
    try:
        # 0xXXXXXX -> use this address
        addr = int(options.address, 0)
    except ValueError:
        # symbol -> resolve then use
        addr = loc_db.get_name_offset(options.address)

asmcfg = mdis.dis_multiblock(addr)
ira = machine.lifter_model_call(mdis.loc_db)
ircfg = ira.new_ircfg_from_asmcfg(asmcfg)
sb = SymbolicExecutionEngine(ira)

# ---- Functions ----

def constraint_memory(address, num_of_bytes):
    """
    Reads `num_of_bytes` from the binary at a given address
    and builds symbolic formulas to pre-configure the symbolic
    execution engine for concolinc execution.
    """
    global cont
    # read bytes from binary
    byte_stream = cont.bin_stream.getbytes(address, num_of_bytes)
    # build symbolic memory address
    sym_address = ExprMem(ExprInt(address, 64), num_of_bytes * 8)
    # build symbolic memory value
    sym_value = ExprInt(int.from_bytes(byte_stream, byteorder='little'), num_of_bytes * 8)

    return sym_address, sym_value

# ---- VM Execution ----

# constraint bytecode -- start address and size (highest address - lowest address)
sym_address, sym_value = constraint_memory(0x4060, 0x42f0 - 0x4060)
sb.symbols[sym_address] = sym_value

# constraint VM input (rdi, first function argument). 
# The value in `ExprInt` rerpesents the function's input value.
rdi = ExprId("RDI", 64)
sb.symbols[rdi] = ExprInt(0, 64)

# init worklist
basic_block_worklist = [ExprInt(addr, 64)]

# worklist algorithm
while basic_block_worklist:
    # get current block
    current_block = basic_block_worklist.pop()

    print(f"\n------ {current_block} ------\n")

    # symbolical execute block -> next_block: symbolic value/address to execute
    next_block = sb.run_block_at(ircfg, current_block, step=False)
    sb.dump()

    print(f"next block: {next_block}")

    # is next block is integer or label, continue execution
    if next_block.is_int() or next_block.is_loc():
        basic_block_worklist.append(next_block)

# dump symbolic state
sb.dump()

# dump VMs/functions' return value -- only works if SE runs until the end
rax = ExprId("RAX", 64)
value = expr_simp(sb.symbols[rax])
print(f"\n-----\nVM return value: {value}\n-----")