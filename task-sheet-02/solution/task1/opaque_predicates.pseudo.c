int64_t foo(int32_t arg1, int32_t arg2)
{
    // Gate 1
    if (arg1 + 0x6333116f != arg1 + 0x7fc4d70d && arg1 + 0x1e73c4e8 != arg1 - 0x65668efb
            && arg1 - 0x152084f != arg1 + 0x33359581
            && arg1 - 0x12fab079 != arg1 + 0x54461ddc
            && arg1 + 0x7a465cb4 != arg1 + 0xa9fd668
            && arg1 + 0x3c255b90 != arg1 - 0x2f7af76c
            && arg1 - 0x1fc6f708 != arg1 + 0x51206ad9
            && arg1 + 0x78c4a9cc != arg1 - 0x3b5d009
            && arg1 - 0x7eb342ab != arg1 - 0x2fc4fd72
            && arg1 - 0x4134923c != arg1 - 0x68cb4cf8
            && arg1 - 0x60f699be != arg1 + 0x3ae78681
            && arg1 + 0x2cbbc394 != arg1 - 0x71cd1f9b && arg2 + arg1 != 0x539)
        return 0; // Return "False"
    
    // Gate 2
    if (arg1 - 0x20db6dc8 == arg1 + 0x27e7cbe6 || arg1 + 0x71883170 == arg1 - 0x2b2b8d50
        || arg1 + 0x1aa36e28 == arg1 + 0x755012ef
        || arg1 + 0x6d192448 == arg1 - 0x3863b5cb
        || arg1 - 0x74b7b5a8 == arg1 - 0x5c72a4a5
        || arg1 + 0x41b5131b == arg1 - 0x643cfa6 || !((arg1
        - (arg1 & (((-(arg1) - arg1 * 2 - 0x3bea0190) & 0x82d220d9) ^ arg1))
        + 0x4569ddae) & 0xc4b32e67) || arg1 + 0x5de28ba7 == arg1 - 0x531cb719
        || !(arg1 | (((arg1 | 0xfbe57ffc) - 0xd0648b6) & 0xc0bffb7c))
        || !((arg1 * 2 * arg1 * 0xd2517ce0 - 0x43e3bcd) & 0xa6418a03)
        || arg1 - 0x31f4909d == arg1 - 0x16047231
        || arg1 - 0xcee3158 == arg1 - 0x5b0e5670
        || arg1 - 0x65692dbf == arg1 - 0x35419ba9
        || arg1 - 0x5ce00f93 == arg1 - 0x7ebe8540
        || arg1 - 0x1a5c02ee == arg1 - 0x10a7b9ae
        || arg1 + 0xe2854a5 == arg1 + 0x244e9597
        || arg1 + 0x74c806 == arg1 + 0x2f1ffaa0
        || arg1 + 0x2a8ffe14 == arg1 + 0x4e5c26c6
        || arg1 + 0x212020ff == arg1 + 0x3da2cfa4)
    {
        // Gate 2.1
        if ((arg1 - (arg1 ^ 0x48e0b58b)) & 0xfc39d2a0
                && arg1 - 0x46f6435c != arg1 - 0x567a032d
                && arg1 + 0xfb57973 != arg1 - 0x4ee56dd2
                && arg1 + 0x71c80c6b != arg1 - 0x2f035089
                && arg1 - 0x72edda09 != arg1 - 0x47ab07e6
                && arg1 - 0x3aac4a6a != arg1 - 0x1c335d85
                && arg1 - 0x3634c60b != arg1 - 0x5c01bc31
                && arg1 - 0x26c292d7 != arg1 + 0xb1e08b
                && arg1 + 0x66a5373 != arg1 + 0x278d1d8
                && arg1 - 0x6ab32bda != arg1 + 0x18e3dfbf
                && arg1 + 0x699ab465 != arg1 + 0x52e4f9e8
                && arg1 - 0x2be2ba58 != arg1 + 0x1045b040 && arg2 & 1)
            return 0; // Return "False"

        // Gate 2.2
    } else if (arg1 <= arg2 || ((arg1 - (arg1 ^ 0x48e0b58b)) & 0xfc39d2a0
            && arg1 - 0x46f6435c != arg1 - 0x567a032d
            && arg1 + 0xfb57973 != arg1 - 0x4ee56dd2
            && arg1 + 0x71c80c6b != arg1 - 0x2f035089
            && arg1 - 0x72edda09 != arg1 - 0x47ab07e6
            && arg1 - 0x3aac4a6a != arg1 - 0x1c335d85
            && arg1 - 0x3634c60b != arg1 - 0x5c01bc31
            && arg1 - 0x26c292d7 != arg1 + 0xb1e08b
            && arg1 + 0x66a5373 != arg1 + 0x278d1d8
            && arg1 - 0x6ab32bda != arg1 + 0x18e3dfbf
            && arg1 + 0x699ab465 != arg1 + 0x52e4f9e8
            && arg1 - 0x2be2ba58 != arg1 + 0x1045b040 && arg2 & 1))
        return 0; // Return "False"
    
    // Gate 3
    if (arg1 + 0x79d343ac != arg1 - 0x6acef598 && arg1 + 0x64e9a3d4 != arg1 - 0x60a4dad9
            && arg1 + 0x1b88d86f != arg1 + 0x6dc58f02
            && arg1 + 0x33e49bc4 != arg1 - 0x78a70645 && arg1 | (arg1 - 0x52ab8ff)
            && arg1 + 0x7c3d7ab1 != arg1 - 0x6cf9b402
            && arg1 - 0x5c40682 != arg1 - 0x7ce7c3e6 && (arg1 + (((((arg1 * 2 * arg1)
            | 0x6421ca5) & arg1 & 0x53227548) - 0x43892b62) ^ 0x53421904)) * 0x2a290737
            && arg1 + 0x14af0faf != arg1 + 0x5d420089
            && arg1 - 0x3f188c24 != arg1 - 0x6448a6e0
            && arg1 - 0x5ea3f324 != arg1 - 0x38741798
            && arg1 - 0x19c46130 != arg1 - 0x727819d9 && ((arg1
            ^ ((((arg1 * 0x764b2050) | 0x6f04f6ac) - arg1 - 0x633efa21) * arg1))
            | 0x743df522) + arg1 != arg1 && arg1 + 0x44488420 != arg1 - 0x31eefe6b
            && arg1 + 0x686bcc8d != arg1 + 0x4b315387
            && arg1 - 0x5ecf5ba2 != arg1 - 0x51847f60
            && arg1 - 0x2037cb86 != arg1 + 0x4ac63c47 && (((((arg1 * 2 + 0x73bb4c4e)
            ^ arg1) & 0xc266cd96) * 0x2cf73822) | 0xe81b6729) + arg1 != arg1
            && arg1 - 0x6e37df50 != arg1 - 0x71208399
            && arg1 * 5 + 0xa != arg2 * 6 + 7)
        return 0; // Return "False"
    
    // Return "True"
    return 1;
}

int32_t main(int32_t argc, char** argv, char** envp)
{
    // convert argv[1], argv[2] to integers and test them
    if (foo(atoi(argv[1]), atoi(argv[2])))
    {
        puts("Correct!");
        return 0;
    }
    
    puts("Wrong!");
    return -1;
}
