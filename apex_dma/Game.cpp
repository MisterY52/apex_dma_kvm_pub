#include <array>
#include "prediction.h"
#include <cassert>
#include <ostream>
#include <iostream>


extern Memory apex_mem;

extern bool firing_range;
float smooth = 12.0f;
extern bool aim_no_recoil;
int bone = 2;
//
bool bone_auto = true;
extern float max_dist;
//
extern float glowr;
extern float glowg;
extern float glowb;

extern int glowtype;
extern int glowtype2;
extern int glowtype3;

// https://github.com/Gerosity/zap-client/blob/master/Core/Player.hpp#L161
bool Entity::Observing(uint64_t localptr)
{
	uint64_t SpectatorList;
	apex_mem.Read<uint64_t>(apex_mem.get_proc_baseaddr() + OFFSET_OBSERVER_LIST, SpectatorList);

	int PlayerData;
	apex_mem.Read<int>(ptr + 0x38, PlayerData);

	int SpecIndex;
	apex_mem.Read<int>(SpectatorList + PlayerData * 8 + OFFSET_OBSERVER_ARRAY, SpecIndex);

	uint64_t SpectatorAddr;
	apex_mem.Read<uint64_t>(apex_mem.get_proc_baseaddr() + OFFSET_ENTITYLIST + ((SpecIndex & 0xFFFF) << 5), SpectatorAddr);

	if (SpectatorAddr == localptr)
	{
        return true;
    }

    return false;
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

//////////////////////////////////
void InState::update(uint32_t address)
{
	extern uint64_t g_Base;
	apex_mem.Read(g_Base + address, button);
	state = (button.state & 1) != 0;
}

void InState::post(uint32_t address)
{
	extern uint64_t g_Base;
	// If active get the most recent state of the button
	if (force && apex_mem.Read(g_Base + address, button))
	{
		// Get the desired state of the button
		int state;
		if (press && !release) {
			state = 5;
		}
		else if (!press && release) {
			state = 4;
		}
		else {
			state = button.down[0] == 0 && button.down[1] == 0 ? 4 : 5;
		}
		// Gently tell the game to that nobody will be harmed if they just do as told
		if (button.state != state) {
			apex_mem.Write(g_Base + address + 8, state);
		}
	}
}

//////////////////////////////////

//////////////////////////////////
uint32_t button_state[4];

bool isPressed(uint32_t button_code)
{
	return (button_state[static_cast<uint32_t>(button_code) >> 5] & (1 << (static_cast<uint32_t>(button_code) & 0x1f))) != 0;
}
//////////////////////////////////

int Entity::getTeamId()
{
	return *(int*)(buffer + OFFSET_TEAM);
}

int Entity::getHealth()
{
	return *(int*)(buffer + OFFSET_HEALTH);
}

int Entity::getShield()
{
	return *(int*)(buffer + OFFSET_SHIELD);
}

int Entity::getMaxShield()
{
	return *(int*)(buffer + OFFSET_MAXSHIELD);
}

int Entity::getArmortype()
{
	return *(int*)(buffer + OFFSET_ARMORTYPE);
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

/////////////////////////////////
bool Entity::isOnGround()
{
	uint32_t flags;
	apex_mem.Read(ptr + OFFSET_FLAGS, flags);

	return (flags & 0x1) != 0;
}

bool Entity::isInSkydive()
{
	return *(int*)(buffer + OFFSET_SKYDIVE_STATE) > 0;
}
/////////////////////////////////

float Entity::lastVisTime()
{
  return *(float*)(buffer + OFFSET_VISIBLE_TIME);
}

///////////////////////////////
float Entity::lastCrossHairTime()
{
	return *(float*)(buffer + OFFSET_CROSSHAIR_LAST);
}
///////////////////////////////

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

	if(Bone < 0 || Bone > 255)
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

bool Entity::isGlowing()
{
	return *(int*)(buffer + OFFSET_GLOW_ENABLE) == 7;
}

bool Entity::isZooming()
{
	return *(int*)(buffer + OFFSET_ZOOMING) == 1;
}

/////////////////////////
/////////////////////////

/////////////////////////
    extern uint64_t g_Base;
    extern int settingIndex;
    extern int contextId;
    extern std::array<float, 3> highlightParameter;
    //custom glow colo RGB
    unsigned char outsidevalue = 125;
    extern unsigned char insidevalue;
    extern unsigned char insidevalueItem;
    extern unsigned char outlinesize;

    void Entity::enableGlow()
    {

		//static const int contextId = 0;
		//int settingIndex = 50;
		std::array<unsigned char, 4> highlightFunctionBits = {
			insidevalue,   // InsideFunction
			outsidevalue, // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE
			outlinesize,  // OutlineRadius: size * 255 / 8
			64   // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7)
		};

		apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, contextId);
		long highlightSettingsPtr;
		apex_mem.Read<long>(g_Base + HIGHLIGHT_SETTINGS, highlightSettingsPtr);
		apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 2);
		apex_mem.Write<typeof(highlightFunctionBits)>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * contextId + 0x0, highlightFunctionBits);
		apex_mem.Write<typeof(highlightParameter)>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * contextId + 0x4, highlightParameter);
		//apex_mem.Write<float>(ptr + 0x264, 8.0E+4);

		//apex_mem.Write(g_Base + OFFSET_GLOW_FIX, 1);
		apex_mem.Write<float>(ptr + GLOW_DISTANCE, 88888);



    }
///////////////////////////

void Entity::disableGlow()
{
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_T1, 0);
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_T2, 0);
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, 2);
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 5);
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
     int name_index;
    apex_mem.Read<int>(ptr + 0x38, name_index);
    uint64_t name_ptr = 0;
    apex_mem.Read<uint64_t>(g_Base + OFFSET_NAME_LIST + ((name_index - 1) * 24), name_ptr);
	apex_mem.ReadArray<char>(name_ptr, name, 32);
}

////test////
    int Entity::read_xp_level() {


      int xp = 0;
      apex_mem.Read<int>(ptr + OFFSET_m_xp, xp);

      static int levels[] = {2750,   6650,   11400,  17000,  23350,  30450,  38300,
                             46450,  55050,  64100,  73600,  83550,  93950,  104800,
                             116100, 127850, 140050, 152400, 164900, 177550, 190350,
                             203300, 216400, 229650, 243050, 256600, 270300, 284150,
                             298150, 312300, 326600, 341050, 355650, 370400, 385300,
                             400350, 415550, 430900, 446400, 462050, 477850, 493800,
                             509900, 526150, 542550, 559100, 575800, 592650, 609650,
                             626800, 644100, 661550, 679150, 696900, 714800};

      if (xp < 0)
        return 0;
      if (xp < 100)
        return 1;

      int level = 56;
      int arraySize = sizeof(levels) / sizeof(levels[0]);

      for (int i = 0; i < arraySize; i++) {
        if (xp < levels[i]) {
          return i + 1;
        }
      }
      // Debugging statement to log the XP being read
      //std::cout << "XP read: " << xp << std::endl;
      return level + ((xp - levels[arraySize - 1] + 1) / 18000) + 1;
    }

//Deathboxes
//bool Item::isBox()
//{
//	char class_name[33] = {};
//	get_class_name(ptr, class_name);
//
//	return strncmp(class_name, "CDeathBoxProp", 13) == 0;
//}
//Traps
//bool Item::isTrap()
//{
//	char class_name[33] = {};
//	get_class_name(ptr, class_name);
//
//	return strncmp(class_name, "caustic_trap", 13) == 0;
//}

//bool Item::isItem()
//{
//	char class_name[33] = {};
//	get_class_name(ptr, class_name);
//
//	return strncmp(class_name, "CPropSurvival", 13) == 0;
//}

//bool Item::isGlowing()
//{
//	return *(int*)(buffer + OFFSET_ITEM_GLOW) == 1363184265;
//}

//Vector Item::getPosition()
//{
//	return *(Vector*)(buffer + OFFSET_ORIGIN);
//}

float CalculateFov(Entity& from, Entity& target)
{
	QAngle ViewAngles = from.GetViewAngles();
	Vector LocalCamera = from.GetCamPos();
	Vector EntityPosition = target.getPosition();
	QAngle Angle = Math::CalcAngle(LocalCamera, EntityPosition);
	return Math::GetFov(ViewAngles, Angle);
}

QAngle CalculateBestBoneAim(Entity& from, uintptr_t t, float max_fov, float smoothing)
{
	Entity target = getEntity(t);
	if(firing_range)
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
	//
	float distanceToTarget;
	//
	Vector TargetBonePosition = target.getBonePositionByHitbox(bone);
	QAngle CalculatedAngles = QAngle(0, 0, 0);
	
	WeaponXEntity curweap = WeaponXEntity();
	curweap.update(from.ptr);
	float BulletSpeed = curweap.get_projectile_speed();
	float BulletGrav = curweap.get_projectile_gravity();
	float zoom_fov = curweap.get_zoom_fov();

	if (zoom_fov != 0.0f && zoom_fov != 1.0f)
	{
		max_fov *= zoom_fov/90.0f;
	}

  // Find best bone
  if (bone_auto) {
    float NearestBoneDistance = max_dist;
    for (int i = 0; i < 4; i++) {
      Vector currentBonePosition = target.getBonePositionByHitbox(i);
      float DistanceFromCrosshair =
          (currentBonePosition - LocalCamera).Length();
      if (DistanceFromCrosshair < NearestBoneDistance) {
        TargetBonePosition = currentBonePosition;
        distanceToTarget = DistanceFromCrosshair;
        NearestBoneDistance = DistanceFromCrosshair;
      }
    }
  } else {
    TargetBonePosition = target.getBonePositionByHitbox(bone);
    distanceToTarget = (TargetBonePosition - LocalCamera).Length();
  }
	/*
	//simple aim prediction
	if (BulletSpeed > 1.f)
	{
		Vector LocalBonePosition = from.getBonePosition(bone);
		float VerticalTime = TargetBonePosition.DistTo(LocalBonePosition) / BulletSpeed;
		TargetBonePosition.z += (BulletGrav * 0.5f) * (VerticalTime * VerticalTime);

		float HorizontalTime = TargetBonePosition.DistTo(LocalBonePosition) / BulletSpeed;
		TargetBonePosition += (target.getAbsVelocity() * HorizontalTime);
	}
	*/
	
	//more accurate prediction
	if (BulletSpeed > 1.f)
	{
		PredictCtx Ctx;
		Ctx.StartPos = LocalCamera;
		Ctx.TargetPos = TargetBonePosition; 
		Ctx.BulletSpeed = BulletSpeed - (BulletSpeed*0.08);
		Ctx.BulletGravity = BulletGrav + (BulletGrav*0.05);
		Ctx.TargetVel = target.getAbsVelocity();

		if (BulletPredict(Ctx))
			CalculatedAngles = QAngle{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
    }

	if (CalculatedAngles == QAngle(0, 0, 0))
    	CalculatedAngles = Math::CalcAngle(LocalCamera, TargetBonePosition);
	QAngle ViewAngles = from.GetViewAngles();
	QAngle SwayAngles = from.GetSwayAngles();
	//remove sway and recoil
	if(aim_no_recoil)
		CalculatedAngles-=SwayAngles-ViewAngles;
	Math::NormalizeAngles(CalculatedAngles);
	QAngle Delta = CalculatedAngles - ViewAngles;

	Math::NormalizeAngles(Delta);

	QAngle SmoothedAngles = ViewAngles + Delta/smoothing;
	return SmoothedAngles;
}

Entity getEntity(uintptr_t ptr)
{
	Entity entity = Entity();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	//return entity;
	if (entity.isPlayer()) { // Use the isPlayer() function to initialize is_player
        entity.is_player = true;
	//entity.player_xp_level = entity.read_xp_level();
	//entity.player_level();
    }
    return entity;}

//Item getItem(uintptr_t ptr)
//{
//	Item entity = Item();
//	entity.ptr = ptr;
//	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
//	return entity;
//}

bool WorldToScreen(Vector from, float* m_vMatrix, int targetWidth, int targetHeight, Vector& to)
{
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f) return false;

	float x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	float y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	x *= invw;
	y *= invw;

	float screenX = targetWidth / 2;
	float screenY = targetHeight / 2;

	screenX += 0.5 * x * targetWidth + 0.5;
	screenY -= 0.5 * y * targetHeight + 0.5;

	to.x = screenX;
	to.y = screenY;
	to.z = 0;

	return (to.x >= 0 && to.x <= targetWidth && to.y >= 0 && to.y <= targetHeight);
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

//////////////////////
//////////////////////

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

//const char *WeaponXEntity::get_name_str() { return name_str; }
