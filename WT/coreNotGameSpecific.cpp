#include "coreNotGameSpecific.h"
#include "stdio.h"
#include "d3d11hook.h"

void deattach(HMODULE dllHandle) {
    //printf("cleanupHook...\r\n");
    //CleanupD3D();
    printf("detaching...\r\n");
    //HookDX11_Release();
    FreeLibraryAndExitThread(dllHandle, 0);
}

void allocConsole() {
    FILE* f;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("------\r\nConsoleAllocated: \r\n");
}