#include "rD3D11.h"
#include "structs.h"
#include "imgui_impl_dx11.h"
#include "d3d11hook.h"


/*
void MasterClass::Render() {

	pContext->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	pContext->OMSetDepthStencilState(states->DepthNone(), 0);
	pContext->RSSetState(states->CullNone());

	basicEffect->Apply(pContext);
	pContext->IASetInputLayout(pVertexLayout);
	primitiveBatch->Begin();
	
	float* matrix = (float*)this->viewMatrix;

	int playerCount = *(int*)(exeBase + playerCountOffset);

	uintptr_t pPlayerList = *(uintptr_t*)(exeBase + playerListOffset);

	byte myTeamVal = 1;
	vec3 myCoordsVal = { 1000, 1000, 1000 };
	uintptr_t* myPlayersUnit;

	for (int instanceIterator = 0; instanceIterator < playerCount; instanceIterator++) {
		CBaseEntity* pPlayerInstance = (CBaseEntity * )*(uintptr_t*)(pPlayerList + instanceIterator * 8);

		if (pPlayerInstance->name == "BORNTOBESMART") {
			myTeamVal = pPlayerInstance->teamId;
			myPlayersUnit = pPlayerInstance->unit;
			*(&myCoordsVal) = *(vec3*)((char*)myPlayersUnit + unitCoordsOffset);
		}

		if (pPlayerInstance->sign == entitySignature) {
			if (pPlayerInstance->teamId != myTeamVal && pPlayerInstance->state == 2) {
				//printf("[%d] iter %d - my %d\r\n", instanceIterator, pPlayerInstance->teamId, mLocalPlayer->teamId);
				uintptr_t* pPlayersUnit = pPlayerInstance->unit;
				vec3 targetPos;
				*(&targetPos) = *(vec3*)((char*)pPlayersUnit + unitCoordsOffset);
				
				vec3 targetVelocity;
				*(&targetVelocity) = *(vec3*)((char*)pPlayersUnit + unitVelocityOffset);
				//printf("%4.2f %4.2f %4.2f\r\n", targetVelocity.x, targetVelocity.y, targetVelocity.z);
				vec3 deltaVector = targetPos - myCoordsVal;
				float distanceToTarget = deltaVector.Length();
				//printf("distance: %4.2f", distanceToTarget);
				if (distanceToTarget < 20000.0f || 1) {
					vec3 ScreenPosPrediction;
					vec3 ScreenOriginPos;
					/*if (DirectXWorldToScreen((targetVelocity + targetPos), ScreenPosPrediction, (D3DX11Matricies*)matrix, 1366, 768) &&
						DirectXWorldToScreen(targetPos, ScreenOriginPos, (D3DX11Matricies*)matrix, 1366, 768)) {
						DrawLine(ScreenOriginPos.x, ScreenOriginPos.y, ScreenPosPrediction.x, ScreenPosPrediction.y, (XMFLOAT4)Colors::Green);
					}
				}



			}
		}
		else
			break;

	}
	primitiveBatch->End();
}*/

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