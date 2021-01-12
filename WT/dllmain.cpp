// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <stdio.h>
#include "offsets.h"
#include "d3d11hook.h"
#include "coreNotGameSpecific.h"
#include "rD3D11.h"
#include "d3d11hook.h"
#include "ESP.h"

#pragma comment(lib, "libMinHook-x64-v140-mdd.lib")

MasterClass MC;

void mainCheatThread(uintptr_t exeBase, uintptr_t playerListBase, HMODULE dllHandle) {
    printf("mainCheatThread()\r\n");

    
    
    while (true){
        if (GetAsyncKeyState(VK_DELETE))
            break;
        if (GetAsyncKeyState(VK_F9))
            esp.drawTracers != esp.drawTracers;
        esp.update();
        Sleep(5);
    }

    mainDettach(dllHandle);

    return;
}

DWORD WINAPI Main(HMODULE dllHandle) {

    MC.InitD3DHook(dllHandle);

    allocConsole();

    MC.exeBase = (uintptr_t) GetModuleHandleA("aces.exe");
    printf("exeBase at %llx\r\n", MC.exeBase);

    uintptr_t playerListBase = *(uintptr_t*)(MC.exeBase + playerListOffset);
    printf("playerListBase at %llx\r\n", playerListBase);

    MC.viewMatrix = MC.exeBase + viewMatrixOffset;
    printf("rd3d11.viewMatrix at %llx\r\n", MC.viewMatrix);
    
    mainCheatThread(MC.exeBase, playerListBase, dllHandle);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:       
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

