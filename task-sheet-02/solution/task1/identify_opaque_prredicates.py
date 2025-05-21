# identify_opaque_predicates.py
# Python 3.7+, requires capstone, lief, keystone, z3-solver

import argparse
import lief
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_GRP_JUMP
from keystone import Ks, KS_ARCH_X86, KS_MODE_64
from z3 import Solver, BitVec

FUNC_ADDR = 0x400546

# extract .text bytes of the function


def get_bytes(bin_path, addr):
    b = lief.parse(bin_path)
    sec = b.get_section(".text")
    base, data = sec.virtual_address, bytes(sec.content)
    off = addr - base
    # read until ret (0xC3)
    end = data.find(b"\xC3", off) + 1
    return data[off:end]


# detect patterns: jcc followed by jmp to same target


def detect(bin_path):
    code = get_bytes(bin_path, FUNC_ADDR)
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    insns = list(md.disasm(code, FUNC_ADDR))
    preds = []

    for i, ins in enumerate(insns[:-1]):
        # find conditional jump
        if ins.mnemonic.startswith("j") and ins.mnemonic != "jmp":
            # must be followed by an unconditional jmp
            nxt = insns[i + 1]
            if nxt.mnemonic == "jmp":
                # both targets must match
                t1 = ins.operands[0].imm
                t2 = nxt.operands[0].imm
                if t1 == t2:
                    preds.append((ins.address, ins.size + nxt.size))
    return preds


# patch opaque predicates by NOPing conditional and keeping jmp


def patch(bin_path, preds, out_path):
    b = lief.parse(bin_path)
    ks = Ks(KS_ARCH_X86, KS_MODE_64)
    for addr, size in preds:
        # NOP conditional jump
        b.patch_address(addr, [0x90] * size)
    b.write(out_path)


# solve inputs via Z3 (example for two predicates)


def solve():
    a, b = BitVec("a", 32), BitVec("b", 32)
    s = Solver()
    # constraints mirror the two opaque checks
    s.add((a * 3 + 5) % 7 == 2)
    s.add((b ^ 0xDEADBEEF) > 0)
    if s.check().r == 1:
        m = s.model()
        print("input0 =", m[a], "input1 =", m[b])
    else:
        print("no solution")


# CLI

if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("--binary", required=True)
    p.add_argument("--detect", action="store_true")
    p.add_argument("--patch", action="store_true")
    p.add_argument("--output")
    p.add_argument("--solve", action="store_true")
    args = p.parse_args()

    if args.detect:
        ps = detect(args.binary)
        print("Found", len(ps), "opaque predicates")
        for a, s in ps:
            print(hex(a), "size", s)
    if args.patch and args.output:
        patch(args.binary, ps, args.output)
        print("patched ->", args.output)
    if args.solve:
        solve()
