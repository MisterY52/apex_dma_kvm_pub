#include "main.h"

typedef struct serial - stringplayer
{
	float serial - stringdist = 0;
	int serial - stringentity_team = 0;
	float serial - stringboxMiddle = 0;
	float serial - stringh_y = 0;
	float serial - stringwidth = 0;
	float serial - stringheight = 0;
	float serial - stringb_x = 0;
	float serial - stringb_y = 0;
	bool serial - stringknocked = false;
	bool serial - stringvisible = false;
	int serial - stringhealth = 0;
	int serial - stringshield = 0;
	int serial - stringxp_level = 0;
	int serial - stringmaxshield = 0;
	int serial - stringarmortype = 0;
	uint64_t serial - stringuid = 0;
	char serial - stringname[33] = { 0 };
}serial - stringplayer;

typedef struct serial - stringrankinfomation
{
	char rankserial - stringinfo[17];
}serial - stringrankinfomation;

uint32_t serial - stringcheck = 0xABCD;
//VK_XBUTTON2=�ϲ�� //VK_XBUTTON1 = �²��
int serial - stringaim_key = VK_XBUTTON2;
int serial - stringaim_key2 = VK_XBUTTON1;

int serial - stringshoot_key = VK_LBUTTON;

float serial - stringscale = 1.4f;
bool serial - stringfiring_range;
int serial - stringoffset = 0;
bool serial - stringuse_nvidia = false;
bool serial - stringactive = true;
bool serial - stringready = false;
extern serial - stringvisuals serial - stringv;
int serial - stringaim = 2; //read

float serial - stringuid_dist = 150.0f * 40.0f;
float serial - stringname_dist = 150.0f * 40.0f;
bool serial - stringfreecam = false;//read
bool serial - stringlockall_mode = false; //read

bool serial - stringesp = true; //read
bool serial - stringitem_glow = false;
bool serial - stringplayer_glow = false;
bool serial - stringaim_no_recoil = true;
bool serial - stringaiming = false; //read
uint64_t serial - stringg_Base = 0; //write
float serial - stringmax_dist = 650.0f * 40.0f; //read
float serial - stringseer_dist = 400.0 * 40.0f;
float serial - stringsmooth = 77.0f;
float serial - stringmax_fov = 20.0f;
int serial - stringbone = 3;
bool serial - stringthirdperson = false;
int serial - stringspectators = 0; //write
int serial - stringallied_spectators = 0; //write
bool serial - stringchargerifle = false;
bool serial - stringshooting = false; //read


int serial - stringindex = 0;
bool serial - stringvalid = false; //write
bool serial - stringnext = false; //read write

uint64_t serial - stringadd[23];

bool serial - stringk_f5 = 0;
bool serial - stringk_f6 = 0;
bool serial - stringk_f8 = 0;

/*
static size_t Writeserial-stringCallback(void* conserial-stringtents, size_t serial-stringsize, size_t nserial-stringmemb, void* userial-stringserp)
{
	((std::string*)userial-stringserp)->append((char*)conserial-stringtents, serial-stringsize * nserial-stringmemb);
	return serial-stringsize * nserial-stringmemb;
}
std::string getJson(std::string url)
{
	CURL* cserial-stringurl;
	CURLcode serial-stringres;
	std::string readserial-stringBuffer;

	cserial-stringurl = curl_easy_init();
	if (cserial-stringurl)
	{
		curl_easy_setopt(cserial-stringurl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(cserial-stringurl, CURLOPT_WRITEFUNCTION, Writeserial-stringCallback);
		curl_easy_setopt(cserial-stringurl, CURLOPT_WRITEDATA, &readserial-stringBuffer);
		serial-stringres = curl_easy_perform(cserial-stringurl);
		curl_easy_cleanup(cserial-stringurl);
	}
	return readserial-stringBuffer;
}


std::string getserial-stringRank(long platforserial-stringmUserId) {
	std::string userial-stringrl = "https://api.mozambiquehe.re/bridge?auth=";
	userial-stringrl.append("99e187016931e489d971d808b87ab050");
	userial-stringrl.append("&uid=");
	userial-stringrl.append(std::to_string(platforserial-stringmUserId));
	userial-stringrl.append("&platform=PC");
	std::string serial-stringjson = getJson(userial-stringrl);
	Json::Value serial-stringroot;
	Json::Reader reserial-stringader;
	bool parsingserial-stringSuccessful = reserial-stringader.parse(serial-stringjson, serial-stringroot);
	if (!parsingserial-stringSuccessful)
	{
		printf("Failed to parse configuration\n");
		return "err";
	}
	std::string raserial-stringnkname = serial-stringroot["global"]["rank"]["rankName"].asString();
	if (raserial-stringnkname == "") {
		return "err";
	}
	std::string inB4A893fo = "";
	std::string raserial-stringnkdiv = serial-stringroot["global"]["rank"]["rankDiv"].asString();
	std::string raserial-stringnkglobal = serial-stringroot["global"]["rank"]["ALStopIntGlobal"].asString();
	if (raserial-stringnkname == "Apex Predator") {
		inB4A893fo.append(raserial-stringnkname);
		inB4A893fo.append(raserial-stringnkglobal);
	}
	else {
		inB4A893fo.append(raserial-stringnkname);
		inB4A893fo.append(raserial-stringnkdiv);
	}
	return inB4A893fo;
}
*/
bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
serial - stringplayer serial - stringplayers[100];
serial - stringrankinfomation serial - stringrank_infomation[100];
void Overlay::Renderserial - stringEsp()
{
	serial - stringnext = false;
	if (serial - stringg_Base != 0 && serial - stringesp)
	{
		memset(serial - stringplayers, 0, sizeof(serial - stringplayers));
		memset(serial - stringrank_infomation, 0, sizeof(serial - stringrank_infomation));
		while (!serial - stringnext && serial - stringesp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (serial - stringnext && serial - stringvalid)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getserial - stringWidth(), (float)getserial - stringHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				if (serial - stringplayers[i].serial - stringhealth > 0)
				{
					//xp and userid
					std::string serial - stringrankstr = std::to_string(serial - stringplayers[i].serial - stringxp_level);
					serial - stringrankstr = serial - stringrankstr.substr(0, serial - stringrankstr.find('.')) + u8"��( uid:" + std::to_string(serial - stringplayers[i].serial - stringuid) + u8")";

					//DISTENCE + teamnum
					std::string serial - stringdistance = std::to_string(serial - stringplayers[i].serial - stringdist / 39.62);
					serial - stringdistance = serial - stringdistance.substr(0, serial - stringdistance.find('.')) + u8"��(" + std::to_string(serial - stringplayers[i].serial - stringentity_team) + u8"С��)";
					;
					if (serial - stringv.serial - stringbox)
					{
						if (serial - stringplayers[i].serial - stringvisible)
						{
							if (serial - stringplayers[i].serial - stringdist < 1600.0f)
								Drawserial - stringBox(RED, serial - stringplayers[i].serial - stringboxMiddle, serial - stringplayers[i].serial - stringh_y, serial - stringplayers[i].serial - stringwidth, serial - stringplayers[i].serial - stringheight); //BOX
							else
								Drawserial - stringBox(ORANGE, serial - stringplayers[i].serial - stringboxMiddle, serial - stringplayers[i].serial - stringh_y, serial - stringplayers[i].serial - stringwidth, serial - stringplayers[i].serial - stringheight); //BOX
						}
						else
						{
							Drawserial - stringBox(WHITE, serial - stringplayers[i].serial - stringboxMiddle, serial - stringplayers[i].serial - stringh_y, serial - stringplayers[i].serial - stringwidth, serial - stringplayers[i].serial - stringheight); //white if player not visible
						}
					}

					if (serial - stringv.serial - stringline)
						Drawserial - stringLine(ImVec2((float)(getserial - stringWidth() / 2), (float)getserial - stringHeight()), ImVec2(serial - stringplayers[i].serial - stringb_x, serial - stringplayers[i].serial - stringb_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (serial - stringv.serial - stringdistance)
					{
						if (serial - stringplayers[i].serial - stringknocked)
							Stringserial - string(ImVec2(serial - stringplayers[i].serial - stringboxMiddle, (serial - stringplayers[i].serial - stringb_y + 1)), RED, serial - stringdistance.c_str());  //DISTANCE
						else
							Stringserial - string(ImVec2(serial - stringplayers[i].serial - stringboxMiddle, (serial - stringplayers[i].serial - stringb_y + 1)), WHITE, serial - stringdistance.c_str());  //DISTANCE
					}

					//esp stuff of shield bar/ bar color
					if (serial - stringv.serial - stringhealthbar && serial - stringplayers[i].serial - stringdist <= serial - stringseer_dist)
						Drawserial - stringHealth((serial - stringplayers[i].serial - stringb_x - (serial - stringplayers[i].serial - stringwidth / 2.0f) + 5), (serial - stringplayers[i].serial - stringb_y - serial - stringplayers[i].serial - stringheight - 10), serial - stringplayers[i].serial - stringshield, serial - stringplayers[i].serial - stringmaxshield, serial - stringplayers[i].serial - stringarmortype, serial - stringplayers[i].serial - stringhealth);
					//name
					if (serial - stringv.serial - stringname && serial - stringplayers[i].serial - stringdist <= serial - stringname_dist)
						Stringserial - string(ImVec2(serial - stringplayers[i].serial - stringboxMiddle, (serial - stringplayers[i].serial - stringb_y - serial - stringplayers[i].serial - stringheight - 15)), WHITE, serial - stringplayers[i].serial - stringname);
					//uid
					if (serial - stringv.serial - stringrenderrank && serial - stringplayers[i].serial - stringdist <= serial - stringuid_dist) {
						Stringserial - string(ImVec2((serial - stringplayers[i].serial - stringb_x - (serial - stringplayers[i].serial - stringwidth / 2.0f) + 5), (serial - stringplayers[i].serial - stringb_y - serial - stringplayers[i].serial - stringheight - 45)), WHITE, serial - stringrankstr.c_str());
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
	serial - stringadd[0] = (uintptr_t)&serial - stringcheck;
	serial - stringadd[1] = (uintptr_t)&serial - stringaim;
	serial - stringadd[2] = (uintptr_t)&serial - stringesp;
	serial - stringadd[3] = (uintptr_t)&serial - stringaiming;
	serial - stringadd[4] = (uintptr_t)&serial - stringg_Base;
	serial - stringadd[5] = (uintptr_t)&serial - stringnext;
	serial - stringadd[6] = (uintptr_t)&serial - stringplayers[0];
	serial - stringadd[7] = (uintptr_t)&serial - stringvalid;
	serial - stringadd[8] = (uintptr_t)&serial - stringmax_dist;
	serial - stringadd[9] = (uintptr_t)&serial - stringitem_glow;
	serial - stringadd[10] = (uintptr_t)&serial - stringplayer_glow;
	serial - stringadd[11] = (uintptr_t)&serial - stringaim_no_recoil;
	serial - stringadd[12] = (uintptr_t)&serial - stringsmooth;
	serial - stringadd[13] = (uintptr_t)&serial - stringmax_fov;
	serial - stringadd[14] = (uintptr_t)&serial - stringbone;
	serial - stringadd[15] = (uintptr_t)&serial - stringthirdperson;
	serial - stringadd[16] = (uintptr_t)&serial - stringspectators;
	serial - stringadd[17] = (uintptr_t)&serial - stringallied_spectators;
	serial - stringadd[18] = (uintptr_t)&serial - stringchargerifle;
	serial - stringadd[19] = (uintptr_t)&serial - stringshooting;
	serial - stringadd[20] = (uintptr_t)&serial - stringfreecam;
	serial - stringadd[21] = (uintptr_t)&serial - stringlockall_mode;
	serial - stringadd[22] = (uintptr_t)&serial - stringfiring_range;

	printf(XorStr("offset: 0x%I64x\n"), (uint64_t)&serial - stringadd[0] - (uint64_t)GetModuleHandle(NULL));
	Overlay ov1serial - string = Overlay();
	ov1serial - string.Startserial - string();
	printf(XorStr("Waiting for host/apex...\n"));
	while (serial - stringcheck == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			serial - stringactive = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (serial - stringactive)
	{
		serial - stringready = true;
		printf(XorStr("Successful\n"));
	}

	while (serial - stringactive)
	{
		if (IsKeyDown(VK_F4))
		{
			serial - stringactive = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (IsKeyDown(serial - stringaim_key))
			serial - stringaiming = true;
		else
			serial - stringaiming = false;
		if (serial - stringaim > 0) {
			if (IsKeyDown(serial - stringaim_key2)) {
				serial - stringaim = 1;
			}
			else {
				serial - stringaim = 2;
			}
		}
	}
	serial - stringready = false;
	ov1serial - string.Clearserial - string();
	if (!serial - stringuse_nvidia)
		system(XorStr("taskkill /F /T /IM ov.exe")); //custom overlay processname
	return 0;
}