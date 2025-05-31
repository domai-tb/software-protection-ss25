# Task 3: Real-World VM Analysis I 

>Downloaded `ci.dll` from (iosninja.io)[https://iosninja.io/dll/download/file/28d8d141924242161e727388c4fd77c14c1f771a] and all memory references are based on this downloaded file (included in the submitted ZIP).

>I also included a Binary Ninja project file. Especially function names will refer to the (renamed) names within this project file. 

### (a) Locate the virtual machine in ci.dll. In how many functions is it used?

The virtual machines init handler is located at `0x1c00b8fb8` and called six times within the `peauthvbn_`-prefixed functions of the DLL. To be fair, I didn't identified the initial location by myself alone. After some Google- and ChatGPT-Searches I found the repository (`warbirdvm`)[https://github.com/airbus-seclab/warbirdvm] that already analyzed the `ci.dll` and pointed to the `peauthvbn_`-prefixed functions. Starting wirth `peauthvbn_InitStore` at `0x1c008cd88`, it was easy to identify the `vmInitHanlder` - it was the only function called. 

### (b) Locate the virtual instruction pointer and the handler table. How many handlers does the VM have?

The handler table starts at address `0x1c00c0b20` and the last handler is located at address `0x1c00c4b18`. This data location could easily identified after looking at the decompiled output of the `vmInitHandler` function at address `0x1c00b8fb8`. Each handler has a size of 8 byte, so calculating `(0x1c00c4b18 - 0x1c00c0b20) / 8 = 0x7ff` and not forgetting the 8 bytes for the last handler, the hanlder table holds `0x800` (2048) handlers. The virtual machine doesn't seem to have a kind of virtual instruction pointer and calculates the next handler based on the last 10 bits of input value (calculated by an bit-wise and `0x7ff`).

### (c) The VM design differs in various aspects from the ones we discussed so far. Name and describe at least two

- It doesn't have a instruction pointer and doesn't implement a classical Fetch-Decode-Execute logic.
- The VM doesn't use bytecode to execute. It initially receives a seed and calculates the next instruction based on the last ten bits of the seed. In the ongoing execution, the following handler is choosen by the last ten bits of the previous handlers output. 
- The VM seems to be using `uint128_t` pointer to allocate the internal storage (properley for registers, stack and/or flags). Due the fact that `uint128_t` isn't a standard C type, it seems like Microsoft implemented a specially designed data structure for storing the VM execution state. 
- It implements some kind of sanitizer that utilize a `secure_cookie` attribute. This "cookie" is properly used to detect tampering and protect the VM from malicious intend. In general it seems that the VM implements several sanity checks as well.

## Ressources

This is a list of ressources I either used directly to solved the tasks above or helped me to understand the VM.

- (https://github.com/airbus-seclab/warbirdvm)[https://github.com/airbus-seclab/warbirdvm]
- (https://cirosec.de/en/news/abusing-microsoft-warbird-for-shellcode-execution/)[https://cirosec.de/en/news/abusing-microsoft-warbird-for-shellcode-execution/]
- (https://downwithup.github.io/blog/post/2023/04/23/post9.html)[https://downwithup.github.io/blog/post/2023/04/23/post9.html]
- (https://blog.xpnsec.com/windows-warbird-privesc/)[https://blog.xpnsec.com/windows-warbird-privesc/]
- (https://synthesis.to/2023/01/26/uncommon_instruction_sequences.html)[https://synthesis.to/2023/01/26/uncommon_instruction_sequences.html]
- (https://synthesis.to/2021/10/21/vm_based_obfuscation.html)[https://synthesis.to/2021/10/21/vm_based_obfuscation.html]