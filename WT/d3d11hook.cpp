#include "stdafx.h"
#include <d3d11.h>
#include <mutex>

#include <MinHook.h>

#include "d3d11hook.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ESP.h"



#pragma comment(lib, "d3d11.lib")

// KeyBoard Options.
const int OpenMenuKey = VK_F9;
const int UninjectLibraryKey = VK_DELETE;


typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef void(__stdcall *D3D11DrawIndexedHook) (ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef void(__stdcall *D3D11CreateQueryHook) (ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery);
typedef void(__stdcall *D3D11PSSetShaderResourcesHook) (ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
typedef void(__stdcall *D3D11ClearRenderTargetViewHook) (ID3D11DeviceContext* pContext, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4]);

static HWND                     g_hWnd = nullptr;
static HMODULE					g_hModule = nullptr;
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static std::once_flag           g_isInitialized;

D3D11PresentHook                phookD3D11Present = nullptr;
D3D11DrawIndexedHook            phookD3D11DrawIndexed = nullptr;
D3D11CreateQueryHook			phookD3D11CreateQuery = nullptr;
D3D11PSSetShaderResourcesHook	phookD3D11PSSetShaderResources = nullptr;
D3D11ClearRenderTargetViewHook  phookD3D11ClearRenderTargetViewHook = nullptr;

DWORD_PTR*                         pSwapChainVTable = nullptr;
DWORD_PTR*						   pDeviceVTable = nullptr;
DWORD_PTR*                         pDeviceContextVTable = nullptr;

ID3D11DepthStencilState* m_DepthStencilState;
BOOL g_bInitialised = false;
bool g_PresentHooked = false;
ID3D11RenderTargetView* mainRenderTargetView;
static IDXGISwapChain* pSwapChain = NULL;
static WNDPROC OriginalWndProcHandler = nullptr;
HWND window = nullptr;


HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain* pSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	HRESULT ret = pSwapChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)ppDevice);

	if (SUCCEEDED(ret))
		(*ppDevice)->GetImmediateContext(ppContext);

	return ret;
}

HRESULT __stdcall PresentHook(IDXGISwapChain* pChain, UINT SyncInterval, UINT Flags)
{
	if (!g_bInitialised) {
		g_PresentHooked = true;
		printf("[+] Present Hook called by first time\r\n");
		if (FAILED(GetDeviceAndCtxFromSwapchain(pChain, &g_pd3dDevice, &g_pd3dContext)))
			return phookD3D11Present(pChain, SyncInterval, Flags);
		pSwapChain = pChain;
		DXGI_SWAP_CHAIN_DESC sd;
		pChain->GetDesc(&sd);
		ImGui::CreateContext();
		window = sd.OutputWindow;


		// Disabling Z-Buffering
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


		g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext);

		ID3D11Texture2D* pBackBuffer;

		pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		pBackBuffer->Release();

		g_bInitialised = true;

	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();

	ImGui::NewFrame();
	//Menu is displayed when g_ShowMenu is TRUE
	esp.draw();
	ImGui::EndFrame();	
	ImGui::Render();

	g_pd3dContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return phookD3D11Present(pChain, SyncInterval, Flags);
}

void __stdcall DrawIndexedHook(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	return phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
}

void __stdcall hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery)
{
	if (pQueryDesc->Query == D3D11_QUERY_OCCLUSION)
	{
		D3D11_QUERY_DESC oqueryDesc = CD3D11_QUERY_DESC();
		(&oqueryDesc)->MiscFlags = pQueryDesc->MiscFlags;
		(&oqueryDesc)->Query = D3D11_QUERY_TIMESTAMP;

		return phookD3D11CreateQuery(pDevice, &oqueryDesc, ppQuery);
	}

	return phookD3D11CreateQuery(pDevice, pQueryDesc, ppQuery);
}

UINT pssrStartSlot;
D3D11_SHADER_RESOURCE_VIEW_DESC Descr;

void __stdcall hookD3D11PSSetShaderResources(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	pssrStartSlot = StartSlot;

	for (UINT j = 0; j < NumViews; j++)
	{
		ID3D11ShaderResourceView* pShaderResView = ppShaderResourceViews[j];
		if (pShaderResView)
		{
			pShaderResView->GetDesc(&Descr);

			if ((Descr.ViewDimension == D3D11_SRV_DIMENSION_BUFFER) || (Descr.ViewDimension == D3D11_SRV_DIMENSION_BUFFEREX))
			{
				continue; //Skip buffer resources
			}
		}
	}

	return phookD3D11PSSetShaderResources(pContext, StartSlot, NumViews, ppShaderResourceViews);
}

void __stdcall ClearRenderTargetViewHook(ID3D11DeviceContext* pContext, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4])
{
	return phookD3D11ClearRenderTargetViewHook(pContext, pRenderTargetView, ColorRGBA);
}

DWORD __stdcall HookDX11_Init()
{
	printf("HookDX11_Init()\r\n");
	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;
	DXGI_SWAP_CHAIN_DESC sd{ 0 };
	sd.BufferCount = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = 800;
	sd.BufferDesc.Width = 600;
	sd.BufferDesc.RefreshRate = { 60, 1 };
	sd.OutputWindow = GetForegroundWindow();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &obtainedLevel, &g_pd3dContext);
	if (FAILED(hr))	{
		MessageBox(g_hWnd, L"Failed to create device and swapchain.", L"Fatal Error", MB_ICONERROR);
		return E_FAIL;
	}
	
	pSwapChainVTable = (DWORD_PTR*)(g_pSwapChain);
	pSwapChainVTable = (DWORD_PTR*)(pSwapChainVTable[0]);

	pDeviceVTable = (DWORD_PTR*)(g_pd3dDevice);
	pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];

	pDeviceContextVTable = (DWORD_PTR*)(g_pd3dContext);
	pDeviceContextVTable = (DWORD_PTR*)(pDeviceContextVTable[0]);

	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pSwapChainVTable[8], PresentHook, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pSwapChainVTable[8]) != MH_OK) { return 1; }
	/*if (MH_CreateHook((DWORD_PTR*)pDeviceContextVTable[12], DrawIndexedHook, reinterpret_cast<void**>(&phookD3D11DrawIndexed)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pDeviceContextVTable[12]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pDeviceVTable[24], hookD3D11CreateQuery, reinterpret_cast<void**>(&phookD3D11CreateQuery)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pDeviceVTable[24]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pDeviceContextVTable[8], hookD3D11PSSetShaderResources, reinterpret_cast<void**>(&phookD3D11PSSetShaderResources)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pDeviceContextVTable[8]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pSwapChainVTable[50], ClearRenderTargetViewHook, reinterpret_cast<void**>(&phookD3D11ClearRenderTargetViewHook)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pSwapChainVTable[50]) != MH_OK) { return 1; }*/

	DWORD old_protect;
	VirtualProtect(phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &old_protect);

	return S_OK;
}

void InitD3DHook(HMODULE hModule) {
	g_hModule = hModule;
	HookDX11_Init();
}

D3D11_HOOK_API void mainDettach(HMODULE hModule) {

	printf("mainDettach()\r\n");
	g_pd3dDevice->Release();
	g_pd3dContext->Release();
	g_pSwapChain->Release();

	ImplHookDX11_Shutdown();

	Beep(220, 300);

	Sleep(1000);
	printf("Calling FreseLibrary...\r\n");
	FreeLibraryAndExitThread(hModule, 0);
}

D3D11_HOOK_API void ImplHookDX11_Init(HMODULE hModule, HWND hwnd)
{
	GetClientRect(hwnd, &esp.rect);
	esp.screenCenterX = esp.rect.right / 2;
	esp.screenCenterY = esp.rect.bottom / 2;
	g_hWnd = (HWND)hwnd;
	g_hModule = hModule;
	HookDX11_Init();
}

D3D11_HOOK_API void ImplHookDX11_Shutdown()
{
	printf("ImplHookDX11_Shutdown\r\n");
	if (MH_DisableHook(MH_ALL_HOOKS)) { return; };
	if (MH_Uninitialize()) { return; }
}