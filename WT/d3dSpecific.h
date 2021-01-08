#pragma once

#include <d3d11.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <stdio.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "D2DBaseTypes.h"
#define safe_release(p) if (p) { p->Release(); p = nullptr; } 
#define VMT_PRESENT (UINT)IDXGISwapChainVMT::Present
#define PRESENT_STUB_SIZE 5

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
	psi.color = color;
	pos = mul( pos, projection );
	psi.pos = pos;
	return psi;
}

// PixelShader
float4 PS(PSI psi) : SV_TARGET
{
	return psi.color;
}
)";

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

bool installD3DHook();
void CleanupD3D();

struct Matrix44 {
	float m[4][4];
};

struct D3DX11Matricies
{
	Matrix44 ViewMatrix;
	Matrix44 WorldMatrix;
	Matrix44 ProjectionMatrix;
};

extern ID3D11Device* pDevice = nullptr;
extern IDXGISwapChain* pSwapchain = nullptr;
extern ID3D11DeviceContext* pContext = nullptr;
extern ID3D11RenderTargetView* pRenderTargetView = nullptr;
extern ID3D11VertexShader* pVertexShader = nullptr;
extern ID3D11InputLayout* pVertexLayout = nullptr;
extern ID3D11PixelShader* pPixelShader = nullptr;
extern ID3D11Buffer* pVertexBuffer = nullptr;
extern ID3D11Buffer* pIndexBuffer = nullptr;
extern ID3D11Buffer* pConstantBuffer = nullptr;