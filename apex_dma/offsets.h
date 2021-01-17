#define ORIGIN 1
#define STEAM 2

#define VERSION STEAM

#if VERSION == STEAM

    #define OFFSET_ENTITYLIST			0x18cdce8
    #define OFFSET_LOCAL_ENT			0x1c7cf28 //LocalPlayer
    #define OFFSET_NAME_LIST            0x816bdb0

    #define OFFSET_TEAM					0x4b0 //m_iTeamNum
    #define OFFSET_HEALTH				0x4a0 //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x5e9 //m_iName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1AEC
    #define OFFSET_ZOOMING      		0x1c01 //m_bZooming

    #define OFFSET_LIFE_STATE			0x7f8  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x26b0 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x14c //m_vecAbsOrigin
    #define OFFSET_BONES				0xF98 //m_bConstrainBetweenEndpoints
    #define OFFSET_AIMPUNCH				0x2450 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1EF0
    #define OFFSET_VIEWANGLES			0x2528
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x3364 //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x3368 //m_hObserverTarget

    #define OFFSET_MATRIX				0x1b3bd0
    #define OFFSET_RENDER				0x407dff8

    #define OFFSET_WEAPON				0x1a8c //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1ea8
    #define OFFSET_BULLET_SCALE         0x1eb0
    #define OFFSET_ZOOM_FOV             0x16e8 + 0xb8 //m_playerData + m_curZoomFOV

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x1ac //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#elif VERSION == ORIGIN

    #define OFFSET_ENTITYLIST			0x18cdce8
    #define OFFSET_LOCAL_ENT			0x1c7cf28 //LocalPlayer
    #define OFFSET_NAME_LIST            0x816bdb0

    #define OFFSET_TEAM					0x4b0 //m_iTeamNum
    #define OFFSET_HEALTH				0x4a0 //m_iHealth
    #define OFFSET_SHIELD				0x170 //m_shieldHealth
    #define OFFSET_NAME					0x5e9 //m_iName
    #define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
    #define OFFSET_VISIBLE_TIME         0x1AEC
    #define OFFSET_ZOOMING      		0x1c01 //m_bZooming

    #define OFFSET_LIFE_STATE			0x7f8  //m_lifeState, >0 = dead
    #define OFFSET_BLEED_OUT_STATE		0x26b0 //m_bleedoutState, >0 = knocked

    #define OFFSET_ORIGIN				0x14c //m_vecAbsOrigin
    #define OFFSET_BONES				0xF98 //m_bConstrainBetweenEndpoints
    #define OFFSET_AIMPUNCH				0x2450 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    #define OFFSET_CAMERAPOS			0x1EF0
    #define OFFSET_VIEWANGLES			0x2528
    #define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
    #define OFFSET_OBSERVER_MODE		0x3364 //m_iObserverMode
    #define OFFSET_OBSERVING_TARGET		0x3368 //m_hObserverTarget

    #define OFFSET_MATRIX				0x1b3bd0
    #define OFFSET_RENDER				0x407dff8

    #define OFFSET_WEAPON				0x1a8c //m_latestPrimaryWeapons
    #define OFFSET_BULLET_SPEED         0x1ea8
    #define OFFSET_BULLET_SCALE         0x1eb0
    #define OFFSET_ZOOM_FOV             0x16e8 + 0xb8 //m_playerData + m_curZoomFOV

    #define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

    #define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
    #define OFFSET_GLOW_T2              0x1ac //1193322764 = enabled, 0 = disabled 
    #define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled
    #define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled

#endif