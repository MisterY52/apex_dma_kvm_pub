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

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

FILE* dfile;

bool firing_range = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f*40.0f;
int team_player = 0;
int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;
int max_fov = 15;
int toRead = 100;
int aim = false;
bool esp = false;
bool item_glow = false;
bool player_glow = false;
extern bool aim_no_recoil;
int safe_level = 0;
bool aiming = false;
extern float smooth;
extern int bone;

bool actions_t = false;
bool esp_t = false;
bool aim_t = false;
bool vars_t = false;
bool item_t = false;
uint64_t g_Base;
uint64_t c_Base;
bool next = false;
bool valid = false;
bool lock = false;

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
	char name[33] = { 0 };
}player;


struct Matrix
{
	float matrix[16];
};

float lastvis_esp[100];
float lastvis_aim[100];

//////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessPlayer(WinProcess& mem, Entity& LPlayer, Entity& target, uint64_t entitylist, int index)
{
	int entity_team = target.getTeamId();
	bool obs = target.Observing(mem, entitylist);
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
	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);
	if (dist > max_dist) return;

	if (!target.isAlive())
		return;

	if(!firing_range)
		if (entity_team < 0 || entity_team>50 || entity_team == team_player) return;
	
	if(aim==2)
	{
		if((target.lastVisTime() > lastvis_aim[index]))
		{
			float fov = CalculateFov(LPlayer, target);
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if(aimentity==target.ptr)
			{
				aimentity=tmp_aimentity=lastaimentity=0;
			}
		}
	}
	else
	{
		float fov = CalculateFov(LPlayer, target);
		if (fov < max)
		{
			max = fov;
			tmp_aimentity = target.ptr;
		}
	}
	lastvis_aim[index] = target.lastVisTime();
}

void DoActions(WinProcess& mem)
{
	actions_t = true;
	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base!=0 && c_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			uint64_t LocalPlayer = mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT);
			if (LocalPlayer == 0) continue;

			Entity LPlayer = getEntity(mem, LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player>50)
			{
				continue;
			}
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

			uint64_t baseent = mem.Read<uint64_t>(entitylist);
			if (baseent == 0)
			{
				continue;
			}

			max = 999.0f;
			tmp_spec = 0;
			tmp_all_spec = 0;
			tmp_aimentity = 0;
			if(firing_range)
			{
				int c=0;
				for (int i = 0; i < 9000; i++)
				{
					uint64_t centity = mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5));
					if (centity == 0) continue;
					if (LocalPlayer == centity) continue;

					Entity Target = getEntity(mem, centity);
					if (!Target.isDummy())
					{
						continue;
					}

					if(player_glow && !Target.isGlowing())
					{
						Target.enableGlow(mem);
					}
					else if(!player_glow && Target.isGlowing())
					{
						Target.disableGlow(mem);
					}

					ProcessPlayer(mem, LPlayer, Target, entitylist, c);
					c++;
				}
			}
			else
			{
				for (int i = 0; i < toRead; i++)
				{
					uint64_t centity = mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5));
					if (centity == 0) continue;
					if (LocalPlayer == centity) continue;

					Entity Target = getEntity(mem, centity);
					if (!Target.isPlayer())
					{
						continue;
					}
					
					ProcessPlayer(mem, LPlayer, Target, entitylist, i);

					int entity_team = Target.getTeamId();
					if (entity_team == team_player)
					{
						continue;
					}

					switch (safe_level)
					{
					case 1:
						if (spectators > 0)
						{
							if(Target.isGlowing())
							{
								Target.disableGlow(mem);
							}
							continue;
						}
						break;
					case 2:
						if (spectators+allied_spectators > 0)
						{
							if(Target.isGlowing())
							{
								Target.disableGlow(mem);
							}
							continue;
						}
						break;
					default:
						break;
					}

					if(player_glow && !Target.isGlowing())
					{
						Target.enableGlow(mem);
					}
					else if(!player_glow && Target.isGlowing())
					{
						Target.disableGlow(mem);
					}
				}
			}
			spectators = tmp_spec;
			allied_spectators = tmp_all_spec;
			if(!lock)
				aimentity = tmp_aimentity;
			else
				aimentity = lastaimentity;
		}
	}
	actions_t = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

player players[100];

static void EspLoop(WinProcess& mem)
{
	esp_t = true;
	while(esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while(g_Base!=0 && c_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;
				switch (safe_level)
				{
				case 1:
					if (spectators > 0)
					{
						next = true;
						while(next && g_Base!=0 && c_Base!=0 && esp)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						continue;
					}
					break;
				case 2:
					if (spectators+allied_spectators > 0)
					{
						next = true;
						while(next && g_Base!=0 && c_Base!=0 && esp)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						continue;
					}
					break;
				default:
					break;
				}

				uint64_t LocalPlayer = mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT);
				if (LocalPlayer == 0)
				{
					next = true;
					while(next && g_Base!=0 && c_Base!=0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Entity LPlayer = getEntity(mem, LocalPlayer);
				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player>50)
				{
					next = true;
					while(next && g_Base!=0 && c_Base!=0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Vector LocalPlayerPosition = LPlayer.getPosition();

				uint64_t viewRenderer = mem.Read<uint64_t>(g_Base + OFFSET_RENDER);
				uint64_t viewMatrix = mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX);
				Matrix m = mem.Read<Matrix>(viewMatrix);

				uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
				
				memset(players,0,sizeof(players));
				if(firing_range)
				{
					int c=0;
					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = mem.Read<uint64_t>( entitylist + ((uint64_t)i << 5));
						if (centity == 0)
						{
							continue;
						}		
						
						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(mem, centity);

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
							Vector HeadPosition = Target.getBonePosition(mem, 8);
							WorldToScreen(HeadPosition, m.matrix, 1920, 1080, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
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
								shield	
							};
							Target.get_name(mem,g_Base, i-1, &players[c].name[0]);
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
						uint64_t centity = mem.Read<uint64_t>( entitylist + ((uint64_t)i << 5));
						if (centity == 0)
						{
							continue;
						}
						
						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(mem, centity);
						
						if (!Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}

						int entity_team = Target.getTeamId();
						if (entity_team < 0 || entity_team>50 || entity_team == team_player)
						{
							continue;
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
							Vector HeadPosition = Target.getBonePosition(mem, 8);
							WorldToScreen(HeadPosition, m.matrix, 1920, 1080, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							
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
								shield
							};
							Target.get_name(mem, g_Base, i-1, &players[i].name[0]);
							lastvis_esp[i] = Target.lastVisTime();
							valid = true;
						}
					}
				}

				next = true;
				while(next && g_Base!=0 && c_Base!=0 && esp)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	esp_t = false;
}

static void AimbotLoop(WinProcess& mem)
{
	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base!=0 && c_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (aim>0)
			{
				switch (safe_level)
				{
				case 1:
					if (spectators > 0)
					{
						continue;
					}
					break;
				case 2:
					if (spectators+allied_spectators > 0)
					{
						continue;
					}
					break;
				default:
					break;
				}
				
				if (aimentity == 0 || !aiming)
				{
					lock=false;
					lastaimentity=0;
					continue;
				}
				lock=true;
				lastaimentity = aimentity;
				uint64_t LocalPlayer = mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT);
				if (LocalPlayer == 0) continue;
				Entity LPlayer = getEntity(mem, LocalPlayer);
				QAngle Angles = CalculateBestBoneAim(mem, LPlayer, aimentity, max_fov);
				if (Angles.x == 0 && Angles.y == 0)
				{
					lock=false;
					lastaimentity=0;
					continue;
				}
				LPlayer.SetViewAngles(mem, Angles);
			}
		}
	}
	aim_t = false;
}

static void set_vars(WinProcess& mem, uint64_t add_addr)
{
	printf("Reading client vars...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//Get addresses of client vars
	uint64_t spec_addr = mem.Read<uint64_t>(add_addr);
	uint64_t all_spec_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t));
	uint64_t aim_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*2);
	uint64_t esp_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*3);
	uint64_t safe_lev_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*4);
	uint64_t aiming_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*5);
	uint64_t g_Base_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*6);
	uint64_t next_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*7);
	uint64_t player_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*8);
	uint64_t valid_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*9);
	uint64_t max_dist_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*10);
	uint64_t item_glow_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*11);
	uint64_t player_glow_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*12);
	uint64_t aim_no_recoil_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*13);
	uint64_t smooth_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*14);
	uint64_t max_fov_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*15);
	uint64_t bone_addr = mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*16);

	if(mem.Read<int>(spec_addr)!=1)
	{
		printf("Incorrect values read. Restart the client or check if the offset is correct. Quitting.\n");
		active = false;
		return;
	}
	vars_t = true;
	while(vars_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if(c_Base!=0 && g_Base!=0)
			printf("\nReady\n");
		while(c_Base!=0 && g_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));	
			mem.Write<int>(spec_addr, spectators);
			mem.Write<int>(all_spec_addr, allied_spectators);
			mem.Write<uint64_t>(g_Base_addr, g_Base);

			aim = mem.Read<int>(aim_addr);
			esp = mem.Read<bool>(esp_addr);
			safe_level = mem.Read<int>(safe_lev_addr);
			aiming = mem.Read<bool>(aiming_addr);
			max_dist = mem.Read<float>(max_dist_addr);
			item_glow = mem.Read<bool>(item_glow_addr);
			player_glow = mem.Read<bool>(player_glow_addr);
			aim_no_recoil = mem.Read<bool>(aim_no_recoil_addr);
			smooth = mem.Read<float>(smooth_addr);
			max_fov = mem.Read<float>(max_fov_addr);
			bone = mem.Read<int>(bone_addr);

			if(esp && next)
			{
				if(valid)
					mem.WriteMem<player>(player_addr, players, sizeof(players));
				mem.Write<bool>(valid_addr, valid);
				mem.Write<bool>(next_addr, true); //next
 
				while (mem.Read<bool>(next_addr) && g_Base!=0 && c_Base!=0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				next = false;
			}
		}
	}
	vars_t = false;
}

static void item_glow_t(WinProcess& mem)
{
	item_t = true;
	while(item_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		int k = 0;
		while(g_Base!=0 && c_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
			if (item_glow)
			{
				for (int i = 0; i < 10000; i++)
				{
					uint64_t centity = mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5));
					if (centity == 0) continue;

					Item item = getItem(mem, centity);

					if(item.isItem() && !item.isGlowing())
					{
						item.enableGlow(mem);
					}
				}
				k=1;
				std::this_thread::sleep_for(std::chrono::milliseconds(600));
			}
			else
			{		
				if(k==1)
				{
					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5));
						if (centity == 0) continue;

						Item item = getItem(mem, centity);

						if(item.isItem() && item.isGlowing())
						{
							item.disableGlow(mem);
						}
					}
					k=0;
				}
			}	
		}
	}
	item_t = false;
}

__attribute__((constructor))
static void init()
{
	FILE* out = stdout;
	const char* cl_proc = "client_ap.exe";
	const char* ap_proc = "r5apex.exe";

	pid_t pid;
	#if (LMODE() == MODE_EXTERNAL())
	FILE* pipe = popen("pidof qemu-system-x86_64", "r");
	fscanf(pipe, "%d", &pid);
	pclose(pipe);
	#else
	out = fopen("/tmp/testr.txt", "w");
	pid = getpid();
	#endif
	fprintf(out, "Using Mode: %s\n", TOSTRING(LMODE));

	dfile = out;

	try
	{
		printf("\nStarting client context...\n");
		WinContext ctx_client(pid);
		printf("\nStarting apex context...\n");
		WinContext ctx_apex(pid);
		printf("\nStarting refresh process list context...\n");
		WinContext ctx_refresh(pid);
		printf("\n");
		bool apex_found = false;
		bool client_found = false;
		//Client "add" offset
		uint64_t add_off = 0x3e870;
		
		while(active)
		{
			if(!apex_found)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				item_t = false;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				printf("Searching apex process...\n");
				ctx_apex.processList.Refresh();
				for (auto& i : ctx_apex.processList)
				{
					if (!strcasecmp(ap_proc, i.proc.name))
					{					
						PEB peb = i.GetPeb();
						short magic = i.Read<short>(peb.ImageBaseAddress);
						g_Base = peb.ImageBaseAddress;
						if(g_Base!=0)
						{
							apex_found = true;
							fprintf(out, "\nApex found %lx:\t%s\n", i.proc.pid, i.proc.name);
							fprintf(out, "\tBase:\t%lx\tMagic:\t%hx (valid: %hhx)\n", peb.ImageBaseAddress, magic, (char)(magic == IMAGE_DOS_SIGNATURE));
							std::thread aimbot(AimbotLoop, std::ref(i));
							std::thread esp_th(EspLoop, std::ref(i));
							std::thread actions(DoActions, std::ref(i));
							std::thread itemglow(item_glow_t, std::ref(i));
							aimbot.detach();
							esp_th.detach();
							actions.detach();
							itemglow.detach();
						}
					}
				}
			}

			if(!client_found)
			{
				vars_t = false;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				printf("Searching client process...\n");
				ctx_client.processList.Refresh();	
				for (auto& i : ctx_client.processList)
				{
					if (!strcasecmp(cl_proc, i.proc.name))
					{	
						PEB peb = i.GetPeb();
						short magic = i.Read<short>(peb.ImageBaseAddress);
						c_Base = peb.ImageBaseAddress;
						if(c_Base!=0)
						{
							client_found = true;
							fprintf(out, "\nClient found %lx:\t%s\n", i.proc.pid, i.proc.name);
							fprintf(out, "\tBase:\t%lx\tMagic:\t%hx (valid: %hhx)\n", peb.ImageBaseAddress, magic, (char)(magic == IMAGE_DOS_SIGNATURE));
							std::thread vars(set_vars, std::ref(i), c_Base + add_off);
							vars.detach();
						}
					}
				}
			}


			if(apex_found || client_found)
			{
				apex_found = false;
				client_found = false;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				ctx_refresh.processList.Refresh();
				for (auto& i : ctx_refresh.processList)
				{
					if (!strcasecmp(cl_proc, i.proc.name))
					{
						PEB peb = i.GetPeb();
						if(peb.ImageBaseAddress != 0)
						{
							if(vars_t)
								client_found = true;
						}
					}

					if (!strcasecmp(ap_proc, i.proc.name))
					{
						PEB peb = i.GetPeb();
						if(peb.ImageBaseAddress != 0)
						{
							if(actions_t)
								apex_found = true;
						}
					}
				}

				if(!apex_found && !client_found)
				{
					g_Base = 0;
					c_Base = 0;
					active = false;
				}
				else
				{
					if(!apex_found)
					{
						g_Base = 0;
					}

					if(!client_found)
					{
						c_Base = 0;
					}
				}
			}
		}
	} catch (VMException& e)
	{
		fprintf(out, "Initialization error: %d\n", e.value);
	}
	fclose(out);
}

int main()
{
	return 0;
}
