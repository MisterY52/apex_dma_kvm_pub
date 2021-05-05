#define ORIGIN 1
#define STEAM 2

#define VERSION STEAM

#if VERSION == STEAM

    #define OFFSET_ENTITYLIST			0x18eda78
    #define OFFSET_LOCAL_ENT			0x1c9d198 //LocalPlayer
    #define OFFSET_NAME_LIST            0x81af4c0

    #define OFFSET_TEAM					0x448 //m_iTeamNum
    #define OFFSET_HEALTH				0x438 //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x589 //m_iName
    #define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1ACC
    #define OFFSET_ZOOMING      		0x1be1 //m_bZooming

    #define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x2688 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x14c //m_vecAbsOrigin
    #define OFFSET_BONES				0xF38 //m_bConstrainBetweenEndpoints
    #define OFFSET_AIMPUNCH				0x2430 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1ED0
    #define OFFSET_VIEWANGLES			0x252c - 0x14 //m_ammoPoolCapacity - 0x14
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x334C //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x3350 //m_hObserverTarget

    #define OFFSET_MATRIX				0x1b3bd0
    #define OFFSET_RENDER				0x40BF2E8

    #define OFFSET_WEAPON				0x1a6c //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1ED0
    #define OFFSET_BULLET_SCALE         0x1ED8
    #define OFFSET_ZOOM_FOV             0x16f8 + 0xb8 //m_playerData + m_curZoomFOV

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#elif VERSION == ORIGIN

    #define OFFSET_ENTITYLIST			0x18eda78
    #define OFFSET_LOCAL_ENT			0x1c9d198 //LocalPlayer
    #define OFFSET_NAME_LIST            0x81af4c0

    #define OFFSET_TEAM					0x448 //m_iTeamNum
    #define OFFSET_HEALTH				0x438 //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x589 //m_iName
    #define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1ACC
    #define OFFSET_ZOOMING      		0x1be1 //m_bZooming

    #define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x2688 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x14c //m_vecAbsOrigin
    #define OFFSET_BONES				0xF38 //m_bConstrainBetweenEndpoints
    #define OFFSET_AIMPUNCH				0x2430 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1ED0
    #define OFFSET_VIEWANGLES			0x252c - 0x14 //m_ammoPoolCapacity - 0x14
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x334C //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x3350 //m_hObserverTarget

    #define OFFSET_MATRIX				0x1b3bd0
    #define OFFSET_RENDER				0x40BF2E8

    #define OFFSET_WEAPON				0x1a6c //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1ED0
    #define OFFSET_BULLET_SCALE         0x1ED8
    #define OFFSET_ZOOM_FOV             0x16f8 + 0xb8 //m_playerData + m_curZoomFOV

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#endif