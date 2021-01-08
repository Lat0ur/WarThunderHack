#include "coreNotGameSpecific.h"
#include "stdio.h"

void detach(HMODULE dllHandle) {
    //printf("cleanupHook...\r\n");
    //CleanupD3D();
    printf("detaching...\r\n");
    FreeLibraryAndExitThread(dllHandle, 0);
}

void allocConsole() {
    FILE* f;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("------\r\nConsoleAllocated: \r\n");
}