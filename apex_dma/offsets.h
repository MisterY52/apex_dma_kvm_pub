#include "offsets_dynamic.h"

#define GameVersion v3.0.1.29 //[Miscellaneous].GameVersion updated 2025/01/08
#define OFFSET_ENTITYLIST (offsets.EntityList ? offsets.EntityList : 0x6b6ce88) //[Miscellaneous].cl_entitylist updated 2025/04/18
#define OFFSET_LOCAL_ENT (offsets.LocalEnt ? offsets.LocalEnt : 0x26c8128) //[Miscellaneous].LocalPlayer updated 2025/03/25
#define OFFSET_LOCAL_ENTITY_HANDLE (offsets.LocalEntityHandle ? offsets.LocalEntityHandle : 0x1ae6c0c) //[Miscellaneous].LocalEntityHandle updated 2025/04/18
#define OFFSET_NAME_LIST (offsets.NameList ? offsets.NameList : 0x95a7e90) //[Miscellaneous].NameList updated 2025/04/18
#define OFFSET_GLOBAL_VARS (offsets.GlobalVars ? offsets.GlobalVars : 0x01BB40D0) //[Miscellaneous].GlobalVars updated 2025/03/25
 
#define OFFSET_LEVELNAME (offsets.LevelName ? offsets.LevelName : 0x1BB5694) //[Miscellaneous].LevelName updated 2025/04/18
#define OFFSET_CLIENTSTATE OFFSET_LEVELNAME - 0x1c0 //ClientState  ClientState = LevelName - 0x1C0  //updated 1/10/2024
//#define OFFSET_CLIENTSTATE 0x1836670 //[Miscellaneous].ClientState updated 2024/10/09
#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE + 0xAC //SignonState (ClientState + 0xAC)  //updated 1/10/2024
//#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE 0x16d699c
#define OFFSET_HOST_MAP (offsets.HostMap ? offsets.HostMap : 0x01ad0680 + 0x58) //[ConVars].host_map + 0x58 updated 2025/04/18
 
#define OFFSET_TEAM (offsets.Team ? offsets.Team : 0x0338) //[RecvTable.DT_BaseEntity].m_iTeamNum updated 2025/03/25
#define OFFSET_HEALTH (offsets.Health ? offsets.Health : 0x0328) //[RecvTable.DT_Player].m_iHealth updated updated 2025/04/18
#define OFFSET_MAXHEALTH (offsets.MaxHealth ? offsets.MaxHealth : 0x0470) //[RecvTable.DT_Player].m_iMaxHealth

#define OFFSET_SHIELD (offsets.Shield ? offsets.Shield : 0x01a0) //[RecvTable.DT_BaseEntity].m_shieldHealth updated 2024/09/30
#define OFFSET_MAXSHIELD (offsets.MaxShield ? offsets.MaxShield : 0x1A4) //[RecvTable.DT_BaseEntity].m_shieldHealthMax updated 2024/09/30
#define OFFSET_ARMORTYPE (offsets.Armortype ? offsets.Armortype : 0x47ec) //[RecvTable.DT_Player].m_armorType updated updated 2025/04/18
#define OFFSET_NAME (offsets.Name ? offsets.Name : 0x0481) //[RecvTable.DT_BaseEntity].m_iName updated 2025/02/25
#define OFFSET_SIGN_NAME (offsets.SignName ? offsets.SignName : 0x0478) //[RecvTable.DT_BaseEntity].m_iSignifierName updated 2025/02/25
#define OFFSET_ABS_VELOCITY (offsets.AbsVelocity ? offsets.AbsVelocity : 0x0170) //[DataMap.DT_BaseEntity].m_vecAbsVelocity updated 2024/09/18
#define OFFSET_VISIBLE_TIME (offsets.VisibleTime ? offsets.VisibleTime : 0x19f0) //[Miscellaneous].CPlayer!lastVisibleTime updated 2025/04/18
#define OFFSET_LAST_AIMEDAT_TIME OFFSET_VISIBLE_TIME + 0x8   //[Miscellaneous].CPlayer!lastVisibleTime + 0x8
#define OFFSET_ZOOMING (offsets.Zooming ? offsets.Zooming : 0x1c41) //[RecvTable.DT_Player].m_bZooming updated 2025/04/18

#define OFFSET_VIEW_OFFSET (offsets.ViewOffset ? offsets.ViewOffset : 0x00e8) //[DataMap.DT_BaseEntity].m_currentFrame.viewOffset updated 2024/09/18
#define OFFSET_ACTIVE_WEAPON (offsets.ActiveWeapon ? offsets.ActiveWeapon : 0x1928 + 0x0058) //[RecvTable.DT_Player].m_inventory + WeaponInventory_Client>activeWeapons updated 2025/04/18

#define OFFSET_SPECTATOR_LIST OFFSET_OBSERVER_LIST //? updated 2024/11/15
//#define OFFSET_OBSERVER_LIST 0x01ff4ce0 + 0x20C8 //[ConVars].gamepad_aim_assist_melee + 0x20C8 updated 2025/01/08
#define OFFSET_OBSERVER_LIST  (offsets.ObserverList ? offsets.ObserverList : 0x0256ebe0) //[ConVars].gamepad_aim_assist_melee + 0x20C8 updated 2025/04/18
#define OFFSET_OBSERVER_ARRAY (offsets.ObserverArray ? offsets.ObserverArray : 0x954) //[DataMapTypes.DT_GlobalNonRewinding].m_playerObserver - [DataMap.C_ObserverMode].m_observerTarget updated 2024/09/20

#define OFFSET_IN_DUCKSTATE (offsets.InDuckState ? offsets.InDuckState : 0x2ab8) //[RecvTable.DT_Player].m_duckState updated 2025/04/18

#define OFFSET_IN_DUCK (offsets.InDuck ? offsets.InDuck : 0x03371200) //[Buttons].in_duck updated 2025/04/18
#define OFFSET_TRAVERSAL_PROGRESS (offsets.TraversalProgress ? offsets.TraversalProgress : 0x2bc4) //[DataMap.DT_Player].m_traversalProgress updated 2025/02/12
#define OFFSET_TRAVERSAL_STARTTIME (offsets.TraversalStartTime ? offsets.TraversalStartTime : 0x2bc8) //[DataMap.DT_Player].m_traversalStartTime updated 2025/02/12
#define OFFSET_TRAVERSAL_RELEASE_TIME 0x2bd0 //[RecvTable.DT_LocalPlayerExclusive].m_traversalReleaseTime updated 2025/02/12

#define OFFSET_IN_JUMP (offsets.InJump ? offsets.InJump : 0x03371110) //[Buttons].in_jump updated 2025/04/18
#define OFFSET_IN_TOGGLE_DUCK (offsets.InToggleDuck ? offsets.InToggleDuck : 0x03371030) //[Buttons].in_toggle_duck updated 2025/04/18
 
#define OFFSET_WEAPON_NAME (offsets.WeaponName ? offsets.WeaponName : 0x17e8) //[RecvTable.DT_WeaponX].m_weaponNameIndex updated 2025/02/25
#define OFFSET_OFF_WEAPON (offsets.OffWeapon ? offsets.OffWeapon : 0x19a4) //[DataMap.DT_BaseCombatCharacter].m_latestNonOffhandWeapons updated 2025/02/25
#define OFFSET_WALL_RUN_START_TIME (offsets.WallRunStartTime ? offsets.WallRunStartTime : 0x36fc) //[RecvTable.DT_LocalPlayerExclusive]->m_wallRunStartTime updated 2025/03/25
#define OFFSET_WALL_RUN_CLEAR_TIME (offsets.WallRunClearTime ? offsets.WallRunClearTime : 0x3700) //[RecvTable.DT_LocalPlayerExclusive]->m_wallRunClearTime updated 2025/03/25

#define OFFSET_FLAGS (offsets.Flags ? offsets.Flags : 0x00c8) //[DataMap.DT_Player].m_fFlags updated 2025/03/25
#define OFFSET_IN_ATTACK (offsets.InAttack ? offsets.InAttack : 0x03370ff0) //[Buttons].in_attack updated 2025/04/18
#define OFFSET_IN_ZOOM (offsets.InZoom ? offsets.InZoom : 0x03371188) //[Buttons].in_zoom updated 2025/04/18
#define OFFSET_IN_FORWARD (offsets.InForward ? offsets.InForward : 0x03370f40) //[Buttons].in_forward updated 2025/04/18
#define OFFSET_IN_BACKWARD (offsets.InBackward ? offsets.InBackward : 0x03370f68) //[Buttons].in_backward updated 2025/04/18
 
#define OFFSET_LIFE_STATE (offsets.LifeState ? offsets.LifeState : 0x0690) //[RecvTable.DT_Player].m_lifeState updated 2025/04/18
#define OFFSET_BLEED_OUT_STATE (offsets.BleedOutState ? offsets.BleedOutState : 0x27b8) //[RecvTable.DT_Player].m_bleedoutState updated 2025/04/18
 
#define OFFSET_ORIGIN (offsets.Origin ? offsets.Origin : 0x017c) //[DataMap.DT_BaseEntity].m_vecAbsOrigin updated 2025/01/08
#define OFFSET_BONES (offsets.Bones ? offsets.Bones : 0x0d88 + 0x48) //[RecvTable.DT_BaseAnimating].m_nForceBone + 0x48 updated 2025/04/18
#define OFFSET_STUDIOHDR (offsets.StudioHdr ? offsets.StudioHdr : 0xfd0) //[Miscellaneous].CBaseAnimating!m_pStudioHdr updated 2025/04/18
#define OFFSET_AIMPUNCH (offsets.AimPunch ? offsets.AimPunch : 0x2438) //[DataMap.DT_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2024/09/18
#define OFFSET_CAMERAPOS (offsets.CameraPos ? offsets.CameraPos : 0x1f40) //[Miscellaneous].CPlayer!camera_origin updated 2025/04/18
#define OFFSET_VIEWANGLES (offsets.ViewAngles ? offsets.ViewAngles : 0x2594 - 0x14) //[RecvTable.DT_Player].m_ammoPoolCapacity - 0x14 updated 2025/04/18
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10 //updated 01/10/2024
#define OFFSET_OBSERVER_MODE (offsets.ObserverMode ? offsets.ObserverMode : 0x35e4) //[RecvTable.DT_LocalPlayerExclusive].m_iObserverMode updated 2025/02/25
#define OFFSET_OBSERVING_TARGET (offsets.ObservingTarget ? offsets.ObservingTarget : 0x35f0) //[RecvTable.DT_LocalPlayerExclusive].m_hObserverTarget updated 2025/02/25
#define OFFSET_IN_USE (offsets.InUse ? offsets.InUse : 0x03371178) //[Buttons].in_use updated 2025/04/18
 
#define OFFSET_MATRIX (offsets.Matrix ? offsets.Matrix : 0x11a350) //[Miscellaneous].ViewMatrix updated 2025/04/18
#define OFFSET_RENDER (offsets.Render ? offsets.Render : 0x3370ee0) //[Miscellaneous].ViewRender updated 2025/04/18
 
#define OFFSET_WEAPON (offsets.Weapon ? offsets.Weapon : 0x1994) //[RecvTable.DT_BaseCombatCharacter].m_latestPrimaryWeapons updated 2025/04/18
#define OFFSET_BULLET_SPEED (offsets.BulletSpeed ? offsets.BulletSpeed : 0x2790) //0x19d8 + 0x04ec //0x1eb4 + 0x19d0 //[Miscellaneous].CWeaponX!m_flProjectileSpeed updated 2025/04/18
#define OFFSET_BULLET_SCALE (offsets.BulletScale ? offsets.BulletScale : 0x2798) //0x19d8 + 0x04f4 //0x1ebc + 0x19d0 //[Miscellaneous].CWeaponX!m_flProjectileScale updated 2025/04/18
#define OFFSET_ZOOM_FOV (offsets.ZoomFov ? offsets.ZoomFov : 0x16ec) //[RecvTable.DT_WeaponX].m_playerData + m_curZoomFOV updated 2025/02/25
#define OFFSET_AMMO (offsets.Ammo ? offsets.Ammo : 0x15b0) //[RecvTable.DT_PropSurvival].m_ammoInClip updated 2025/04/18
 
//#define OFFSET_ITEM_GLOW 0x02f0 //m_highlightFunctionBits
#define OFFSET_ITEM_ID (offsets.ItemId ? offsets.ItemId : 0x15b4) //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2025/04/18
#define OFFSET_MODELNAME (offsets.ModelName ? offsets.ModelName : 0x0030) //[DataMap.DT_BaseEntity].m_ModelName updated 2024/09/18
#define OFFSET_M_CUSTOMSCRIPTINT OFFSET_ITEM_ID //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2025/04/18
#define OFFSET_YAW (offsets.Yaw ? offsets.Yaw : 0x2294) //[DataMap.DT_Player].m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2025/02/12
 
//#define OFFSET_GLOW_T1 0x292 //16256 = enabled, 0 = disabled
//#define OFFSET_GLOW_T2 0x30c //1193322764 = enabled, 0 = disabled
#define OFFSET_GLOW_ENABLE (offsets.GlowEnable ? offsets.GlowEnable : 0x29c) //0x27c //0x28c ?//[RecvTable.DT_HighlightSettings].m_highlightGenericContexts - 0x1 updated 2024/08/31
#define OFFSET_GLOW_THROUGH_WALLS (offsets.GlowThroughWalls ? offsets.GlowThroughWalls : 0x26c) ////[RecvTable.DT_HighlightSettings.m_highlightGenericContexts - 0x21 updated 2024/07/23
 
#define OFFSET_TIME_BASE (offsets.TimeBase ? offsets.TimeBase : 0x20E8) //[DataMap.DT_Player].m_currentFramePlayer.timeBase updated 2025/02/12
#define GLOW_START_TIME 0x02c8 + 0x30 //m_playerFloatLookStartTime=0x02c4 //updated 01/10/2024 DataMap.C_PlayerLocalData
#define OFFSET_HIGHLIGHTSERVERACTIVESTATES 0x29C //[DT_HighlightSettings].m_highlightServerActiveStates updated 2024/06/26

#define OFFSET_GLOW_FIX 0x278 //0x268 //updated 2025/02/25
//#define GLOW_VISIBLE_TYPE 0x27c      // updated 6/26/2024
#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT OFFSET_GLOW_ENABLE // Script_Highlight_SetCurrentContext updated 01/10/2024
#define OFFSET_GLOW_CONTEXT_ID 0x29c                                            //updated 6/26/2024
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS // Script_Highlight_SetVisibilityType 5th mov updated 01/10/2024
#define GLOW_LIFE_TIME 0x3A4+ 0x30 // Script_Highlight_SetLifeTime + 4 updated 01/10/2024
#define GLOW_DISTANCE 0x264 //0x294 //0x26c // Script_Highlight_SetFarFadeDist updated 01/10/2024
#define GLOW_TYPE 0x29c // Script_Highlight_GetState + 4 updated 01/10/2024
#define GLOW_COLOR 0x1D0+ 0x30 // Script_CopyHighlightState 15th mov updated 01/10/2024
#define GLOW_FADE 0x388+ 0x30 // ats 3rd result of 3 offsets consecutive or first + 8 updated 01/10/2024
#define HIGHLIGHT_SETTINGS (offsets.HighlightSettings ? offsets.HighlightSettings : 0x72b6430) //[Miscellaneous].HighlightSettings updated 2025/04/18
#define HIGHLIGHT_TYPE_SIZE 0x34 //? updated 01/10/2024
#define OFFSET_CROSSHAIR_LAST OFFSET_VISIBLE_TIME + 0x19f8 //[Miscellaneous].CWeaponX!lastCrosshairTargetTime updated 2025/04/18
//#define OFFSET_CROSSHAIR_START 0x1958 //CPlayer!crosshairTargetStartTime updated 01/9/2024
#define OFFSET_INPUT_SYSTEM (offsets.InputSystem ? offsets.InputSystem : 0x1c64440) //[Miscellaneous].InputSystem updated 2025/04/18

#define OFFSET_SKYDIVE_STATE (offsets.SkydiveState ? offsets.SkydiveState : 0x484c) //[RecvTable.DT_Player].m_skydiveState updated 2025/04/18
#define OFFSET_GRAPPLEACTIVED       (offsets.GrappleActive ? offsets.GrappleActive : 0x2d90) //[RecvTable.DT_Player].m_grappleActive updated 2025/04/18
#define OFFSET_GRAPPLE              (offsets.Grapple ? offsets.Grapple : 0x2d08) //[RecvTable.DT_Player].m_grapple updated 2025/04/18
#define OFFSET_GRAPPLEATTACHED      (offsets.GrappleAttached ? offsets.GrappleAttached : 0x0048) //[RecvTable.DT_GrappleData].m_grappleAttached updated 2025/02/25
#define OFFSET_m_xp		    (offsets.m_xp ? offsets.m_xp : 0x380c) //[RecvTable.DT_Player].m_xp updated 2025/04/18
#define OFFSET_GRADE 0x0348 //[RecvTable.DT_BaseEntity].m_grade updated 2025/02/25
