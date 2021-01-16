#include <stdint.h>
#include "w2sAndUtils.h"

class Unit
{
public:
	char pad_0000[2036]; //0x0000
	vec3 rotationMatricies[3]; //0x07F4
	vec3 position; //0x0818
	char pad_0824[1784]; //0x0824
	uint32_t N00000241; //0x0F1C
	uint32_t N00002F71; //0x0F20
	char pad_0F24[34]; //0x0F24
	class N00002F77* N00000246; //0x0F46
	char pad_0F4E[216]; //0x0F4E
	class N00002F8B* N00000262; //0x1026
	char pad_102E[92264]; //0x102E
}; //Size: 0x17896


class GameInfo {
public:
	char pad_0000[208]; //0x0000
	bool N000000FC; //0x00D0
	bool N00000116; //0x00D1
	bool N0000011A; //0x00D2
	bool isInFlightPositive; //0x00D3
	bool N0000011B; //0x00D4
	bool isInFlightNegative; //0x00D5
	bool N00000121; //0x00D6
	bool N00000118; //0x00D7
	char pad_00D8[176]; //0x00D8
}; //Size: 0x0188


class CBaseEntity
{
public:
	char pad_0000[32]; //0x0000
	uint32_t sign; //0x0020
	char pad_0024[132]; //0x0024
	int64_t UID; //0x00A8
	char name[64]; //0x00B0
	char pad_00F0[276]; //0x00F0
	uint32_t N000000B6; //0x0204
	uint8_t teamId; //0x0208
	char pad_0209[35]; //0x0209
	uint32_t N000000BB; //0x022C
	uint8_t teasdasdamId; //0x0230
	char pad_0231[627]; //0x0231
	uint32_t N000001CB; //0x04A4
	uint8_t state; //0x04A8
	char pad_04A9[547]; //0x04A9
	uint32_t N000003B1; //0x06CC
	Unit* unit; //0x06D0
	char pad_06D8[56]; //0x06D8

	bool is_valid() const { return true; }

	//std::string GetName();
	bool IsFriendly();
	bool IsEnemy();
	bool IsAlive();
	//int GetState();
	uint32_t GetTeam();
	//EntityState GetState();
	//int GetFlags();
	float GetRespawnTime();
	//Unit* GetUnit();
	bool IsBot();
	//bool IsVisible();
	//bool IsDormant();
	//Vector GetPosition();
}; //Size: 0x0e10

class unit {

};