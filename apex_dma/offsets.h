#define ORIGIN 1
#define STEAM 2

#define VERSION STEAM

#if VERSION == STEAM

    #define OFFSET_ENTITYLIST			0x1a1e3b8
    #define OFFSET_LOCAL_ENT			0x1dcf5e8 //LocalPlayer
    #define OFFSET_NAME_LIST            0xba2ea30
    #define OFFSET_THIRDPERSON          0x01a02db0 + 0x6c //thirdperson_override + 0x6c
    #define OFFSET_TIMESCALE            0x013fa370 //host_timescale

    #define OFFSET_TEAM					0x448 //m_iTeamNum
    #define OFFSET_HEALTH				0x438 //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x589 //m_iName
    #define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1ad4 //CPlayer!lastVisibleTime
    #define OFFSET_ZOOMING      		0x1c51 //m_bZooming
    #define OFFSET_THIRDPERSON_SV       0x36a8 //m_thirdPersonShoulderView
    #define OFFSET_YAW                  0x22a0 - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

    #define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x2720 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x014c //m_vecAbsOrigin
    #define OFFSET_BONES				0x0f50 - 0x18 //m_bConstrainBetweenEndpoints - 0x18
    #define OFFSET_AIMPUNCH				0x2498 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1f40 //CPlayer!camera_origin
    #define OFFSET_VIEWANGLES			0x2594 - 0x14 //m_ammoPoolCapacity - 0x14
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x34bc //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x34c8 //m_hObserverTarget

    #define OFFSET_MATRIX				0x11a210
    #define OFFSET_RENDER				0x75441d0

    #define OFFSET_WEAPON				0x1a6c //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1f28 //CWeaponX!m_flProjectileSpeed
    #define OFFSET_BULLET_SCALE         0x1f30 //CWeaponX!m_flProjectileScale
    #define OFFSET_ZOOM_FOV             0x1718 + 0xb8 //m_playerData + m_curZoomFOV
    #define OFFSET_AMMO                 0x16a4 //m_ammoInClip

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#elif VERSION == ORIGIN

    #define OFFSET_ENTITYLIST			0x1a1e3b8
    #define OFFSET_LOCAL_ENT			0x1dcf5e8 //LocalPlayer
    #define OFFSET_NAME_LIST            0xba2ea30
    #define OFFSET_THIRDPERSON          0x01a02db0 + 0x6c //thirdperson_override + 0x6c
    #define OFFSET_TIMESCALE            0x013fa370 //host_timescale

    #define OFFSET_TEAM					0x448 //m_iTeamNum
    #define OFFSET_HEALTH				0x438 //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x589 //m_iName
    #define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1ad4 //CPlayer!lastVisibleTime
    #define OFFSET_ZOOMING      		0x1c51 //m_bZooming
    #define OFFSET_THIRDPERSON_SV       0x36a8 //m_thirdPersonShoulderView
    #define OFFSET_YAW                  0x22a0 - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

    #define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x2720 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x014c //m_vecAbsOrigin
    #define OFFSET_BONES				0x0f50 - 0x18 //m_bConstrainBetweenEndpoints - 0x18
    #define OFFSET_AIMPUNCH				0x2498 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1f40 //CPlayer!camera_origin
    #define OFFSET_VIEWANGLES			0x2594 - 0x14 //m_ammoPoolCapacity - 0x14
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x34bc //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x34c8 //m_hObserverTarget

    #define OFFSET_MATRIX				0x11a210
    #define OFFSET_RENDER				0x75441d0

    #define OFFSET_WEAPON				0x1a6c //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1f28 //CWeaponX!m_flProjectileSpeed
    #define OFFSET_BULLET_SCALE         0x1f30 //CWeaponX!m_flProjectileScale
    #define OFFSET_ZOOM_FOV             0x1718 + 0xb8 //m_playerData + m_curZoomFOV
    #define OFFSET_AMMO                 0x16a4 //m_ammoInClip

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#endif