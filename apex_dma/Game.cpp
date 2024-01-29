#include "prediction.h"

extern Memory apex_mem;

int miss = 0;

extern bool firing_range;
float smooth = 85.0f;
bool aim_no_recoil = true;
int bone = 2;

bool Entity::Observing(uint64_t entitylist)
{
	/*uint64_t index = *(uint64_t*)(buffer + OFFSET_OBSERVING_TARGET);
	index &= ENT_ENTRY_MASK;
	if (index > 0)
	{
		uint64_t centity2 = apex_mem.Read<uint64_t>(entitylist + ((uint64_t)index << 5));
		return centity2;
	}
	return 0;*/
	return *(bool*)(buffer + OFFSET_OBSERVER_MODE);
}

//https://github.com/CasualX/apexbot/blob/master/src/state.cpp#L104
void get_class_name(uint64_t entity_ptr, char* out_str)
{
	uint64_t client_networkable_vtable;
	apex_mem.Read<uint64_t>(entity_ptr + 8 * 3, client_networkable_vtable);

	uint64_t get_client_class;
	apex_mem.Read<uint64_t>(client_networkable_vtable + 8 * 3, get_client_class);

	uint32_t disp;
	apex_mem.Read<uint32_t>(get_client_class + 3, disp);
	const uint64_t client_class_ptr = get_client_class + disp + 7;

	ClientClass client_class;
	apex_mem.Read<ClientClass>(client_class_ptr, client_class);

	apex_mem.ReadArray<char>(client_class.pNetworkName, out_str, 32);
}

int Entity::getTeamId()
{
	return *(int*)(buffer + OFFSET_TEAM);
}

int calc_level(int m_xp)
{
	if (m_xp < 0) return 0;
	if (m_xp < 100) return 1;
	if (m_xp < 2750) return 2;
	if (m_xp < 6650) return 3;
	if (m_xp < 11400) return 4;
	if (m_xp < 17000) return 5;
	if (m_xp < 23350) return 6;
	if (m_xp < 30450) return 7;
	if (m_xp < 38300) return 8;
	if (m_xp < 46450) return 9;
	if (m_xp < 55050) return 10;
	if (m_xp < 64100) return 11;
	if (m_xp < 73600) return 12;
	if (m_xp < 83550) return 13;
	if (m_xp < 93950) return 14;
	if (m_xp < 104800) return 15;
	if (m_xp < 116100) return 16;
	if (m_xp < 127850) return 17;
	if (m_xp < 140050) return 18;
	if (m_xp < 152400) return 19;
	if (m_xp < 164900) return 20;
	if (m_xp < 177550) return 21;
	if (m_xp < 190350) return 22;
	if (m_xp < 203300) return 23;
	if (m_xp < 216400) return 24;
	if (m_xp < 229650) return 25;
	if (m_xp < 243050) return 26;
	if (m_xp < 256600) return 27;
	if (m_xp < 270300) return 28;
	if (m_xp < 284150) return 29;
	if (m_xp < 298150) return 30;
	if (m_xp < 312300) return 31;
	if (m_xp < 326600) return 32;
	if (m_xp < 341050) return 33;
	if (m_xp < 355650) return 34;
	if (m_xp < 370400) return 35;
	if (m_xp < 385300) return 36;
	if (m_xp < 400350) return 37;
	if (m_xp < 415550) return 38;
	if (m_xp < 430900) return 39;
	if (m_xp < 446400) return 40;
	if (m_xp < 462050) return 41;
	if (m_xp < 477850) return 42;
	if (m_xp < 493800) return 43;
	if (m_xp < 509900) return 44;
	if (m_xp < 526150) return 45;
	if (m_xp < 542550) return 46;
	if (m_xp < 559100) return 47;
	if (m_xp < 575800) return 48;
	if (m_xp < 592650) return 49;
	if (m_xp < 609650) return 50;
	if (m_xp < 626800) return 51;
	if (m_xp < 644100) return 52;
	if (m_xp < 661550) return 53;
	if (m_xp < 679150) return 54;
	if (m_xp < 696900) return 55;
	if (m_xp < 714800) return 56;
	return floor((m_xp - 714800 + 1) / 18000) + 57;
}

int Entity::getHealth()
{
	return *(int*)(buffer + OFFSET_HEALTH);
}

int Entity::getArmortype()
{
	int armortype;
	apex_mem.Read<int>(ptr + OFFSET_ARMOR_TYPE, armortype);
	return armortype;
}

int Entity::getMaxshield()
{
	return *(int*)(buffer + OFFSET_MAXSHIELD);
}

int Entity::getShield()
{
	return *(int*)(buffer + OFFSET_SHIELD);
}

Vector Entity::getAbsVelocity()
{
	return *(Vector*)(buffer + OFFSET_ABS_VELOCITY);
}

Vector Entity::getPosition()
{
	return *(Vector*)(buffer + OFFSET_ORIGIN);
}

bool Entity::isPlayer()
{
	return *(uint64_t*)(buffer + OFFSET_NAME) == 125780153691248;
}

bool Entity::isDummy()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CAI_BaseNPC", 11) == 0;
}

bool Entity::isKnocked()
{
	return *(int*)(buffer + OFFSET_BLEED_OUT_STATE) > 0;
}

bool Entity::isAlive()
{
	return *(int*)(buffer + OFFSET_LIFE_STATE) == 0;
}

float Entity::lastVisTime()
{
	return *(float*)(buffer + OFFSET_VISIBLE_TIME);
}

Vector Entity::getBonePosition(int id)
{
	Vector position = getPosition();
	uintptr_t boneArray = *(uintptr_t*)(buffer + OFFSET_BONES);
	Vector bone = Vector();
	uint32_t boneloc = (id * 0x30);
	Bone bo = {};
	apex_mem.Read<Bone>(boneArray + boneloc, bo);
	bone.x = bo.x + position.x;
	bone.y = bo.y + position.y;
	bone.z = bo.z + position.z;
	return bone;
}

//https://www.unknowncheats.me/forum/apex-legends/496984-getting-hitbox-positions-cstudiohdr-externally.html
//https://www.unknowncheats.me/forum/3499185-post1334.html
//https://www.unknowncheats.me/forum/3562047-post11000.html
Vector Entity::getBonePositionByHitbox(int id)
{
	Vector origin = getPosition();

	//BoneByHitBox
	uint64_t Model = *(uint64_t*)(buffer + OFFSET_STUDIOHDR);

	//get studio hdr
	uint64_t StudioHdr;
	apex_mem.Read<uint64_t>(Model + 0x8, StudioHdr);

	//get hitbox array
	uint16_t HitboxCache;
	apex_mem.Read<uint16_t>(StudioHdr + 0x34, HitboxCache);
	uint64_t HitBoxsArray = StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));

	uint16_t IndexCache;
	apex_mem.Read<uint16_t>(HitBoxsArray + 0x4, IndexCache);
	int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));

	uint16_t Bone;
	apex_mem.Read<uint16_t>(HitBoxsArray + HitboxIndex + (id * 0x20), Bone);

	if (Bone < 0 || Bone > 255)
		return Vector();

	//hitpos
	uint64_t BoneArray = *(uint64_t*)(buffer + OFFSET_BONES);

	matrix3x4_t Matrix = {};
	apex_mem.Read<matrix3x4_t>(BoneArray + Bone * sizeof(matrix3x4_t), Matrix);

	return Vector(Matrix.m_flMatVal[0][3] + origin.x, Matrix.m_flMatVal[1][3] + origin.y, Matrix.m_flMatVal[2][3] + origin.z);
}

QAngle Entity::GetSwayAngles()
{
	return *(QAngle*)(buffer + OFFSET_BREATH_ANGLES);
}

QAngle Entity::GetViewAngles()
{
	return *(QAngle*)(buffer + OFFSET_VIEWANGLES);
}

Vector Entity::GetViewAnglesV()
{
	return *(Vector*)(buffer + OFFSET_VIEWANGLES);
}

float Entity::GetYaw()
{
	float yaw = 0;
	apex_mem.Read<float>(ptr + OFFSET_YAW, yaw);

	if (yaw < 0)
		yaw += 360;
	yaw += 90;
	if (yaw > 360)
		yaw -= 360;

	return yaw;
}

void Entity::SetViewAngles(SVector angles)
{
	apex_mem.Write<SVector>(ptr + OFFSET_VIEWANGLES, angles);
}

void Entity::SetViewAngles(QAngle& angles)
{
	SetViewAngles(SVector(angles));
}

Vector Entity::GetCamPos()
{
	return *(Vector*)(buffer + OFFSET_CAMERAPOS);
}

QAngle Entity::GetRecoil()
{
	return *(QAngle*)(buffer + OFFSET_AIMPUNCH);
}

void Entity::get_name(uint64_t g_Base, uint64_t index, char* name)
{
	index *= 0x10;
	uint64_t name_ptr = 0;
	apex_mem.Read<uint64_t>(g_Base + OFFSET_NAME_LIST + index, name_ptr);
	apex_mem.ReadArray<char>(name_ptr, name, 32);
}

bool Item::isItem()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CPropSurvival", 13) == 0;
}

Vector Item::getPosition()
{
	return *(Vector*)(buffer + OFFSET_ORIGIN);
}

float CalculateFov(Entity& from, Entity& target)
{
	QAngle ViewAngles = from.GetViewAngles();
	Vector LocalCamera = from.GetCamPos();
	Vector EntityPosition = target.getPosition();
	QAngle Angle = Math::CalcAngle(LocalCamera, EntityPosition);
	return Math::GetFov(ViewAngles, Angle);
}

QAngle CalculateBestBoneAim(Entity& from, uintptr_t t, float max_fov , float rcs_pitch , float rcs_yaw)
{
	Entity target = getEntity(t);
	if (firing_range)
	{
		if (!target.isAlive())
		{
			return QAngle(0, 0, 0);
		}
	}
	else
	{
		if (!target.isAlive() || target.isKnocked())
		{
			return QAngle(0, 0, 0);
		}
	}

	Vector LocalCamera = from.GetCamPos();
	Vector TargetBonePosition = target.getBonePositionByHitbox(bone);
	QAngle CalculatedAngles = QAngle(0, 0, 0);

	WeaponXEntity curweap = WeaponXEntity();
	curweap.update(from.ptr);
	float BulletSpeed = curweap.get_projectile_speed();
	float BulletGrav = curweap.get_projectile_gravity();
	float zoom_fov = curweap.get_zoom_fov();
	//printf("%f \n", zoom_fov);
	if (zoom_fov != 0.0f && zoom_fov != 1.0f)
	{
		max_fov *= zoom_fov/90.0f;
	}

	//more accurate prediction
	if (BulletSpeed > 1.f)
	{
		PredictCtx Ctx;
		Ctx.StartPos = LocalCamera;
		Ctx.TargetPos = TargetBonePosition;
		Ctx.BulletSpeed = BulletSpeed - (BulletSpeed * 0.08);
		Ctx.BulletGravity = BulletGrav + (BulletGrav * 0.05);
		Ctx.TargetVel = target.getAbsVelocity();

		if (BulletPredict(Ctx))
			CalculatedAngles = QAngle{ Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f };
	}

	if (CalculatedAngles == QAngle(0, 0, 0))

	{
		float random1 = (float)(rand() % 7 + 1) / 100;
		float random2 = (float)(rand() % 7 + 1) / 100;
		Vector start = LocalCamera;
		Vector end = TargetBonePosition;
		Vector dir = (end - start).Normalize();
		Vector mid1 = start + (dir * ((end - start).Length() * (0.33f + random1)));
		Vector mid2 = start + (dir * ((end - start).Length() * (0.67f + random2)));
		mid1.z += 25.f;
		mid2.z += 25.f;

		// Calculate aim angle using the final intermediate point
		Vector aim = Math::Bezier(start, mid1, mid2, end, 0.5f + random2);
		CalculatedAngles = Math::CalcAngle(LocalCamera, aim);
	}

	QAngle ViewAngles = from.GetViewAngles();
	QAngle SwayAngles = from.GetSwayAngles();
	//remove sway and recoil
	if (aim_no_recoil) {
		QAngle sway = SwayAngles - ViewAngles;
		sway.x = sway.x * rcs_pitch;   //pitch 0.3
		sway.y = sway.y * rcs_yaw;  //yaw 0.5
		CalculatedAngles -= sway;
	}
	Math::NormalizeAngles(CalculatedAngles);
	QAngle Delta = CalculatedAngles - ViewAngles;
	double fov = Math::GetFov(SwayAngles, CalculatedAngles);
	if (fov > max_fov)
	{
		return QAngle(0, 0, 0);
	}

	Math::NormalizeAngles(Delta);

	QAngle RandomAngles = QAngle(
		(rand() % 4 - 2) * 0.001f,
		(rand() % 4 - 2) * 0.001f,
		(rand() % 4 - 2) * 0.001f
	);
	QAngle RandomAnglesMax = QAngle(
		(rand() % 6 - 2) * 0.001f,
		(rand() % 6 - 2) * 0.001f,
		(rand() % 6 - 2) * 0.001f
	);
	miss = rand() % 100;
	QAngle SmoothedAngles = ViewAngles + Delta / smooth + RandomAngles;
	if (miss >= 70) {
		QAngle SmoothedAngles = ViewAngles + Delta / smooth + RandomAnglesMax;
	}
	else {
		QAngle SmoothedAngles = ViewAngles + Delta / smooth + RandomAngles;
	}

	return SmoothedAngles;
}

Entity getEntity(uintptr_t ptr)
{
	Entity entity = Entity();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	return entity;
}

Item getItem(uintptr_t ptr)
{
	Item entity = Item();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	return entity;
}

bool WorldToScreen(Vector from, float* m_vMatrix, int targetWidth, int targetHeight, Vector& to)
{
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f) return false;

	to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = targetWidth / 2;
	float y = targetHeight / 2;

	x += 0.5 * to.x * targetWidth + 0.5;
	y -= 0.5 * to.y * targetHeight + 0.5;

	to.x = x;
	to.y = y;
	to.z = 0;
	return true;
}

void WeaponXEntity::update(uint64_t LocalPlayer)
{
	extern uint64_t g_Base;
	uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
	uint64_t wephandle = 0;
	apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON, wephandle);

	wephandle &= 0xffff;

	uint64_t wep_entity = 0;
	apex_mem.Read<uint64_t>(entitylist + (wephandle << 5), wep_entity);

	projectile_speed = 0;
	apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SPEED, projectile_speed);
	projectile_scale = 0;
	apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SCALE, projectile_scale);
	zoom_fov = 0;
	apex_mem.Read<float>(wep_entity + OFFSET_ZOOM_FOV, zoom_fov);
	ammo = 0;
	apex_mem.Read<int>(wep_entity + OFFSET_AMMO, ammo);
}

float WeaponXEntity::get_projectile_speed()
{
	return projectile_speed;
}

float WeaponXEntity::get_projectile_gravity()
{
	return 750.0f * projectile_scale;
}

float WeaponXEntity::get_zoom_fov()
{
	return zoom_fov;
}

int WeaponXEntity::get_ammo()
{
	return ammo;
}
