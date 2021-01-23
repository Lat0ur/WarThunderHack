#pragma once
#include <Windows.h>
#include "structs.h"
#include "w2sAndUtils.h"



class ESP {
private:
	int playerCount;
	CBaseEntity* playerInstance;

	uintptr_t instanceArray;

public:
	GameInfo* gameInfo;
	uintptr_t exeBase;
	float* viewMatrix;
	RECT rect;
	int screenCenterX, screenCenterY;
	bool playingFlag = false;
	void update();
	void draw();
	void draw3DBox(vec3 position, float* rotationMatrix, ImDrawList* draw, float distance);
	float k = 4;	// distance scale coeff
	int8_t mLocalPlayerTeamId = 1;
	Unit* mLocalPlayerUnit;
	vec3 mLocalPlayerCoords = { 1000, 1000, 1000 };
	vec3 mLocalPlayerVelocity = { 0,0,0 };
	bool drawTracers = false;
};


extern ESP esp;