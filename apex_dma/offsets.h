#define ORIGIN 1
#define STEAM 2

#define VERSION STEAM

#if VERSION == STEAM

    #define OFFSET_ENTITYLIST			0x1b2c6e8
    #define OFFSET_LOCAL_ENT			0x1edd7e8 //LocalPlayer //.?AVC_GameMovement@@ + 0x8
    #define OFFSET_NAME_LIST            0xbbdc630
    #define OFFSET_THIRDPERSON          0x01b11020 + 0x6c //thirdperson_override + 0x6c
    #define OFFSET_TIMESCALE            0x014c72c0 //host_timescale

    #define OFFSET_TEAM					0x044c //m_iTeamNum
    #define OFFSET_HEALTH				0x043c //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x589 //m_iName
    #define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1a70 //CPlayer!lastVisibleTime
    #define OFFSET_ZOOMING      		0x1c51 //m_bZooming
    #define OFFSET_THIRDPERSON_SV       0x36c8 //m_thirdPersonShoulderView
    #define OFFSET_YAW                  0x22ac - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

    #define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x2738 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x014c //m_vecAbsOrigin
    #define OFFSET_BONES				0x0e98 + 0x48 //m_nForceBone + 0x48
    #define OFFSET_STUDIOHDR            0x10f0 //CBaseAnimating!m_pStudioHdr
    #define OFFSET_AIMPUNCH				0x24a8 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1f48 //CPlayer!camera_origin
    #define OFFSET_VIEWANGLES			0x25a4 - 0x14 //m_ammoPoolCapacity - 0x14
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x34d4 //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x34e0 //m_hObserverTarget

    #define OFFSET_MATRIX				0x11a210
    #define OFFSET_RENDER				0x7665f38

    #define OFFSET_WEAPON				0x1a14 //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1ef0 //CWeaponX!m_flProjectileSpeed
    #define OFFSET_BULLET_SCALE         0x1ef8 //CWeaponX!m_flProjectileScale
    #define OFFSET_ZOOM_FOV             0x16b8 + 0xb8 //m_playerData + m_curZoomFOV
    #define OFFSET_AMMO                 0x1670 //m_ammoInClip

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#elif VERSION == ORIGIN

    #define OFFSET_ENTITYLIST			0x1b2c6e8
    #define OFFSET_LOCAL_ENT			0x1edd7e8 //LocalPlayer //.?AVC_GameMovement@@ + 0x8
    #define OFFSET_NAME_LIST            0xbbdc630
    #define OFFSET_THIRDPERSON          0x01b11020 + 0x6c //thirdperson_override + 0x6c
    #define OFFSET_TIMESCALE            0x014c72c0 //host_timescale

    #define OFFSET_TEAM					0x044c //m_iTeamNum
    #define OFFSET_HEALTH				0x043c //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x589 //m_iName
    #define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1a70 //CPlayer!lastVisibleTime
    #define OFFSET_ZOOMING      		0x1c51 //m_bZooming
    #define OFFSET_THIRDPERSON_SV       0x36c8 //m_thirdPersonShoulderView
    #define OFFSET_YAW                  0x22ac - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

    #define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x2738 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x014c //m_vecAbsOrigin
    #define OFFSET_BONES				0x0e98 + 0x48 //m_nForceBone + 0x48
    #define OFFSET_STUDIOHDR            0x10f0 //CBaseAnimating!m_pStudioHdr
    #define OFFSET_AIMPUNCH				0x24a8 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1f48 //CPlayer!camera_origin
    #define OFFSET_VIEWANGLES			0x25a4 - 0x14 //m_ammoPoolCapacity - 0x14
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x34d4 //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x34e0 //m_hObserverTarget

    #define OFFSET_MATRIX				0x11a210
    #define OFFSET_RENDER				0x7665f38

    #define OFFSET_WEAPON				0x1a14 //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1ef0 //CWeaponX!m_flProjectileSpeed
    #define OFFSET_BULLET_SCALE         0x1ef8 //CWeaponX!m_flProjectileScale
    #define OFFSET_ZOOM_FOV             0x16b8 + 0xb8 //m_playerData + m_curZoomFOV
    #define OFFSET_AMMO                 0x1670 //m_ammoInClip

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#endif