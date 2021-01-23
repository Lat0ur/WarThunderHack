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
    
    MC.InitD3DHook(dllHandle);

    printf("mainCheatThread()\r\n");
    printf("Windows Size: %d %d\r\n", esp.rect.right, esp.rect.bottom);
    while (true){
        if (GetAsyncKeyState(VK_INSERT))
            break;
        if (GetAsyncKeyState(VK_F9))
            esp.drawTracers != esp.drawTracers;
        if (GetAsyncKeyState(VK_NUMPAD9)) {
            esp.k += 1;
            Sleep(200);
        }
        if (GetAsyncKeyState(VK_NUMPAD6)) {
            esp.k -= 1;
            Sleep(200);
        }
        esp.update();
        Sleep(50);
    }

    mainDettach(dllHandle);

    return;
}

DWORD WINAPI Main(HMODULE dllHandle) {


    allocConsole();

    MC.exeBase = (uintptr_t) GetModuleHandleA("aces.exe");
    printf("exeBase at %llx\r\n", MC.exeBase);

    uintptr_t playerListBase = *(uintptr_t*)(MC.exeBase + playerListOffset);
    printf("playerListBase at %llx\r\n", playerListBase);

    MC.viewMatrix = MC.exeBase + viewMatrixOffset;
    printf("MC.viewMatrix at %llx\r\n", MC.viewMatrix);

    MC.pToGameInfo = MC.exeBase + gameInfoOffset;
    printf("MC.pToGameInfo = %llx\r\n", MC.pToGameInfo);
    
    esp.gameInfo = *(GameInfo**)(MC.pToGameInfo);

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

