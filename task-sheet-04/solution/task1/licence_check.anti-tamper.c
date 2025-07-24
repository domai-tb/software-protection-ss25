#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <signal.h>

// Anti-debug detection globals
#define DEBUGGER_DETECTED (trapCount == 0)
static int trapCount = 0;
static long validChecksumBytecode = 1678863;
static long validChecksumMain = 290344834828;
static long validChecksumValidate = 32423511090;

static void debugTrapHandler(int signo) {
    trapCount++;
}

static inline void executeCpuid(unsigned int *eax, unsigned int *ebx, 
                               unsigned int *ecx, unsigned int *edx) {
    asm volatile("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (*eax), "2" (*ecx));
}

// Runtime string decryption
void decryptString(char *encrypted, char *output, char *key, int length) {
    for (int i = 0; i < length; ++i)
        output[i] = encrypted[i] ^ key[i % 19];
    output[length] = 0x00;
}

// Checksum calculation for tamper detection
long calculateMemoryChecksum(long startValue, long startAddr, long endAddr) {
    unsigned char *ptr;
    long checksum = startValue;
    for (ptr = (unsigned char *)startAddr; ptr < (unsigned char *)endAddr; ptr++) {
        checksum += *ptr;
    }
    return checksum;
}

// VM detection via hypervisor bit
int detectHypervisor() {
    unsigned eax, ebx, ecx, edx;
    eax = 1;
    executeCpuid(&eax, &ebx, &ecx, &edx);
    return (ecx >> 31) & 0x01;
}

// VM detection via unsupported CPUID leaf
int detectVirtualMachine() {
    unsigned eax, ebx, ecx, edx;
    eax = 0x40000000;
    executeCpuid(&eax, &ebx, &ecx, &edx);
    return !(eax == 0 && ebx == 0 && ecx == 0 && edx == 0);
}

// Anti-debug check using signal trap
int performTrapDetection(char* libcPath, char* signalFunc, char* alarmFunc) {
    void* handle = dlopen(libcPath, RTLD_LAZY);
    int (*setSignal)(int, void (*)(int)) = dlsym(handle, signalFunc);
    int (*sendAlarm)(int) = dlsym(handle, alarmFunc);
    
    if (setSignal(5, debugTrapHandler) == -1) return 0;
    sendAlarm(5);
    return 1;
}

// Main license validation with anti-debug protection
int validate(char *username, char *license, char* libcPath, char* strlenFunc, char* ptraceFunc) {
    void* handle = dlopen(libcPath, RTLD_LAZY);
    int (*ptraceCall)(int, int, int, int) = dlsym(handle, ptraceFunc);
    
    // Anti-debug check using ptrace
    int debugCheck = ptraceCall(0, 0, 1, 0);

    // VM detection check
    int vmCheck = detectHypervisor();
    if (debugCheck < 0 || vmCheck == 1) {
        return 0;
    }
    
    // Dynamic API resolution
    int (*getStringLength)(char *) = dlsym(handle, strlenFunc);
    if (getStringLength(username) != getStringLength(license)) {
        return 0;
    }
    else if (getStringLength(username) < 2) {
        return 0;
    }
    
    int i = 0;
    for (i = 0; i < getStringLength(username); ++i) {
        license[i] ^= 0x17;
        if (username[i] != license[i]) {
            return 0;
        }
    }

    // Close lib handler
    dlclose(handle);

    return 1;
}

// Custom VM implementation for checksum verification
void executeChecksumVM(char* libcPath, char* printfFunc, char* invalidMsg, 
                      char* exitFunc, char* checksumMsg, long startAddr) {
    void* handle = dlopen(libcPath, RTLD_LAZY);
    void (*printMessage)(char *) = dlsym(handle, printfFunc);
    void (*exitProgram)(int) = dlsym(handle, exitFunc);
    void (*printChecksum)(char *, long) = dlsym(handle, printfFunc);
    
    // VM bytecode for checksum verification
    long vmBytecode[27] = {
        0x00, 0x00, 0x158391,
        0x01, 0x00, startAddr,
        0x02, 0x00, startAddr + 1846,
        0x03, 0x13,
        0x05, 0x00, 0x00,
        0x04, 0x00, 0x01,
        0x06, 0x06,
        0x08, 0x00, validChecksumBytecode,
        0x09, 0x19,
        0x07,
        0x0A,
        0x0B
    };
    
    int instructionPointer = 0;
    unsigned char* charRegisters[256];
    long longRegisters[256];
    unsigned int zeroFlag, compareFlag;
    long opcode;
    unsigned int reg0, reg1, jumpTarget;
    unsigned long constant;
    
    while (1) {
        if (instructionPointer < 0 || instructionPointer > 27) {
            return;
        }
        
        opcode = vmBytecode[instructionPointer];
        
        switch (opcode) {
            case 0x0: // MOV long register, constant
                reg0 = vmBytecode[instructionPointer + 1];
                constant = vmBytecode[instructionPointer + 2];
                longRegisters[reg0] = constant;
                instructionPointer += 3;
                break;
                
            case 0x1: // MOV char pointer register, constant
                reg0 = vmBytecode[instructionPointer + 1];
                constant = vmBytecode[instructionPointer + 2];
                charRegisters[reg0] = (unsigned char *)constant;
                instructionPointer += 3;
                break;
                
            case 0x2: // CMP char pointer register, constant
                reg0 = vmBytecode[instructionPointer + 1];
                constant = vmBytecode[instructionPointer + 2];
                if (charRegisters[reg0] > (unsigned char *)constant) {
                    zeroFlag = 0;
                    compareFlag = 0;
                } else if (charRegisters[reg0] < (unsigned char *)constant) {
                    zeroFlag = 0;
                    compareFlag = 1;
                } else {
                    zeroFlag = 1;
                    compareFlag = 0;
                }
                instructionPointer += 3;
                break;
                
            case 0x3: // JGE
                jumpTarget = vmBytecode[instructionPointer + 1];
                if (compareFlag == 0) {
                    instructionPointer = jumpTarget;
                } else {
                    instructionPointer += 2;
                }
                break;
                
            case 0x4: // ADD char pointer register, constant
                reg0 = vmBytecode[instructionPointer + 1];
                constant = vmBytecode[instructionPointer + 2];
                charRegisters[reg0] += constant;
                instructionPointer += 3;
                break;
                
            case 0x5: // ADD long register, dereferenced char register
                reg0 = vmBytecode[instructionPointer + 1];
                reg1 = vmBytecode[instructionPointer + 2];
                longRegisters[reg0] += *charRegisters[reg1];
                instructionPointer += 3;
                break;
                
            case 0x6: // JMP
                jumpTarget = vmBytecode[instructionPointer + 1];
                instructionPointer = jumpTarget;
                break;
                
            case 0x7: // VM EXIT
                return;
                
            case 0x8: // CMP long register, constant
                reg0 = vmBytecode[instructionPointer + 1];
                constant = vmBytecode[instructionPointer + 2];
                if (longRegisters[reg0] > constant) {
                    zeroFlag = 0;
                    compareFlag = 0;
                } else if (longRegisters[reg0] < constant) {
                    zeroFlag = 0;
                    compareFlag = 1;
                } else {
                    zeroFlag = 1;
                    compareFlag = 0;
                }
                instructionPointer += 3;
                break;
                
            case 0x9: // JNE
                jumpTarget = vmBytecode[instructionPointer + 1];
                if (zeroFlag == 1) {
                    instructionPointer += 2;
                } else {
                    instructionPointer = jumpTarget;
                }
                break;
                
            case 0xA: // PRINT ERROR
                printMessage(invalidMsg);
                printChecksum(checksumMsg, longRegisters[reg0]);
                instructionPointer += 1;
                break;
                
            case 0xB: // EXIT PROGRAM
                exitProgram(0);
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        return -1;
    }
    
    char *username = argv[1];
    char *license = argv[2];
    
    // Decryption key
    char decryptionKey[20] = {0x78, 0x63, 0x6e, 0x76, 0x72, 0x6a, 0x66, 0x6f, 
                             0x33, 0x39, 0x38, 0x34, 0x39, 0x39, 0x28, 0x39, 
                             0x24, 0x29, 0x29};

    // Encrypted strings
    char userPromptEnc[10] = {0x0d, 0x10, 0x0b, 0x04, 0x48, 0x4a, 0x43, 0x1c, 0x39};
    char licensePromptEnc[13] = {0x14, 0x0a, 0x0d, 0x13, 0x1c, 0x19, 0x03, 0x55, 0x13, 0x1c, 0x4b, 0x3e};
    char newlineEnc[2] = {0x72};
    char validMsgEnc[19] = {0x34, 0x0a, 0x0d, 0x13, 0x1c, 0x19, 0x03, 0x4f, 0x52, 0x5a, 0x5b, 0x51, 0x49, 0x4d, 0x4d, 0x5d, 0x0a, 0x23};
    char invalidMsgEnc[81] = {0x31, 0x0d, 0x18, 0x17, 0x1e, 0x03, 0x02, 0x4f, 0x5f, 0x50, 0x5b, 0x51, 0x57, 0x4a, 0x4d, 0x17, 0x04, 0x79, 0x45, 0x1d, 0x02, 0x1d, 0x13, 0x52, 0x08, 0x13, 0x16, 0x13, 0x56, 0x4d, 0x46, 0x19, 0x4a, 0x47, 0x5f, 0x50, 0x5e, 0x48, 0x0a, 0x06, 0x4e, 0x17, 0x1c, 0x0e, 0x46, 0x1c, 0x52, 0x4f, 0x5d, 0x14, 0x56, 0x4c, 0x5a, 0x19, 0x47, 0x41, 0x40, 0x14, 0x07, 0x1c, 0x13, 0x1c, 0x4a, 0x00, 0x1d, 0x5c, 0x54, 0x18, 0x47, 0x4d, 0x58, 0x5a, 0x4f, 0x45, 0x5d, 0x40, 0x17, 0x0d, 0x40, 0x7c};
    char libcPathEnc[15] = {0x57, 0x0f, 0x07, 0x14, 0x5d, 0x06, 0x0f, 0x0d, 0x50, 0x17, 0x4b, 0x5b, 0x17, 0x0f};
    char printfEnc[7] = {0x08, 0x11, 0x07, 0x18, 0x06, 0x0c};
    char sprintfEnc[7] = {0x15, 0x06, 0x03, 0x05, 0x17, 0x1e};
    char strlenEnc[7] = {0x0b, 0x17, 0x1c, 0x1a, 0x17, 0x04};
    char signalEnc[7] = {0x0b, 0x0a, 0x09, 0x18, 0x13, 0x06};
    char alarmEnc[6] = {0x0a, 0x02, 0x07, 0x05, 0x17};
    char ptraceEnc[7] = {0x08, 0x17, 0x1c, 0x17, 0x11, 0x0f};
    char exitEnc[5] = {0x1d, 0x1b, 0x07, 0x02};
    char checksumEnc[6] = {0x5d, 0x0f, 0x0a, 0x7c};

    // Decrypt all strings at runtime
    char userPrompt[10], licensePrompt[13], newline[2], validMsg[19];
    char invalidMsg[81], libcPath[15], printfStr[7], sprintfStr[7];
    char strlenStr[7], signalStr[7], alarmStr[6], ptraceStr[7];
    char exitStr[5], checksumStr[6];

    decryptString(userPromptEnc, userPrompt, decryptionKey, 9);
    decryptString(licensePromptEnc, licensePrompt, decryptionKey, 12);
    decryptString(newlineEnc, newline, decryptionKey, 1);
    decryptString(validMsgEnc, validMsg, decryptionKey, 18);
    decryptString(invalidMsgEnc, invalidMsg, decryptionKey, 80);
    decryptString(libcPathEnc, libcPath, decryptionKey, 14);
    decryptString(printfEnc, printfStr, decryptionKey, 6);
    decryptString(sprintfEnc, sprintfStr, decryptionKey, 6);
    decryptString(strlenEnc, strlenStr, decryptionKey, 6);
    decryptString(signalEnc, signalStr, decryptionKey, 6);
    decryptString(alarmEnc, alarmStr, decryptionKey, 5);
    decryptString(ptraceEnc, ptraceStr, decryptionKey, 6);
    decryptString(exitEnc, exitStr, decryptionKey, 4);
    decryptString(checksumEnc, checksumStr, decryptionKey, 4);

    // Dynamic API resolution to hide imports
    void* handle = dlopen(libcPath, RTLD_LAZY);
    void (*printFunc)(char *) = dlsym(handle, printfStr);
    void (*printFormatted)(char *, char *) = dlsym(handle, printfStr);
    void (*sprintfFunc)(char *, char *, int) = dlsym(handle, sprintfStr);
    void (*printChecksumFunc)(char *, long) = dlsym(handle, printfStr);

    // Anti-debug trap detection
    performTrapDetection(libcPath, signalStr, alarmStr);
    
    printFormatted(userPrompt, username);
    printFormatted(licensePrompt, license);
    printFunc(newline);

    // VM-based checksum verification
    executeChecksumVM(libcPath, printfStr, invalidMsg, exitStr, checksumStr, (long)&main);

    // Code integrity check for main function conditional jump
    // Offsets are calculated manually by using the output of objdump.
    long mainChecksum = calculateMemoryChecksum(290344782374, (long)&main + 1391, (long)&main + 1749);
    if (mainChecksum != validChecksumMain) {
        printFunc(invalidMsg);
        printChecksumFunc(checksumStr, mainChecksum);
        return 0;
    }
    
    // Code integrity check for validate function
    // Offsets are calculated manually by using the output of objdump.
    long validateChecksum = calculateMemoryChecksum(32423466109, (long)&validate, (long)&validate + 387);
    if (validateChecksum != validChecksumValidate) {
        printFunc(invalidMsg);
        printChecksumFunc(checksumStr, validateChecksum);
        return 0;
    }

    // Second VM detection technique
    int vmDetected = detectVirtualMachine();
    
    // Anti-debug response - modify input if debugger detected
    if (DEBUGGER_DETECTED == 1) {
        username[0] = 0x41;
        license[0] = 0x41;
    }
    
    // Final validation with multiple protection layers
    int check = validate(username, license, libcPath, strlenStr, ptraceStr);
    if (vmDetected == 1 || check == 0) {
        printFunc(invalidMsg);
    } else {
        printFunc(validMsg);
    }
    
    // Prevent segfault in VM environments
    if (vmDetected != 1) {
        dlclose(handle);
    }
    
    return 0;
}