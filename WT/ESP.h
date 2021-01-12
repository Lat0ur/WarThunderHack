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
	uintptr_t exeBase;
	float* viewMatrix;
	RECT rect;
	int screenCenterX, screenCenterY;
	
	void update();
	void draw();

	int8_t mLocalPlayerTeamId = 1;
	uintptr_t* mLocalPlayerUnit;
	vec3 mLocalPlayerCoords = { 1000, 1000, 1000 };
	vec3 mLocalPlayerVelocity = { 0,0,0 };
	bool drawTracers = false;
};


extern ESP esp;