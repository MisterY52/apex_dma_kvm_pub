#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "Game.h"
#include <thread>

Memory apex_mem;
Memory client_mem;

uint64_t add_off = 0x40980;

bool freecam = false;
bool lockall_enable = false;

bool firing_range = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float kmax = 999.0f;
float max_dist = 200.0f * 40.0f;

//rcs setting
float rcs_pitch = 0.3f;
float rcs_yaw = 0.4f;

int team_player = 0;
float max_fov = 15;
const int toRead = 100;
int aim = false;
bool esp = false;
bool item_glow = false;
bool player_glow = false;
extern bool aim_no_recoil;
bool aiming = false;
extern float smooth;
extern int bone;
bool shooting = false;

bool actions_t = false;
bool esp_t = false;
bool aim_t = false;
bool vars_t = false;
bool item_t = false;
uint64_t g_Base;
uint64_t c_Base;
bool knext = false;
bool valid = false;
bool klock = false;

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
	int xp_level = 0;
	int maxshield = 0;
	int armortype = 0;
	char name[33] = { 0 };
	char model_name[100] = { 0 };
}player;

typedef struct spectator{
	bool is_spec = false;
	int xp_level = 0;
	char name[33] = { 0 };
}spectator;

struct Matrix
{
	float matrix[16];
};

spectator spectator_list[toRead];

float lastvis_esp[toRead]; float lastvis_aim[toRead];

int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessPlayer(Entity& LPlayer, Entity& target, uint64_t entitylist, int index)
{
	int entity_team = target.getTeamId();

	if (!target.isAlive())
	{
		float localyaw = LPlayer.GetYaw();
		float targetyaw = target.GetYaw();

		if (localyaw == targetyaw)
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
	if (dist > max_dist) return;

	if (!firing_range && !lockall_enable)
		if (entity_team < 0 || entity_team>50 || entity_team == team_player) return;

	if (aim == 2)
	{
		if ((target.lastVisTime() > lastvis_aim[index]))
		{
			float fov = CalculateFov(LPlayer, target);
			if (fov < kmax)
			{
				kmax = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if (aimentity == target.ptr)
			{
				aimentity = tmp_aimentity = lastaimentity = 0;
			}
		}
	}
	else
	{
		float fov = CalculateFov(LPlayer, target);
		if (fov < kmax)
		{
			kmax = fov;
			tmp_aimentity = target.ptr;
		}
	}
	lastvis_aim[index] = target.lastVisTime();
}

void DoActions()
{
	actions_t = true;
	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		uint32_t counter = 0;

		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

			if (LocalPlayer == 0) continue;

			Entity LPlayer = getEntity(LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player>50 && !lockall_enable)
			{
				continue;
			}


			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}

			kmax = 999.0f;
			tmp_aimentity = 0;
			tmp_spec = 0;
			tmp_all_spec = 0;

			memset(spectator_list,0, sizeof(spectator_list));

			int current_obid;
			if(LPlayer.isAlive()){
				if (freecam) {
					apex_mem.Write<int>(LocalPlayer + OFFSET_OBSERVER_MODE, 7);
				}
				else {
					apex_mem.Read<int>(LocalPlayer + OFFSET_OBSERVER_MODE, current_obid);
					if (current_obid == 7) {
						apex_mem.Write<int>(LocalPlayer + OFFSET_OBSERVER_MODE, 0);
					}
				}
			}

			if (firing_range)
			{
				int c = 0;
				for (int i = 0; i < 10000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0) continue;
					if (LocalPlayer == centity) continue;

					Entity Target = getEntity(centity);
					if (!Target.isDummy())
					{
						continue;
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
					if (centity == 0) continue;
					if (LocalPlayer == centity) continue;

					Entity Target = getEntity(centity);
					if (!Target.isPlayer())
					{
						continue;
					}

					float localyaw = LPlayer.GetYaw();
					float targetyaw = Target.GetYaw();
					if(!Target.isAlive() && localyaw == targetyaw){//if this player a spectator
						Target.get_name(g_Base,i-1,&spectator_list[i].name[0]);
						int xp;
						apex_mem.Read<int>(centity + OFFSET_XP, xp);
						int xp_level = calc_level(xp);
						spectator_list[i].xp_level = xp_level;
						if(spectator_list[i].name[0]){ //detect if this player quit
							spectator_list[i].is_spec = true;
						}else{spectator_list[i].is_spec = false;}
					}else{
						spectator_list[i].is_spec = false;
					}
					ProcessPlayer(LPlayer, Target, entitylist, i);

					int entity_team = Target.getTeamId();
					if (entity_team == team_player && !lockall_enable)
					{
						continue;
					}
				}
			}

			if (!spectators && !allied_spectators)
			{
				spectators = tmp_spec;
				allied_spectators = tmp_all_spec;
			}
			else
			{
				//refresh spectators count every ~2 seconds
				counter++;
				if (counter == 70)
				{
					spectators = tmp_spec;
					allied_spectators = tmp_all_spec;
					counter = 0;
				}
			}

			if (!klock){
				aimentity = tmp_aimentity;
			}else{
				aimentity = lastaimentity;
			}
		}
	}
	actions_t = false;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////

player players[toRead];

static void EspLoop()
{
	esp_t = true;
	while (esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
				{
					knext = true;
					while (knext && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Entity LPlayer = getEntity(LocalPlayer);
				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player>50)
				{
					knext = true;
					while (knext && g_Base != 0 && c_Base != 0 && esp)
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

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{
							continue;
						}

						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, 1920, 1080, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							WorldToScreen(HeadPosition, m.matrix, 1920, 1080, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxshield();
							int armortype = Target.getArmortype();
							//xp
							int xp;
							apex_mem.Read<int>(centity + OFFSET_XP, xp);
							int xp_level = calc_level(xp);
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
								xp_level,
								maxshield,
								armortype
							};
							Target.get_name(g_Base, i - 1, &players[c].name[0]);
							uint64_t model_ptr;
							apex_mem.Read(centity + OFFSET_MODELNAME, model_ptr);
							apex_mem.ReadArray(model_ptr,&players[c].model_name[0], 99);

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
						Entity Lplayer = getEntity(LocalPlayer);

						if (!Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}

						int entity_team = Target.getTeamId();
						if (!lockall_enable) {
							if (entity_team < 0 || entity_team>50 || entity_team == team_player)
							{
								continue;
							}
						}
						else {
							if (entity_team < 0 || entity_team>50) {
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
						WorldToScreen(EntityPosition, m.matrix, 1920, 1080, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							WorldToScreen(HeadPosition, m.matrix, 1920, 1080, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxshield();
							int armortype = Target.getArmortype();
							//xp
							int xp;
							apex_mem.Read<int>(centity + OFFSET_XP, xp);

							int xp_level = calc_level(xp);

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
								xp_level,
								maxshield,
								armortype,
							};
							Target.get_name(g_Base, i - 1, &players[i].name[0]);
							uint64_t model_ptr;
                                                        apex_mem.Read(centity + OFFSET_MODELNAME, model_ptr);
                                                        apex_mem.ReadArray(model_ptr,&players[i].model_name[0], 99);

							lastvis_esp[i] = Target.lastVisTime();
							valid = true;
						}
					}
				}

				knext = true;
				while (knext && g_Base != 0 && c_Base != 0 && esp)
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
	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (aim > 0)
			{
				if (aimentity == 0 || !aiming)
				{
					klock = false;
					lastaimentity = 0;
					continue;
				}
				klock = true;
				lastaimentity = aimentity;
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0) continue;
				Entity LPlayer = getEntity(LocalPlayer);
				QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, max_fov , rcs_pitch , rcs_yaw);
				if (Angles.x == 0 && Angles.y == 0)
				{
					klock = false;
					lastaimentity = 0;
					continue;
				}
				LPlayer.SetViewAngles(Angles);
			}
		}
	}
	aim_t = false;
}

static void set_vars(uint64_t add_addr)
{
	printf("Checking CLIENT...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//Get addresses of client vars
	uint64_t check_addr = 0;
	client_mem.Read<uint64_t>(add_addr, check_addr);
	uint64_t aim_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t), aim_addr);
	uint64_t esp_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 2, esp_addr);
	uint64_t aiming_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 3, aiming_addr);
	uint64_t g_Base_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 4, g_Base_addr);
	uint64_t next_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 5, next_addr);
	uint64_t player_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 6, player_addr);
	uint64_t spec_list_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 7, spec_list_addr);
	uint64_t valid_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 8, valid_addr);
	uint64_t max_dist_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 9, max_dist_addr);
	uint64_t item_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 10, item_glow_addr);
	uint64_t player_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 11, player_glow_addr);
	uint64_t aim_no_recoil_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 12, aim_no_recoil_addr);
	uint64_t smooth_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 13, smooth_addr);
	uint64_t max_fov_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 14, max_fov_addr);
	uint64_t bone_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 15, bone_addr);
	uint64_t thirdperson_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 16, thirdperson_addr);
	uint64_t spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 17, spectators_addr);
	uint64_t allied_spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 18, allied_spectators_addr);
	uint64_t chargerifle_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 19, chargerifle_addr);
	uint64_t shooting_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 20, shooting_addr);
	uint64_t freecam_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 21, freecam_addr);
	uint64_t lockall_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 22, lockall_addr);
	uint64_t firing_range_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 23, firing_range_addr);
    	uint64_t rcs_p_addr = 0;
    	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 24, rcs_p_addr);
    	uint64_t rcs_y_addr = 0;
    	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 25, rcs_y_addr);
	uint32_t check = 0;
	client_mem.Read<uint32_t>(check_addr, check);

	if (check != 0xABCD)
	{
		printf("check add_off,or start client before start host\n");
		active = false;
		return;
	}
	vars_t = true;
	while (vars_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (c_Base != 0 && g_Base != 0)
		{
			client_mem.Write<uint32_t>(check_addr, 0);
			printf("\nsuccess.\n");
		}

		while (c_Base != 0 && g_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			client_mem.Write<uint64_t>(g_Base_addr, g_Base);
			client_mem.Write<int>(spectators_addr, spectators);
			client_mem.Write<int>(allied_spectators_addr, allied_spectators);
			client_mem.WriteArray<spectator>(spec_list_addr, spectator_list , toRead);

			client_mem.Read<int>(aim_addr, aim);
			client_mem.Read<bool>(esp_addr, esp);
			client_mem.Read<bool>(aiming_addr, aiming);
			client_mem.Read<float>(max_dist_addr, max_dist);
			client_mem.Read<bool>(item_glow_addr, item_glow);
			client_mem.Read<bool>(player_glow_addr, player_glow);
			client_mem.Read<bool>(aim_no_recoil_addr, aim_no_recoil);
			client_mem.Read<float>(smooth_addr, smooth);
			client_mem.Read<float>(max_fov_addr, max_fov);
			client_mem.Read<int>(bone_addr, bone);
			client_mem.Read<bool>(shooting_addr, shooting);
			client_mem.Read<bool>(freecam_addr, freecam);
			client_mem.Read<bool>(lockall_addr, lockall_enable);
			client_mem.Read<bool>(firing_range_addr, firing_range);
			client_mem.Read<float>(rcs_p_addr, rcs_pitch);
			client_mem.Read<float>(rcs_y_addr, rcs_yaw);

			if (esp && knext)
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

				knext = false;
			}
		}
	}
	vars_t = false;
}


int main(int argc, char* argv[])
{
	if (geteuid() != 0)
	{
		printf("Error: %s please run in root\n", argv[0]);
		return 0;
	}

	const char* cl_proc = "client.exe";
	const char* ap_proc = "r5apex.exe";

	std::thread aimbot_thr;
	std::thread esp_thr;
	std::thread actions_thr;
	std::thread vars_thr;
	while (active)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (aim_t)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				g_Base = 0;

				aimbot_thr.~thread();
				esp_thr.~thread();
				actions_thr.~thread();
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Wating for apex process...\n");

			apex_mem.open_proc(ap_proc);

			if (apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("\nFounded Apex!\n");

				aimbot_thr = std::thread(AimbotLoop);
				esp_thr = std::thread(EspLoop);
				actions_thr = std::thread(DoActions);

				aimbot_thr.detach();
				esp_thr.detach();
				actions_thr.detach();
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
			printf("Waiting for client...\n");

			client_mem.open_proc(cl_proc);

			if (client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nFounded client!\n");

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
