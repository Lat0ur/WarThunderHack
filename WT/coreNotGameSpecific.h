#pragma once
#include <Windows.h>
#include <TlHelp32.h>

typedef struct D3DXVECTOR3 {
	FLOAT x;
	FLOAT y;
	FLOAT z;
} D3DXVECTOR3, * LPD3DXVECTOR3;

void deattach(HMODULE dllHandle);
void allocConsole();

