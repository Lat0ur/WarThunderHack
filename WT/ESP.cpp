#include "ESP.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "offsets.h"
#include "rD3D11.h"
#include "mathStuff.h"

#define alpha (color >> 24) & 0xff
#define ImRed (color >> 16) & 0xff
#define ImGreen (color >> 8) & 0xff
#define ImBlue (color) & 0xff

ESP esp;

void ESP::update(){

	if (!esp.gameInfo->isInFlightPositive || esp.gameInfo->isInFlightNegative)
		return;
	
	esp.playerCount = *(int*)(MC.exeBase + playerCountOffset);
	esp.viewMatrix = (float*)(MC.exeBase + viewMatrixOffset);
	esp.instanceArray = *(uintptr_t*)(MC.exeBase + playerListOffset);
	//printf("%d: %d\r\n", esp.rect.left, esp.rect.bottom);

	for (int instanceIterator = 0; instanceIterator < playerCount; instanceIterator++) {
		CBaseEntity* pPlayerInstance = (CBaseEntity*)*(uintptr_t*)(instanceArray + instanceIterator * 8);

		//printf("[%d]: %s\n", instanceIterator, (char*) pPlayerInstance->name);
		if (!strncmp(pPlayerInstance->name, "BORNTOB", 7)) {
			//printf("[%d] Gotten smyplayah\r\n", instanceIterator);
			//printf("%llx %llx\r\n", pPlayerInstance->unit, *pPlayerInstance->unit);
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
	
	char textBuffer[30];
	sprintf_s(textBuffer, "%s %.0f", "PredictionScaleCoeff =", esp.k);
	draw->AddText(ImGui::GetFont(), 16, ImVec2(30.0f, 30.f), ImColor(255, 255, 255, 255), textBuffer, 0, 0.0f, 0);
	
	if (!esp.gameInfo->isInFlightPositive || esp.gameInfo->isInFlightNegative)
		return;
	
	//printf("playerCount: %d\r\n", playerCount);
	for (int instanceIterator = 0; instanceIterator < playerCount; instanceIterator++) {
		CBaseEntity* pPlayerInstance = (CBaseEntity*)*(uintptr_t*)(instanceArray + instanceIterator * 8);
		
		//printf("%p\r\n", pPlayerInstance);
		
		if (pPlayerInstance->sign == entitySignature) {
			if (pPlayerInstance->teamId != mLocalPlayerTeamId && pPlayerInstance->state == 2) {
				// if enemy and IsAlive state (==2)
				Unit* pPlayersUnit = pPlayerInstance->unit;
				vec3 targetPlayerPos;
				vec3 targetPlayerVelocity;

				*(&targetPlayerPos) = (pPlayersUnit->position);				
				*(&targetPlayerVelocity) = *(vec3*)((char*)pPlayersUnit + unitVelocityOffset);
				vec3 deltaPosVector = targetPlayerPos - mLocalPlayerCoords;
				float distanceToTarget = deltaPosVector.Length();

				float projectileFlightTime = distanceToTarget / bulletMuzzleVelocity;
				vec3 predictionPosition = (targetPlayerVelocity * projectileFlightTime +  targetPlayerPos);
				//float deltaZ = 4.9f * projectileFlightTime * projectileFlightTime;
				if (distanceToTarget < (28000.0f + 1000.0f * esp.k) ) {
					vec3 targetPlayerScreenPos;
					vec3 ScreenPosPrediction;
					if (DirectXWorldToScreen(targetPlayerPos, targetPlayerScreenPos, (D3DX11Matricies*)esp.viewMatrix, esp.rect.right, esp.rect.bottom)) {
						// if can be drawn trace, drawing trace
						// draw->AddLine(ImVec2(screenCenterX, screenCenterY), ImVec2(ScreenOriginPos.x, ScreenOriginPos.y), ImColor(255, 255, 255), 0.3f);	// trace
						// 
						float width = 25000 / distanceToTarget;
						draw->AddRect(ImVec2(targetPlayerScreenPos.x - width, targetPlayerScreenPos.y - width), ImVec2(targetPlayerScreenPos.x + width, targetPlayerScreenPos.y + width), ImColor(255, 255, 255), 0, 0, 2);
						if (distanceToTarget > 4000.f) {
							sprintf_s(textBuffer, "%.0fk", distanceToTarget / 1000);
							draw->AddText(ImGui::GetFont(), 12, ImVec2(targetPlayerScreenPos.x + 5 + width, targetPlayerScreenPos.y - width - 5), ImColor(255, 0, 0, 255), textBuffer, 0, 0.0f, 0);
						}

						if (distanceToTarget < 1000.0f * esp.k) {
							// if enemy player near us, lets draw a prediction marker
							// 
							if (DirectXWorldToScreen(predictionPosition, ScreenPosPrediction, (D3DX11Matricies*)esp.viewMatrix, esp.rect.right, esp.rect.bottom)) {
								float angle = deltaPosVector.calcAngle(deltaPosVector, targetPlayerVelocity);
								if (isnan(angle))
									continue;

								float red;
								float green;
								if (angle <= M_PI / 2 ) {
									green = 1;
									red = 2 * angle / M_PI;
								}
								else {
									red = 1;
									green = 2 * (M_PI - angle) / M_PI;
								}

								draw->AddLine(ImVec2(targetPlayerScreenPos.x, targetPlayerScreenPos.y), ImVec2(ScreenPosPrediction.x, ScreenPosPrediction.y), ImColor(red, green, 0.0f), 1.f);	// prediction
								/*
								float mLocalPlayerVelocityAbs = mLocalPlayerVelocity.Length();
								float targetPlayerAbsVelocity = targetPlayerVelocity.Length();
								float perpendicalarTargetPlayerVelocity = sin(M_PI - angle) * targetPlayerAbsVelocity;
								float parallelTargetPlayerVelocity = cos(M_PI - angle) * targetPlayerAbsVelocity;
								float imaginaryDistance = parallelTargetPlayerVelocity + distanceToTarget;
								float predictionDistance = sqrtf(imaginaryDistance * imaginaryDistance + perpendicalarTargetPlayerVelocity * perpendicalarTargetPlayerVelocity) / 750;*/
								

								//printf("deltaVector: %f %f; targetPlayerVelocity: %f %f [angle: %f]\r\n", deltaVector.x, deltaVector.y, targetPlayerVelocity.x, targetPlayerVelocity.y, angle);								
								//draw->AddCircleFilled(ImVec2(ScreenPosPrediction.x, ScreenPosPrediction.y), 4.0f, ImColor(230, 0, 0, 150), 12);
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