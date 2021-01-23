#include "rD3D11.h"
#include "structs.h"
#include "imgui_impl_dx11.h"
#include "d3d11hook.h"

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
	HandleData& data = *(HandleData*)lParam;
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (pid == data.pid && GetWindow(hWnd, GW_OWNER) == HWND(0) && IsWindowVisible(hWnd))
	{
		data.hWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}

HWND FindMainWindow(DWORD dwPID)
{
	HandleData handleData{ 0 };
	handleData.pid = dwPID;
	EnumWindows(EnumWindowsCallback, (LPARAM)&handleData);
	return handleData.hWnd;
}


void MasterClass::InitD3DHook(HMODULE hModule) {
	ImplHookDX11_Init(hModule, FindMainWindow(GetCurrentProcessId()));
}