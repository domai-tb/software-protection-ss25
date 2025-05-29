#
# Check out the following example(s):
#   - https://github.com/cea-sec/miasm/blob/master/example/symbol_exec/symbol_exec.py
#

from z3 import Solver, unsat
from argparse import ArgumentParser
from miasm.analysis.binary import Container
from miasm.analysis.machine import Machine
from miasm.core.locationdb import LocationDB
from miasm.ir.symbexec import SymbolicExecutionEngine
from miasm.ir.translators.z3_ir import TranslatorZ3
from miasm.core.bin_stream import bin_stream_str
from miasm.core.utils import pck32

# ---- Arg Parse ----

parser = ArgumentParser("Simple SymbolicExecution demonstrator")
parser.add_argument("target_binary", help="Target binary path")
parser.add_argument("--address", help="Starting address for emulation. If not set, use the entrypoint")
parser.add_argument("--patch", help="Patch binary (no guarantee)", action="store_true")
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

# ---- Opaque Removing ----

def branch_cannot_be_taken(expression, jump_target):
    solver = Solver()
    translator = TranslatorZ3()
    solver.add(
        translator.from_expr(expression) == translator.from_expr(jump_target)
    )

    return solver.check() == unsat

def elf_virt2off(cont, va):
    elf = cont.executable

    # First, check if VA lies in any PT_LOAD segment (program headers)
    for segment in elf.ph:
        phdr = segment.ph
        if phdr.type != 'PT_LOAD':
            continue
        seg_start = phdr.vaddr
        seg_end = seg_start + phdr.memsz
        if seg_start <= va < seg_end:
            # Calculate offset within segment and add segment file offset
            return phdr.offset + (va - seg_start)

    # If not in any PT_LOAD, check if VA is inside a section header (sections)
    for section in elf.sh:
        shdr = section.sh
        sec_start = shdr.addr
        sec_end = sec_start + shdr.size
        if sec_start <= va < sec_end:
            # Calculate offset within section and add section file offset
            return shdr.offset + (va - sec_start)

    # Not found in segments or sections
    return None

# countopaques and store bytes to patch
opaque_predicats_counter = 0
opaque_predicats_patches = {}

# iterate over basic blocks
for basic_block in asmcfg.blocks:
    
    # get first instruction of basic block
    address = basic_block.lines[0].offset
    
    # init symbolic execution based on intermediate representation
    sb = SymbolicExecutionEngine(ira)
    
    # symbolic execute basic block
    bb_exp = sb.run_block_at(ircfg, address)
    
    # skip basic block if there is no jump => cannot be a opaque predicate
    if not bb_exp.is_cond():
        continue

    # get jump onstruction => last instruction in basic block
    jump_instruction = basic_block.lines[-1]

    # check if opaque predicate - jump
    if branch_cannot_be_taken(bb_exp, bb_exp.src1):
        print(f'Opaque Predicate at {hex(address)} (jump never taken)')
        opaque_predicats_counter += 1

        # get jump offset
        offset_of_jump_instruction = elf_virt2off(cont, jump_instruction.offset)

        # walk over all instruction bytes and set corresponding file offset to NOP / 0x90
        for idx in range(offset_of_jump_instruction, offset_of_jump_instruction + len(jump_instruction.b)):
            opaque_predicats_patches[idx] = 0x90

    # check if opaque predicate - fall-through
    elif branch_cannot_be_taken(bb_exp, bb_exp.src2):
        print(f'Opaque Predicate at {hex(address)} (jump always taken)')
        opaque_predicats_counter += 1

print(f"Number of detected and patched opaque predicates: {opaque_predicats_counter}")

if options.patch:
    # read original file
    raw_bytes = bytearray(open(options.target_binary, 'rb').read())
    # apply patch
    for index, byte in opaque_predicats_patches.items():
        raw_bytes[index] = byte
    # save pacthed file
    open(f"{options.target_binary}_patched", 'wb').write(raw_bytes)