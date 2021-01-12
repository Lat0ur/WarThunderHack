#include <stdint.h>

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
	uintptr_t* unit; //0x06D0
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