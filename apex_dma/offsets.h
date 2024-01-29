#define OFFSET_ARMOR_TYPE           0x45cc //m_armorType
#define OFFSET_MAXSHIELD            0x01a4 //m_shieldhealthmax
#define OFFSET_MODELNAME            0x0030 //m_modelname
#define OFFSET_XP                   0x3634 //m_xp
#define OFFSET_USERID               0x2508 //m_platformUserId           

#define OFFSET_WEAPON               0x18f4 //m_latestPrimaryWeapons
#define OFFSET_WEAPON_NAME          0x1738 //m_weaponNameIndex 2nd one
#define OFFSET_ENTITYLIST		0x1db2e38 //cl_entitylist
#define OFFSET_LOCAL_ENT		0x2161538 //LocalPlayer
#define OFFSET_NAME_LIST            0xc5e9fd0 //NameList

#define OFFSET_TEAM					0x030c //m_iTeamNum
#define OFFSET_HEALTH				0x02fc //m_iHealth
#define OFFSET_SHIELD				0x1a0 //m_shieldHealth
#define OFFSET_NAME					0x0449 //m_iName
#define OFFSET_SIGN_NAME			0x0440 //m_iSignifierName
#define OFFSET_ABS_VELOCITY         0x0170 //m_vecAbsVelocity
#define OFFSET_VISIBLE_TIME         0x1950 //CPlayer!lastVisibleTime
#define OFFSET_ZOOMING      		0x19b1 //m_bZooming
#define OFFSET_YAW                  0x21fc - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

#define OFFSET_LIFE_STATE			0x0658  //m_lifeState, >0 = dead
#define OFFSET_BLEED_OUT_STATE		0x26a0 //m_bleedoutState, >0 = knocked

#define OFFSET_ORIGIN				0x017c //m_vecAbsOrigin
#define OFFSET_BONES				0x0d60 + 0x48 //m_nForceBone + 0x48
#define OFFSET_STUDIOHDR            0xfb0 //CBaseAnimating!m_pStudioHdr
#define OFFSET_AIMPUNCH			0x23f8 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS			0x1e90 //CPlayer!camera_origin
#define OFFSET_VIEWANGLES			0x24f4 - 0x14 //m_ammoPoolCapacity - 0x14
#define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
#define OFFSET_OBSERVER_MODE		0x3454 //m_iObserverMode
#define OFFSET_OBSERVING_TARGET		0x3460 //m_hObserverTarget

#define OFFSET_MATRIX				0x11a350 //ViewMatrix
#define OFFSET_RENDER				0x73cc3e0 //ViewRender

#define OFFSET_BULLET_SPEED 0x1e5c //CWeaponX!m_flProjectileSpeed
#define OFFSET_BULLET_SCALE 0x1e64 //CWeaponX!m_flProjectileScale
#define OFFSET_ZOOM_FOV             0x1590 + 0xb8 //m_playerData + m_curZoomFOV
#define OFFSET_AMMO                 0x1540 //[RecvTable.DT_WeaponX_LocalWeaponData]
