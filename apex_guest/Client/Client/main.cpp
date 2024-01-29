#include "main.h"

#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")

#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define CA_CERT_FILE "./ca-bundle.crt"

typedef struct E01C6Dplayer
{
	float E01C6Ddist = 0;
	int E01C6Dentity_team = 0;
	float E01C6DboxMiddle = 0;
	float E01C6Dh_y = 0;
	float E01C6Dwidth = 0;
	float E01C6Dheight = 0;
	float E01C6Db_x = 0;
	float E01C6Db_y = 0;
	bool E01C6Dknocked = false;
	bool E01C6Dvisible = false;
	int E01C6Dhealth = 0;
	int E01C6Dshield = 0;
	int E01C6Dxp_level = 0;
	int E01C6Dmaxshield = 0;
	int E01C6Darmortype = 0;
	char E01C6Dname[33] = { 0 };
	char E01C6Dmodel_name[100] = { 0 };
}E01C6Dplayer;

typedef struct E01C6Dspectator {
	bool is_E01C6Dspec = false;
	int xp_E01C6Dlevel = 0;
	int E01C6Dpro_level = 0;
	char nE01C6Dame[33] = { 0 };
}E01C6Dspectator;

typedef struct E01C6Dplayer_list {
	uint64_t userE01C6DID = 0;
	char nE01C6Dame[33] = { 0 };
}E01C6Dplayer_list;

//rank info list
typedef struct rE01C6DList {
	uint64_t RankE01C6DPos = 0;
	uint64_t E01C6Duid = 0;
	int RankE01C6DDiv = 0;
	std::string raE01C6nk[33];
}rE01C6DList;

uint32_t E01C6Dcheck = 0xABCD;
int E01C6Daim_key = VK_XBUTTON2;
int E01C6Daim_key2 = VK_XBUTTON1;

int E01C6Dshoot_key = VK_LBUTTON;

float E01C6Drcs_pitch = 0.3f;
float E01C6Drcs_yaw = 0.4f;

float E01C6Dscale = 1.4f;
bool E01C6Dfiring_range;
int E01C6Doffset = 0;
bool E01C6Duse_nvidia = false;
bool E01C6Dactive = true;
bool E01C6Dready = false;
bool E01C6Dstart_rank_process = false;
bool E01C6Drender_plist = false;
extern E01C6Dvisuals E01C6Dv;
int E01C6Daim = 2; //read

float E01C6Dxp_dist = 600.0f * 40.0f;
bool E01C6Dfreecam = false;//read
bool E01C6Dlockall_mode = false; //read

bool E01C6Desp = true; //read
bool E01C6Ditem_glow = false;
bool E01C6Dplayer_glow = false;
bool E01C6Daim_no_recoil = true;
bool E01C6Daiming = false; //read
uint64_t E01C6Dg_Base = 0; //write
float E01C6Dmax_dist = 650.0f * 40.0f; //read
float E01C6Dseer_dist = 400.0 * 40.0f;
float E01C6Dsmooth = 82.0f;
float E01C6Dmax_fov = 12.0f;
int E01C6Dbone = 3;
bool E01C6Dthirdperson = false;
int E01C6Dspectators = 0; //write
int E01C6Dallied_spectators = 0; //write
bool E01C6Dchargerifle = false;
bool E01C6Dshooting = false; //read

int E01C6Dindex = 0;
bool E01C6Dvalid = false; //write
bool E01C6Dnext = false; //read write

bool E01C6Drefresh = false; //write

uint64_t E01C6Dadd[28];


bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

E01C6Dplayer E01C6Dplayers[100];

std::string E01C6Dprocess_model_name(char E01C6Dmodel_name[]) {
	if (strstr(E01C6Dmodel_name, "dummie")) {
		return u8"人机";
	}
	else if (strstr(E01C6Dmodel_name, "bangalore")) {
		return u8"班加罗尔";
	}
	else if (strstr(E01C6Dmodel_name, "fuse")) {
		return u8"暴雷";
	}
	else if (strstr(E01C6Dmodel_name, "ash")) {
		return u8"艾许";
	}
	else if (strstr(E01C6Dmodel_name, "madmaggie")) {
		return u8"疯马吉";
	}
	else if (strstr(E01C6Dmodel_name, "ballistic")) {
		return u8"弹道";
	}
	else if (strstr(E01C6Dmodel_name, "pathfinder")) {
		return u8"探路者";
	}
	else if (strstr(E01C6Dmodel_name, "octane")) {
		return u8"动力小子";
	}
	else if (strstr(E01C6Dmodel_name, "stim")) {
		return u8"动力小子";
	}
	else if (strstr(E01C6Dmodel_name, "revenant")) {
		return u8"亡灵";
	}
	else if (strstr(E01C6Dmodel_name, "nova")) {
		return u8"地平线";
	}
	else if (strstr(E01C6Dmodel_name, "horizon")) {
		return u8"地平线";
	}
	else if (strstr(E01C6Dmodel_name, "valkyrie")) {
		return u8"瓦尔基里";
	}
	else if (strstr(E01C6Dmodel_name, "bloodhound")) {
		return u8"寻血猎犬";
	}
	else if (strstr(E01C6Dmodel_name, "crypto")) {
		return u8"密客";
	}
	else if (strstr(E01C6Dmodel_name, "seer")) {
		return u8"希尔";
	}
	else if (strstr(E01C6Dmodel_name, "wraith")) {
		return u8"恶灵";
	}
	else if (strstr(E01C6Dmodel_name, "vantage")) {
		return u8"万蒂奇";
	}
	else if (strstr(E01C6Dmodel_name, "gibraltar")) {
		return u8"直布罗陀";
	}
	else if (strstr(E01C6Dmodel_name, "lifeline")) {
		return u8"命脉";
	}
	else if (strstr(E01C6Dmodel_name, "loba")) {
		return u8"罗芭";
	}
	else if (strstr(E01C6Dmodel_name, "holo")) {
		return u8"幻象";
	}
	else if (strstr(E01C6Dmodel_name, "mirage")) {
		return u8"幻象";
	}
	else if (strstr(E01C6Dmodel_name, "newcastle")) {
		return u8"纽卡斯尔";
	}
	else if (strstr(E01C6Dmodel_name, "conduit")) {
		return u8"导线管";
	}
	else if (strstr(E01C6Dmodel_name, "caustic")) {
		return u8"侵蚀";
	}
	else if (strstr(E01C6Dmodel_name, "wattson")) {
		return u8"沃特森";
	}
	else if (strstr(E01C6Dmodel_name, "rampart")) {
		return u8"兰伯特";
	}
	else if (strstr(E01C6Dmodel_name, "catalyst")) {
		return u8"催化剂";
	}
	else return E01C6Dmodel_name;
}

E01C6Dspectator E01C6Dspectator_list[100];
E01C6Dplayer_list PE01C6Dlist[100];
rE01C6DList player_rankE01C6DList[100];

void DownloadE01C6D_rank(uint64_t urE01C6Did , int i){
	httplib::SSLClient cli("api.mozambiquehe.re", 443);
	cli.set_ca_cert_path(CA_CERT_FILE);
	cli.enable_server_certificate_verification(true);
	std::string url = "/bridge?auth=YOUR_API_KEY&uid=" + std::to_string(urE01C6Did) + "&platform=PC";

	if (urE01C6Did != NULL) {

		if (auto res = cli.Get(url)) {
			std::string boE01C6Ddy = res->body;
			//std::cout << body;
			json E01C6Dj = json::parse(boE01C6Ddy);
			if (res->status == 200) {
				std::cout << "\033[32m[OK] \033[0m UID:" << urE01C6Did << res->status << std::endl;
				E01C6Dj.dump();
				/*
				rankScore": 1,
				"rankName": "Master",
				"rankDiv": 1,
				"ladderPosPlatform": 1
				*/
				if (!E01C6Dj["global"].is_null()) {
					player_rankE01C6DList[i].RankE01C6DPos = E01C6Dj.at("global").at("rank").at("ladderPosPlatform");
					player_rankE01C6DList[i].E01C6Duid = urE01C6Did;
					player_rankE01C6DList[i].RankE01C6DDiv = E01C6Dj.at("global").at("rank").at("rankDiv");
					player_rankE01C6DList[i].raE01C6nk[0] = E01C6Dj.at("global").at("rank").at("rankName");
				}
				if (res->status == 429) {
					//API speed limit
					std::cout << "\033[**33m**[WARN] API SPEED LIMIT\033[**0m**" << urE01C6Did << std::endl;
					Sleep(100);
					DownloadE01C6D_rank(urE01C6Did, i);
				}
				if (res->status == 404) {
					//cannot find this player
					std::cout << "\033[**1m**[Error] Can not find player with\033[**0m**" << urE01C6Did << std::endl;
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

void Overlay::RenderE01C6DSpectator() {
	if (E01C6Drender_plist) {
		//如果要覆盖观战列表来渲染玩家列表
		float render_E01C6Dindex = 0;
		int	textE01C6D_index = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)getE01C6DWidth(), (float)getE01C6DHeight()));
		ImGui::Begin(XorStr("##player_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

		//渲染玩家列表(测试用)
		for (int i = 0; i < 61; i++) {
			if (PE01C6Dlist[i].userE01C6DID != 0) {
				StringE01C6D(ImVec2(5, 60 + render_E01C6Dindex * 20), WHITE, std::to_string(textE01C6D_index + 1).c_str(), 16);
				StringE01C6D(ImVec2(25, 60 + render_E01C6Dindex * 20), WHITE, PE01C6Dlist[i].nE01C6Dame, 16);
				StringE01C6D(ImVec2(sizeof(PE01C6Dlist[i].nE01C6Dame) * 5 + 25, 60 + render_E01C6Dindex * 20), WHITE, std::to_string(PE01C6Dlist[i].userE01C6DID).c_str(), 16);
				textE01C6D_index++;
				render_E01C6Dindex++;
			}
		}
		ImGui::End();
	}
	else {
		float render_E01C6Dindex = 0;
		int text_E01C6D_index = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)getE01C6DWidth(), (float)getE01C6DHeight()));
		ImGui::Begin(XorStr("##spectator_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

		for (int i = 0; i < 61; i++) {
			if (E01C6Dspectator_list[i].is_E01C6Dspec) {
				StringE01C6D(ImVec2(5, 60 + render_E01C6Dindex * 20), WHITE, std::to_string(text_E01C6D_index + 1).c_str(), 16);
				if (E01C6Dspectator_list[i].E01C6Dpro_level == 0) {
					StringE01C6D(ImVec2(25.0f, 60.0f + render_E01C6Dindex * 20.0f), WHITE, E01C6Dspectator_list[i].nE01C6Dame, 16);
				}
				else if (E01C6Dspectator_list[i].E01C6Dpro_level == 1) {
					StringE01C6D(ImVec2(25.0f, 60.0f + render_E01C6Dindex * 20.0f), GREEN, E01C6Dspectator_list[i].nE01C6Dame, 16);
				}
				else if (E01C6Dspectator_list[i].E01C6Dpro_level == 2) {
					StringE01C6D(ImVec2(25.0f, 60.0f + render_E01C6Dindex * 20.0f), ORANGE, E01C6Dspectator_list[i].nE01C6Dame, 16);
				}
				else if (E01C6Dspectator_list[i].E01C6Dpro_level == 3) {
					StringE01C6D(ImVec2(25.0f, 60.0f + render_E01C6Dindex * 20.0f), RED, E01C6Dspectator_list[i].nE01C6Dame, 16);
				}
				else {
					StringE01C6D(ImVec2(25.0f, 60.0f + render_E01C6Dindex * 20.0f), PURPLE, E01C6Dspectator_list[i].nE01C6Dame, 16);
					StringE01C6D(ImVec2(25.0f, 60.0f + render_E01C6Dindex * 20.0f), PURPLE, std::to_string(E01C6Dspectator_list[i].E01C6Dpro_level).c_str(), 16);
				}

				StringE01C6D(ImVec2(sizeof(E01C6Dspectator_list[i].nE01C6Dame) * 5 + 25, 60 + render_E01C6Dindex * 20), WHITE, std::to_string(E01C6Dspectator_list[i].xp_E01C6Dlevel).c_str(), 16);
				text_E01C6D_index++;
				render_E01C6Dindex++;
			}
		}
		ImGui::End();
		if (E01C6Dspectator_list) memset(E01C6Dspectator_list, 0, sizeof(E01C6Dspectator_list));
	}
}


void Overlay::RenderE01C6DEsp() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2((float)getE01C6DWidth(), (float)getE01C6DHeight()));
	ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
	for (int i = 0; i < 100; i++)
	{
		if (E01C6Dplayers[i].E01C6Dhealth > 0 && E01C6Dplayers[i].E01C6Db_x > 0 && E01C6Dplayers[i].E01C6Db_y > 0)
		{
			//level / legend
			std::string E01C6Dxpstr = std::to_string(E01C6Dplayers[i].E01C6Dxp_level);
			E01C6Dxpstr = E01C6Dxpstr.substr(0, E01C6Dxpstr.find('.')) + u8"级" + E01C6Dprocess_model_name(E01C6Dplayers[i].E01C6Dmodel_name); // E01C6Dprocess_model_name(E01C6Dplayers[i].E01C6Dmodel_name)

			//DISTENCE + teamnum
			std::string E01C6Ddistance = std::to_string(E01C6Dplayers[i].E01C6Ddist / 39.62);
			E01C6Ddistance = E01C6Ddistance.substr(0, E01C6Ddistance.find('.')) + u8"M (No." + std::to_string(E01C6Dplayers[i].E01C6Dentity_team) + u8")";

			if (E01C6Dv.E01C6Dbox)
			{
				if (E01C6Dplayers[i].E01C6Dvisible)
				{
					if (E01C6Dplayers[i].E01C6Ddist < 1600.0f)
						DrawE01C6DBox(RED, E01C6Dplayers[i].E01C6DboxMiddle, E01C6Dplayers[i].E01C6Dh_y, E01C6Dplayers[i].E01C6Dwidth, E01C6Dplayers[i].E01C6Dheight); //BOX
					else
						DrawE01C6DBox(ORANGE, E01C6Dplayers[i].E01C6DboxMiddle, E01C6Dplayers[i].E01C6Dh_y, E01C6Dplayers[i].E01C6Dwidth, E01C6Dplayers[i].E01C6Dheight); //BOX
				}
				else
				{
					DrawE01C6DBox(WHITE, E01C6Dplayers[i].E01C6DboxMiddle, E01C6Dplayers[i].E01C6Dh_y, E01C6Dplayers[i].E01C6Dwidth, E01C6Dplayers[i].E01C6Dheight); //white if player not visible
				}
			}

			if (E01C6Dv.E01C6Dline)
				DrawE01C6DLine(ImVec2((float)(getE01C6DWidth() / 2), (float)getE01C6DHeight()), ImVec2(E01C6Dplayers[i].E01C6Db_x, E01C6Dplayers[i].E01C6Db_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

			if (E01C6Dv.E01C6Ddistance)
			{
				if (E01C6Dplayers[i].E01C6Dknocked)
					StringE01C6D(ImVec2(E01C6Dplayers[i].E01C6DboxMiddle, (E01C6Dplayers[i].E01C6Db_y + 1)), RED, E01C6Ddistance.c_str());  //DISTANCE
				else
					StringE01C6D(ImVec2(E01C6Dplayers[i].E01C6DboxMiddle, (E01C6Dplayers[i].E01C6Db_y + 1)), WHITE, E01C6Ddistance.c_str());  //DISTANCE
			}

			//	Seer ESP
			if (E01C6Dv.E01C6Dhealthbar && E01C6Dplayers[i].E01C6Ddist <= E01C6Dseer_dist)
				DrawE01C6DHealth(E01C6Dplayers[i].E01C6Db_x, (E01C6Dplayers[i].E01C6Db_y - E01C6Dplayers[i].E01C6Dheight - 10), E01C6Dplayers[i].E01C6Dshield, E01C6Dplayers[i].E01C6Dmaxshield, E01C6Dplayers[i].E01C6Darmortype, E01C6Dplayers[i].E01C6Dhealth);
				//DrawE01C6DHealth((E01C6Dplayers[i].E01C6Db_x - (E01C6Dplayers[i].E01C6Dwidth / 2.0f) + 5), (E01C6Dplayers[i].E01C6Db_y - E01C6Dplayers[i].E01C6Dheight - 10), E01C6Dplayers[i].E01C6Dshield, E01C6Dplayers[i].E01C6Dmaxshield, E01C6Dplayers[i].E01C6Darmortype, E01C6Dplayers[i].E01C6Dhealth);
			//	level / legend
			if (E01C6Dv.E01C6Drenderxp && E01C6Dplayers[i].E01C6Ddist <= E01C6Dxp_dist) {
				StringE01C6D(ImVec2(E01C6Dplayers[i].E01C6DboxMiddle, E01C6Dplayers[i].E01C6Db_y - E01C6Dplayers[i].E01C6Dheight - 15), GREEN, E01C6Dxpstr.c_str());
			}
		}
	}
	ImGui::End();
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Overlay::ProcessE01C6DEsp()
{
	E01C6Dnext = false;
	if (E01C6Dg_Base != 0 && E01C6Desp)
	{
		RenderE01C6DEsp();
	}

<<<<<<< HEAD
	if (E01C6Dnext && E01C6Dvalid)
	{
		RenderE01C6DEsp();
=======
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
>>>>>>> 046f9c6ef56140fc611ab4d38f4c9f0426c98dfd
	}
}



void ProcessE01C6DRank() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	printf("test\n");
	while (TRUE) {
		if (E01C6Dstart_rank_process) {
			for (int i = 0; i < 100; i++) {
				player_rankE01C6DList[i] = { 0 };
			}
			for (int i = 0; i < 61; i++) {
				printf("\033[32m[Downloading %d / 60] \033[0m : ",i);
				DownloadE01C6D_rank(PE01C6Dlist[i].userE01C6DID, i);
			}
			for (int i = 0; i < 61; i++) {
				printf("\033[32m[%d / 60] \033[0m  :",i);
				printf("NAME : %s  ", PE01C6Dlist[i].nE01C6Dame);
				printf("RANK : %s ", player_rankE01C6DList[i].raE01C6nk);
				printf("%d \n", player_rankE01C6DList[i].RankE01C6DDiv);
				if (i == 60) { E01C6Dstart_rank_process = false; }
			}
		}
	}
}

int main(int argc, char** argv)
{
	std::thread ranE01C6Dk_process_thr;
	//ShowWindow(GetForegroundWindow(), 0);  // Hide the window
	E01C6Dadd[0] = (uintptr_t)&E01C6Dcheck;
	E01C6Dadd[1] = (uintptr_t)&E01C6Daim;
	E01C6Dadd[2] = (uintptr_t)&E01C6Desp;
	E01C6Dadd[3] = (uintptr_t)&E01C6Daiming;
	E01C6Dadd[4] = (uintptr_t)&E01C6Dg_Base;
	E01C6Dadd[5] = (uintptr_t)&E01C6Dnext;
	E01C6Dadd[6] = (uintptr_t)&E01C6Dplayers[0];
	E01C6Dadd[7] = (uintptr_t)&E01C6Dspectator_list[0];
	E01C6Dadd[8] = (uintptr_t)&E01C6Dvalid;
	E01C6Dadd[9] = (uintptr_t)&E01C6Dmax_dist;
	E01C6Dadd[10] = (uintptr_t)&E01C6Ditem_glow;
	E01C6Dadd[11] = (uintptr_t)&E01C6Dplayer_glow;
	E01C6Dadd[12] = (uintptr_t)&E01C6Daim_no_recoil;
	E01C6Dadd[13] = (uintptr_t)&E01C6Dsmooth;
	E01C6Dadd[14] = (uintptr_t)&E01C6Dmax_fov;
	E01C6Dadd[15] = (uintptr_t)&E01C6Dbone;
	E01C6Dadd[16] = (uintptr_t)&E01C6Dthirdperson;
	E01C6Dadd[17] = (uintptr_t)&E01C6Dspectators;
	E01C6Dadd[18] = (uintptr_t)&E01C6Dallied_spectators;
	E01C6Dadd[19] = (uintptr_t)&E01C6Dchargerifle;
	E01C6Dadd[20] = (uintptr_t)&E01C6Dshooting;
	E01C6Dadd[21] = (uintptr_t)&E01C6Dfreecam;
	E01C6Dadd[22] = (uintptr_t)&E01C6Dlockall_mode;
	E01C6Dadd[23] = (uintptr_t)&E01C6Dfiring_range;
	E01C6Dadd[24] = (uintptr_t)&E01C6Drcs_pitch;
	E01C6Dadd[25] = (uintptr_t)&E01C6Drcs_yaw;
	E01C6Dadd[26] = (uintptr_t)&E01C6Drefresh;
	E01C6Dadd[27] = (uintptr_t)&PE01C6Dlist[0];

	printf(XorStr("offset: 0x%I64x\n"), (uint64_t)&E01C6Dadd[0] - (uint64_t)GetModuleHandle(NULL));
	Overlay ov1E01C6D = Overlay();
	ov1E01C6D.StartE01C6D();
	printf(XorStr("Waiting for host/apex...\n"));

	while (E01C6Dcheck == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			E01C6Dactive = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (E01C6Dactive)
	{
		E01C6Dready = true;
		printf(XorStr("Successful\n"));
		ranE01C6Dk_process_thr = std::thread(ProcessE01C6DRank);
		ranE01C6Dk_process_thr.detach();
	}

	while (E01C6Dactive)
	{
		if (IsKeyDown(VK_F4))
		{
			E01C6Dactive = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (IsKeyDown(E01C6Daim_key))
			E01C6Daiming = true;
		else
			E01C6Daiming = false;
		if (E01C6Daim > 0) {
			if (IsKeyDown(E01C6Daim_key2)) {
				E01C6Daim = 1;
			}
			else {
				E01C6Daim = 2;
			}
		}
	}
	E01C6Dready = false;
	ov1E01C6D.ClearE01C6D();
	if (!E01C6Duse_nvidia)
		system(XorStr("taskkill /F /T /IM ov.exe")); //custom overlay processname
	return 0;
}
