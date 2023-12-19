#include "main.h"

typedef struct changeableplaceplayer
{
	float changeableplacedist = 0;
	int changeableplaceentity_team = 0;
	float changeableplaceboxMiddle = 0;
	float changeableplaceh_y = 0;
	float changeableplacewidth = 0;
	float changeableplaceheight = 0;
	float changeableplaceb_x = 0;
	float changeableplaceb_y = 0;
	bool changeableplaceknocked = false;
	bool changeableplacevisible = false;
	int changeableplacehealth = 0;
	int changeableplaceshield = 0;
	int changeableplacexp_level = 0;
	int changeableplacemaxshield = 0;
	int changeableplacearmortype = 0;
	char changeableplacename[33] = { 0 };
	char changeableplacemodel_name[33] = { 0 };
}changeableplaceplayer;

uint32_t changeableplacecheck = 0xABCD;
int changeableplaceaim_key = VK_XBUTTON2;
int changeableplaceaim_key2 = VK_XBUTTON1;

int changeableplaceshoot_key = VK_LBUTTON;

float changeableplacescale = 1.4f;
bool changeableplacefiring_range;
int changeableplaceoffset = 0;
bool changeableplaceuse_nvidia = false;
bool changeableplaceactive = true;
bool changeableplaceready = false;
extern changeableplacevisuals changeableplacev;
int changeableplaceaim = 2; //read

float changeableplacexp_dist = 200.0f * 40.0f;
float changeableplacename_dist = 100.0f * 40.0f;
bool changeableplacefreecam = false;//read
bool changeableplacelockall_mode = false; //read

bool changeableplaceesp = true; //read
bool changeableplaceitem_glow = false;
bool changeableplaceplayer_glow = false;
bool changeableplaceaim_no_recoil = true;
bool changeableplaceaiming = false; //read
uint64_t changeableplaceg_Base = 0; //write
float changeableplacemax_dist = 650.0f * 40.0f; //read
float changeableplaceseer_dist = 400.0 * 40.0f;
float changeableplacesmooth = 82.0f;
float changeableplacemax_fov = 12.0f;
int changeableplacebone = 3;
bool changeableplacethirdperson = false;
int changeableplacespectators = 0; //write
int changeableplaceallied_spectators = 0; //write
bool changeableplacechargerifle = false;
bool changeableplaceshooting = false; //read


int changeableplaceindex = 0;
bool changeableplacevalid = false; //write
bool changeableplacenext = false; //read write

uint64_t changeableplaceadd[23];

bool changeableplacek_f5 = 0;
bool changeableplacek_f6 = 0;
bool changeableplacek_f8 = 0;

bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
changeableplaceplayer changeableplaceplayers[100];

std::string changeableplaceprocess_model_name(char changeableplacemodel_name[]) {
	if (strstr(changeableplacemodel_name, "dummie")) {
		return u8"人机";
	}
	else if (strstr(changeableplacemodel_name, "bangalore")) {
		return u8"班加罗尔";
	}
	else if (strstr(changeableplacemodel_name, "fuse")) {
		return u8"暴雷";
	}
	else if (strstr(changeableplacemodel_name, "ash")) {
		return u8"艾许";
	}
	else if (strstr(changeableplacemodel_name, "madmaggie")) {
		return u8"疯马吉";
	}
	else if (strstr(changeableplacemodel_name, "ballistic")) {
		return u8"弹道";
	}
	else if (strstr(changeableplacemodel_name, "pathfinder")) {
		return u8"探路者";
	}
	else if (strstr(changeableplacemodel_name, "stim")) {
		return u8"动力小子";
	}
	else if (strstr(changeableplacemodel_name, "revenant")) {
		return u8"亡灵";
	}
	else if (strstr(changeableplacemodel_name, "nova")) {
		return u8"地平线";
	}
	else if (strstr(changeableplacemodel_name, "valkyrie")) {
		return u8"瓦尔基里";
	}
	else if (strstr(changeableplacemodel_name, "bloodhound")) {
		return u8"寻血猎犬";
	}
	else if (strstr(changeableplacemodel_name, "crypto")) {
		return u8"密客";
	}
	else if (strstr(changeableplacemodel_name, "seer")) {
		return u8"希尔";
	}
	else if (strstr(changeableplacemodel_name, "wraith")) {
		return u8"恶灵";
	}
	else if (strstr(changeableplacemodel_name, "vantage")) {
		return u8"万蒂奇";
	}
	else if (strstr(changeableplacemodel_name, "gibraltar")) {
		return u8"直布罗陀";
	}
	else if (strstr(changeableplacemodel_name, "lifeline")) {
		return u8"命脉";
	}
	else if (strstr(changeableplacemodel_name, "loba")) {
		return u8"罗芭";
	}
	else if (strstr(changeableplacemodel_name, "holo")) {
		return u8"幻象";
	}
	else if (strstr(changeableplacemodel_name, "newcastle")) {
		return u8"纽卡斯尔";
	}
	else if (strstr(changeableplacemodel_name, "conduit")) {
		return u8"导线管";
	}
	else if (strstr(changeableplacemodel_name, "caustic")) {
		return u8"侵蚀";
	}
	else if (strstr(changeableplacemodel_name, "wattson")) {
		return u8"沃特森";
	}
	else if (strstr(changeableplacemodel_name, "rampart")) {
		return u8"兰伯特";
	}
	else if (strstr(changeableplacemodel_name, "catalyst")) {
		return u8"催化剂";
	}
	else return u8"读取出错";
}

void Overlay::RenderchangeableplaceEsp()
{
	changeableplacenext = false;
	if (changeableplaceg_Base != 0 && changeableplaceesp)
	{
		memset(changeableplaceplayers, 0, sizeof(changeableplaceplayers));
		while (!changeableplacenext && changeableplaceesp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (changeableplacenext && changeableplacevalid)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getchangeableplaceWidth(), (float)getchangeableplaceHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (changeableplaceplayers[i].changeableplacehealth > 0)
				{
					//xp+legend
					std::string changeableplacexpstr = std::to_string(changeableplaceplayers[i].changeableplacexp_level);
					changeableplacexpstr = u8"Level:" + changeableplacexpstr.substr(0, changeableplacexpstr.find('.')) + "   Legend:" + changeableplaceprocess_model_name(changeableplaceplayers[i].changeableplacemodel_name);

					//DISTENCE + teamnum
					std::string changeableplacedistance = std::to_string(changeableplaceplayers[i].changeableplacedist / 39.62);
					changeableplacedistance = changeableplacedistance.substr(0, changeableplacedistance.find('.')) + u8"M( No." + std::to_string(changeableplaceplayers[i].changeableplaceentity_team) + u8")";

					if (changeableplacev.changeableplacebox)
					{
						if (changeableplaceplayers[i].changeableplacevisible)
						{
							if (changeableplaceplayers[i].changeableplacedist < 1600.0f)
								DrawchangeableplaceBox(RED, changeableplaceplayers[i].changeableplaceboxMiddle, changeableplaceplayers[i].changeableplaceh_y, changeableplaceplayers[i].changeableplacewidth, changeableplaceplayers[i].changeableplaceheight); //BOX
							else
								DrawchangeableplaceBox(ORANGE, changeableplaceplayers[i].changeableplaceboxMiddle, changeableplaceplayers[i].changeableplaceh_y, changeableplaceplayers[i].changeableplacewidth, changeableplaceplayers[i].changeableplaceheight); //BOX
						}
						else
						{
							DrawchangeableplaceBox(WHITE, changeableplaceplayers[i].changeableplaceboxMiddle, changeableplaceplayers[i].changeableplaceh_y, changeableplaceplayers[i].changeableplacewidth, changeableplaceplayers[i].changeableplaceheight); //white if player not visible
						}
					}

					if (changeableplacev.changeableplaceline)
						DrawchangeableplaceLine(ImVec2((float)(getchangeableplaceWidth() / 2), (float)getchangeableplaceHeight()), ImVec2(changeableplaceplayers[i].changeableplaceb_x, changeableplaceplayers[i].changeableplaceb_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (changeableplacev.changeableplacedistance)
					{
						if (changeableplaceplayers[i].changeableplaceknocked)
							Stringchangeableplace(ImVec2(changeableplaceplayers[i].changeableplaceboxMiddle, (changeableplaceplayers[i].changeableplaceb_y + 1)), RED, changeableplacedistance.c_str());  //DISTANCE
						else
							Stringchangeableplace(ImVec2(changeableplaceplayers[i].changeableplaceboxMiddle, (changeableplaceplayers[i].changeableplaceb_y + 1)), WHITE, changeableplacedistance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if (changeableplacev.changeableplacehealthbar && changeableplaceplayers[i].changeableplacedist <= changeableplaceseer_dist)
						DrawchangeableplaceHealth((changeableplaceplayers[i].changeableplaceb_x - (changeableplaceplayers[i].changeableplacewidth / 2.0f) + 5), (changeableplaceplayers[i].changeableplaceb_y - changeableplaceplayers[i].changeableplaceheight - 10), changeableplaceplayers[i].changeableplaceshield, changeableplaceplayers[i].changeableplacemaxshield, changeableplaceplayers[i].changeableplacearmortype, changeableplaceplayers[i].changeableplacehealth);
					//name
					if (changeableplacev.changeableplacename && changeableplaceplayers[i].changeableplacedist <= changeableplacename_dist)
						Stringchangeableplace(ImVec2(changeableplaceplayers[i].changeableplaceboxMiddle, (changeableplaceplayers[i].changeableplaceb_y - changeableplaceplayers[i].changeableplaceheight - 15)), WHITE, changeableplaceplayers[i].changeableplacename);
					//xp and legend
					if (changeableplacev.changeableplacerenderxp && changeableplaceplayers[i].changeableplacedist <= changeableplacexp_dist) {
						Stringchangeableplace(ImVec2(changeableplaceplayers[i].changeableplaceboxMiddle - changeableplaceplayers[i].changeableplacewidth , (changeableplaceplayers[i].changeableplaceb_y - changeableplaceplayers[i].changeableplaceheight - 15)), GREEN , changeableplacexpstr.c_str());
					}
				}
			}
			ImGui::End();
		}
	}
}


int main(int argc, char** argv)
{
	//ShowWindow(GetForegroundWindow(), 0);  // Hide the window
	changeableplaceadd[0] = (uintptr_t)&changeableplacecheck;
	changeableplaceadd[1] = (uintptr_t)&changeableplaceaim;
	changeableplaceadd[2] = (uintptr_t)&changeableplaceesp;
	changeableplaceadd[3] = (uintptr_t)&changeableplaceaiming;
	changeableplaceadd[4] = (uintptr_t)&changeableplaceg_Base;
	changeableplaceadd[5] = (uintptr_t)&changeableplacenext;
	changeableplaceadd[6] = (uintptr_t)&changeableplaceplayers[0];
	changeableplaceadd[7] = (uintptr_t)&changeableplacevalid;
	changeableplaceadd[8] = (uintptr_t)&changeableplacemax_dist;
	changeableplaceadd[9] = (uintptr_t)&changeableplaceitem_glow;
	changeableplaceadd[10] = (uintptr_t)&changeableplaceplayer_glow;
	changeableplaceadd[11] = (uintptr_t)&changeableplaceaim_no_recoil;
	changeableplaceadd[12] = (uintptr_t)&changeableplacesmooth;
	changeableplaceadd[13] = (uintptr_t)&changeableplacemax_fov;
	changeableplaceadd[14] = (uintptr_t)&changeableplacebone;
	changeableplaceadd[15] = (uintptr_t)&changeableplacethirdperson;
	changeableplaceadd[16] = (uintptr_t)&changeableplacespectators;
	changeableplaceadd[17] = (uintptr_t)&changeableplaceallied_spectators;
	changeableplaceadd[18] = (uintptr_t)&changeableplacechargerifle;
	changeableplaceadd[19] = (uintptr_t)&changeableplaceshooting;
	changeableplaceadd[20] = (uintptr_t)&changeableplacefreecam;
	changeableplaceadd[21] = (uintptr_t)&changeableplacelockall_mode;
	changeableplaceadd[22] = (uintptr_t)&changeableplacefiring_range;

	printf(XorStr("offset: 0x%I64x\n"), (uint64_t)&changeableplaceadd[0] - (uint64_t)GetModuleHandle(NULL));
	Overlay ov1changeableplace = Overlay();
	ov1changeableplace.Startchangeableplace();
	printf(XorStr("Waiting for host/apex...\n"));
	while (changeableplacecheck == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			changeableplaceactive = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (changeableplaceactive)
	{
		changeableplaceready = true;
		printf(XorStr("Successful\n"));
	}

	while (changeableplaceactive)
	{
		if (IsKeyDown(VK_F4))
		{
			changeableplaceactive = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (IsKeyDown(changeableplaceaim_key))
			changeableplaceaiming = true;
		else
			changeableplaceaiming = false;
		if (changeableplaceaim > 0) {
			if (IsKeyDown(changeableplaceaim_key2)) {
				changeableplaceaim = 1;
			}
			else {
				changeableplaceaim = 2;
			}
		}
	}
	changeableplaceready = false;
	ov1changeableplace.Clearchangeableplace();
	if (!changeableplaceuse_nvidia)
		system(XorStr("taskkill /F /T /IM ov.exe")); //custom overlay processname
	return 0;
}