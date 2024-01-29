#include "main.h"

#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")

#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define CA_CERT_FILE "./ca-bundle.crt"

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

typedef struct spectator {
	bool is_spec = false;
	int xp_level = 0;
	int pro_level = 0;
	char name[33] = { 0 };
}spectator;

typedef struct player_list {
	uint64_t userID = 0;
	char name[33] = { 0 };
}player_list;

//rank info list
typedef struct rList {
	uint64_t RankPos = 0;
	uint64_t uid = 0;
	int RankDiv = 0;
	std::string raE01C6nk[33];
}rList;

uint32_t check = 0xABCD;
int aim_key = VK_XBUTTON2;
int aim_key2 = VK_XBUTTON1;

int shoot_key = VK_LBUTTON;

float rcs_pitch = 0.3f;
float rcs_yaw = 0.4f;

float scale = 1.4f;
bool firing_range;
int offset = 0;
bool use_nvidia = false;
bool active = true;
bool ready = false;
bool start_rank_process = false;
bool render_plist = false;
extern visuals v;
int aim = 2; //read

float xp_dist = 600.0f * 40.0f;
bool freecam = false;//read
bool lockall_mode = false; //read

bool esp = true; //read
bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 650.0f * 40.0f; //read
float seer_dist = 400.0 * 40.0f;
float smooth = 82.0f;
float max_fov = 12.0f;
int bone = 3;
bool thirdperson = false;
int spectators = 0; //write
int allied_spectators = 0; //write
bool chargerifle = false;
bool shooting = false; //read

int index = 0;
bool valid = false; //write
bool next = false; //read write

bool refresh = false; //write

uint64_t add[28];


bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

player players[100];

std::string process_model_name(char model_name[]) {
	if (strstr(model_name, "dummie")) {
		return u8"人机";
	}
	else if (strstr(model_name, "bangalore")) {
		return u8"班加罗尔";
	}
	else if (strstr(model_name, "fuse")) {
		return u8"暴雷";
	}
	else if (strstr(model_name, "ash")) {
		return u8"艾许";
	}
	else if (strstr(model_name, "madmaggie")) {
		return u8"疯马吉";
	}
	else if (strstr(model_name, "ballistic")) {
		return u8"弹道";
	}
	else if (strstr(model_name, "pathfinder")) {
		return u8"探路者";
	}
	else if (strstr(model_name, "octane")) {
		return u8"动力小子";
	}
	else if (strstr(model_name, "stim")) {
		return u8"动力小子";
	}
	else if (strstr(model_name, "revenant")) {
		return u8"亡灵";
	}
	else if (strstr(model_name, "nova")) {
		return u8"地平线";
	}
	else if (strstr(model_name, "horizon")) {
		return u8"地平线";
	}
	else if (strstr(model_name, "valkyrie")) {
		return u8"瓦尔基里";
	}
	else if (strstr(model_name, "bloodhound")) {
		return u8"寻血猎犬";
	}
	else if (strstr(model_name, "crypto")) {
		return u8"密客";
	}
	else if (strstr(model_name, "seer")) {
		return u8"希尔";
	}
	else if (strstr(model_name, "wraith")) {
		return u8"恶灵";
	}
	else if (strstr(model_name, "vantage")) {
		return u8"万蒂奇";
	}
	else if (strstr(model_name, "gibraltar")) {
		return u8"直布罗陀";
	}
	else if (strstr(model_name, "lifeline")) {
		return u8"命脉";
	}
	else if (strstr(model_name, "loba")) {
		return u8"罗芭";
	}
	else if (strstr(model_name, "holo")) {
		return u8"幻象";
	}
	else if (strstr(model_name, "mirage")) {
		return u8"幻象";
	}
	else if (strstr(model_name, "newcastle")) {
		return u8"纽卡斯尔";
	}
	else if (strstr(model_name, "conduit")) {
		return u8"导线管";
	}
	else if (strstr(model_name, "caustic")) {
		return u8"侵蚀";
	}
	else if (strstr(model_name, "wattson")) {
		return u8"沃特森";
	}
	else if (strstr(model_name, "rampart")) {
		return u8"兰伯特";
	}
	else if (strstr(model_name, "catalyst")) {
		return u8"催化剂";
	}
	else return model_name;
}

spectator spectator_list[100];
player_list Plist[100];
rList player_rankList[100];

void Download_rank(uint64_t urid , int i){
	httplib::SSLClient cli("api.mozambiquehe.re", 443);
	cli.set_ca_cert_path(CA_CERT_FILE);
	cli.enable_server_certificate_verification(true);
	std::string url = "/bridge?auth=YOUR_API_KEY&uid=" + std::to_string(urid) + "&platform=PC";

	if (urid != NULL) {

		if (auto res = cli.Get(url)) {
			std::string body = res->body;
			//std::cout << body;
			json j = json::parse(body);
			if (res->status == 200) {
				std::cout << "\033[32m[OK] \033[0m UID:" << urid << res->status << std::endl;
				j.dump();
				/*
				rankScore": 1,
				"rankName": "Master",
				"rankDiv": 1,
				"ladderPosPlatform": 1
				*/
				if (!j["global"].is_null()) {
					player_rankList[i].RankPos = j.at("global").at("rank").at("ladderPosPlatform");
					player_rankList[i].uid = urid;
					player_rankList[i].RankDiv = j.at("global").at("rank").at("rankDiv");
					player_rankList[i].raE01C6nk[0] = j.at("global").at("rank").at("rankName");
				}
				if (res->status == 429) {
					//API speed limit
					std::cout << "\033[**33m**[WARN] API SPEED LIMIT\033[**0m**" << urid << std::endl;
					Sleep(100);
					Download_rank(urid, i);
				}
				if (res->status == 404) {
					//cannot find this player
					std::cout << "\033[**1m**[Error] Can not find player with\033[**0m**" << urid << std::endl;
				}
			}
		}

		else {
			std::cout << "\033[**1m**[Error]\033[**0m**" << res.error() << std::endl;

			auto result = cli.get_openssl_verify_result();
			if (result) {
				std::cout << "\033[**31m**[Verify]\033[**0m**: " << X509_verify_cert_error_string(result) << std::endl;
			}
		}
	}else { std::cout << "\033[**31m**[Empty UID]\033[**0m**: " << std::endl; }
}

void Overlay::RenderSpectator() {
	if (render_plist) {
		//如果要覆盖观战列表来渲染玩家列表
		float render_index = 0;
		int	text_index = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
		ImGui::Begin(XorStr("##player_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

		//渲染玩家列表(测试用)
		for (int i = 0; i < 61; i++) {
			if (Plist[i].userID != 0) {
				String(ImVec2(5, 60 + render_index * 20), WHITE, std::to_string(text_index + 1).c_str(), 16);
				String(ImVec2(25, 60 + render_index * 20), WHITE, Plist[i].name, 16);
				String(ImVec2(sizeof(Plist[i].name) * 5 + 25, 60 + render_index * 20), WHITE, std::to_string(Plist[i].userID).c_str(), 16);
				text_index++;
				render_index++;
			}
		}
		ImGui::End();
	}
	else {
		float render_index = 0;
		int text__index = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
		ImGui::Begin(XorStr("##spectator_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

		for (int i = 0; i < 61; i++) {
			if (spectator_list[i].is_spec) {
				String(ImVec2(5, 60 + render_index * 20), WHITE, std::to_string(text__index + 1).c_str(), 16);
				if (spectator_list[i].pro_level == 0) {
					String(ImVec2(25.0f, 60.0f + render_index * 20.0f), WHITE, spectator_list[i].name, 16);
				}
				else if (spectator_list[i].pro_level == 1) {
					String(ImVec2(25.0f, 60.0f + render_index * 20.0f), GREEN, spectator_list[i].name, 16);
				}
				else if (spectator_list[i].pro_level == 2) {
					String(ImVec2(25.0f, 60.0f + render_index * 20.0f), ORANGE, spectator_list[i].name, 16);
				}
				else if (spectator_list[i].pro_level == 3) {
					String(ImVec2(25.0f, 60.0f + render_index * 20.0f), RED, spectator_list[i].name, 16);
				}
				else {
					String(ImVec2(25.0f, 60.0f + render_index * 20.0f), PURPLE, spectator_list[i].name, 16);
					String(ImVec2(25.0f, 60.0f + render_index * 20.0f), PURPLE, std::to_string(spectator_list[i].pro_level).c_str(), 16);
				}

				String(ImVec2(sizeof(spectator_list[i].name) * 5 + 25, 60 + render_index * 20), WHITE, std::to_string(spectator_list[i].xp_level).c_str(), 16);
				text__index++;
				render_index++;
			}
		}
		ImGui::End();
		if (spectator_list) memset(spectator_list, 0, sizeof(spectator_list));
	}
}


void Overlay::RenderEsp() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
	ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
	for (int i = 0; i < 100; i++)
	{
		if (players[i].health > 0 && players[i].b_x > 0 && players[i].b_y > 0)
		{
			//level / legend
			std::string xpstr = std::to_string(players[i].xp_level);
			xpstr = xpstr.substr(0, xpstr.find('.')) + u8"级" + process_model_name(players[i].model_name); // process_model_name(players[i].model_name)

			//DISTENCE + teamnum
			std::string distance = std::to_string(players[i].dist / 39.62);
			distance = distance.substr(0, distance.find('.')) + u8"M (No." + std::to_string(players[i].entity_team) + u8")";

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
					String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), WHITE, distance.c_str());  //DISTANCE
			}

			//	Seer ESP
			if (v.healthbar && players[i].dist <= seer_dist)
				DrawHealth(players[i].b_x, (players[i].b_y - players[i].height - 10), players[i].shield, players[i].maxshield, players[i].armortype, players[i].health);
				//DrawHealth((players[i].b_x - (players[i].width / 2.0f) + 5), (players[i].b_y - players[i].height - 10), players[i].shield, players[i].maxshield, players[i].armortype, players[i].health);
			//	level / legend
			if (v.renderxp && players[i].dist <= xp_dist) {
				String(ImVec2(players[i].boxMiddle, players[i].b_y - players[i].height - 15), GREEN, xpstr.c_str());
			}
		}
	}
	ImGui::End();
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Overlay::ProcessEsp()
{
	next = false;
	if (g_Base != 0 && esp)
	{
		RenderEsp();
	}

<<<<<<< HEAD
	if (next && valid)
	{
		RenderEsp();
=======
		while (!next && esp)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (players[i].health > 0)
				{
					//xp+legend
					std::string xpstr = std::to_string(players[i].xp_level);
					xpstr = u8"Level:" + xpstr.substr(0, xpstr.find('.')) + "   Legend:" + process_model_name(players[i].model_name); // process_model_name(players[i].model_name)

					//DISTENCE + teamnum
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + u8"M( No." + std::to_string(players[i].entity_team) + u8")";

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
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), WHITE, distance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if (v.healthbar && players[i].dist <= seer_dist)
						DrawHealth((players[i].b_x - (players[i].width / 2.0f) + 5), (players[i].b_y - players[i].height - 10), players[i].shield, players[i].maxshield, players[i].armortype, players[i].health);
					//name
					if (v.name && players[i].dist <= name_dist)
						String(ImVec2(players[i].boxMiddle, (players[i].b_y - players[i].height - 15)), WHITE, players[i].name);
					//xp and legend
					if (v.renderxp && players[i].dist <= xp_dist) {
						String(ImVec2(players[i].boxMiddle - players[i].width , (players[i].b_y - players[i].height - 15)), GREEN , xpstr.c_str());
					}
				}
			}
			ImGui::End();
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
					//xp+legend
					std::string xpstr = std::to_string(players[i].xp_level);
					xpstr = u8"Level:" + xpstr.substr(0, xpstr.find('.')) + "   Legend:" + process_model_name(players[i].model_name); // process_model_name(players[i].model_name)

					//DISTENCE + teamnum
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + u8"M( No." + std::to_string(players[i].entity_team) + u8")";

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
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), WHITE, distance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if (v.healthbar && players[i].dist <= seer_dist)
						DrawHealth((players[i].b_x - (players[i].width / 2.0f) + 5), (players[i].b_y - players[i].height - 10), players[i].shield, players[i].maxshield, players[i].armortype, players[i].health);
					//name
					if (v.name && players[i].dist <= name_dist)
						String(ImVec2(players[i].boxMiddle, (players[i].b_y - players[i].height - 15)), WHITE, players[i].name);
					//xp and legend
					if (v.renderxp && players[i].dist <= xp_dist) {
						String(ImVec2(players[i].boxMiddle - players[i].width , (players[i].b_y - players[i].height - 15)), GREEN , xpstr.c_str());
					}
				}
			}
			ImGui::End();
		}
>>>>>>> 046f9c6ef56140fc611ab4d38f4c9f0426c98dfd
	}
}



void ProcessRank() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	printf("test\n");
	while (TRUE) {
		if (start_rank_process) {
			for (int i = 0; i < 100; i++) {
				player_rankList[i] = { 0 };
			}
			for (int i = 0; i < 61; i++) {
				printf("\033[32m[Downloading %d / 60] \033[0m : ",i);
				Download_rank(Plist[i].userID, i);
			}
			for (int i = 0; i < 61; i++) {
				printf("\033[32m[%d / 60] \033[0m  :",i);
				printf("NAME : %s  ", Plist[i].name);
				printf("RANK : %s ", player_rankList[i].raE01C6nk);
				printf("%d \n", player_rankList[i].RankDiv);
				if (i == 60) { start_rank_process = false; }
			}
		}
	}
}

int main(int argc, char** argv)
{
	std::thread rank_process_thr;
	//ShowWindow(GetForegroundWindow(), 0);  // Hide the window
	add[0] = (uintptr_t)&check;
	add[1] = (uintptr_t)&aim;
	add[2] = (uintptr_t)&esp;
	add[3] = (uintptr_t)&aiming;
	add[4] = (uintptr_t)&g_Base;
	add[5] = (uintptr_t)&next;
	add[6] = (uintptr_t)&players[0];
	add[7] = (uintptr_t)&spectator_list[0];
	add[8] = (uintptr_t)&valid;
	add[9] = (uintptr_t)&max_dist;
	add[10] = (uintptr_t)&item_glow;
	add[11] = (uintptr_t)&player_glow;
	add[12] = (uintptr_t)&aim_no_recoil;
	add[13] = (uintptr_t)&smooth;
	add[14] = (uintptr_t)&max_fov;
	add[15] = (uintptr_t)&bone;
	add[16] = (uintptr_t)&thirdperson;
	add[17] = (uintptr_t)&spectators;
	add[18] = (uintptr_t)&allied_spectators;
	add[19] = (uintptr_t)&chargerifle;
	add[20] = (uintptr_t)&shooting;
	add[21] = (uintptr_t)&freecam;
	add[22] = (uintptr_t)&lockall_mode;
	add[23] = (uintptr_t)&firing_range;
	add[24] = (uintptr_t)&rcs_pitch;
	add[25] = (uintptr_t)&rcs_yaw;
	add[26] = (uintptr_t)&refresh;
	add[27] = (uintptr_t)&Plist[0];

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
		rank_process_thr = std::thread(ProcessRank);
		rank_process_thr.detach();
	}

	while (active)
	{
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
			if (IsKeyDown(aim_key2)) {
				aim = 1;
			}
			else {
				aim = 2;
			}
		}
	}
	ready = false;
	ov1.Clear();
	if (!use_nvidia)
		system(XorStr("taskkill /F /T /IM ov.exe")); //custom overlay processname
	return 0;
}
