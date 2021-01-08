#pragma once
#include <Windows.h>
#include "mem.h"

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);
void* TrampHook64(void* src, void* dst, int len);

class Hook
{
public:
	bool bEnabled{ false };

	BYTE* src{ nullptr };
	BYTE* dst{ nullptr };
	uintptr_t* pTrampoline{ 0 };
	uintptr_t len{ 0 };

	BYTE originalBytes[30]{ 0 };

	Hook() {}

	Hook(BYTE* src, BYTE* dst, uintptr_t* pTrampoline, uintptr_t len);
	// hook exported function via GetProcAddress
	Hook(const char* exportName, const char* modName, BYTE* dst, uintptr_t* pTrampoline, uintptr_t len);

	void Enable();
	void Disable();
	void Toggle();

	~Hook();
};