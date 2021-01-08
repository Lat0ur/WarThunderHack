#include "hook.h"

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return false;

	DWORD oldProtect;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	uintptr_t jmpTargetRelative = dst - src - 5;

	//write the jmp instruction & relative address
	*src = 0xE9;
	*(uintptr_t*)(src + 1) = jmpTargetRelative;

	VirtualProtect(src, len, oldProtect, &oldProtect);
	return true;
}

BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return 0;

	//Create trampoline
	BYTE* pTrampoline = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//write the stolen bytes into the trampoline function
	memcpy_s(pTrampoline, len, src, len);

	//add the jmp opcode after the stolen bytes
	*(pTrampoline + len) = 0xE9;

	//Get the relative address to jmp back to
	uintptr_t jmpBackAddress = src - pTrampoline - 5;

	//Write the jmpBackAddress after the jmp instruction
	*(uintptr_t*)((uintptr_t)pTrampoline + len + 1) = jmpBackAddress;

	//Perform the detour
	Detour32(src, dst, len);

	return pTrampoline;
}

void* TrampHook64(void* src, void* dst, int len)
{
	if (len < 14) return nullptr;

	BYTE stub[14] = {
	0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,				// jmp qword ptr instruction
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 8 byte ptr to jmp destination
	};

	void* pTrampoline = VirtualAlloc(0, len + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD oldProtect = 0;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	uintptr_t jmpBackAddr = (uintptr_t)src + len;

	// copy trampoline jmpback addr to stub
	memcpy(stub + 6, &jmpBackAddr, 8);
	// copy stolen bytes to trampoline
	if (pTrampoline != NULL)
		memcpy((void*)(pTrampoline), src, len);
	// copy stub to trampoline
	memcpy((void*)((uintptr_t)pTrampoline + len), stub, sizeof(stub));

	// copy dst to the stub, creating our jmp to our hook function
	memcpy(stub + 6, &dst, 8);
	// copy new stub to src
	memcpy(src, stub, sizeof(stub));

	// nop any stolen bytes in src
	for (int i = 14; i < len; i++)
	{
		*(BYTE*)((uintptr_t)src + i) = 0x90;
	}

	VirtualProtect(src, len, oldProtect, &oldProtect);
	return (void*)(pTrampoline);
}

Hook::Hook(BYTE* src, BYTE* dst, uintptr_t* pTrampoline, uintptr_t len)
{
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->pTrampoline = pTrampoline;
}

Hook::Hook(const char* exportName, const char* modName, BYTE* dst, uintptr_t* pTrampoline, uintptr_t len)
{
	HMODULE _hModule = GetModuleHandleA(modName);
	if (_hModule != NULL) {
		this->src = (BYTE*)GetProcAddress(_hModule, exportName);
		this->dst = dst;
		this->len = len;
		this->pTrampoline = pTrampoline;
	}
}

void Hook::Enable()
{
	if (!bEnabled)
	{
		memcpy(originalBytes, src, len);

#ifdef _WIN64
		* (uintptr_t*)pTrampoline = (uintptr_t)TrampHook64(src, dst, len);
#else
		* (uintptr_t*)pTrampoline = (uintptr_t)TrampHook32(src, dst, len);
#endif

		bEnabled = true;
	}
}

void Hook::Disable()
{
	if (bEnabled)
	{
		mem::Patch(src, originalBytes, len);
		VirtualFree((void*)*pTrampoline, 0, MEM_RELEASE);
		bEnabled = false;
	}
}

void Hook::Toggle()
{
	if (!bEnabled) Enable();
	else Disable();
}

Hook::~Hook()
{
	if (bEnabled)
	{
		Disable();
	}
}