#define OFFSET_UID                  0x25c8 //m_platformUserId 
#define OFFSET_ARMOR_TYPE           0x4654 //m_armorType
#define OFFSET_MAXSHIELD            0x0174 //m_shieldhealthmax

#define OFFSET_XP                   0x36c4 //m_xp

#define OFFSET_WEAPON               0x1a14 //m_latestPrimaryWeapons
#define OFFSET_WEAPON_NAME          0x1858 //m_weaponNameIndex 2nd one
#define OFFSET_ENTITYLIST		0x1e53c68 //cl_entitylist
#define OFFSET_LOCAL_ENT		0x22036C0 + 0x8 //.?AVC_GameMovement@@ + 0x8
#define OFFSET_NAME_LIST            0xbe94be0 //NameList

#define OFFSET_TEAM					0x044c //m_iTeamNum
#define OFFSET_HEALTH				0x043c //m_iHealth
#define OFFSET_SHIELD				0x170 //m_shieldHealth
#define OFFSET_NAME					0x589 //m_iName
#define OFFSET_SIGN_NAME			0x580 //m_iSignifierName
#define OFFSET_ABS_VELOCITY         0x140 //m_vecAbsVelocity
#define OFFSET_VISIBLE_TIME         0x1A70 //CPlayer!lastVisibleTime
#define OFFSET_ZOOMING      		0x1c51 //m_bZooming
#define OFFSET_YAW                  0x22bc - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

#define OFFSET_LIFE_STATE			0x798  //m_lifeState, >0 = dead
#define OFFSET_BLEED_OUT_STATE		0x2750 //m_bleedoutState, >0 = knocked

#define OFFSET_ORIGIN				0x014c //m_vecAbsOrigin
#define OFFSET_BONES				0x0e98 + 0x48 //m_nForceBone + 0x48
#define OFFSET_STUDIOHDR            0x10e8 //CBaseAnimating!m_pStudioHdr
#define OFFSET_AIMPUNCH			0x24b8 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS			0x1f50 //CPlayer!camera_origin
#define OFFSET_VIEWANGLES			0x25b4 - 0x14 //m_ammoPoolCapacity - 0x14
#define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLES - 0x10
#define OFFSET_OBSERVER_MODE		0x34f4 //m_iObserverMode
#define OFFSET_OBSERVING_TARGET		0x3500 //m_hObserverTarget

#define OFFSET_MATRIX				0x11a350 //ViewMatrix
#define OFFSET_RENDER				0x743AB20 //ViewRender

#define OFFSET_BULLET_SPEED         0x1f3c
#define OFFSET_BULLET_SCALE         0x1f44
#define OFFSET_ZOOM_FOV             0x16b0 + 0xb8 //m_playerData + m_curZoomFOV
#define OFFSET_AMMO                 0x1634 //m_ammoInClip //1634
    
#define OFFSET_ITEM_GLOW            0x2c0 //m_highlightFunctionBits

#define OFFSET_GLOW_T1              0x262 //16256 = enabled, 0 = disabled 
#define OFFSET_GLOW_T2              0x2dc //1193322764 = enabled, 0 = disabled 
#define OFFSET_GLOW_ENABLE          0x3c8 //7 = enabled, 2 = disabled//for BH GLOW
#define OFFSET_GLOW_THROUGH_WALLS   0x3d0 //2 = enabled, 5 = disabled//for BH GLOW
#define GLOW_COLOR                  0x1D0 // Script_CopyHighlightState 15th mov
#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT                 OFFSET_GLOW_ENABLE // Script_Highlight_SetCurrentContext
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE     OFFSET_GLOW_THROUGH_WALLS // Script_Highlight_SetVisibilityType 5th mov
