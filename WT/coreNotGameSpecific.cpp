#include "coreNotGameSpecific.h"
#include "stdio.h"
#include "d3d11hook.h"
#include "rD3D11.h"
#include "ESP.h"

void deattach(HMODULE dllHandle) {
    //printf("cleanupHook...\r\n");
    //CleanupD3D();
    printf("detaching...\r\n");
    //HookDX11_Release();
    FreeLibraryAndExitThread(dllHandle, 0);
}

void allocConsole() {
    
    GetClientRect(FindMainWindow(GetCurrentProcessId()), &esp.rect);
    esp.screenCenterX = esp.rect.right / 2;
    esp.screenCenterY = esp.rect.bottom / 2;
    FILE* f;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("------\r\nConsoleAllocated: \r\n");
}