#include "Math.h"
#include "offsets.h"
#include "memory.h"

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
	bool isGlowing();
	bool isZooming();
	Vector getAbsVelocity();
	QAngle GetSwayAngles();
	QAngle GetViewAngles();
	Vector GetCamPos();
	QAngle GetRecoil();
	Vector GetViewAnglesV();

	void enableGlow();
	void disableGlow();
	void SetViewAngles(SVector angles);
	void SetViewAngles(QAngle& angles);
	Vector getBonePosition(int id);
	bool Observing(uint64_t entitylist);
	void get_name(uint64_t g_Base, uint64_t index, char* name);
};

class Item
{
public:
	uint64_t ptr;
	uint8_t buffer[0x3FF0];
	Vector getPosition();
	bool isItem();
	bool isGlowing();
	
	void enableGlow();
	void disableGlow();
};

class WeaponXEntity
{
public:
	void update(uint64_t LocalPlayer);
	float get_projectile_speed();
	float get_projectile_gravity();
	float get_zoom_fov();

private:
	float projectile_scale;
	float projectile_speed;
	float zoom_fov;
};

struct ClientClass {
	uint64_t pCreateFn;
	uint64_t pCreateEventFn;
	uint64_t pNetworkName;
	uint64_t pRecvTable;
	uint64_t pNext;
	uint32_t ClassID;
	uint32_t ClassSize;
};

Entity getEntity(uintptr_t ptr);
Item getItem(uintptr_t ptr);
bool WorldToScreen(Vector from, float* m_vMatrix, int targetWidth, int targetHeight, Vector& to);
float CalculateFov(Entity& from, Entity& target);
QAngle CalculateBestBoneAim(Entity& from, uintptr_t target, float max_fov);
void get_class_name(uint64_t entity_ptr, char* out_str);