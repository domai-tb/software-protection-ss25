char magic_numer_-0x35 = -0x35
char magic_numer_0x16 = 0x16
char magic_numer_-0x36 = -0x36
char magic_numer_-0x35_2 = -0x35
int16_t magic_numer_-0x3435 = -0x3435

uint64_t vm_routine(int32_t arg1)
{  // VM context setup
    int64_t i = 0;
    int32_t input_num = arg1;
    void dec_bytecode;
    void* vip = &dec_bytecode;
    
    // Decrypt / XOR bytecode with hardcoded
    // key 
    do
    {
        *(uint64_t*)(&dec_bytecode + i) =
            *(uint64_t*)(&enc_bytecode + i)
            ^ 0xcbcbcbcbcbcbcbcb;
        i += 8;
    } while (i != 0x298);
    
    // VM register setup with magic numbers
    int16_t rax;
    rax = magic_numer_-0x35 ^ 0xcb;
    *(uint8_t*)((char*)rax)[1] =
        magic_numer_0x16 ^ 0xcb;
    int32_t var_8 = (uint32_t)rax
        | ((uint32_t)magic_numer_-0x36 ^ 0xcb)
        << 0x10 | ((uint32_t)magic_numer_-0x35_2
        ^ 0xffffffcb) << 0x18;
    int16_t var_4 =
        magic_numer_-0x3435 ^ 0xcbcb;
    // Fetch opcode byte from dec_bytecode at
    // vip
    next_op:
    uint8_t op = *(uint8_t*)vip;
    check_if_halt:
    bool op_gt_0x80 = op > 0x80;
    // Base of VM register file (array of
    // 64-bit slots)
    int32_t registers_array;
    
    if (op != 0x80)
    {
        // If opcode is not the special 0x80
        // (exit / halt signal)
        // => start dispatcher loop
        while (true)
        {
            if (op_gt_0x80)
            {
                // Return given register 
                if (op == 0xdd)
                    return (uint64_t)(&registers_array)[(
                        int64_t)*(uint32_t*)((char*)vip + 1)
                        * 2];
                
                if (op > 0xdd)
                {
                    if (op == 0xf1)
                    {
                        // XOR: registers[X]
                        // = registers[Y] ^
                        // registers[Z]
                        int64_t reg_y =
                            (int64_t)*(uint32_t*)((char*)vip + 5);
                        int64_t reg_z =
                            (int64_t)*(uint32_t*)((char*)vip + 1);
                        // vip += 13
                        vip += 0xd;
                        (&registers_array)[(int64_t)
                            *(uint32_t*)((char*)vip - 4) * 2] =
                            (&registers_array)[reg_y * 2]
                            ^ (&registers_array)[reg_z * 2];
                        goto next_op;
                    }
                    
                    if (op != 0xf6)
                        goto check_if_halt;
                    
                    // Load pointer from src
                    // into dest register
                    // => MOV
                    int64_t dest_reg =
                        (int64_t)*(uint32_t*)((char*)vip + 5);
                    int64_t src_reg =
                        (int64_t)*(uint32_t*)((char*)vip + 1);
                    vip += 9;
                    **(uint32_t**)(
                        &registers_array + (dest_reg << 3)) =
                        (&registers_array)[src_reg * 2];
                    goto next_op;
                }
                
                if (op == 0xc2)
                {
                    // CMPLE: reg[X] = reg[Y]
                    // < reg[Z]
                    int64_t reg_x =
                        (int64_t)*(uint32_t*)((char*)vip + 1);
                    int32_t reg_y_2;
                    reg_y_2 = (&registers_array)[(int64_t)
                        *(uint32_t*)((char*)vip + 5) * 2] <= (
                        &registers_array)[(int64_t)
                        *(uint32_t*)((char*)vip + 9) * 2];
                    vip += 0xd;
                    (&registers_array)[reg_x * 2] =
                        (uint32_t)reg_y_2;
                    goto next_op;
                }
                
                if (op != 0xd5)
                    goto check_if_halt;
                
                // MOVI: Load immediate into
                // dest register
                int64_t dest_reg_2 =
                    (int64_t)*(uint32_t*)((char*)vip + 1);
                int32_t immediate =
                    *(uint32_t*)((char*)vip + 5);
                vip += 9;
                (&registers_array)[dest_reg_2 * 2] =
                    immediate;
                goto next_op;
            }
            
            // mov dest_idx src_idx
            // move value of dest_idx into
            // src_idx  
            if (op == 0x30)
            {
                int64_t src_idx =
                    (int64_t)*(uint32_t*)((char*)vip + 5);
                int64_t dest_idx =
                    (int64_t)*(uint32_t*)((char*)vip + 1);
                vip += 9;
                (&registers_array)[dest_idx * 2] = **(
                    uint32_t**)(
                    &registers_array + (src_idx << 3));
                goto next_op;
            }
            
            if (op <= 0x30)
            {
                if (op != 0xe)
                {
                    // Computing a memory
                    // address by combining
                    // the given offset with
                    // a constant. Stores the
                    // computed address into
                    // the specified
                    // register. 
                    if (op == 0x26)
                    {
                        void local_var;
                        *(uint64_t*)(&registers_array + (
                            (int64_t)*(uint32_t*)((char*)vip + 5)
                            << 3)) =
                            (int64_t)*(uint32_t*)((char*)vip + 1) +
                            &local_var;
                        goto next_op_or_return;
                    }
                    
                    // if op == 0x00 do
                    // redispatching
                    if (op)
                        goto check_if_halt;
                    
                    // Perform a less-than
                    // comparison
                    // => set flags
                    int64_t src_reg_2 =
                        (int64_t)*(uint32_t*)((char*)vip + 9);
                    int32_t less_flag;
                    less_flag = (&registers_array)[(int64_t)
                        *(uint32_t*)((char*)vip + 1) * 2] < (
                        &registers_array)[(int64_t)
                        *(uint32_t*)((char*)vip + 5) * 2];
                    vip += 0xd;
                    (&registers_array)[src_reg_2 * 2] =
                        (uint32_t)less_flag;
                    goto next_op;
                }
                
                // if register at source
                // register (vip + 5) is zero
                // if (!value) → if (value
                // == 0)
                // 
                // => Jump not zero
                if (!(&registers_array)[(int64_t)
                    *(uint32_t*)((char*)vip + 5) * 2])
                {
                    next_op_or_return:
                    op = *(uint8_t*)((char*)vip + 9);
                    vip += 9;
                    op_gt_0x80 = op > 0x80;
                    
                    // halt / stop execution
                    if (op == 0x80)
                        break;
                    
                    continue;
                }
            }
            else if (op != 0x51)
            {
                if (op != 0x66)
                    goto check_if_halt;
                
                // ADD: reg[dst] = reg[X] +
                // reg[Y]
                int64_t reg_x_2 =
                    (int64_t)*(uint32_t*)((char*)vip + 5);
                int64_t reg_y_3 =
                    (int64_t)*(uint32_t*)((char*)vip + 1);
                vip += 0xd;
                (&registers_array)[(int64_t)
                    *(uint32_t*)((char*)vip - 4) * 2] =
                    (&registers_array)[reg_y_3 * 2]
                    + (&registers_array)[reg_x_2 * 2];
                goto next_op;
            }
            
            // Next opcode / vip+1
            vip = (char*)vip
                + (int64_t)*(uint32_t*)((char*)vip + 1)
                + 1;
            goto next_op;
        }
    }
    
    // init check? => operand != 0
    if (*(uint32_t*)((char*)vip + 5))
        goto next_op_or_return;
    
    // Store input value in register 
    // => Calling convention that input must
    // be in this register (?)
    *(uint64_t*)(&registers_array + (
        (int64_t)*(uint32_t*)((char*)vip + 1)
        << 3)) = &input_num;
    goto next_op_or_return;
}


int32_t main(int32_t argc, char** argv, char** envp)
{
    arguments_cnt = argc;
    arguments = argv;
    environment = envp;
    
    if (argc <= 1)
        return 0xffffffff;
    
    printf("result: 0x%lx\n", (uint64_t)vm_routine(atoi(argv[1])));
    return 0;
}
