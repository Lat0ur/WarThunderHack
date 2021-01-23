#pragma once
#include <functional>//
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h> // color https://github.com/microsoft/DirectXMath/
#include "w2sAndUtils.h"
#include "offsets.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define safe_release(p) if (p) { p->Release(); p = nullptr; }
#define PRESENT_STUB_SIZE 5
#define MAINVP 0
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present

struct HandleData {
	DWORD pid;
	HWND hWnd;
};

class MasterClass;

class MasterClass
{
public:
	ID3D11Device* g_pd3dDevice = nullptr;
	IDXGISwapChain* pSwapchain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11VertexShader* pVertexShader = nullptr;
	ID3D11InputLayout* pVertexLayout = nullptr;
	ID3D11PixelShader* pPixelShader = nullptr;
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
	ID3D11Buffer* pConstantBuffer = nullptr;

	HWND hWnd;
	RECT windowRect;
	
	uintptr_t exeBase;
	uintptr_t viewMatrix;
	uintptr_t pToGameInfo;

	D3D11_VIEWPORT pViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{ 0 };
	D3D11_VIEWPORT myViewport;
	DirectX::XMMATRIX mOrtho;

	//hook stuff
	using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
	void* originalPresentFcn;					// Pointer to the original Present function
	fnPresent ogPresentTramp;		// Pointer to our trampoline

	void* pTrampoline = nullptr;		// Pointer to jmp instruction in our trampoline that leads to hkPresent
	char originalPresentBytes[PRESENT_STUB_SIZE];	// Buffer to store original bytes from Present

	void InitD3DHook(HMODULE hModule);

};

extern MasterClass MC;

HWND FindMainWindow(DWORD dwPID);