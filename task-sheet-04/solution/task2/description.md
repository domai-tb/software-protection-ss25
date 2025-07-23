# Task 2: Bypassing Anti-tamper Techniques I

## a)

Looking through the file with Ghidra or BinaryNinja fails becaus ethese tools fail to disassembling the binary.
If we look closer, especially at the strings in the binary, we can see the reason for that:

```
❯ strings ../../material/license_validation | grep UPX
VUPX!
$Info: This file is packed with the UPX executable packer http://upx.sf.net $
$Id: UPX 3.96 Copyright (C) 1996-2020 the UPX Team. All Rights Reserved. $
UPX!u
UPX!
UPX!
```

The binary is packaged with [UPX](https://upx.github.io/) version 3.96 and can use UPX to unpack the binary:

```
❯ upx -d ../../material/license_validation
                       Ultimate Packer for eXecutables
                          Copyright (C) 1996 - 2025
UPX 5.0.2       Markus Oberhumer, Laszlo Molnar & John Reiser   Jul 20th 2025

        File size         Ratio      Format      Name
   --------------------   ------   -----------   -----------
[WARNING] bad b_info at 0x4bed4

[WARNING] ... recovery at 0x4bed0

    778992 <-    311120   39.94%   linux/amd64   license_validation

Unpacked 1 file.
```

Without unpacking the binary, an attacker can not practically understand anything of the logic of the binary and also can not patch the binary.

The second anti-tampering mechanism that can be identified is checksum calculation and validation starting at `0x401f4a`.
This calculates a checksum over the binary between address `0x4026f8` and `0x410077`. At first, a variable is initialized with the value `0x1be2e3c82a1c3057`.
Then, the values of all addresses between 0x4026f8 and 0x410077 are interpreted as signed long int and are all added together in the variable. If the variable does not
equals `0x316957d789d1e864` after the addition, the checksum check fails and the program crashes because of the execution of the function at `0x4010da`.

## b)

After unpacking the binary, we search for strings in the binary that would lead us to the addresses to patch.
At `0x402807`, we see that the license check is checked at `0x4027fe`. If the binary should accept all licenses,
we need to patch the instruction at `0x402805`. This jump instruction must be "noped".

If we simply apply this patch, the binary crashes on executing. This is caused by an anti-temper
check that is implemented in the function where we patched the license check. The function is located at `0x401765`.

There are different parts of the function that look like checksum checks. At first, a value is calculated and then the
value is compared with a hard coded value.

But actually there is only one check that needs to be patched to bypass the anti-temper checks after the license check was patched.
We can investigate this by checking which address range of the binary is protected by which potential anti-temper check.
Some anti-temper checks do not protect parts of the license checking, but protect other anti-temper checks.

The only conditional jump of an anti-temper check we need to patch is located at the address `0x401f9c`. If we patch the
byte at this address to `0xeb`, the conditional jump changes to an unconditional jump, and the branch that would crash the
program is never taken. This is the only anti-temper check that protects the address `0x402805`.

Thus, applying these patches results in a binary that accepts every license and does not crash.
The resulting binary is `license_validation_unpacked_patched`.
