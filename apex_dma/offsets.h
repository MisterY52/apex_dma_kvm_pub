//v3.0.3.26

#define OFFSET_ENTITYLIST 0x2022bf8
#define OFFSET_LOCAL_ENT 0x24F6288
#define OFFSET_NAME_LIST 0xd506390
#define OFFSET_THIRDPERSON 0x01e3b190 + 0x6c // thirdperson_override + 0x6c
#define OFFSET_TIMESCALE 0x01841ee0          // host_timescale

#define OFFSET_TEAM 0x0338           // m_iTeamNum
#define OFFSET_HEALTH 0x0328         // m_iHealth
#define OFFSET_SHIELD 0x01a0         // m_shieldHealth
#define OFFSET_NAME 0x0481           // m_iName
#define OFFSET_SIGN_NAME 0x0478      // m_iSignifierName
#define OFFSET_ABS_VELOCITY 0x0170   // m_vecAbsVelocity
#define OFFSET_VISIBLE_TIME 0x1990   // CPlayer!lastVisibleTime
#define OFFSET_ZOOMING 0x1be1        // m_bZooming
#define OFFSET_THIRDPERSON_SV 0x36c8 // m_thirdPersonShoulderView
#define OFFSET_YAW 0x223c - 0x8      // m_currentFramePlayer.m_ammoPoolCount - 0x8

#define OFFSET_LIFE_STATE 0x0690      // m_lifeState, >0 = dead
#define OFFSET_BLEED_OUT_STATE 0x2760 // m_bleedoutState, >0 = knocked

#define OFFSET_ORIGIN 0x017c            // m_vecAbsOrigin
#define OFFSET_BONES 0x0da8 + 0x48      // m_nForceBone + 0x48
#define OFFSET_STUDIOHDR 0xff0          // CBaseAnimating!m_pStudioHdr
#define OFFSET_AIMPUNCH 0x2438          // m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS 0x1ee0         // CPlayer!camera_origin
#define OFFSET_VIEWANGLES 0x2534 - 0x14 // m_ammoPoolCapacity - 0x14
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10
#define OFFSET_OBSERVER_MODE 0x34a4    // m_iObserverMode
#define OFFSET_OBSERVING_TARGET 0x34b0 // m_hObserverTarget
#define OFFSET_OBSERVER_LIST 0x02022b50 + 0x20C8

#define OFFSET_MATRIX 0x11A350
#define OFFSET_RENDER 0x77BD448

#define OFFSET_WEAPON 0x1934                // m_latestPrimaryWeapons
#define OFFSET_BULLET_SPEED 0x19d8 + 0x04ec // CWeaponX!m_flProjectileSpeed
#define OFFSET_BULLET_SCALE 0x19d8 + 0x04f4 // CWeaponX!m_flProjectileScale
#define OFFSET_ZOOM_FOV 0x15e0 + 0x00b8     // m_playerData + m_curZoomFOV
#define OFFSET_AMMO 0x1590                  // m_ammoInClip

#define OFFSET_ITEM_GLOW 0x02f0 // m_highlightFunctionBits

#define OFFSET_GLOW_T1 0x292            // 16256 = enabled, 0 = disabled
#define OFFSET_GLOW_T2 0x30c            // 1193322764 = enabled, 0 = disabled
#define OFFSET_GLOW_ENABLE 0x28C        // 7 = enabled, 2 = disabled
#define OFFSET_GLOW_THROUGH_WALLS 0x26c // 2 = enabled, 5 = disabled
