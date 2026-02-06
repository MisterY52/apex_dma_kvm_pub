#ifndef OFFSETS_DYNAMIC_H
#define OFFSETS_DYNAMIC_H

#include <stdint.h>

struct DynamicOffsets {
    uint64_t EntityList;
    uint64_t LocalEnt;
    uint64_t LocalEntityHandle;
    uint64_t NameList;
    uint64_t GlobalVars;
    uint64_t LevelName;
    uint64_t ClientState;
    uint64_t SignonState;
    uint64_t HostMap;
    uint64_t Team;
    uint64_t Health;
    uint64_t MaxHealth;
    uint64_t Shield;
    uint64_t MaxShield;
    uint64_t Armortype;
    uint64_t Name;
    uint64_t SignName;
    uint64_t AbsVelocity;
    uint64_t VisibleTime;
    uint64_t Zooming;
    uint64_t ViewOffset;
    uint64_t ActiveWeapon;
    uint64_t ObserverList;
    uint64_t ObserverArray;
    uint64_t InDuckState;
    uint64_t InDuck;
    uint64_t TraversalProgress;
    uint64_t TraversalStartTime;
    uint64_t InJump;
    uint64_t InToggleDuck;
    uint64_t WeaponName;
    uint64_t OffWeapon;
    uint64_t WallRunStartTime;
    uint64_t WallRunClearTime;
    uint64_t Flags;
    uint64_t InAttack;
    uint64_t InZoom;
    uint64_t InForward;
    uint64_t InBackward;
    uint64_t LifeState;
    uint64_t BleedOutState;
    uint64_t Origin;
    uint64_t Bones;
    uint64_t StudioHdr;
    uint64_t AimPunch;
    uint64_t CameraPos;
    uint64_t ViewAngles;
    uint64_t ObserverMode;
    uint64_t ObservingTarget;
    uint64_t InUse;
    uint64_t Matrix;
    uint64_t Render;
    uint64_t Weapon;
    uint64_t BulletSpeed;
    uint64_t BulletScale;
    uint64_t ZoomFov;
    uint64_t Ammo;
    uint64_t ItemId;
    uint64_t ModelName;
    uint64_t Yaw;
    uint64_t GlowEnable;
    uint64_t GlowThroughWalls;
    uint64_t TimeBase;
    uint64_t HighlightSettings;
    uint64_t InputSystem;
    uint64_t SkydiveState;
    uint64_t GrappleActive;
    uint64_t Grapple;
    uint64_t GrappleAttached;
    uint64_t m_xp;
};

extern DynamicOffsets offsets;

bool load_offsets_from_ini(const char* offsets_file, const char* convars_file, const char* buttons_file);

#endif
