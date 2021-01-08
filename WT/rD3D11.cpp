#include "rD3D11.h"

//https://guidedhacking.com/threads/directx11-d3d11-hook-drawing-template.15252/

using namespace DirectX;

std::unique_ptr<PrimitiveBatch<VertexPositionColor>> primitiveBatch;
std::unique_ptr<BasicEffect> basicEffect;
std::unique_ptr<DirectX::SpriteFont> m_font;
std::unique_ptr<CommonStates> states;

bool WriteMem(void* pDst, char* pBytes, size_t size) {
	DWORD dwOld;
	if (!VirtualProtect(pDst, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	memcpy(pDst, pBytes, PRESENT_STUB_SIZE);

	VirtualProtect(pDst, size, dwOld, &dwOld);
	return true;
}

/// Prints a shitty box using rD3D11::DrawLine
/// 
void rD3D11::DrawBox(float x, float y, float width, float height, DirectX::XMFLOAT4 color) {

	float fWidth = (float)windowRect.right;
	float fHeight = (float)windowRect.bottom;

	float left = fWidth / -2;
	float top = fHeight / 2;

	DrawLine(x, y, x + width, y, color);					//  1->2
	DrawLine(x + width, y, x + width, y + height, color);	// 2 -> 3
	DrawLine(x + width, y + height, x, y + height, color);	// 3 -> 4
	DrawLine(x, y + height, x, y, color);			// 4 -> 1
}

void rD3D11::DrawLine(float x, float y, float x2, float y2, XMFLOAT4 color) {

	float fWidth = (float)windowRect.right;
	float fHeight = (float)windowRect.bottom;

	float left = fWidth / -2;
	float top = fHeight / 2;
											
	primitiveBatch->DrawTriangle(
		VertexPositionColor(XMFLOAT3(x, y, 1.0f), color),
		VertexPositionColor(XMFLOAT3(x2 - 0.005f, y2 - 0.005f, 1.0f), color),
		VertexPositionColor(XMFLOAT3(x2, y2, 1.0f), color)
	);

	primitiveBatch->DrawTriangle(
		VertexPositionColor(XMFLOAT3(x, y , 1.0f), color),
		VertexPositionColor(XMFLOAT3(x + 0.005f, y + 0.005f, 1.0f), color),
		VertexPositionColor(XMFLOAT3(x2, y2, 1.0f), color)
	);
}

void rD3D11::Render() {

	pContext->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	pContext->OMSetDepthStencilState(states->DepthNone(), 0);
	pContext->RSSetState(states->CullNone());

	basicEffect->Apply(pContext);
	pContext->IASetInputLayout(pVertexLayout);
	primitiveBatch->Begin();
	
	//DrawLine(0.0f, 0.2f, 0.5f, 0.5f, (XMFLOAT4)Colors::Green);
	DrawBox(0.0f, 0.0f, 0.5f, 0.2f, (XMFLOAT4)Colors::Green);
	primitiveBatch->End();
}

bool rD3D11::Hook(void* pSrc, void* pDst, size_t size) {
	DWORD dwOld;
	uintptr_t src = (uintptr_t)pSrc;
	uintptr_t dst = (uintptr_t)pDst;

	if (!VirtualProtect(pSrc, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	*(char*)src = (char)0xE9;
	*(int*)(src + 1) = (int)(dst - src - 5);

	VirtualProtect(pSrc, size, dwOld, &dwOld);
	return true;
}

bool rD3D11::InitD3DHook(IDXGISwapChain* pSwapchain)
{
	HRESULT hr = pSwapchain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
	if (FAILED(hr))
		return false;

	pDevice->GetImmediateContext(&pContext);
	pContext->OMGetRenderTargets(1, &pRenderTargetView, nullptr);

	// If for some reason we fail to get a render target, create one.
	// This will probably never happen with a real game but maybe certain test environments... :P
	if (!pRenderTargetView)
	{
		// Get a pointer to the back buffer for the render target view
		ID3D11Texture2D* pBackbuffer = nullptr;
		hr = pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackbuffer));
		if (FAILED(hr))
			return false;

		// Create render target view
		hr = pDevice->CreateRenderTargetView(pBackbuffer, nullptr, &pRenderTargetView);
		pBackbuffer->Release();
		if (FAILED(hr))
			return false;

		// Make sure our render target is set.
		pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	}

	// initialize shaders
	ID3D10Blob* pBlob = nullptr;

	// create vertex shader
	if (!CompileShader(szShadez, "VS", "vs_5_0", &pBlob))
		return false;

	hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	if (FAILED(hr))
		return false;

	// Define/create the input layout for the vertex shader
	D3D11_INPUT_ELEMENT_DESC layout[2] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = pDevice->CreateInputLayout(layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout);
	if (FAILED(hr))
		return false;

	safe_release(pBlob);

	// create pixel shader
	if (!CompileShader(szShadez, "PS", "ps_5_0", &pBlob))
		return false;

	hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	if (FAILED(hr))
		return false;

	UINT numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	float fWidth = 0;
	float fHeight = 0;

	// Apparently this isn't universal. Works on some games
	pContext->RSGetViewports(&numViewports, pViewports);

	//
	if (!numViewports || !pViewports[MAINVP].Width)
	{
		//printf("path1\r\n");
		// This should be retrieved dynamically
		//HWND hWnd0 = FindWindowA( "W2ViewportClass", nullptr );
		HWND hWnd = FindMainWindow(GetCurrentProcessId());
		RECT rc{ 0 };
		if (!GetClientRect(hWnd, &rc))
			return false;
		windowRect = rc;
		//fWidth = 1600.0f;
		//fHeight = 900.0f;
		fWidth = (float)rc.right;
		fHeight = (float)rc.bottom;

		// Setup viewport
		pViewports[MAINVP].Width = (float)fWidth;
		pViewports[MAINVP].Height = (float)fHeight;
		pViewports[MAINVP].MinDepth = 0.0f;
		pViewports[MAINVP].MaxDepth = 1.0f;

		// Set viewport to context
		pContext->RSSetViewports(1, pViewports);
	}
	else
	{
		//printf("path2\r\n");
		fWidth = pViewports[MAINVP].Width;
		fHeight = pViewports[MAINVP].Height;
		rD3D11::windowRect.right = (long)pViewports[MAINVP].Width;
		rD3D11::windowRect.bottom = (long)pViewports[MAINVP].Height;
		printf("windowsRect: %2.f:%2.f\r\n", fWidth, fHeight);
	}

	// DirectXTK -----------------------------------------------------------------------------
	// These don't have to be globals
	void const* shaderByteCode;
	size_t byteCodeLength;
	states = std::make_unique<CommonStates>(pDevice);

	primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(pContext);

	basicEffect = std::make_unique<BasicEffect>(pDevice);

	basicEffect->SetVertexColorEnabled(true);

	basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	hr = pDevice->CreateInputLayout(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		&pVertexLayout
	);

	if (FAILED(hr)) {
		printf("Unable to create device input layout; result = %d\n", hr);
		return false;
	}
	// ---------------------------------------------------------------------------------------

	return true;
}

bool rD3D11::CompileShader(const char* szShader, const char* szEntrypoint, const char* szTarget, ID3D10Blob** pCompiledShaderBlob)
{
	ID3D10Blob* pErrorBlob = nullptr;

	auto hr = D3DCompile(szShader, strlen(szShader), 0, nullptr, nullptr, szEntrypoint, szTarget, D3DCOMPILE_ENABLE_STRICTNESS, 0, pCompiledShaderBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			char szError[256]{ 0 };
			memcpy(szError, pErrorBlob->GetBufferPointer(), pErrorBlob->GetBufferSize());
			MessageBoxA(nullptr, szError, "Error", MB_OK);
		}
		return false;
	}
	return true;
}

bool rD3D11::installD3DHook() {
	printf("hookD3d()...\r\n");

	while (!GetModuleHandle("dxgi") || !GetModuleHandle("D3D11.dll"))
		Sleep(500);

	// Create a dummy device, get swapchain vmt, hook present.
	D3D_FEATURE_LEVEL featLevel;
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
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pSwapchain, &pDevice, &featLevel, nullptr);
	if (FAILED(hr)) {
		printf("D3D11CreateDeviceAndSwapChain failed\r\n");
		return 0;
	}

	// Get swapchain vmt
	void** pVMT = *(void***)pSwapchain;
	//printf("NEW:pVMT = %p\r\n", pVMT);
	// Get Present's address out of vmt
	originalPresentFcn = (fnPresent)(pVMT[VMT_PRESENT]);

	// got what we need, we can release device and swapchain now
	// we'll be stealing the game's.
	safe_release(pSwapchain);
	safe_release(pDevice);

	// Create a code cave to trampoline to our hook
	// We'll try to do this close to present to make sure we'll be able to use a 5 byte jmp (important for x64)
	void* pLoc = (void*)((uintptr_t)originalPresentFcn - 0x2000);
	void* pTrampLoc = nullptr;
	while (!pTrampLoc) {
		//pTrampLoc = VirtualAlloc(pLoc, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		pTrampLoc = VirtualAlloc(pLoc, 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		pLoc = (void*)((uintptr_t)pLoc + 0x200);
	}
	ogPresentTramp = (fnPresent)pTrampLoc;

	// write original bytes to trampoline
	// write jmp to hook
	memcpy(originalPresentBytes, originalPresentFcn, PRESENT_STUB_SIZE);
	memcpy(pTrampLoc, originalPresentBytes, PRESENT_STUB_SIZE);

	pTrampLoc = (void*)((uintptr_t)pTrampLoc + PRESENT_STUB_SIZE);

	// write the jmp back into present
	*(char*)pTrampLoc = (char)0xE9;
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + 1);
	uintptr_t ogPresRet = (uintptr_t)originalPresentFcn + 5;
	*(int*)pTrampLoc = (int)(ogPresRet - (uintptr_t)pTrampLoc - 4);

	// write the jmp to our hook
	pTrampoline = pTrampLoc = (void*)((uintptr_t)pTrampLoc + 4);

	// if x64, gazzillion byte absolute jmp
	unsigned char pJmp[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
	WriteMem(pTrampLoc, (char*)pJmp, ARRAYSIZE(pJmp));
	pTrampLoc = (void*)((uintptr_t)pTrampLoc + ARRAYSIZE(pJmp));
	*(uintptr_t*)pTrampLoc = (uintptr_t)hookedPresent;

	// hook present, place a normal mid-function at the beginning of the Present function
	return Hook(originalPresentFcn, pTrampoline, PRESENT_STUB_SIZE);
}

void rD3D11::CleanupD3D() {
	WriteMem(originalPresentFcn, originalPresentBytes, PRESENT_STUB_SIZE);
	//VirtualFree((void*)ogPresentTramp, 0x1000, MEM_RELEASE);
	VirtualFree((void*)ogPresentTramp, 0, MEM_RELEASE);
	safe_release(pVertexBuffer);
	safe_release(pIndexBuffer);
	safe_release(pConstantBuffer);
	safe_release(pPixelShader);
	safe_release(pVertexShader);
	safe_release(pVertexLayout);
}

HWND FindMainWindow(DWORD dwPID)
{
	HandleData handleData{ 0 };
	handleData.pid = dwPID;
	EnumWindows(EnumWindowsCallback, (LPARAM)&handleData);
	return handleData.hWnd;
}

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