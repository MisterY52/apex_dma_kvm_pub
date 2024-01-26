#include "main.h"

typedef struct placeholderplayer
{
	float placeholderdist = 0;
	int placeholderentity_team = 0;
	float placeholderboxMiddle = 0;
	float placeholderh_y = 0;
	float placeholderwidth = 0;
	float placeholderheight = 0;
	float placeholderb_x = 0;
	float placeholderb_y = 0;
	bool placeholderknocked = false;
	bool placeholdervisible = false;
	int placeholderhealth = 0;
	int placeholdershield = 0;
	int placeholderxp_level = 0;
	int placeholdermaxshield = 0;
	int placeholderarmortype = 0;
	char placeholdername[33] = { 0 };
	char placeholdermodel_name[100] = { 0 };
}placeholderplayer;

typedef struct placeholderspectator {
	bool is_placeholderspec = false;
	int xp_placeholderlevel = 0;
	char nplaceholderame[33] = { 0 };
}placeholderspectator;

uint32_t placeholdercheck = 0xABCD;
int placeholderaim_key = VK_XBUTTON2;
int placeholderaim_key2 = VK_XBUTTON1;

int placeholdershoot_key = VK_LBUTTON;

float placeholderrcs_pitch = 0.3f;
float placeholderrcs_yaw = 0.4f;

float placeholderscale = 1.4f;
bool placeholderfiring_range;
int placeholderoffset = 0;
bool placeholderuse_nvidia = false;
bool placeholderactive = true;
bool placeholderready = false;
extern placeholdervisuals placeholderv;
int placeholderaim = 2; //read

float placeholderxp_dist = 600.0f * 40.0f;
float placeholdername_dist = 100.0f * 40.0f;
bool placeholderfreecam = false;//read
bool placeholderlockall_mode = false; //read

bool placeholderesp = true; //read
bool placeholderitem_glow = false;
bool placeholderplayer_glow = false;
bool placeholderaim_no_recoil = true;
bool placeholderaiming = false; //read
uint64_t placeholderg_Base = 0; //write
float placeholdermax_dist = 650.0f * 40.0f; //read
float placeholderseer_dist = 400.0 * 40.0f;
float placeholdersmooth = 82.0f;
float placeholdermax_fov = 12.0f;
int placeholderbone = 3;
bool placeholderthirdperson = false;
int placeholderspectators = 0; //write
int placeholderallied_spectators = 0; //write
bool placeholderchargerifle = false;
bool placeholdershooting = false; //read

int placeholderindex = 0;
bool placeholdervalid = false; //write
bool placeholdernext = false; //read write

uint64_t placeholderadd[26];


bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

placeholderplayer placeholderplayers[100];

std::string placeholderprocess_model_name(char placeholdermodel_name[]) {
	if (strstr(placeholdermodel_name, "dummie")) {
		return u8"人机";
	}
	else if (strstr(placeholdermodel_name, "bangalore")) {
		return u8"班加罗尔";
	}
	else if (strstr(placeholdermodel_name, "fuse")) {
		return u8"暴雷";
	}
	else if (strstr(placeholdermodel_name, "ash")) {
		return u8"艾许";
	}
	else if (strstr(placeholdermodel_name, "madmaggie")) {
		return u8"疯马吉";
	}
	else if (strstr(placeholdermodel_name, "ballistic")) {
		return u8"弹道";
	}
	else if (strstr(placeholdermodel_name, "pathfinder")) {
		return u8"探路者";
	}
	else if (strstr(placeholdermodel_name, "octane")) {
		return u8"动力小子";
	}
	else if (strstr(placeholdermodel_name, "stim")) {
		return u8"动力小子";
	}
	else if (strstr(placeholdermodel_name, "revenant")) {
		return u8"亡灵";
	}
	else if (strstr(placeholdermodel_name, "nova")) {
		return u8"地平线";
	}
	else if (strstr(placeholdermodel_name, "horizon")) {
		return u8"地平线";
	}
	else if (strstr(placeholdermodel_name, "valkyrie")) {
		return u8"瓦尔基里";
	}
	else if (strstr(placeholdermodel_name, "bloodhound")) {
		return u8"寻血猎犬";
	}
	else if (strstr(placeholdermodel_name, "crypto")) {
		return u8"密客";
	}
	else if (strstr(placeholdermodel_name, "seer")) {
		return u8"希尔";
	}
	else if (strstr(placeholdermodel_name, "wraith")) {
		return u8"恶灵";
	}
	else if (strstr(placeholdermodel_name, "vantage")) {
		return u8"万蒂奇";
	}
	else if (strstr(placeholdermodel_name, "gibraltar")) {
		return u8"直布罗陀";
	}
	else if (strstr(placeholdermodel_name, "lifeline")) {
		return u8"命脉";
	}
	else if (strstr(placeholdermodel_name, "loba")) {
		return u8"罗芭";
	}
	else if (strstr(placeholdermodel_name, "holo")) {
		return u8"幻象";
	}
	else if (strstr(placeholdermodel_name, "newcastle")) {
		return u8"纽卡斯尔";
	}
	else if (strstr(placeholdermodel_name, "conduit")) {
		return u8"导线管";
	}
	else if (strstr(placeholdermodel_name, "caustic")) {
		return u8"侵蚀";
	}
	else if (strstr(placeholdermodel_name, "wattson")) {
		return u8"沃特森";
	}
	else if (strstr(placeholdermodel_name, "rampart")) {
		return u8"兰伯特";
	}
	else if (strstr(placeholdermodel_name, "catalyst")) {
		return u8"催化剂";
	}
	else return placeholdermodel_name;
}

placeholderspectator placeholderspectator_list[100];

void Overlay::RenderplaceholderSpectator() {
	int text_placeholderindex = 0;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2((float)getplaceholderWidth(), (float)getplaceholderHeight()));
	ImGui::Begin(XorStr("##spectator_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

	for (int i = 0; i < 61; i++) {
		if (placeholderspectator_list[i].is_placeholderspec) {
			Stringplaceholder(ImVec2(5, 60 + text_placeholderindex * 20), WHITE, std::to_string(text_placeholderindex + 1).c_str(), 16);
			Stringplaceholder(ImVec2(25, 60 + text_placeholderindex * 20), WHITE, placeholderspectator_list[i].nplaceholderame , 16);
			Stringplaceholder(ImVec2(sizeof(placeholderspectator_list[i].nplaceholderame)*5 + 25, 60 + text_placeholderindex * 20), WHITE, std::to_string(placeholderspectator_list[i].xp_placeholderlevel).c_str(), 16);
			text_placeholderindex++;
		}
	}
	ImGui::End();
	memset(placeholderspectator_list, 0, sizeof(placeholderspectator_list));
}


void Overlay::RenderplaceholderEsp()
{
	placeholdernext = false;
	if (placeholderg_Base != 0 && placeholderesp)
	{
		memset(placeholderplayers, 0, sizeof(placeholderplayers));

		while (!placeholdernext && placeholderesp)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getplaceholderWidth(), (float)getplaceholderHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (placeholderplayers[i].placeholderhealth > 0)
				{
					//xp+legend
					std::string placeholderxpstr = std::to_string(placeholderplayers[i].placeholderxp_level);
					placeholderxpstr = u8"Level:" + placeholderxpstr.substr(0, placeholderxpstr.find('.')) + "   Legend:" + placeholderprocess_model_name(placeholderplayers[i].placeholdermodel_name); // placeholderprocess_model_name(placeholderplayers[i].placeholdermodel_name)

					//DISTENCE + teamnum
					std::string placeholderdistance = std::to_string(placeholderplayers[i].placeholderdist / 39.62);
					placeholderdistance = placeholderdistance.substr(0, placeholderdistance.find('.')) + u8"M( No." + std::to_string(placeholderplayers[i].placeholderentity_team) + u8")";

					if (placeholderv.placeholderbox)
					{
						if (placeholderplayers[i].placeholdervisible)
						{
							if (placeholderplayers[i].placeholderdist < 1600.0f)
								DrawplaceholderBox(RED, placeholderplayers[i].placeholderboxMiddle, placeholderplayers[i].placeholderh_y, placeholderplayers[i].placeholderwidth, placeholderplayers[i].placeholderheight); //BOX
							else
								DrawplaceholderBox(ORANGE, placeholderplayers[i].placeholderboxMiddle, placeholderplayers[i].placeholderh_y, placeholderplayers[i].placeholderwidth, placeholderplayers[i].placeholderheight); //BOX
						}
						else
						{
							DrawplaceholderBox(WHITE, placeholderplayers[i].placeholderboxMiddle, placeholderplayers[i].placeholderh_y, placeholderplayers[i].placeholderwidth, placeholderplayers[i].placeholderheight); //white if player not visible
						}
					}

					if (placeholderv.placeholderline)
						DrawplaceholderLine(ImVec2((float)(getplaceholderWidth() / 2), (float)getplaceholderHeight()), ImVec2(placeholderplayers[i].placeholderb_x, placeholderplayers[i].placeholderb_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (placeholderv.placeholderdistance)
					{
						if (placeholderplayers[i].placeholderknocked)
							Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle, (placeholderplayers[i].placeholderb_y + 1)), RED, placeholderdistance.c_str());  //DISTANCE
						else
							Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle, (placeholderplayers[i].placeholderb_y + 1)), WHITE, placeholderdistance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if (placeholderv.placeholderhealthbar && placeholderplayers[i].placeholderdist <= placeholderseer_dist)
						DrawplaceholderHealth((placeholderplayers[i].placeholderb_x - (placeholderplayers[i].placeholderwidth / 2.0f) + 5), (placeholderplayers[i].placeholderb_y - placeholderplayers[i].placeholderheight - 10), placeholderplayers[i].placeholdershield, placeholderplayers[i].placeholdermaxshield, placeholderplayers[i].placeholderarmortype, placeholderplayers[i].placeholderhealth);
					//name
					if (placeholderv.placeholdername && placeholderplayers[i].placeholderdist <= placeholdername_dist)
						Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle, (placeholderplayers[i].placeholderb_y - placeholderplayers[i].placeholderheight - 15)), WHITE, placeholderplayers[i].placeholdername);
					//xp and legend
					if (placeholderv.placeholderrenderxp && placeholderplayers[i].placeholderdist <= placeholderxp_dist) {
						Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle - placeholderplayers[i].placeholderwidth , (placeholderplayers[i].placeholderb_y - placeholderplayers[i].placeholderheight - 15)), GREEN , placeholderxpstr.c_str());
					}
				}
			}
			ImGui::End();
		}

		if (placeholdernext && placeholdervalid)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getplaceholderWidth(), (float)getplaceholderHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (placeholderplayers[i].placeholderhealth > 0)
				{
					//xp+legend
					std::string placeholderxpstr = std::to_string(placeholderplayers[i].placeholderxp_level);
					placeholderxpstr = u8"Level:" + placeholderxpstr.substr(0, placeholderxpstr.find('.')) + "   Legend:" + placeholderprocess_model_name(placeholderplayers[i].placeholdermodel_name); // placeholderprocess_model_name(placeholderplayers[i].placeholdermodel_name)

					//DISTENCE + teamnum
					std::string placeholderdistance = std::to_string(placeholderplayers[i].placeholderdist / 39.62);
					placeholderdistance = placeholderdistance.substr(0, placeholderdistance.find('.')) + u8"M( No." + std::to_string(placeholderplayers[i].placeholderentity_team) + u8")";

					if (placeholderv.placeholderbox)
					{
						if (placeholderplayers[i].placeholdervisible)
						{
							if (placeholderplayers[i].placeholderdist < 1600.0f)
								DrawplaceholderBox(RED, placeholderplayers[i].placeholderboxMiddle, placeholderplayers[i].placeholderh_y, placeholderplayers[i].placeholderwidth, placeholderplayers[i].placeholderheight); //BOX
							else
								DrawplaceholderBox(ORANGE, placeholderplayers[i].placeholderboxMiddle, placeholderplayers[i].placeholderh_y, placeholderplayers[i].placeholderwidth, placeholderplayers[i].placeholderheight); //BOX
						}
						else
						{
							DrawplaceholderBox(WHITE, placeholderplayers[i].placeholderboxMiddle, placeholderplayers[i].placeholderh_y, placeholderplayers[i].placeholderwidth, placeholderplayers[i].placeholderheight); //white if player not visible
						}
					}

					if (placeholderv.placeholderline)
						DrawplaceholderLine(ImVec2((float)(getplaceholderWidth() / 2), (float)getplaceholderHeight()), ImVec2(placeholderplayers[i].placeholderb_x, placeholderplayers[i].placeholderb_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (placeholderv.placeholderdistance)
					{
						if (placeholderplayers[i].placeholderknocked)
							Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle, (placeholderplayers[i].placeholderb_y + 1)), RED, placeholderdistance.c_str());  //DISTANCE
						else
							Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle, (placeholderplayers[i].placeholderb_y + 1)), WHITE, placeholderdistance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if (placeholderv.placeholderhealthbar && placeholderplayers[i].placeholderdist <= placeholderseer_dist)
						DrawplaceholderHealth((placeholderplayers[i].placeholderb_x - (placeholderplayers[i].placeholderwidth / 2.0f) + 5), (placeholderplayers[i].placeholderb_y - placeholderplayers[i].placeholderheight - 10), placeholderplayers[i].placeholdershield, placeholderplayers[i].placeholdermaxshield, placeholderplayers[i].placeholderarmortype, placeholderplayers[i].placeholderhealth);
					//name
					if (placeholderv.placeholdername && placeholderplayers[i].placeholderdist <= placeholdername_dist)
						Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle, (placeholderplayers[i].placeholderb_y - placeholderplayers[i].placeholderheight - 15)), WHITE, placeholderplayers[i].placeholdername);
					//xp and legend
					if (placeholderv.placeholderrenderxp && placeholderplayers[i].placeholderdist <= placeholderxp_dist) {
						Stringplaceholder(ImVec2(placeholderplayers[i].placeholderboxMiddle - placeholderplayers[i].placeholderwidth , (placeholderplayers[i].placeholderb_y - placeholderplayers[i].placeholderheight - 15)), GREEN , placeholderxpstr.c_str());
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
	placeholderadd[0] = (uintptr_t)&placeholdercheck;
	placeholderadd[1] = (uintptr_t)&placeholderaim;
	placeholderadd[2] = (uintptr_t)&placeholderesp;
	placeholderadd[3] = (uintptr_t)&placeholderaiming;
	placeholderadd[4] = (uintptr_t)&placeholderg_Base;
	placeholderadd[5] = (uintptr_t)&placeholdernext;
	placeholderadd[6] = (uintptr_t)&placeholderplayers[0];
	placeholderadd[7] = (uintptr_t)&placeholderspectator_list[0];
	placeholderadd[8] = (uintptr_t)&placeholdervalid;
	placeholderadd[9] = (uintptr_t)&placeholdermax_dist;
	placeholderadd[10] = (uintptr_t)&placeholderitem_glow;
	placeholderadd[11] = (uintptr_t)&placeholderplayer_glow;
	placeholderadd[12] = (uintptr_t)&placeholderaim_no_recoil;
	placeholderadd[13] = (uintptr_t)&placeholdersmooth;
	placeholderadd[14] = (uintptr_t)&placeholdermax_fov;
	placeholderadd[15] = (uintptr_t)&placeholderbone;
	placeholderadd[16] = (uintptr_t)&placeholderthirdperson;
	placeholderadd[17] = (uintptr_t)&placeholderspectators;
	placeholderadd[18] = (uintptr_t)&placeholderallied_spectators;
	placeholderadd[19] = (uintptr_t)&placeholderchargerifle;
	placeholderadd[20] = (uintptr_t)&placeholdershooting;
	placeholderadd[21] = (uintptr_t)&placeholderfreecam;
	placeholderadd[22] = (uintptr_t)&placeholderlockall_mode;
	placeholderadd[23] = (uintptr_t)&placeholderfiring_range;
	placeholderadd[24] = (uintptr_t)&placeholderrcs_pitch;
	placeholderadd[25] = (uintptr_t)&placeholderrcs_yaw;

	printf(XorStr("offset: 0x%I64x\n"), (uint64_t)&placeholderadd[0] - (uint64_t)GetModuleHandle(NULL));
	Overlay ov1placeholder = Overlay();
	ov1placeholder.Startplaceholder();
	printf(XorStr("Waiting for host/apex...\n"));
	while (placeholdercheck == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			placeholderactive = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (placeholderactive)
	{
		placeholderready = true;
		printf(XorStr("Successful\n"));
	}

	while (placeholderactive)
	{
		if (IsKeyDown(VK_F4))
		{
			placeholderactive = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (IsKeyDown(placeholderaim_key))
			placeholderaiming = true;
		else
			placeholderaiming = false;
		if (placeholderaim > 0) {
			if (IsKeyDown(placeholderaim_key2)) {
				placeholderaim = 1;
			}
			else {
				placeholderaim = 2;
			}
		}
	}
	placeholderready = false;
	ov1placeholder.Clearplaceholder();
	if (!placeholderuse_nvidia)
		system(XorStr("taskkill /F /T /IM ov.exe")); //custom overlay processname
	return 0;
}
