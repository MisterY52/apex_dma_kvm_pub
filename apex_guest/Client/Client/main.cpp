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
	//seer
	int maxshield = 0;
	int armortype = 0;
	D3DXVECTOR3 EntityPosition;
	D3DXVECTOR3 LocalPlayerPosition;
	D3DXVECTOR3 localviewangle;
	char name[33] = { 0 };
}player;


uint32_t check = 0xABCD;

//Aiming keys: left and right mouse button
int aim_key = VK_LBUTTON; //Left Click
int aim_key2 = VK_RBUTTON; //Right Click
int shoot_key = VK_LBUTTON; //Left Click
int shoot_key2 = VK_RBUTTON; //Right Click
//Left and Right Aim key toggle
bool toggleaim = false;
bool toggleaim2 = false;
extern int e;
//World to Screen res
int lengthws = 1920;
int widthws = 1920;
extern int wss;
bool firing_range = false;
bool use_nvidia = true; //Nvidia Shadowplay Overlay
bool active = true;
bool ready = false;
extern visuals v;
int aim = 2; //read
bool esp = true; //read
bool item_glow = true;
bool player_glow = true;
bool aim_no_recoil = true;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 3800.0f * 40.0f; //read //Max Distance of ESP 3800 is full map
float smooth = 110.0f; //Min 100 for safe aimbotting
float max_fov = 15.0f; //15 is the sweetspot for 1080p
int bone = 2; //0 Head, 1 Neck, 2 Body, 3 Stomace, 4 Nuts
//Player Glow Color and Brightness
float glowr = 120.0f; //Red Value
float glowg = 0.0f; //Green Value
float glowb = 0.0f; //Blue Value
float glowcolor[3] = { 000.0f, 000.0f, 000.0f };
//Radar multi res
int wstimesx = 0;
int wstimesy = 0;
float fwstimesx = 170.0f;
float fwstimesy = 170.0f;
//MiniMap Radar
int minimapposx = 0;
int minimapposy = 0;
extern int minimapradardotsize1;
extern int minimapradardotsize2;
bool minimapradar = false;
extern unsigned int radarcolorr; //Red Value
extern unsigned int radarcolorg; //Green Value
extern unsigned int radarcolorb; //Blue Value
float radarcolor[3];
//Full Map Radar
bool mainradartoggle = 0; //Toggle for Main Map radar
bool mainradarmap = false; //if the Main Map Radar is enabled
bool kingscanyon = false; //Set for map, ONLY ONE THO
bool stormpoint = true; //Set for map, ONLY ONE THO
extern int mainmapradardotsize1;
extern int mainmapradardotsize2;
//Ha think i was done ?
//Item Filter Brute Force!
bool lightbackpack = false;
bool medbackpack = false;
bool heavybackpack = false;
bool shieldupgrade = false;
bool shieldupgradehead = false;
bool accelerant = false;
bool phoenix = false;
bool healthlarge = false;
bool healthsmall = false;
bool shieldbattsmall = false;
bool shieldbattlarge = false;
bool ammosniper = false;
bool ammohc = false;
bool optic = false;
bool ammosc = false;
bool ammonrg = false;
bool ammoshotgun = false;
bool lasersight = false;
bool magsniper = false;
bool magenergy = false;
bool stocksniper = false;
bool stockregular = false;
bool shielddown = false;
bool lightammomag = false;
bool heavyammomag = false;
bool optic2x = false;
bool opticholo1x = false;
bool opticsniper6x = false;
bool opticsniper4x8x = false;
bool opticsniperthreat = false;
bool optic2x4x = false;
bool opticthreat = false;
bool optic3x = false;
bool opticholo1x2x = false;
bool suppressor = false;
bool weaponmod = false;
bool grenade_frag = false;
bool grenade_arc_star = false;
bool grenade_thermite = false;
bool shotgunbolt = false;
bool weapon_kraber = false;
bool weapon_mastiff = false;
bool weapon_lstar = false;
bool weapon_havoc = false;
bool weapon_devotion = false;
bool weapon_triple_take = false;
bool weapon_flatline = false;
bool weapon_hemlock = false;
bool weapon_g7_scout = false;
bool weapon_alternator = false;
bool weapon_r99 = false;
bool weapon_prowler = false;
bool weapon_volt = false;
bool weapon_longbow = false;
bool weapon_charge_rifle = false;
bool weapon_spitfire = false;
bool weapon_r301 = false;
bool weapon_eva8 = false;
bool weapon_peacekeeper = false;
bool weapon_mozambique = false;
bool weapon_wingman = false;
bool weapon_p2020 = false;
bool weapon_re45 = false;
bool weapon_sentinel = false;
bool weapon_bow = false;
bool weapon_3030_repeater = false;
bool weapon_rampage = false;
bool weapon_car_smg = false;
// Aim distance check
float aimdist = 9905.0f;
//item glow brightness
int itemglowbrightness = 10;


bool thirdperson = false;
int spectators = 0; //write
int allied_spectators = 0; //write
bool valid = true; //write
bool next2 = true; //read write

uint64_t add[97];

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f7 = 0;
bool k_f8 = 0;
bool k_f9 = 0;
bool k_f10 = 0;


bool k_f20 = 0;

bool k_f100 = 0;

player players[100];

//Radar Code
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

static D3DXVECTOR3 RotatePoint(D3DXVECTOR3 EntityPos, D3DXVECTOR3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return D3DXVECTOR3(x_1, y_1, 0);
}
//Main Map Radar Color
typedef struct
{

	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;
static void FilledRectangle(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}

//Color Team Radar Test. oh god why... This is stupid.. dont do this.. it works tho
static void Team1(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team2(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team3(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team4(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team5(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team6(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team7(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team8(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team9(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team10(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team11(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team12(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team13(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team14(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team15(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team16(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team17(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team18(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team19(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}
static void Team20(int x, int y, int w, int h, RGBA color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 0, 0);
}

bool menu = true;
bool firstS = true;
//Radar Settings.. ToDO: Put in ImGui menu to change in game
namespace RadarSettings
{
	bool Radar = true;
	bool teamRadar = true;
	bool enemyRadar = true;
	int xAxis_Radar = 0;
	int yAxis_Radar = 400;
	int radartype = 0;
	int width_Radar = 400;
	int height_Radar = 400;
	int distance_Radar = 250;
	int distance_Radar2 = 1000;
};

void DrawRadarPoint(D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist, int TeamID, int xAxis, int yAxis, int width, int height, D3DXCOLOR color)
{
	bool out = false;
	D3DXVECTOR3 siz;
	siz.x = width;
	siz.y = height;
	D3DXVECTOR3 pos;
	pos.x = xAxis;
	pos.y = yAxis;
	bool ck = false;

	D3DXVECTOR3 single = RotatePoint(EneamyPos, LocalPos, pos.x, pos.y, siz.x, siz.y, LocalPlayerY, 0.3f, &ck);
	if (eneamyDist >= 0.f && eneamyDist < RadarSettings::distance_Radar)
	{
			if (TeamID == 1)
			{
				Team1(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 255, 255, 255, 255 });
			}
			if (TeamID == 2)
			{
				Team2(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 242, 86, 38, 255 });
			}
			if (TeamID == 3)
			{
				Team3(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 242, 86, 38, 255 });
			}
			if (TeamID == 4)
			{
				Team4(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 174, 247, 89, 255 });
			}
			if (TeamID == 5)
			{
				Team5(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 102, 214, 173, 255 });
			}
			if (TeamID == 6)
			{
				Team6(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 98, 244, 234, 255 });
			}
			if (TeamID == 7)
			{
				Team7(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 92, 208, 250, 255 });
			}
			if (TeamID == 8)
			{
				Team8(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 93, 137, 238, 255 });
			}
			if (TeamID == 9)
			{
				Team9(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 164, 105, 252, 255 });
			}
			if (TeamID == 10)
			{
				Team10(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 243, 98, 161, 255 });
			}
			if (TeamID == 11)
			{
				Team11(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 214, 67, 67, 255 });
			}
			if (TeamID == 12)
			{
				Team12(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 230, 116, 51, 255 });
			}
			if (TeamID == 13)
			{
				Team13(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 185, 179, 167, 255 });
			}
			if (TeamID == 14)
			{
				Team14(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 148, 200, 65, 255 });
			}
			if (TeamID == 15)
			{
				Team15(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 86, 174, 91, 255 });
			}
			if (TeamID == 16)
			{
				Team16(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 55, 188, 200, 255 });
			}
			if (TeamID == 17)
			{
				Team17(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 84, 169, 212, 255 });
			}
			if (TeamID == 18)
			{
				Team18(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 98, 121, 203, 255 });
			}
			if (TeamID == 19)
			{
				Team19(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 102, 61, 174, 255 });
			}
			if (TeamID == 20)
			{
				Team20(single.x, single.y, minimapradardotsize1, minimapradardotsize2, { 218, 73, 145, 255 });
			}
	}
}
//MiniMap Radar Stuff
void MiniMapRadar(D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist, int TeamId)
{
	
	ImGuiWindowFlags TargetFlags;
	//Radar Window Flags: No Move, Resize, Title bar, Background etc. makes it so you can change it once set.

	//slash out  | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove to move the minimap
	TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;

	
	if (RadarSettings::Radar == true)
	{
		ImGui::SetNextWindowPos(ImVec2{ fwstimesx - 125, fwstimesy - 125});
		ImGui::SetNextWindowSize({ 250, 250 });
		ImGui::Begin(("Radar"), 0, TargetFlags);
		//if (ImGui::Begin(xorstr("Radar", 0, ImVec2(200, 200), -1.f, TargetFlags))) {
		{
			ImDrawList* Draw = ImGui::GetWindowDrawList();
			ImVec2 DrawPos = ImGui::GetCursorScreenPos();
			ImVec2 DrawSize = ImGui::GetContentRegionAvail();
			ImVec2 midRadar = ImVec2(DrawPos.x + (DrawSize.x / 2), DrawPos.y + (DrawSize.y / 2));

			//unslash to set to minimap, it helps line it up
			//ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x - DrawSize.x / 2.f, midRadar.y), ImVec2(midRadar.x + DrawSize.x / 2.f, midRadar.y), IM_COL32(255, 255, 255, 255));
			//ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x, midRadar.y - DrawSize.y / 2.f), ImVec2(midRadar.x, midRadar.y + DrawSize.y / 2.f), IM_COL32(255, 255, 255, 255));

			DrawRadarPoint(EneamyPos, LocalPos, LocalPlayerY, eneamyDist, TeamId, DrawPos.x, DrawPos.y, DrawSize.x, DrawSize.y, { 255, 255, 255, 255 });
		}
		ImGui::End();
	}
	
}
bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}




//Full map radar test, Needs Manual setting of cords
//ImVec2 can be replaced with Vector2D
class world {
public:
	ImVec2 w1; //origin of point 1
	ImVec2 w2; //origin of point 2
	ImVec2 s1; //screen coord of point 1
	ImVec2 s2; //screen coord of point 2
	float ratioX;
	float ratioY;
	world(ImVec2 w1, ImVec2 s1, ImVec2 w2, ImVec2 s2) {
		this->w1 = w1;
		this->w2 = w2;
		this->s1 = s1;
		this->s2 = s2;
		this->ratioX = (s2.x - s1.x) / (w2.x - w1.x);
		this->ratioY = (s1.y - s2.y) / (w2.y - w1.y);
	}
};
//These values only work with 1920x1080 fullscreen, you have to redo the values for anything else.
// 
// Take screenshot, First is top right random pos, then bttm left random pos from screen shot
// 
// First set is the x cord, then the y cord, then the screen pos from the screenshot, do the same for the second set.
//Battel Royal
//Screen res multi
world KingsCanyon(ImVec2(25223.177734, 28906.144531), ImVec2(1197 * wstimesx, 185 * wstimesy), ImVec2(10399.223633, 13334.792969), ImVec2(1014 * wstimesx, 381 * wstimesy)); //could be more accurate 
world WorldsEdge(ImVec2(-9190.608398, 8443.554688), ImVec2(824, 412), ImVec2(-19529.794922, -8933.173828), ImVec2(707, 608));
world Olympus(ImVec2(0, 0), ImVec2(0, 0), ImVec2(0, 0), ImVec2(0, 0)); //to be measured
world StormPoint(ImVec2(-21264.427734, -47086.878906), ImVec2(711 * wstimesx, 983 * wstimesy), ImVec2(40298.070313, 21163.728516), ImVec2(1321 * wstimesx, 306 * wstimesy));
//Arena
world Overflow(ImVec2(-3344.994629, -4018.093018), ImVec2(552, 431), ImVec2(5039.592773, -4639.289063), ImVec2(1322, 489));
world DropOff(ImVec2(3135.113281, 1654.107666), ImVec2(1151, 603), ImVec2(-2920.918701, 811.240479), ImVec2(722, 663));
world Habitat4(ImVec2(4482.470215, -604.362854), ImVec2(1205, 544), ImVec2(-4464.019043, 593.067688), ImVec2(650, 470));
world Encore(ImVec2(4144.926270, 468.957611), ImVec2(1184, 472), ImVec2(-3791.070313, 3.092307), ImVec2(692, 501));
world PartyCrasher(ImVec2(-3275.972900, 3646.970703), ImVec2(589, 197), ImVec2(1085.708740, -3869.658936), ImVec2(1022, 943));
//TODO get map auto 
ImVec2 worldToScreenMap(D3DXVECTOR3 origin, int TeamID) {
		float ratioX;
		float ratioY;
		ImVec2 w1;
		ImVec2 s1;
		//Is it me being lazy? or that i dont know how? prob both. True or False for the map detection, set in the overlay menu.
		/*if (kingscanyon == true) { //KingsCanyon
			ratioX = KingsCanyon.ratioX;
			ratioY = KingsCanyon.ratioY;
			w1 = KingsCanyon.w1;
			s1 = KingsCanyon.s1;
		}
		*/
		if (stormpoint == true) { //Storm Point
			ratioX = StormPoint.ratioX;
			ratioY = StormPoint.ratioY;
			w1 = StormPoint.w1;
			s1 = StormPoint.s1;
		}
		
		else {
			return ImVec2(0, 0);
		}

		//difference from location 1
		float world_diff_x = origin.x - w1.x;
		float world_diff_y = origin.y - w1.y;

		//get the screen offsets by applying the ratio
		float scr_diff_x = world_diff_x * ratioX;
		float scr_diff_y = world_diff_y * ratioY;

		//for x, add the offset to the screen x of location 1
		//for y, subtract the offset from the screen y of location 1 (cuz Y is from bottom to up in Apex but it's from up to bottom in screen)
		float pos_x = s1.x + scr_diff_x;
		float pos_y = s1.y - scr_diff_y;

		if (TeamID == 1)
		{
			Team1(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 255, 255, 255, 255 });
		}
		if (TeamID == 2)
		{
			Team2(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 242, 86, 38, 255 });
		}
		if (TeamID == 3)
		{
			Team3(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 242, 86, 38, 255 });
		}
		if (TeamID == 4)
		{
			Team4(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 174, 247, 89, 255 });
		}
		if (TeamID == 5)
		{
			Team5(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 102, 214, 173, 255 });
		}
		if (TeamID == 6)
		{
			Team6(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 98, 244, 234, 255 });
		}
		if (TeamID == 7)
		{
			Team7(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 92, 208, 250, 255 });
		}
		if (TeamID == 8)
		{
			Team8(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 93, 137, 238, 255 });
		}
		if (TeamID == 9)
		{
			Team9(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 164, 105, 252, 255 });
		}
		if (TeamID == 10)
		{
			Team10(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 243, 98, 161, 255 });
		}
		if (TeamID == 11)
		{
			Team11(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 214, 67, 67, 255 });
		}
		if (TeamID == 12)
		{
			Team12(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 230, 116, 51, 255 });
		}
		if (TeamID == 13)
		{
			Team13(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 185, 179, 167, 255 });
		}
		if (TeamID == 14)
		{
			Team14(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 148, 200, 65, 255 });
		}
		if (TeamID == 15)
		{
			Team15(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 86, 174, 91, 255 });
		}
		if (TeamID == 16)
		{
			Team16(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 55, 188, 200, 255 });
		}
		if (TeamID == 17)
		{
			Team17(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 84, 169, 212, 255 });
		}
		if (TeamID == 18)
		{
			Team18(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 98, 121, 203, 255 });
		}
		if (TeamID == 19)
		{
			Team19(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 102, 61, 174, 255 });
		}
		if (TeamID == 20)
		{
			Team20(pos_x, pos_y, mainmapradardotsize1, mainmapradardotsize2, { 218, 73, 145, 255 });
		}
		
	
}

void Overlay::RenderEsp()
{
	next2 = false;
	if (g_Base != 0 && esp)
	{
		
		memset(players, 0, sizeof(players));
				
		while (!next2 && esp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (next2 && valid)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{
				
				if (players[i].health > 0)
				{
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + "m(" + std::to_string(players[i].entity_team) + ")";

					float radardistance = (int)((players[i].LocalPlayerPosition, players[i].dist) / 39.62);
					//Radar Stuff
					if (minimapradar == true)
					{
							MiniMapRadar(players[i].EntityPosition, players[i].LocalPlayerPosition, players[i].localviewangle.y, radardistance, players[i].entity_team);
					}
					if (v.line)
						DrawLine(ImVec2((float)(getWidth() / 2), (float)getHeight()), ImVec2(players[i].b_x, players[i].b_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (v.distance)
					{
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), RED, distance.c_str());  //DISTANCEs			else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), GREEN, distance.c_str());  //DISTANCE
					}
					if (v.healthbar)
						if (players[i].dist < 16000.0f)
						{

							DrawSeerLikeHealth((players[i].b_x - (players[i].width / 2.0f) + 5), (players[i].b_y - players[i].height - 10), players[i].shield, players[i].maxshield, players[i].armortype, players[i].health); //health bar					
						}
					//Full Radar map, Need Manual setting of cords
					if (mainradarmap == true)

						worldToScreenMap(players[i].EntityPosition, players[i].entity_team);

						//String(ImVec2(players[i].boxMiddle, (players[i].b_y - players[i].height - 15)), WHITE, players[i].name);
				}
			}
			ImGui::End();
		}
	}
}

int main(int argc, char** argv)
{
	add[0] = (uintptr_t)&check;
	add[1] = (uintptr_t)&aim;
	add[2] = (uintptr_t)&esp;
	add[3] = (uintptr_t)&aiming;
	add[4] = (uintptr_t)&g_Base;
	add[5] = (uintptr_t)&next2;
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
	add[20] = (uintptr_t)&glowr;
	add[21] = (uintptr_t)&glowg;
	add[22] = (uintptr_t)&glowb;
	add[23] = (uintptr_t)&firing_range;
	add[24] = (uintptr_t)&lightbackpack;
	add[25] = (uintptr_t)&medbackpack;
	add[26] = (uintptr_t)&heavybackpack;
	add[27] = (uintptr_t)&shieldupgrade;
	add[28] = (uintptr_t)&shieldupgradehead;
	add[29] = (uintptr_t)&accelerant;
	add[30] = (uintptr_t)&phoenix;
	add[31] = (uintptr_t)&healthlarge;
	add[32] = (uintptr_t)&healthsmall;
	add[33] = (uintptr_t)&shieldbattsmall;
	add[34] = (uintptr_t)&shieldbattlarge;
	add[35] = (uintptr_t)&ammosniper;
	add[36] = (uintptr_t)&ammohc;
	add[37] = (uintptr_t)&optic;
	add[38] = (uintptr_t)&ammosc;
	add[39] = (uintptr_t)&ammonrg;
	add[40] = (uintptr_t)&ammoshotgun;
	add[41] = (uintptr_t)&lasersight;
	add[42] = (uintptr_t)&magsniper;
	add[43] = (uintptr_t)&magenergy;
	add[44] = (uintptr_t)&stocksniper;
	add[45] = (uintptr_t)&stockregular;
	add[46] = (uintptr_t)&shielddown;
	add[47] = (uintptr_t)&lightammomag;
	add[48] = (uintptr_t)&heavyammomag;
	add[49] = (uintptr_t)&optic2x;
	add[50] = (uintptr_t)&opticholo1x;
	add[51] = (uintptr_t)&opticholo1x2x;
	add[52] = (uintptr_t)&opticthreat;
	add[53] = (uintptr_t)&optic3x;
	add[54] = (uintptr_t)&optic2x4x;
	add[55] = (uintptr_t)&opticsniper6x;
	add[56] = (uintptr_t)&opticsniper4x8x;
	add[57] = (uintptr_t)&opticsniperthreat;
	add[58] = (uintptr_t)&suppressor;
	add[59] = (uintptr_t)&weaponmod;
	add[60] = (uintptr_t)&grenade_frag;
	add[61] = (uintptr_t)&grenade_arc_star;
	add[62] = (uintptr_t)&grenade_thermite;
	add[63] = (uintptr_t)&shotgunbolt;
	add[64] = (uintptr_t)&weapon_kraber;
	add[65] = (uintptr_t)&weapon_mastiff;
	add[66] = (uintptr_t)&weapon_lstar;
	add[67] = (uintptr_t)&weapon_havoc;
	add[68] = (uintptr_t)&weapon_devotion;
	add[69] = (uintptr_t)&weapon_triple_take;
	add[70] = (uintptr_t)&weapon_flatline;
	add[71] = (uintptr_t)&weapon_hemlock;
	add[72] = (uintptr_t)&weapon_g7_scout;
	add[73] = (uintptr_t)&weapon_alternator;
	add[74] = (uintptr_t)&weapon_r99;
	add[75] = (uintptr_t)&weapon_prowler;
	add[76] = (uintptr_t)&weapon_volt;
	add[77] = (uintptr_t)&weapon_longbow;
	add[78] = (uintptr_t)&weapon_charge_rifle;
	add[79] = (uintptr_t)&weapon_spitfire;
	add[80] = (uintptr_t)&weapon_r301;
	add[81] = (uintptr_t)&weapon_eva8;
	add[82] = (uintptr_t)&weapon_peacekeeper;
	add[83] = (uintptr_t)&weapon_mozambique;
	add[84] = (uintptr_t)&weapon_wingman;
	add[85] = (uintptr_t)&weapon_p2020;
	add[86] = (uintptr_t)&weapon_re45;
	add[87] = (uintptr_t)&weapon_sentinel;
	add[88] = (uintptr_t)&weapon_bow;
	add[89] = (uintptr_t)&weapon_3030_repeater;
	add[90] = (uintptr_t)&weapon_rampage;
	add[91] = (uintptr_t)&weapon_car_smg;
	add[92] = (uintptr_t)&lengthws;
	add[93] = (uintptr_t)&widthws;
	add[94] = (uintptr_t)&aimdist;
	add[95] = (uintptr_t)&itemglowbrightness
		;



	
	printf(XorStr("Game Version v3.0.12.19 |-| Main Map Radar Test |-| Add me offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));

	Overlay ov1 = Overlay();
	ov1.Start();
	printf(XorStr("Waiting for The Ban .... Never Gonna Get it!\n"));
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
		printf(XorStr("Ready To Bring The Cure\n"));
	}

	while (active)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}

		//Load at start for saved settings to take effect. Need to save once to make the file. 

		for (static bool once = true; once; once = false) {
			std::ifstream config("Settings.txt");
			if (config.is_open())
			{
				config >> std::boolalpha >> firing_range;
				config >> aim;
				config >> std::boolalpha >> esp;
				config >> std::boolalpha >> item_glow;
				config >> std::boolalpha >> player_glow;
				config >> std::boolalpha >> aim_no_recoil;
				config >> max_dist;
				config >> smooth;
				config >> max_fov;
				config >> bone;
				config >> glowr;
				config >> glowg;
				config >> glowb;
				config >> glowcolor[0];
				config >> glowcolor[1];
				config >> glowcolor[2];
				config >> radarcolorr;
				config >> radarcolorg;
				config >> radarcolorb;
				config >> radarcolor[0];
				config >> radarcolor[1];
				config >> radarcolor[2];
				config >> v.healthbar;
				config >> v.shieldbar;
				config >> v.distance;
				config >> thirdperson;
				config >> minimapradar;
				config >> lightbackpack;
				config >> medbackpack;
				config >> heavybackpack;
				config >> shieldupgrade;
				config >> shieldupgradehead;
				config >> accelerant;
				config >> phoenix;
				config >> healthlarge;
				config >> healthsmall;
				config >> shieldbattsmall;
				config >> shieldbattlarge;
				config >> ammosniper;
				config >> ammohc;
				config >> optic;
				config >> ammosc;;
				config >> ammonrg;
				config >> ammoshotgun;
				config >> lasersight;;
				config >> magsniper;
				config >> magenergy;
				config >> stocksniper;
				config >> stockregular;
				config >> shielddown;
				config >> lightammomag;
				config >> heavyammomag;
				config >> optic2x;
				config >> opticholo1x;
				config >> opticsniper6x;
				config >> opticsniper4x8x;
				config >> opticsniperthreat;
				config >> optic2x4x;
				config >> opticthreat;
				config >> optic3x;
				config >> opticholo1x2x;

				config >> suppressor;
				config >> weaponmod;
				config >> grenade_frag;
				config >> grenade_arc_star;
				config >> grenade_thermite;
				config >> shotgunbolt;
				config >> weapon_kraber;
				config >> weapon_mastiff;
				config >> weapon_lstar;
				config >> weapon_havoc;
				config >> weapon_devotion;
				config >> weapon_triple_take;
				config >> weapon_flatline;
				config >> weapon_hemlock;
				config >> weapon_g7_scout;
				config >> weapon_alternator;
				config >> weapon_r99;
				config >> weapon_prowler;
				config >> weapon_volt;
				config >> weapon_longbow;
				config >> weapon_charge_rifle;
				config >> weapon_spitfire;
				config >> weapon_r301;
				config >> weapon_eva8;
				config >> weapon_peacekeeper;
				config >> weapon_mozambique;
				config >> weapon_wingman;
				config >> weapon_p2020;
				config >> weapon_re45;
				config >> weapon_sentinel;
				config >> weapon_bow;
				config >> weapon_3030_repeater;
				config >> weapon_rampage;
				config >> weapon_car_smg;
				config >> toggleaim;
				config >> toggleaim2;
				config >> e;
				config >> lengthws;
				config >> widthws;
				config >> wss;
				config >> wstimesx;
				config >> wstimesy;
				config >> minimapradardotsize1;
				config >> minimapradardotsize2;
				config >> aimdist;
				config >> itemglowbrightness;
				config.close();
			}
		}
		//Hotkey to Turn on and off Aimbot
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
		//Main Map Radar, Needs Manual Setting of cords
		if (IsKeyDown(0x4D) && mainradartoggle == 0)
		{
			mainradartoggle = 1;
			switch (mainradarmap)
			{
			case 0:
				mainradarmap = true;
				minimapradar = false;
				break;
			case 1:
				mainradarmap = false;
				minimapradar = true;
				break;
			default:
				break;
			}
		}
		else if (!IsKeyDown(0x4D) && mainradartoggle == 1)
		{
			mainradartoggle = 0;
		}
		
		if (IsKeyDown(aim_key) && toggleaim)
		{
			aiming = true;
		}

		else if (IsKeyDown(aim_key2) && toggleaim2)
			aiming = true;
		else
		{
			aiming = false;
		}
		
	}
	ready = false;
	ov1.Clear();
	if (!use_nvidia)
		system(XorStr("taskkill /F /T /IM Nvspcaps64.exe")); //custom overlay process name
	return 0;
}



