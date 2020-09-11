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
}player;

bool use_nvidia = true;
bool active = true;
int spectators = 1; //write
int allied_spectators = 1; //write
int aim = 0; //read
bool esp = false; //read
int safe_level = 0; //read
bool item_glow = false;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 200.0f*40.0f; //read

bool valid = false; //write
bool next = false; //read write

uint64_t add[12];

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f7 = 0;
bool k_f8 = 0;

bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

player players[100];

void render(void* ovv)
{
	next = false;
	if (g_Base != 0 && esp)
	{
		memset(players, 0, sizeof(players));
		Overlay* ov = (Overlay*)ovv;
		Direct dx = ov->CurrentDirectX;
		while (!next && esp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (next && valid)
		{
			for (int i = 0; i < 100; i++)
			{
				if (players[i].width > 0)
				{
					std::wstring distance = std::to_wstring(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + L"m(" + std::to_wstring(players[i].entity_team) + L")";

					if (players[i].visible)
					{
						if (players[i].dist < 1600.0f)
							dx.DrawBox2((int)players[i].boxMiddle, (int)players[i].h_y, (int)players[i].width, (int)players[i].height, 255, 0, 0, 255); //BOX
						else
							dx.DrawBox2((int)players[i].boxMiddle, (int)players[i].h_y, (int)players[i].width, (int)players[i].height, 255, 165, 0, 255); //BOX
					}
					else
					{
						dx.DrawBox2((int)players[i].boxMiddle, (int)players[i].h_y, (int)players[i].width, (int)players[i].height, 255, 255, 255, 255); //white if player not visible
					}

					dx.DrawLine((float)(ov->getWidth() / 2), (float)ov->getHeight(), players[i].b_x, players[i].b_y, 255, 0, 0, 255); //LINE FROM MIDDLE SCREEN
					if (players[i].knocked)
						dx.DrawString((int)players[i].boxMiddle, (int)(players[i].b_y + 1), 255, 255, 0, 0, distance.c_str());  //DISTANCE
					else
						dx.DrawString((int)players[i].boxMiddle, (int)(players[i].b_y + 1), 255, 0, 255, 0, distance.c_str());  //DISTANCE

					int r = 0, g = 0, b = 0;
					if (players[i].health > 75 && players[i].health <= 100)
					{
						r = 0;
						g = 200;
						b = 0;
					}
					else if (players[i].health > 50 && players[i].health <= 75)
					{
						r = 255;
						g = 215;
						b = 0;
					}
					else
					{
						r = 255;
						g = 0;
						b = 0;
					}
					dx.Fill((int)(players[i].b_x - (players[i].width / 2.0f) - 4), (int)(players[i].b_y - players[i].height), 3, (int)((players[i].height / 100.0f) * (float)players[i].health), r, g, b, 255);

					if (players[i].shield > 75 && players[i].shield <= 125)
					{
						r = 0;
						g = 200;
						b = 0;
					}
					else if (players[i].shield > 50 && players[i].shield <= 75)
					{
						r = 255;
						g = 215;
						b = 0;
					}
					else
					{
						r = 255;
						g = 0;
						b = 0;
					}
					dx.Fill((int)(players[i].b_x + (players[i].width / 2.0f) + 1), (int)(players[i].b_y - players[i].height), 3, (int)((players[i].height / 100.0f) * (float)players[i].shield), r, g, b, 255);
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	add[0] = (uintptr_t)&spectators;
	add[1] = (uintptr_t)&allied_spectators;
	add[2] = (uintptr_t)&aim;
	add[3] = (uintptr_t)&esp;
	add[4] = (uintptr_t)&safe_level;
	add[5] = (uintptr_t)&aiming;
	add[6] = (uintptr_t)&g_Base;
	add[7] = (uintptr_t)&next;
	add[8] = (uintptr_t)&players[0];
	add[9] = (uintptr_t)&valid;
	add[10] = (uintptr_t)&max_dist;
	add[11] = (uintptr_t)&item_glow;
	printf("add offset: 0x%I64x\n", (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));
	Overlay ov1 = Overlay();
	ov1.SetRender(render);
	ov1.Start();
	printf("Waiting for host process...\n");
	while (spectators == 1)
	{
		if (IsKeyDown(VK_F4))
		{
			active = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if(active)
		printf("Ready\n");
	while (active)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}

		if (IsKeyDown(VK_F5) && k_f5 == 0)
		{
			k_f5 = 1;
			esp = !esp;
		}
		else if (!IsKeyDown(VK_F5) && k_f5 == 1)
		{
			k_f5 = 0;
		}

		if (IsKeyDown(VK_F6) && k_f6 == 0)
		{
			k_f6 = 1;
			switch (aim)
			{
			case 0:
				aim = 1;
				break;
			case 1:
				aim = 2;
				break;
			case 2:
				aim = 0;
				break;
			default:
				break;
			}
		}
		else if (!IsKeyDown(VK_F6) && k_f6 == 1)
		{
			k_f6 = 0;
		}

		if (IsKeyDown(VK_F7) && k_f7 == 0)
		{
			k_f7 = 1;
			switch (safe_level)
			{
			case 0:
				safe_level = 1;
				break;
			case 1:
				safe_level = 2;
				break;
			case 2:
				safe_level = 0;
				break;
			default:
				break;
			}
		}
		else if (!IsKeyDown(VK_F7) && k_f7 == 1)
		{
			k_f7 = 0;
		}

		if (IsKeyDown(VK_F8) && k_f8 == 0)
		{
			k_f8 = 1;
			item_glow = !item_glow;
		}
		else if (!IsKeyDown(VK_F8) && k_f8 == 1)
		{
			k_f8 = 0;
		}

		if (IsKeyDown(VK_LEFT))
		{
			if (max_dist > 100.0f * 40.0f)
				max_dist -= 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_RIGHT))
		{
			if (max_dist < 800.0f * 40.0f)
				max_dist += 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_RBUTTON))
			aiming = true;
		else
			aiming = false;
	}
	ov1.Clear();
	if(!use_nvidia)
		system("taskkill /F /T /IM overlay.exe"); //custom overlay process name
	return 0;
}

