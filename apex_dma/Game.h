#include "Math.h"
#include "offsets.h"
#include "../vmread/hlapi/hlapi.h"

#define NUM_ENT_ENTRIES			(1 << 12)
#define ENT_ENTRY_MASK			(NUM_ENT_ENTRIES - 1)

typedef struct Bone
{
	uint8_t pad1[0xCC];
	float x;
	uint8_t pad2[0xC];
	float y;
	uint8_t pad3[0xC];
	float z;
}Bone;

class Entity
{
public:
	uint64_t ptr;
	uint8_t buffer[0x3FF0];
	Vector getPosition();
	bool isDummy();
	bool isPlayer();
	bool isKnocked();
	bool isAlive();
	float lastVisTime();
	int getTeamId();
	int getHealth();
	int getShield();
	Vector getAbsVelocity();
	QAngle GetSwayAngles();
	QAngle GetViewAngles();
	Vector GetCamPos();
	QAngle GetRecoil();
	Vector GetViewAnglesV();

	void SetViewAngles(WinProcess& mem, SVector angles);
	void SetViewAngles(WinProcess& mem, QAngle& angles);
	Vector getBonePosition(WinProcess& mem, int id);
	uint64_t Observing(WinProcess& mem, uint64_t entitylist);
};

class Item
{
public:
	uint64_t ptr;
	uint8_t buffer[0x3FF0];
	Vector getPosition();
	bool isItem();
	bool isGlowing();
	
	void enableGlow(WinProcess& mem);
	void disableGlow(WinProcess& mem);
};

class WeaponXEntity
{
public:
	void update(WinProcess& mem, uint64_t LocalPlayer);
	float get_projectile_speed();
	float get_projectile_gravity();

private:
	float projectile_scale;
	float projectile_speed;
};
Entity getEntity(WinProcess& mem, uintptr_t ptr);
Item getItem(WinProcess& mem, uintptr_t ptr);
bool WorldToScreen(Vector from, float* m_vMatrix, int targetWidth, int targetHeight, Vector& to);
float CalculateFov(Entity& from, Entity& target);
QAngle CalculateBestBoneAim(WinProcess& mem, Entity& from, uintptr_t target, float max_fov);