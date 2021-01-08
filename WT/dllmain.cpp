// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <stdio.h>
#include "coreNotGameSpecific.h"
#include "offsets.h"
#include "rD3D11.h"

rD3D11 rd3d11;

HRESULT __stdcall hookedPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
{
    if (!rd3d11.pDevice)
        rd3d11.InitD3DHook(pThis);

    //enable this to test or debug viewport
    rd3d11.Render();

    return rd3d11.ogPresentTramp(pThis, SyncInterval, Flags);
}

void mainCheatStuff(uintptr_t exeBase, uintptr_t playerListBase, HMODULE dllHandle) {
    rd3d11.installD3DHook();
    
    while (!GetAsyncKeyState(VK_END))
        Sleep(10);

    rd3d11.CleanupD3D();
    return;
}

DWORD WINAPI Main(HMODULE dllHandle) {
    allocConsole();
    uintptr_t exeBase = (uintptr_t) GetModuleHandleA("aces.exe");
    printf("exeBase at %llx\r\n", exeBase);
    uintptr_t playerListBase = *(uintptr_t*)(exeBase + playerListOffset);
    printf("playerListBase at %llx\r\n", playerListBase);

    uintptr_t d3d11 = (uintptr_t)GetModuleHandleA("d3d11.dll");
    printf("d3d11 at %llx\r\n", d3d11);

    mainCheatStuff(exeBase, playerListBase, dllHandle);

    detach(dllHandle);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:       
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

