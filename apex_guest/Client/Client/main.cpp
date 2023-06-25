#include "main.h"

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
	uint64_t uid = 0;
	char name[33] = { 0 };
}player;
 
uint32_t check = 0xABCD;
//VK_XBUTTON2=�ϲ�� //VK_XBUTTON1 = �²��
int aim_key = VK_XBUTTON2;
int aim_key2 = VK_XBUTTON1;

int shoot_key = VK_LBUTTON;

float scale = 1.4f;
bool firing_range;
int offset = 0;
bool use_nvidia = false;
bool active = true;
bool ready = false;
extern visuals v;
int aim = 2; //read

bool freecam = false;//read
bool lockall_mode = false; //read

int recoil_100 = 75;
int recoil = 0.5;
bool esp = true; //read
bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 650.0f * 40.0f; //read
float seer_dist = 400.0 * 40.0f;
float smooth = 85.0f;
float max_fov = 20.0f;
int bone = 3;
bool thirdperson = false;
int spectators = 0; //write
int allied_spectators = 0; //write
bool chargerifle = false;
bool shooting = false; //read


int index = 0;
bool valid = false; //write
bool next = false; //read write

uint64_t add[24];

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f8 = 0;

bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
player players[100];
void Overlay::RenderEsp()
{
	next = false;
	if (g_Base != 0 && esp)
	{
		memset(players, 0, sizeof(players));
		while (!next && esp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (next && valid)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (players[i].health > 0)
				{
					//xp and userid
					std::string rankstr = std::to_string(players[i].xp_level);
					rankstr = rankstr.substr(0, rankstr.find('.')) + u8"��( uid:" + std::to_string(players[i].uid) + u8")";

					//DISTENCE + teamnum
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + u8"��(" + std::to_string(players[i].entity_team) + u8"С��)";
;
					if (v.box)
					{
						if (players[i].visible)
						{
							if (players[i].dist < 1600.0f)
								DrawBox(RED, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //BOX
							else
								DrawBox(ORANGE, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //BOX
						}
						else
						{
							DrawBox(WHITE, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //white if player not visible
						}
					}

					if (v.line)
						DrawLine(ImVec2((float)(getWidth() / 2), (float)getHeight()), ImVec2(players[i].b_x, players[i].b_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (v.distance)
					{
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), RED, distance.c_str());  //DISTANCE
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), WHITE , distance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if(v.healthbar && players[i].dist < seer_dist)
						DrawHealth((players[i].b_x - (players[i].width / 2.0f) + 5), (players[i].b_y - players[i].height - 10), players[i].shield, players[i].maxshield, players[i].armortype, players[i].health); 
					//����
					if(v.name)
						String(ImVec2(players[i].boxMiddle, (players[i].b_y - players[i].height - 15)), WHITE, players[i].name);
					if (v.renderrank) {
						String(ImVec2((players[i].b_x - (players[i].width / 2.0f) + 5), (players[i].b_y - players[i].height - 45)), WHITE, rankstr.c_str());
					}
				}
			}

			ImGui::End();
		}
	}
}


int main(int argc, char** argv)
{
	ShowWindow(GetForegroundWindow(), 0);
	add[0] = (uintptr_t)&check;
	add[1] = (uintptr_t)&aim;
	add[2] = (uintptr_t)&esp;
	add[3] = (uintptr_t)&aiming;
	add[4] = (uintptr_t)&g_Base;
	add[5] = (uintptr_t)&next;
	add[6] = (uintptr_t)&players[0];
	add[7] = (uintptr_t)&valid;
	add[8] = (uintptr_t)&max_dist;
	add[9] = (uintptr_t)&item_glow;
	add[10] = (uintptr_t)&player_glow;
	add[11] = (uintptr_t)&aim_no_recoil;
	add[12] = (uintptr_t)&smooth;
	add[13] = (uintptr_t)&max_fov;
	add[14] = (uintptr_t)&bone;
	add[15] = (uintptr_t)&thirdperson;
	add[16] = (uintptr_t)&spectators;
	add[17] = (uintptr_t)&allied_spectators;
	add[18] = (uintptr_t)&chargerifle;
	add[19] = (uintptr_t)&shooting;
	add[20] = (uintptr_t)&freecam;
	add[21] = (uintptr_t)&lockall_mode;
	add[22] = (uintptr_t)&firing_range;
	add[23] = (uintptr_t)&recoil;

	printf(XorStr("offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));
	Overlay ov1 = Overlay();
	ov1.Start();
	printf(XorStr("Waiting for host/apex...\n"));
	while (check == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			active = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (active)
	{
		ready = true;
		printf(XorStr("Successful\n"));
	}
		
	while (active)
	{
		recoil = recoil_100 / 100;
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (IsKeyDown(aim_key))
			aiming = true;
		else
			aiming = false;
		if (aim > 0) {
			if (IsKeyDown(aim_key2))
				aim = 1;
			else
				aim = 2;
		}
	}
	ready = false;
	ov1.Clear();
	if(!use_nvidia)
		system(XorStr("taskkill /F /T /IM ov.exe")); //custom overlay processname
	return 0;
}