# Sample: `challenge-4-x86_64-Linux.exe`

All extracted information is based on the [analysis](./challenge-4-x86_64-Linux.exe.bndb) I did with BinaryNinja Community. 

## a)

The basic block at `0x407566` calls the standard / extern function `printf`. The call is obfuscted by calculating the pointer to the `printf`-function first. For that, the constant `0x3928b266` is added to the original / un-modified function pointer within the `.data` section. Within the given basic block, the offset is substracted again, what results to the callable `printf` function pointer. This calculation is performed within the `rcx` register, so the function call at address `0x40758c` (`call rcx`) actually calls the the `printf` function. Additionally to that, the arguments of the call seems also be obfuscated as the formatted string seems to be `txhsvcsbpmvgdepbclxusgpgryirvxi`.

## b) 

The control-flow flattening is implemented by a switch-case statement that state depends on the input, which makes it hard to guess what path the program will take. Each code block (or state handler) is obfuscated using MBAs. The handlers also depend on each other. One handler might set a variable that another hanlder uses later. This means you can't analyze them separately—you need to follow the entire flow. There are also indirect function calls like the one in the previous task. It’s used at least five times to call `printf`, once to call `strlen`, and once to call a function at `0x40ce32` that I couldn’t figure out. 

## c) 

The MBAs are used to compute the state variable that determines the next state in the control-flow flattening. At first, the expressions combine arithmetic operations like `+` and `-` with boolean operators such as `&` and `|`, which still look somewhat familiar. Later, the calculation becomes more complex by using the high and low bits of values that were set in other switch cases. This makes the MBA harder to simplify. Our Python script(s) from the previous task(s) can only analyze single basic blocks and don’t track how values are passed between different blocks. Because of that, they can't fully resolve these kinds of cross-block dependencies.

## d) 

To identify a conditional jump within the control-flow flattening, we need to clarify "what" a conditional jump means. In this situation it should be a handler that sets the state based on comparison of a before calculated value. E.g. it checks the value itself or kind of flag registers. So, the first search was for a handler that sets the next state based on a comparison and I decided to analyze the case `0x3290fc25` at basic block `0x402436` furture. 

The comparison checks derived values of the variable `var_374_1`, which is set at runtime. It compares this value with a hardcoded constant: `0x54fd9739` (at address `0x4024a3`). Depending on the result of this comparison, a different value gets assigned to the state variable, which then determines the next state to execute. The outcome of the comparison controls which block runs next.

The commented BinaryNinja High Level IL code for this basic block looks like this:

```C
...

00402452   case 0x3290fc25
00402452       // Maybe a conditional jump:
00402452       // 
00402452       // - compare based on runtime value of var_374_1
00402452       // - compare with hardcoded value 0x54fd9739
00402452       // - state variable depends on variable that
00402452       // depends on comparison
00402452       // 
00402452       // => next state depends on comparison
00402452       // => conditional jumo
00402452       //  
00402452       var_2a0_1[0x10] = var_2a8_1
00402462       var_2a0_1[2] = var_2b4_1
00402472       var_2a0_1[0x12] = var_2b8_1
00402482       var_2a0_1[2].d = var_2bc_1
00402485       int32_t var_0x54fd9739 = 0x54fd9739
00402496       // mods.dp.d <=> modulo / % 
00402496       int32_t rdi_57 = var_374_1 - mods.dp.d(0:0x3ab3217e, 0x7fffffff)
004024a5       r8.b = 0x54fd9739 != rdi_57  // condition
004024a9       r8.b &= 1  // use last bit => boolean
004024ad       char isNot_rdi_57_0x54fd9739 = r8.b
004024b4       r8.b = isNot_rdi_57_0x54fd9739
004024bb       r8.b &= 1  // use last bit again?
004024c3       // sx.q(rdi_57) => to 64bit sign extended
004024c3       int32_t rdi_59 = zx.d(r8.b) * mods.dp.d(sx.q(rdi_57), 0x7fffffff)
004024c6       r8.b = isNot_rdi_57_0x54fd9739
004024cd       r8.b ^= 0xff
004024d1       r8.b &= 1
004024e1       // jump
004024e1       state = rdi_59 + zx.d(r8.b) * 0x3290fc1c
004024e7       continue

...

```
