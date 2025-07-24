# Task 1: Design & Implementation of Anti-tamper Techniques

**a) String Encryption and Decryption:**
A random 19-byte key is used for XOR encryption and decryption of strings. All sensitive strings within the code, such as user prompts, error messages, and API function names, are encrypted. At runtime, these encrypted strings are decrypted using the `decryptString` function before they are used. This prevents static analysis tools like Ghidra or IDA from easily extracting plaintext strings from the compiled binary.

**b) Dynamic API Resolution:**
To hide API function calls from import tables, all necessary API functions (e.g., `printf`, `strlen`, `ptrace`, `signal`, `alarm`, `exit`) are resolved at runtime. This is achieved by dynamically loading `libc.so` using `dlopen` and then retrieving function pointers using `dlsym`. The names of these API functions, along with the path to `libc.so`, are also encrypted and decrypted at runtime.

**c) Debugger Detection:**
Two methods are employed to detect debuggers:

1.  **`ptrace(PTRACE_TRACEME)` Check:** The `validate` function attempts to call `ptrace(PTRACE_TRACEME, 0, 1, 0)`. If this call returns a value less than 0, it indicates that another debugger or tracer has already used `PTRACE_TRACEME`, signifying that the program is being debugged. In such a case, the `validate` function immediately fails, preventing the execution of the license validation logic.
2.  **SIGTRAP Detection:** A `SIGTRAP` (signal 5) handler (`debugTrapHandler`) is installed in the `main` function using `signal`. A `SIGTRAP` signal is then sent to the program using `alarm` (which appears to be a misnomer in the description, as `alarm` typically sends `SIGALRM`, but the code explicitly uses signal 5 for `SIGTRAP`). If `trapCount` remains 0 after the signal is sent and the handler should have incremented it, it means a debugger intercepted the `SIGTRAP`. If a debugger is detected this way, the first character of both the username and license input strings are modified to 'A', ensuring the license check will always fail without explicitly indicating a debugger detection.

**d) Virtual Machine Detection:**
Two techniques are used to detect execution within a virtual machine:

1.  **Hypervisor Bit Check:** The `detectHypervisor` function uses the `CPUID` instruction with `EAX=1`. It then checks the 31st bit of the `ECX` register. If this bit is set (1), it typically indicates execution within a virtual machine. If a hypervisor is detected, similar to the `ptrace` debugger detection, the license validation fails, and its logic is not executed.
2.  **`CPUID` Leaf `0x40000000` Check:** The `detectVirtualMachine` function calls `CPUID` with `EAX=0x40000000`. On physical CPUs, this input typically returns all zeros. However, on virtual CPUs, it often returns a hypervisor brand string (e.g., "QEMU Virtual CPU"). If any of the `EAX`, `EBX`, `ECX`, or `EDX` registers are non-zero after this `CPUID` call, a VM is detected. This check is performed in the `main` function, and if a VM is detected, the `validate` function is not called, and the program outputs an "invalid license" message.

**e) Code Checksumming:**
A `calculateMemoryChecksum` function is implemented to verify the integrity of specific code sections. This function takes a starting value, a start address, and an end address, iterating through each byte in the specified range and adding it to a checksum.

- **Main Function Check:** A checksum is calculated over a critical part of the `main` function, specifically where the `validate` function is called and its return value is checked. If this checksum does not match a hardcoded `validChecksumMain`, the program exits as if the license is invalid. This protects against an attacker patching the conditional jump associated with the validation result.
- **Validate Function Check:** Another checksum is calculated over the entire `validate` function. If this checksum does not match a hardcoded `validChecksumValidate`, the program exits, preventing an attacker from modifying the core license validation logic.
  These checksums are sensitive to compiler optimizations, requiring the code to be compiled without them.

**f) Custom Virtual Machine for Checksum Verification:**
A custom, register-based virtual machine (`executeChecksumVM`) is implemented to perform an additional checksum verification.

- **VM Architecture:** The VM has two registers: an `unsigned char` pointer register (`reg_c`) and a `long` register (`reg_l`).
- **Instruction Set:** The VM includes instructions for moving constants into registers (`mov`), comparing register values with constants (`cmp`), conditional and unconditional jumps (`jge`, `jne`, `jmp`), adding values to registers (`add`), and exiting the VM (`vmexit`). It also has instructions for printing an error message and exiting the program.
- **Checksum Logic in VM:** The VM executes a specific bytecode sequence to calculate a checksum over the entire `main` function (from its start address to `startAddr + 1846`). The calculated checksum is then compared against a hardcoded `validChecksumBytecode`. If the checksum does not match, the VM prints an invalid license message and exits the program. This layer of protection aims to make it more difficult for an attacker to bypass the checksum checks, as they would need to understand and reverse-engineer the custom VM.
