#include<stdlib.h>
#include<stdio.h>

int func(int input) {

    unsigned int local_array[6];

    // Magic Numbers
    local_array[1] = 0x1337;
    local_array[2] = 0xbeef;
    local_array[3] = 0x00;

    // "Special" Cases
    if (input < 0x05) {
        return 0;
    }
    if (input < 0x0a) {
        return 1;
    }

    // Compution
    for (local_array[5] = 0x0a; local_array[5] <= input; local_array[5] += 1) {
        int res = local_array[1] ^ local_array[5];
        int res2 = input + res;
        local_array[3] = res2;

        local_array[1] = local_array[2];
        local_array[2] = local_array[3];
    }
    
    local_array[4] = local_array[3];
    return local_array[4];
}

int main(int argc, char ** argv) {
    int input = atoi(argv[1]);
    printf("result: 0x%x\n", func(input));
}