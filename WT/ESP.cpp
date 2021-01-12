#include "ESP.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "offsets.h"
#include "rD3D11.h"

#define alpha (color >> 24) & 0xff
#define ImRed (color >> 16) & 0xff
#define ImGreen (color >> 8) & 0xff
#define ImBlue (color) & 0xff

ESP esp;

void ESP::update(){
	esp.playerCount = *(int*)(MC.exeBase + playerCountOffset);
	esp.viewMatrix = (float*)(MC.exeBase + viewMatrixOffset);
	esp.instanceArray = *(uintptr_t*)(MC.exeBase + playerListOffset);
	//printf("%d: %d\r\n", esp.rect.left, esp.rect.bottom);

	for (int instanceIterator = 0; instanceIterator < playerCount; instanceIterator++) {
		CBaseEntity* pPlayerInstance = (CBaseEntity*)*(uintptr_t*)(instanceArray + instanceIterator * 8);

		//printf("[%d]: %s\n", instanceIterator, (char*) pPlayerInstance->name);
		if (!strncmp(pPlayerInstance->name, "BORNTOB", 7)) {
			//printf("[%d] Gotten smyplayah\r\n", instanceIterator);
			mLocalPlayerUnit = pPlayerInstance->unit;
			mLocalPlayerTeamId = pPlayerInstance->teamId;
			//printf("tId: %d\r\n", mPlayerTeamId);
			*(&mLocalPlayerCoords) = *(vec3*)((char*)mLocalPlayerUnit + unitCoordsOffset);
			*(&mLocalPlayerVelocity) = *(vec3*)((char*)mLocalPlayerUnit + unitVelocityOffset);
			continue;
		}
	}
	return;
};

void ESP::draw() {
	
	auto draw = ImGui::GetForegroundDrawList();
	//draw->AddRect(ImVec2(50, 50), ImVec2(200, 400), ImColor(255, 255, 255), 0.f, 0);
	draw->AddText(ImGui::GetFont(), 16, ImVec2(30.0f, 30.f), ImColor(0, 0, 0, 255), "Drawing...", 0, 0.0f, 0);
	

	//printf("playerCount: %d\r\n", playerCount);
	for (int instanceIterator = 0; instanceIterator < playerCount; instanceIterator++) {
		CBaseEntity* pPlayerInstance = (CBaseEntity*)*(uintptr_t*)(instanceArray + instanceIterator * 8);
		
		//printf("%p\r\n", pPlayerInstance);
		if (pPlayerInstance->sign == entitySignature) {
			if (pPlayerInstance->teamId != mLocalPlayerTeamId && pPlayerInstance->state == 2) {
				// if enemy and IsAlive state (==2)
				uintptr_t* pPlayersUnit = pPlayerInstance->unit;
				vec3 targetPos;
				vec3 targetVelocity;

				*(&targetPos) = *(vec3*)((char*)pPlayersUnit + unitCoordsOffset);				
				*(&targetVelocity) = *(vec3*)((char*)pPlayersUnit + unitVelocityOffset);
	
				vec3 deltaVector = targetPos - mLocalPlayerCoords;
				float distanceToTarget = deltaVector.Length();
				vec3 velocityVector = (targetVelocity * distanceToTarget / 780 + targetPos);
				if (distanceToTarget < 20000.0f) {
					vec3 ScreenOriginPos;
					vec3 ScreenPosPrediction;
					if (DirectXWorldToScreen(targetPos, ScreenOriginPos, (D3DX11Matricies*)esp.viewMatrix, esp.rect.right, esp.rect.bottom)) {
						// if can be drawn trace, drawing trace
						// draw->AddLine(ImVec2(screenCenterX, screenCenterY), ImVec2(ScreenOriginPos.x, ScreenOriginPos.y), ImColor(255, 255, 255), 0.3f);	// trace
						// 
						float width = 15000 / distanceToTarget;
						draw->AddRect(ImVec2(ScreenOriginPos.x - width, ScreenOriginPos.y - width), ImVec2(ScreenOriginPos.x + width, ScreenOriginPos.y + width), ImColor(255, 255, 255), 0, 0, 1);
						if (distanceToTarget < 4000.0f) {
							// if enemy player near us, lets draw a prediction marker
							if (DirectXWorldToScreen(velocityVector, ScreenPosPrediction, (D3DX11Matricies*)esp.viewMatrix, esp.rect.right, esp.rect.bottom)) {
								draw->AddLine(ImVec2(ScreenOriginPos.x, ScreenOriginPos.y), ImVec2(ScreenPosPrediction.x, ScreenPosPrediction.y), ImColor(255, 0, 0), 1.f);	// prediction
							}
						}

					}
				}
			}
		}
		else
			break;
	}
	return;
}