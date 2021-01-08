#pragma once
#include <functional>//
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h> // color https://github.com/microsoft/DirectXMath/
#include "w2sAndUtils.h"
#include "hook.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "GeometricPrimitive.h"
#include "PrimitiveBatch.h"
#include "Effects.h"
#include "CommonStates.h"

//https://guidedhacking.com/threads/directx11-d3d11-hook-drawing-template.15252/
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib")

#define safe_release(p) if (p) { p->Release(); p = nullptr; }
#define PRESENT_STUB_SIZE 5
#define MAINVP 0
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present

const D3DCOLORVALUE red{ 1.0f, 0.0f, 0.0f, 1.0f };
const D3DCOLORVALUE green{ 0.0f, 1.0f, 0.0f, 1.0f };
const D3DCOLORVALUE blue{ 0.0f, 0.0f, 1.0f, 1.0f };
const D3DCOLORVALUE magenta{ 1.0f, 0.0f, 1.0f, 1.0f };
const D3DCOLORVALUE yellow{ 1.0f, 1.0f, 0.0f, 1.0f };

struct ConstantBuffer {
	DirectX::XMMATRIX mProjection;
};

struct Vertex3D {
	DirectX::XMFLOAT3 pos;
	D3DCOLORVALUE d3dcolor;
};

struct Vertex2D {
	float x;
	float y;
};

struct HandleData
{
	DWORD pid;
	HWND hWnd;
};

HWND FindMainWindow(DWORD dwPID);
BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);

HRESULT __stdcall hookedPresent(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);

class rD3D11;

class rD3D11
{
public:
	ID3D11Device* pDevice = nullptr;
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

	D3D11_VIEWPORT pViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{ 0 };
	D3D11_VIEWPORT myViewport;
	DirectX::XMMATRIX mOrtho;

	//hook stuff
	using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
	void* originalPresentFcn;					// Pointer to the original Present function
	fnPresent ogPresentTramp;		// Pointer to our trampoline

	void* pTrampoline = nullptr;		// Pointer to jmp instruction in our trampoline that leads to hkPresent
	char originalPresentBytes[PRESENT_STUB_SIZE];	// Buffer to store original bytes from Present

	bool installD3DHook();
	bool CompileShader(const char* szShader, const char* szEntrypoint, const char* szTarget, ID3D10Blob** pBlob);

	//These are all executed inside InitD3Draw()
	bool GetDeviceContextRenderTarget(IDXGISwapChain* pSwapchain);
	bool GetViewport();
	bool SetupOrtho();

	bool InitD3DHook(IDXGISwapChain* pSwapchain); //call this inside the Present() hook

	void BeginDraw();
	void DrawLine(float x, float y, float x2, float y2, DirectX::XMFLOAT4 color);
	void DrawLineWH(float x, float y, float width, float height, D3DCOLORVALUE color); //uses 1 vertex + width and height
	void DrawBox(float x, float y, float width, float height, DirectX::XMFLOAT4 color);
	void Render();

	void CleanupD3D();
private:
	bool Hook(void* pSrc, void* pDst, size_t size);
};

enum class IDXGISwapChainVMT {
	QueryInterface,
	AddRef,
	Release,
	SetPrivateData,
	SetPrivateDataInterface,
	GetPrivateData,
	GetParent,
	GetDevice,
	Present,
	GetBuffer,
	SetFullscreenState,
	GetFullscreenState,
	GetDesc,
	ResizeBuffers,
	ResizeTarget,
	GetContainingOutput,
	GetFrameStatistics,
	GetLastPresentCount,
};

enum class ID3D11DeviceVMT {
	QueryInterface,
	AddRef,
	Release,
	CreateVideoDecoder,
	CreateVideoProcessor,
	CreateAuthenticatedChannel,
	CreateCryptoSession,
	CreateVideoDecoderOutputView,
	CreateVideoProcessorInputView,
	CreateVideoProcessorOutputView,
	CreateVideoProcessorEnumerator,
	GetVideoDecoderProfileCount,
	GetVideoDecoderProfile,
	CheckVideoDecoderFormat,
	GetVideoDecoderConfigCount,
	GetVideoDecoderConfig,
	GetContentProtectionCaps,
	CheckCryptoKeyExchange,
	SetPrivateData,
	SetPrivateDataInterface,
};

enum class ID3D11DeviceContextVMT {
	QueryInterface,
	AddRef,
	Release,
	GetDevice,
	GetPrivateData,
	SetPrivateData,
	SetPrivateDataInterface,
	VSSetConstantBuffers,
	PSSetShaderResources,
	PSSetShader,
	PSSetSamplers,
	VSSetShader,
	DrawIndexed,
	Draw,
	Map,
	Unmap,
	PSSetConstantBuffers,
	IASetInputLayout,
	IASetVertexBuffers,
	IASetIndexBuffer,
	DrawIndexedInstanced,
	DrawInstanced,
	GSSetConstantBuffers,
	GSSetShader,
	IASetPrimitiveTopology,
	VSSetShaderResources,
	VSSetSamplers,
	Begin,
	End,
	GetData,
	SetPredication,
	GSSetShaderResources,
	GSSetSamplers,
	OMSetRenderTargets,
	OMSetRenderTargetsAndUnorderedAccessViews,
	OMSetBlendState,
	OMSetDepthStencilState,
	SOSetTargets,
	DrawAuto,
	DrawIndexedInstancedIndirect,
	DrawInstancedIndirect,
	Dispatch,
	DispatchIndirect,
	RSSetState,
	RSSetViewports,
	RSSetScissorRects,
	CopySubresourceRegion,
	CopyResource,
	UpdateSubresource,
	CopyStructureCount,
	ClearRenderTargetView,
	ClearUnorderedAccessViewUint,
	ClearUnorderedAccessViewFloat,
	ClearDepthStencilView,
	GenerateMips,
	SetResourceMinLOD,
	GetResourceMinLOD,
	ResolveSubresource,
	ExecuteCommandList,
	HSSetShaderResources,
	HSSetShader,
	HSSetSamplers,
	HSSetConstantBuffers,
	DSSetShaderResources,
	DSSetShader,
	DSSetSamplers,
	DSSetConstantBuffers,
	CSSetShaderResources,
	CSSetUnorderedAccessViews,
	CSSetShader,
	CSSetSamplers,
	CSSetConstantBuffers,
	VSGetConstantBuffers,
	PSGetShaderResources,
	PSGetShader,
	PSGetSamplers,
	VSGetShader,
	PSGetConstantBuffers,
	IAGetInputLayout,
	IAGetVertexBuffers,
	IAGetIndexBuffer,
	GSGetConstantBuffers,
	GSGetShader,
	IAGetPrimitiveTopology,
	VSGetShaderResources,
	VSGetSamplers,
	GetPredication,
	GSGetShaderResources,
	GSGetSamplers,
	OMGetRenderTargets,
	OMGetRenderTargetsAndUnorderedAccessViews,
	OMGetBlendState,
	OMGetDepthStencilState,
	SOGetTargets,
	RSGetState,
	RSGetViewports,
	RSGetScissorRects,
	HSGetShaderResources,
	HSGetShader,
	HSGetSamplers,
	HSGetConstantBuffers,
	DSGetShaderResources,
	DSGetShader,
	DSGetSamplers,
	DSGetConstantBuffers,
	CSGetShaderResources,
	CSGetUnorderedAccessViews,
	CSGetShader,
	CSGetSamplers,
	CSGetConstantBuffers,
	ClearState,
	Flush,
	GetType,
	GetContextFlags,
	FinishCommandList,
};

constexpr const char* szShadez = R"(
// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix projection;
}

// PSI (PixelShaderInput)
struct PSI
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

// VertexShader
PSI VS( float4 pos : POSITION, float4 color : COLOR )
{
    PSI psi;
    psi.pos = mul( projection, pos  );
    psi.color = color;
    return psi;
}

// PixelShader
float4 PS(PSI psi) : SV_TARGET
{
    return psi.color;
}
)";