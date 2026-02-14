#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "offsets_dynamic.h"
#include "Game.h"
#include <thread>
#include <array>
#include <fstream>
////////////////////////
////////////////////////

Memory apex_mem;
Memory client_mem;

bool firing_range = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f * 40.0f;
int team_player = 0;
float max_fov = 5;
const int toRead = 100;

int aim = false;
bool esp = false;
bool skeleton = false;
//bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool aiming = false;

extern float smooth;
//added stuff
//extern float min_max_fov;
//extern float max_max_fov;
//extern float min_smooth;
//extern float max_smooth;
//en stuff

extern int bone;
bool shooting = false;

//const int SuperKey = VK_SPACE;
int SuperKey = false;
//bool SuperKeyToggle = true;

//int itementcount = 10000;

bool isGrappling;
int grappleAttached;

//Firing Range 1v1 toggle
bool onevone = false;

///////////
//bool medbackpack = true;
///////////
bool updateInsideValue_t = false;
///////////////////////////
//Player Glow Color and Brightness.
//inside fill
unsigned char insidevalue = 6;  //0 = no fill, 14 = full fill
//Outline size
unsigned char outlinesize = 32; // 0-255
//Not Visable 
float glowr = 1; //Red 0-255, higher is brighter color.
float glowg = 0; //Green 0-255, higher is brighter color.
float glowb = 0; //Blue 0-255, higher is brighter color.
//Visable
float glowrviz = 0; //Red 0-255, higher is brighter color.
float glowgviz = 1; //Green 0-255, higher is brighter color.
float glowbviz = 0; //Blue 0-255, higher is brighter color.
//Knocked
float glowrknocked = 0; //Red 0-255, higher is brighter color.
float glowgknocked = 0; //Green 0-255, higher is brighter color.
float glowbknocked = 1; //Blue 0-255, higher is brighter color.
//Item Configs
//loot Fill
//unsigned char lootfilled = 5;  //0 no fill, 14 100% fill
//loot outline siez
//unsigned char lootoutline = 0;
///////////////////////////

bool actions_t = false;
bool esp_t = false;
bool aim_t = false;
bool vars_t = false;
//bool item_t = false;
uint64_t g_Base;
uint64_t c_Base;
bool next = false;
bool valid = false;
bool lock = false;
bool is_aimentity_visible = false;

//map
int map = 0;

int screen_width = 2560;
int screen_height = 1440;

// DÃ©clarations au niveau global (ou dans votre classe si besoin)

int tapstrafe = 0;  // InitialisÃ© Ã  0, commence le comptage

bool forward_hold = false;  // InitialisÃ© Ã  faux, pas de clÃ© maintenue au dÃ©but

typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	int maxshield = 0;
	int armortype = 0;
	int player_xp_level = 0;
	char name[33] = { 0 };
	float bones[15][2] = { 0 };
}player;

typedef struct spectator{
	bool is_spec = false;
	char name[33] = { 0 };
}spectator;

struct Matrix
{
	float matrix[16];
};

spectator spectator_list[toRead];

float lastvis_esp[toRead];
float lastvis_aim[toRead];

int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;

////////////
int settingIndex;
int contextId;
std::array<float, 3> highlightParameter;
///////////

//////////////////////////////////////////
//works
// Inside SetPlayerGlow function
void SetPlayerGlow(Entity& LPlayer, Entity& Target, int index)
{
	if (player_glow >= 1)
    	{
			if (!Target.isGlowing() || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE] != 1) {
				float currentEntityTime = 5000.f;
				if (!isnan(currentEntityTime) && currentEntityTime > 0.f) {
					if (!(firing_range) && (Target.isKnocked() || !Target.isAlive()))
					{
						contextId = 5;
						settingIndex = 80;
						highlightParameter = { glowrknocked, glowgknocked, glowbknocked };
					}
					else if (Target.lastVisTime() > lastvis_aim[index] || (Target.lastVisTime() < 0.f && lastvis_aim[index] > 0.f))
					{
						contextId = 6;
						settingIndex = 81;
						highlightParameter = { glowrviz, glowgviz, glowbviz };
					}
					else
					{
						contextId = 7;
						settingIndex = 82;
						highlightParameter = { glowr, glowg, glowb };
					}
					Target.enableGlow();
				}
			}
    	}
    	//////////////////////////////////////////////////////////////////////////////////////////////////
		else if((player_glow == 0) && Target.isGlowing())
		{
			Target.disableGlow();
		}
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessPlayer(Entity &LPlayer, Entity &target, uint64_t entitylist, int index)
{
	char name[33];
	target.get_name(g_Base, index - 1, name);

	int entity_team = target.getTeamId();
	bool obs = target.Observing(entitylist);

	if (obs)
	{
		/*if(obs == LPlayer.ptr)
		{
			if (entity_team == team_player)
			{
				tmp_all_spec++;
			}
			else
			{
				tmp_spec++;
			}
		}*/
		tmp_spec++;
		return;
	}
	
	if (!target.isAlive())
	{
		if (target.Observing(LPlayer.ptr))
		{
			if (LPlayer.getTeamId() == entity_team)
				tmp_all_spec++;
			else
				tmp_spec++;
		}
		return;
	}

	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);
		if (dist > max_dist)
		return;

	if(!firing_range && !onevone)
		if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
			return;
	
	bool visible = (target.lastVisTime() > lastvis_aim[index]);
	float fov = CalculateFov(LPlayer, target);

	if (target.ptr == aimentity)
	{
		is_aimentity_visible = visible;
	}

	if (aimentity != 0 && lock)
	{
		// Stick to target
	}
	else if (aim == 2)
	{
		if (visible && fov <= max_fov)
		{
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if (aimentity == target.ptr && !shooting)
			{
				aimentity = tmp_aimentity = lastaimentity = 0;
			}
		}
	}
	else
	{
		if (fov <= max_fov)
		{
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
	}
	////
	SetPlayerGlow(LPlayer, target, index);
	////
	lastvis_aim[index] = target.lastVisTime();
}

////////////////////////////////////////
//Used to change things on a timer
/* unsigned char insidevalueItem = 0;
void updateInsideValue()
{
	updateInsideValue_t = true;
	while (updateInsideValue_t)
	{
		insidevalueItem++;
		insidevalueItem %= 256;
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		std::this_thread::sleep_for(std::chrono::seconds(2));
		printf("smooth: %f\n", smooth);
		printf("bone: %i\n", bone);
		printf("glowrnot: %f\n", glowr);
		printf("glowgnot: %f\n", glowg);
		printf("glowbnot: %f\n", glowb);
		
		//printf("%i\n", insidevalueItem);
		
	}
	updateInsideValue_t = false;
} */
//walljump +//////////////////////////////////////
	int onWallTmp = 0;
	int wallJumpNow = 0;
	int onWallOffTmp = 0;
	float onEdgeTmp = 0;

	//bool autoWallJumpEnabled = true; // Initialize auto wall jump as enabled
//walljump *//////////////////////////////////////

void DoActions()
{
	actions_t = true;

///////////////

//////////////

	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		while (g_Base != 0 && c_Base != 0)
		{
///////////////
			char MapName[200] = { 0 };
			uint64_t MapName_ptr = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_HOST_MAP, MapName_ptr);
			apex_mem.ReadArray<char>(MapName_ptr, MapName, 200);
					
			//printf("%s\n", MapName);
			if (strcmp(MapName, "mp_rr_tropic_island_mu1_storm") == 0) 
			{
				map = 1;
			} 
			else if (strcmp(MapName, "mp_rr_canyonlands_mu") == 0) 
			{
				map = 2;
			}
			else if (strcmp(MapName, "mp_rr_desertlands_hu") == 0) 
			{
				map = 3;
			}
			else if (strcmp(MapName, "mp_rr_olympus") == 0) 
			{
				map = 4;
			} 
			else  if (strcmp(MapName, "mp_rr_divided_moon") == 0)
			{
				map = 5;
			}
			else
			{
				map = 0;
			}
			
///////////////
			std::this_thread::sleep_for(std::chrono::milliseconds(30));

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

// Ensure the local player entity is valid
			if (LocalPlayer == 0)
				continue;

// Retrieve the local player entity object
Entity LPlayer = getEntity(LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player > 50 && !onevone)
			{
				continue;
			}

			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

//////////////////////////////////

//walljump ++/////////////////////////////////////
    bool success; // Declare success once
    int onWall;
    // Corrected memory read call
    success = apex_mem.Read<int>(LocalPlayer + OFFSET_WALL_RUN_START_TIME, onWall);
    if (success && onWall != onWallTmp)
    {
        int inForward;
        success = apex_mem.Read<int>(g_Base + OFFSET_IN_FORWARD, inForward);
        if (success && inForward == 0)  
        {
            wallJumpNow = 1;
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
        }
    }
    onWallTmp = onWall;

    int onWallOff;
    success = apex_mem.Read<int>(LocalPlayer + OFFSET_WALL_RUN_CLEAR_TIME, onWallOff);
    if (success && wallJumpNow == 1 && onWallOff != onWallOffTmp)
    {
        wallJumpNow = 0;
        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
    }
    onWallOffTmp = onWallOff;

    float onEdge;
    success = apex_mem.Read<float>(LocalPlayer + OFFSET_TRAVERSAL_PROGRESS, onEdge);
    if (success && onEdge != onEdgeTmp)
    {
        int inForward;
        success = apex_mem.Read<int>(g_Base + OFFSET_IN_FORWARD, inForward);
        if (success && inForward == 0)
        {
            wallJumpNow = 2;
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
            // Consider the impact of sleep here, potentially remove or adjust
            // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    onEdgeTmp = onEdge;

    if (wallJumpNow == 2)
    {
        uint32_t flags;
        success = apex_mem.Read<uint32_t>(LocalPlayer + OFFSET_FLAGS, flags);
        if (success && (flags & 0x1) == 1)
        {
            wallJumpNow = 0;
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
        }
    }

//walljump ++///////////////

    // SUPERGLIDE

       static float startjumpTime = 0;
       static bool startSg = false;
       static float traversalProgressTmp = 0.0;
        
       float worldtime;
       if (!apex_mem.Read<float>(LocalPlayer + OFFSET_TIME_BASE, worldtime)) {
         // error handling 
       }
        
       float traversalStartTime;
       if (!apex_mem.Read<float>(LocalPlayer + OFFSET_TRAVERSAL_STARTTIME, traversalStartTime)) {
         // error handling
       }
        
       float traversalProgress;
       if (!apex_mem.Read<float>(LocalPlayer + OFFSET_TRAVERSAL_PROGRESS, traversalProgress)) {
         // error handling
       }
        
       auto HangOnWall = -(traversalStartTime - worldtime);
        
       // Adjust thresholds and delays based on frame rate
       float wallHangThreshold = 0.1f;
       float wallHangMax = 1.5f;
       float traversalProgressThreshold = 0.87f;
       float jumpPressLoopTime = 0.011f;
       int duckActionDelay = 50;
       int jumpResetDelay = 800;
        
       // Check if SPACEBAR is pressed and held
       if (SuperKey) {
           if (HangOnWall > wallHangThreshold && HangOnWall < wallHangMax) {
             apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
           }
            
           if (traversalProgress > traversalProgressThreshold && !startSg && HangOnWall > wallHangThreshold && HangOnWall < wallHangMax) {
             // Start SG
             startjumpTime = worldtime; 
             startSg = true;
           }
            
           if (startSg) {
             // Press button
             apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
            
             float currentTime;
             while (true) {
               if (apex_mem.Read<float>(LocalPlayer + OFFSET_TIME_BASE, currentTime)) {
                 if (currentTime - startjumpTime < jumpPressLoopTime) {
                   // Keep looping
                   std::this_thread::sleep_for(std::chrono::microseconds(100));
                 } else {
                   break; 
                 }
               } else {
                 break;
               }
             }
             
             // Execute actions during SG
             apex_mem.Write<int>(g_Base + OFFSET_IN_DUCK + 0x8, 6);
             std::this_thread::sleep_for(std::chrono::milliseconds(duckActionDelay));
             apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
             std::this_thread::sleep_for(std::chrono::milliseconds(jumpResetDelay));
            
             startSg = false;
           }
       }
       //////////////////////////////

////////////////////////////////
//WALLJUMP END
////////////////////////////////

// Check if grapple is active
apex_mem.Read<bool>(LocalPlayer + OFFSET_GRAPPLEACTIVED, isGrappling);

// Check if grapple is attached
apex_mem.Read<int>(LocalPlayer + OFFSET_GRAPPLE + OFFSET_GRAPPLEATTACHED, grappleAttached);

// Main logic
if (isGrappling && grappleAttached == 1) {

  // Boost by jumping
  apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5); 
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);

} else {

  // Reset values
  isGrappling = false;
  grappleAttached = 0;

}

//grapple END/////////////////////////////

//bhop///
//if (bhop_enable) {
//apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
//std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
//apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
//std::this_thread::sleep_for(std::chrono::milliseconds(1));
//}
//bhop END/////////////////////////////

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}

			max = 999.0f;
			tmp_aimentity = 0;
			is_aimentity_visible = false;

			// Read shooting state from game
			kbutton_t in_attack_button;
			apex_mem.Read<kbutton_t>(g_Base + OFFSET_IN_ATTACK, in_attack_button);
			shooting = in_attack_button.down[0] != 0 || in_attack_button.down[1] != 0;

			tmp_spec = 0;
			tmp_all_spec = 0;

			memset(spectator_list,0, sizeof(spectator_list));

			if (firing_range)
			{
				int c = 0;
				for (int i = 0; i < 10000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
						continue;

					Entity Target = getEntity(centity);
					if (!Target.isDummy())
					{
						continue;
					}

					if (player_glow && !Target.isGlowing())
					{
						Target.enableGlow();
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}

					ProcessPlayer(LPlayer, Target, entitylist, c);
					c++;
				}
			}
			else
			{
				for (int i = 0; i < toRead; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
						continue;
					Entity Target = getEntity(centity);
					if (!Target.isPlayer())
					{
						continue;
					}
					
//////////////////
					float localyaw = LPlayer.GetYaw();
					float targetyaw = Target.GetYaw();
					if (!Target.isAlive() && localyaw == targetyaw) { // If this player is a spectator
					char temp_name[34];  // Assuming MAX_NAME_LENGTH + 1 for null terminator
					Target.get_name(g_Base, i - 1, &temp_name[0]);
					
					//Target.get_name(data_buf.name);					

    					if (temp_name[0]) { // Check if the player has a name (i.e., hasn't quit)
        				strcpy(spectator_list[i].name, temp_name);
        				spectator_list[i].is_spec = true;
    					} else {
        				spectator_list[i].is_spec = false; // Player has quit
        				strcpy(spectator_list[i].name, ""); // Clear the name
    					}
					} else {
    					spectator_list[i].is_spec = false;
    					strcpy(spectator_list[i].name, ""); // Clear the name if not a spectator
					}

					// Print spectator list
					//if (strlen(spectator_list[i].name) > 0) {
    					//printf("Spectator name: %s\n", spectator_list[i].name);
					//std::cout << "Corresponding level: " << player_level << std::endl;
					//}
//////////////////

					ProcessPlayer(LPlayer, Target, entitylist, i);

					int entity_team = Target.getTeamId();
					if (entity_team == team_player && !onevone)
					{
						continue;
					}

					if (player_glow && !Target.isGlowing())
					{
						Target.enableGlow();
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.enableGlow();
						//Target.disableGlow();
					}
				}
			}

			spectators = tmp_spec;
			allied_spectators = tmp_all_spec;

			if (!lock){
				aimentity = tmp_aimentity;
			}else{
				aimentity = lastaimentity;
			}
		}
	}

	actions_t = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

player players[toRead];

static void EspLoop()
{
	esp_t = true;
	while (esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while(g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
{
    next = true;
    while(next && g_Base != 0 && c_Base != 0 && esp)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    continue;
}

// Ensure the local player entity is valid
if (LocalPlayer == 0)
{
    next = true;
    while(next && g_Base != 0 && c_Base != 0 && esp)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    continue;
}

// Retrieve the local player entity object
Entity LPlayer = getEntity(LocalPlayer);

				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player > 50)
				{
					next = true;
					while(next && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Vector LocalPlayerPosition = LPlayer.getPosition();

				uint64_t viewRenderer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer);
				uint64_t viewMatrix = 0;
				apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrix);
				Matrix m = {};
				apex_mem.Read<Matrix>(viewMatrix, m);

				uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
				
				memset(players, 0, sizeof(players));
				if (firing_range)
				{
					int c = 0;
					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}

						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);

						if (!Target.isDummy())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();


						if (!onevone)
						{
							if (entity_team < 0 || entity_team>50 || entity_team == team_player)
							{
								continue;
							}
						}
						else
						{
							if (entity_team < 0 || entity_team>50)
							{
                              continue;
                            }
						}

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{	
							continue;
						}
						
						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, screen_width, screen_height, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							WorldToScreen(HeadPosition, m.matrix, screen_width, screen_height, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxShield();
							int armortype = Target.getArmortype();
							players[c] = 
							{
								dist,
								entity_team,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								0,
								(Target.lastVisTime() > lastvis_esp[c]),
								health,
								shield,
								maxshield,
								armortype,
								//Target.read_xp_level()
							};

							if (skeleton)
							{
								// 0:Pelvis, 1:UpperChest, 2:LowerChest, 3:L_Shoulder, 4:L_Elbow, 5:L_Hand, 6:R_Shoulder, 7:R_Elbow, 8:R_Hand, 9:L_Hip, 10:L_Knee, 11:L_Foot, 12:R_Hip, 13:R_Knee, 14:R_Foot
								int boneIDs[] = { 0, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18 };
								for (int b = 0; b < 15; b++)
								{
									Vector bonePos = Target.getBonePositionByHitbox(boneIDs[b]);
									Vector boneScreen = Vector();
									if (WorldToScreen(bonePos, m.matrix, screen_width, screen_height, boneScreen))
									{
										players[c].bones[b][0] = boneScreen.x;
										players[c].bones[b][1] = boneScreen.y;
									}
								}
							}

							Target.get_name(g_Base, i - 1, &players[c].name[0]);
							lastvis_esp[c] = Target.lastVisTime();
							valid = true;
							c++;
						}
					}
				}	
				else
				{
					for (int i = 0; i < toRead; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}
						
						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);
						
						if (!Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}

						int entity_team = Target.getTeamId();
						if (!onevone)
						{
							if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
							{
								continue;
							}
						}
						else
						{
							if (entity_team < 0 || entity_team>50)
							{
                              continue;
                            }						}

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{	
							continue;
						}

						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, screen_width, screen_height, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							WorldToScreen(HeadPosition, m.matrix, screen_width, screen_height, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxShield();
							int armortype = Target.getArmortype();
							players[i] = 
							{
								dist,
								entity_team,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								Target.isKnocked(),
								(Target.lastVisTime() > lastvis_esp[i]),
								health,
								shield,
								maxshield,
								armortype,
								//Target.read_xp_level()
							};

							if (skeleton)
							{
								// 0:Pelvis, 1:UpperChest, 2:LowerChest, 3:L_Shoulder, 4:L_Elbow, 5:L_Hand, 6:R_Shoulder, 7:R_Elbow, 8:R_Hand, 9:L_Hip, 10:L_Knee, 11:L_Foot, 12:R_Hip, 13:R_Knee, 14:R_Foot
								int boneIDs[] = { 0, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18 };
								for (int b = 0; b < 15; b++)
								{
									Vector bonePos = Target.getBonePositionByHitbox(boneIDs[b]);
									Vector boneScreen = Vector();
									if (WorldToScreen(bonePos, m.matrix, screen_width, screen_height, boneScreen))
									{
										players[i].bones[b][0] = boneScreen.x;
										players[i].bones[b][1] = boneScreen.y;
									}
								}
							}

							Target.get_name(g_Base, i - 1, &players[i].name[0]);
							lastvis_esp[i] = Target.lastVisTime();
							valid = true;
						}
					}
				}

				next = true;
				while(next && g_Base != 0 && c_Base != 0 && esp)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	esp_t = false;
}

static void AimbotLoop()
{
	static uintptr_t last_locked_entity = 0;
	static std::chrono::steady_clock::time_point lock_start_time;

	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			if (aim > 0)
			{
				if (aimentity == 0 || !aiming)
				{
					lock = false;
					lastaimentity = 0;
					last_locked_entity = 0;
					continue;
				}

				Entity Target = getEntity(aimentity);
				if (!Target.isAlive() || (Target.isKnocked() && !firing_range))
				{
					lock = false;
					lastaimentity = 0;
					last_locked_entity = 0;
					aimentity = 0;
					continue;
				}

				lock = true;
				lastaimentity = aimentity;

				if (aimentity != last_locked_entity) {
					last_locked_entity = aimentity;
					lock_start_time = std::chrono::steady_clock::now();
				}

				float current_smooth = smooth;
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lock_start_time).count();
				if (elapsed < 500) {
					current_smooth = smooth * 2.0f;
				}

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

				if (LocalPlayer == 0)
					continue;

				Entity LPlayer = getEntity(LocalPlayer);

				float fov = CalculateFov(LPlayer, Target);
				if (fov > max_fov)
				{
					if (!shooting) {
						lock = false;
						lastaimentity = 0;
						last_locked_entity = 0;
						aimentity = 0;
					}
					continue;
				}

				if (aim == 2 && !is_aimentity_visible)
				{
					continue;
				}

				QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, max_fov, current_smooth);
				if (Angles.x == 0 && Angles.y == 0)
				{
					continue;
				}

				QAngle currentAngles = LPlayer.GetViewAngles();
				QAngle Delta = Angles - currentAngles;
				Math::NormalizeAngles(Delta);
				if (abs(Delta.x) > 0.001f || abs(Delta.y) > 0.001f)
				{
					LPlayer.SetViewAngles(Angles);
				}
			}
		}
	}
	aim_t = false;
}

static void set_vars(uint64_t add_addr)
{
	printf("Reading client vars...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//Get addresses of client vars
uint64_t check_addr = 0;
printf("Reading check address: %lx\n", add_addr);
if(!client_mem.Read<uint64_t>(add_addr, check_addr)) {
  printf("Read failed!\n");
}

uint64_t aim_addr = 0;  
printf("Reading aim address: %lx\n", add_addr + sizeof(uint64_t));
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t), aim_addr)) {
  printf("Read failed!\n");
}

uint64_t esp_addr = 0;
printf("Reading esp address: %lx\n", add_addr + sizeof(uint64_t) * 2);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 2, esp_addr)) {
  printf("Read failed!\n");
}
uint64_t aiming_addr = 0;
printf("Reading aiming address: %lx\n", add_addr + sizeof(uint64_t) * 3);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 3, aiming_addr)) {
  printf("Read failed!\n");
}

uint64_t g_Base_addr = 0;
printf("Reading g_Base address: %lx\n", add_addr + sizeof(uint64_t) * 4);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 4, g_Base_addr)) {
  printf("Read failed!\n");
}

uint64_t next_addr = 0;  
printf("Reading next address: %lx\n", add_addr + sizeof(uint64_t) * 5);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 5, next_addr)) {
  printf("Read failed!\n");
}
uint64_t player_addr = 0;
printf("Reading player address: %lx\n", add_addr + sizeof(uint64_t) * 6);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 6, player_addr)) {
  printf("Read failed!\n");
}

uint64_t valid_addr = 0;
printf("Reading valid address: %lx\n", add_addr + sizeof(uint64_t) * 7);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 7, valid_addr)) {
  printf("Read failed!\n");
}

uint64_t max_dist_addr = 0;
printf("Reading max_dist address: %lx\n", add_addr + sizeof(uint64_t) * 8);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 8, max_dist_addr)) {
  printf("Read failed!\n");
}

//uint64_t item_glow_addr = 0;
//printf("Reading item_glow address: %lx\n", add_addr + sizeof(uint64_t) * 9);
//if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 9, item_glow_addr)) {
 // printf("Read failed!\n");
//}

uint64_t player_glow_addr = 0;
printf("Reading player_glow address: %lx\n", add_addr + sizeof(uint64_t) * 9);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 9, player_glow_addr)) {
  printf("Read failed!\n");
}

uint64_t aim_no_recoil_addr = 0;
printf("Reading aim_no_recoil address: %lx\n", add_addr + sizeof(uint64_t) * 10);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 10, aim_no_recoil_addr)) {
  printf("Read failed!\n");
}

uint64_t smooth_addr = 0;
printf("Reading smooth address: %lx\n", add_addr + sizeof(uint64_t) * 11);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 11, smooth_addr)) {
  printf("Read failed!\n");
}

uint64_t max_fov_addr = 0;
printf("Reading max_fov address: %lx\n", add_addr + sizeof(uint64_t) * 12); 
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 12, max_fov_addr)) {
  printf("Read failed!\n");
}

uint64_t bone_addr = 0;
printf("Reading bone address: %lx\n", add_addr + sizeof(uint64_t) * 13);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 13, bone_addr)) {
  printf("Read failed!\n");
}

uint64_t spectators_addr = 0;
printf("Reading spectators address: %lx\n", add_addr + sizeof(uint64_t) * 14);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 14, spectators_addr)) {
  printf("Read failed!\n");
}

uint64_t allied_spectators_addr = 0;  
printf("Reading allied_spectators address: %lx\n", add_addr + sizeof(uint64_t) * 15);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 15, allied_spectators_addr)) {
  printf("Read failed!\n");
}

uint64_t glowr_addr = 0;
printf("Reading glowr address: %lx\n", add_addr + sizeof(uint64_t) * 16);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 16, glowr_addr)) {
  printf("Read failed!\n");
}

uint64_t glowg_addr = 0;
printf("Reading glowg address: %lx\n", add_addr + sizeof(uint64_t) * 17);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 17, glowg_addr)) {
  printf("Read failed!\n");
}

uint64_t glowb_addr = 0;
printf("Reading glowb address: %lx\n", add_addr + sizeof(uint64_t) * 18);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 18, glowb_addr)) {
  printf("Read failed!\n");
}

uint64_t glowrviz_addr = 0;
printf("Reading glowrviz address: %lx\n", add_addr + sizeof(uint64_t) * 19);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 19, glowrviz_addr)) {
  printf("Read failed!\n");
}

uint64_t glowgviz_addr = 0;
printf("Reading glowgviz address: %lx\n", add_addr + sizeof(uint64_t) * 20);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 20, glowgviz_addr)) {
  printf("Read failed!\n");
}

uint64_t glowbviz_addr = 0;
printf("Reading glowbviz address: %lx\n", add_addr + sizeof(uint64_t) * 21);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 21, glowbviz_addr)) {
  printf("Read failed!\n");
}

uint64_t glowrknocked_addr = 0;
printf("Reading glowrknocked address: %lx\n", add_addr + sizeof(uint64_t) * 22);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 22, glowrknocked_addr)) {
  printf("Read failed!\n");
}

uint64_t glowgknocked_addr = 0;  
printf("Reading glowgknocked address: %lx\n", add_addr + sizeof(uint64_t) * 23);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 23, glowgknocked_addr)) {
  printf("Read failed!\n");
}

uint64_t glowbknocked_addr = 0;
printf("Reading glowbknocked address: %lx\n", add_addr + sizeof(uint64_t) * 24);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 24, glowbknocked_addr)) {
  printf("Read failed!\n"); 
}

uint64_t firing_range_addr = 0;
printf("Reading firing_range address: %lx\n", add_addr + sizeof(uint64_t) * 25);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 25, firing_range_addr)) {
  printf("Read failed!\n"); 
}

uint64_t shooting_addr = 0;
printf("Reading shooting address: %lx\n", add_addr + sizeof(uint64_t) * 26);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 26, shooting_addr)) {
  printf("Read failed!\n"); 
}

////////

uint64_t onevone_addr = 0;
printf("Reading onevone address: %lx\n", add_addr + sizeof(uint64_t) * 27);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 27, onevone_addr)) {
  printf("Read failed!\n");
}

uint64_t spec_list_addr = 0;
printf("Reading spec_list address: %lx\n", add_addr + sizeof(uint64_t) * 28);
if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 28, spec_list_addr)) {
  printf("Read failed!\n");
}

//
//uint64_t min_max_fov_addr = 0;
//printf("Reading min_max_fov address: %lx\n", add_addr + sizeof(uint64_t) * 29);
//if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 29, min_max_fov_addr)) {
//  printf("Read failed!\n");
//}
//
//uint64_t max_max_fov_addr = 0;
//printf("Reading max_max_fov address: %lx\n", add_addr + sizeof(uint64_t) * 30);
//if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 30, max_max_fov_addr)) {
//  printf("Read failed!\n");
//}
//
//uint64_t min_smooth_addr = 0;
//printf("Reading min_smooth address: %lx\n", add_addr + sizeof(uint64_t) * 31);
//if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 31, min_smooth_addr)) {
//  printf("Read failed!\n");
//}
//
//uint64_t max_smooth_addr = 0;
//printf("Reading max_smooth address: %lx\n", add_addr + sizeof(uint64_t) * 32);
//if(!client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 32, max_smooth_addr)) {
//  printf("Read failed!\n");
//}

////////

uint32_t check = 0;
client_mem.Read<uint32_t>(check_addr, check);

if (check != 0xABCD)
{
    printf("Incorrect values read. Check if the add_off is correct. Quitting.\n");
    active = false;
    return;
}

bool new_client = true;
vars_t = true;

while (vars_t)
{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (new_client && c_Base != 0 && g_Base != 0)
		{
			client_mem.Write<uint32_t>(check_addr, 0);
			new_client = false;
			printf("\nReady\n");
		}

    while (c_Base != 0 && g_Base != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        client_mem.Write<uint64_t>(g_Base_addr, g_Base);
        client_mem.Write<int>(spectators_addr, spectators);
        client_mem.Write<int>(allied_spectators_addr, allied_spectators);

        client_mem.Read<int>(aim_addr, aim);
        client_mem.Read<bool>(esp_addr, esp);
        client_mem.Read<bool>(aiming_addr, aiming);
        client_mem.Read<float>(max_dist_addr, max_dist);
        client_mem.Read<bool>(player_glow_addr, player_glow);
        client_mem.Read<bool>(aim_no_recoil_addr, aim_no_recoil);
        client_mem.Read<float>(smooth_addr, smooth);
        client_mem.Read<float>(max_fov_addr, max_fov);
        client_mem.Read<int>(bone_addr, bone);

        client_mem.Read<float>(glowr_addr, glowr);
        client_mem.Read<float>(glowg_addr, glowg);
        client_mem.Read<float>(glowb_addr, glowb);
        client_mem.Read<float>(glowrviz_addr, glowrviz);
        client_mem.Read<float>(glowgviz_addr, glowgviz);
        client_mem.Read<float>(glowbviz_addr, glowbviz);
        client_mem.Read<float>(glowrknocked_addr, glowrknocked);
        client_mem.Read<float>(glowgknocked_addr, glowgknocked);
        client_mem.Read<float>(glowbknocked_addr, glowbknocked);
        client_mem.Read<bool>(firing_range_addr, firing_range);
        //client_mem.Read<bool>(shooting_addr, shooting);
        client_mem.Read<bool>(onevone_addr, onevone);

        uint64_t skeleton_addr = 0;
        client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 31, skeleton_addr);
        if (skeleton_addr)
            client_mem.Read<bool>(skeleton_addr, skeleton);

        uint64_t dump_req_addr = 0;
        client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 29, dump_req_addr);
        if (dump_req_addr) {
            bool dump_req = false;
            client_mem.Read<bool>(dump_req_addr, dump_req);
            if (dump_req)
            {
                printf("Dump requested\n");
                if (apex_mem.Dump("dump.bin"))
                {
                    printf("Running r5dumper...\n");
                    system("cd r5dumper && ./r5dumper ../dump.bin");
                }
                client_mem.Write<bool>(dump_req_addr, false);
            }
        }

        uint64_t update_req_addr = 0;
        client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 30, update_req_addr);
        if (update_req_addr) {
            bool update_req = false;
            client_mem.Read<bool>(update_req_addr, update_req);
            if (update_req)
            {
                printf("Update offsets requested\n");
                load_offsets_from_ini("r5dumper/_offsets.ini", "r5dumper/_convars.ini", "r5dumper/_buttons.ini");
                client_mem.Write<bool>(update_req_addr, false);
            }
        }

        client_mem.WriteArray<spectator>(spec_list_addr, spectator_list, toRead);

        uint64_t screen_width_addr = 0;
        client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 32, screen_width_addr);
        if (screen_width_addr)
            client_mem.Read<int>(screen_width_addr, screen_width);

        uint64_t screen_height_addr = 0;
        client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 33, screen_height_addr);
        if (screen_height_addr)
            client_mem.Read<int>(screen_height_addr, screen_height);

        if (esp && next)
        {
            if (valid)
                client_mem.WriteArray<player>(player_addr, players, toRead);
            client_mem.Write<bool>(valid_addr, valid);
            client_mem.Write<bool>(next_addr, true); //next

            bool next_val = false;
            do
            {
                client_mem.Read<bool>(next_addr, next_val);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } while (next_val && g_Base != 0 && c_Base != 0);

            next = false;
        }
    }
}
vars_t = false;
}

// Item Glow Stuff


int main(int argc, char *argv[])
{
	mf_log_init(LevelFilter::LevelFilter_Info);

	if(geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}

	const char* cl_proc = "Client.exe";
	const char* ap_proc = "r5apex_dx12.ex";
	//const char* ap_proc = "EasyAntiCheat_launcher.exe";

	//Client "add" offset
	uint64_t add_off = 0x000000;
	std::thread aimbot_thr;
	std::thread esp_thr;
	std::thread actions_thr;
	//std::thread itemglow_thr;

	std::thread vars_thr;
	bool proc_not_found = false;
	while (active)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (aim_t)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				//item_t = false;
				g_Base = 0;

				aimbot_thr.~thread();
				esp_thr.~thread();
				actions_thr.~thread();
				//itemglow_thr.~thread();

			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");
			proc_not_found = apex_mem.get_proc_status() == process_status::NOT_FOUND;
			if (proc_not_found)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				printf("Searching for apex process...\n");
			}

			apex_mem.open_proc(ap_proc);

			if (apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				if (proc_not_found)
				{
					printf("\nApex process found\n");
					printf("Base: %lx\n", g_Base);
				}

				aimbot_thr = std::thread(AimbotLoop);
				esp_thr = std::thread(EspLoop);
				actions_thr = std::thread(DoActions);
				//itemglow_thr = std::thread(item_glow_t);
				aimbot_thr.detach();
				esp_thr.detach();
				actions_thr.detach();
				//itemglow_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}

		if (client_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (vars_t)
			{
				vars_t = false;
				c_Base = 0;

				vars_thr.~thread();
			}
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for client process...\n");

			client_mem.open_proc(cl_proc);

			if (client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nClient process found\n");
				printf("Base: %lx\n", c_Base);

				vars_thr = std::thread(set_vars, c_Base + add_off);
				vars_thr.detach();
			}
		}
		else
		{
			client_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}
