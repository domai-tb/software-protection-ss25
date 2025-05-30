# Task 3: Real-World VM Analysis I 

>Downloaded `ci.dll` from (iosninja.io)[https://iosninja.io/dll/download/file/28d8d141924242161e727388c4fd77c14c1f771a]

## Interesting Function within the DLL

- `CipInitialize`: Could be the VM init point and hold a reference to other VM functions (?)

```
1c0043c4f                    label_1c0043c4f:
1c0043c4f                    bool cond:0_1 = !g_HvciSupported;
1c0043c5d                    *(uint64_t*)((char*)arg3 + 0x20) = CiValidateImageHeader;
1c0043c68                    *(uint64_t*)((char*)arg3 + 0x28) = CiValidateImageData;
1c0043c73                    *(uint64_t*)((char*)arg3 + 0x18) = CiQueryInformation;
1c0043c7e                    *(uint64_t*)((char*)arg3 + 8) = CiSetFileCache;
1c0043c89                    *(uint64_t*)((char*)arg3 + 0x10) = CiGetFileCache;
1c0043c94                    *(uint64_t*)((char*)arg3 + 0x30) = CiHashMemory;
1c0043c9f                    *(uint64_t*)((char*)arg3 + 0x38) = KappxIsPackageFile;
1c0043caa                    *(uint64_t*)((char*)arg3 + 0x40) = CiCompareSigningLevels;
1c0043cb5                    *(uint64_t*)((char*)arg3 + 0x48) = CiValidateFileAsImageType;
1c0043cc0                    *(uint64_t*)((char*)arg3 + 0x50) = CiRegisterSigningInformation;
1c0043ccb                    *(uint64_t*)((char*)arg3 + 0x58) = CiUnregisterSigningInformation;
1c0043cd6                    *(uint64_t*)((char*)arg3 + 0x60) = CiInitializePolicy;
1c0043ce1                    *(uint64_t*)((char*)arg3 + 0x88) = CipQueryPolicyInformation;
1c0043cef                    *(uint64_t*)((char*)arg3 + 0x90) = CiValidateDynamicCodePages;
1c0043cfd                    *(uint64_t*)((char*)arg3 + 0x98) = CiQuerySecurityPolicy;
1c0043d0b                    *(uint64_t*)((char*)arg3 + 0xa0) = CiRevalidateImage;
1c0043d19                    *(uint64_t*)((char*)arg3 + 0xa8) = CiSetInformation;
1c0043d27                    *(uint64_t*)((char*)arg3 + 0xb0) = CiSetInformationProcess;
1c0043d35                    *(uint64_t*)((char*)arg3 + 0xb8) = CiGetBuildExpiryTime;
1c0043d43                    *(uint64_t*)((char*)arg3 + 0xc0) = CiCheckProcessDebugAccessPolicy;
```

