//game version: 3.0.51.45 / date:2023/11/21
#define OFFSET_ARMOR_TYPE           0x4634 //m_armorType
#define OFFSET_MAXSHIELD            0x01a4 //m_shieldhealthmax
#define OFFSET_MODELNAME            0x0030 //m_modelname
#define OFFSET_XP                   0x369c //m_xp

#define OFFSET_WEAPON               0x1964 //m_latestPrimaryWeapons
#define OFFSET_WEAPON_NAME          0x17a8 //m_weaponNameIndex 2nd one
#define OFFSET_ENTITYLIST		0x1d71858 //cl_entitylist
#define OFFSET_LOCAL_ENT		0x211fac8 //.?AVC_GameMovement@@ + 0x8
#define OFFSET_NAME_LIST            0xc275bf0 //NameList

#define OFFSET_TEAM					0x037c //m_iTeamNum
#define OFFSET_HEALTH				0x036c //m_iHealth
#define OFFSET_SHIELD				0x1a0 //m_shieldHealth
#define OFFSET_NAME					0x04b9 //m_iName
#define OFFSET_SIGN_NAME			0x04b0 //m_iSignifierName
#define OFFSET_ABS_VELOCITY         0x0170 //m_vecAbsVelocity
#define OFFSET_VISIBLE_TIME         0x19BD+0x3 //CPlayer!lastVisibleTime
#define OFFSET_ZOOMING      		0x1c01 //m_bZooming
#define OFFSET_YAW                  0x226c - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

#define OFFSET_LIFE_STATE			0x06c8  //m_lifeState, >0 = dead
#define OFFSET_BLEED_OUT_STATE		0x2710 //m_bleedoutState, >0 = knocked

#define OFFSET_ORIGIN				0x017c //m_vecAbsOrigin
#define OFFSET_BONES				0x0dd0 + 0x48 //m_nForceBone + 0x48
#define OFFSET_STUDIOHDR            0x1020 //CBaseAnimating!m_pStudioHdr
#define OFFSET_AIMPUNCH			0x2468 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS			0x1f00 //CPlayer!camera_origin
#define OFFSET_VIEWANGLES			0x2564 - 0x14 //m_ammoPoolCapacity - 0x14
#define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
#define OFFSET_OBSERVER_MODE		0x34c4 //m_iObserverMode
#define OFFSET_OBSERVING_TARGET		0x34d0 //m_hObserverTarget

#define OFFSET_MATRIX				0x11a350 //ViewMatrix
#define OFFSET_RENDER				0x7389d40 //ViewRender

#define OFFSET_BULLET_SPEED 0x19e0 + 0x04dc //CWeaponX!m_flProjectileSpeed maybe its WeaponSettings.projectile_launch_speed now
#define OFFSET_BULLET_SCALE 0x19e0 + 0x04e4 //CWeaponX!m_flProjectileScale maybe its WeaponSettings.projectile_gravity_scale now 
#define OFFSET_ZOOM_FOV             0x1600 + 0xb8 //m_playerData + m_curZoomFOV
#define OFFSET_AMMO                 0x15b0 //m_ammoInClip