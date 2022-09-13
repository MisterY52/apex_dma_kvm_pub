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

//chargerifle toggle to ~
int chargeriflekp = 0;

//Aiming keys: left and right mouse button
int aim_key = VK_RBUTTON;
int aim_key2 = VK_LBUTTON;
int shoot_key = VK_LBUTTON;
int shoot_key2 = VK_RBUTTON;

bool firing_range = false;
bool use_nvidia = true;
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
float max_dist = 3800.0f * 40.0f; //read
float smooth = 100.0f;
float max_fov = 15.0f;
int bone = 2;
//Player Glow Color and Brightness
float glowr = 0.0f;
float glowg = 120.0f;
float glowb = 120.0f;
float glowcolor[3] = { 000.0f, 000.0f, 000.0f };
int glowtype = 1;
int glowtype2 = 2;
//Radar Color
bool minimapradar = false;
extern unsigned int radarcolorr;
extern unsigned int radarcolorg;
extern unsigned int radarcolorb;
float radarcolor[3];
//Bigger Radar, Hotket T
bool biggerradartoggle = 0;
bool bigradar = false;
float circradarsize100 = 59.0f;
float circradarsize200 = 118.0f;
float circradarsize300 = 177.0f;
float circradarsize400 = 236.0f;
float circradarsize500 = 295.0f;

//Broken 
/*
//Full Map Radar
bool mainradartoggle = 0;
bool mainradarmap = false;
bool stormmap = true;
bool worldsedge = true;
*/
bool thirdperson = false;
int spectators = 0; //write
int allied_spectators = 0; //write

//chargerifle hack
bool chargerifle = false;
bool shooting = false; //read

bool valid = true; //write
bool next2 = true; //read write

//Fov Circle stuff, Its not working, have not finished it yet.
float fovsize = max_fov * 8.4;
float fovsize2 = max_fov * 10.7;
int zoomf1 = 0;
int zoomf2 = 0;
bool fovcircle = true;
float fovcolorset[4] = { 000.0f, 000.0f, 000.0f, 000.0f };
float fovcolor1 = 50.0f;
float fovcolor2 = 50.0f;
float fovcolor3 = 50.0f;
float fovthick = 0.0f;

uint64_t add[27];

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f7 = 0;
bool k_f8 = 0;
bool k_f9 = 0;
bool k_f10 = 0;


bool k_f20 = 0;

bool k_f100 = 0;

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

void DrawRadarPoint(D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist, int xAxis, int yAxis, int width, int height, D3DXCOLOR color)
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
		FilledRectangle(single.x, single.y, 5, 5, { radarcolorr, radarcolorg, radarcolorb, 255 });
	}
}

//Bigger Radar Stuff, Hotket T
void DrawRadarPoint2(D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist, int xAxis, int yAxis, int width, int height, D3DXCOLOR color)
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
	if (eneamyDist >= 0.f && eneamyDist < RadarSettings::distance_Radar2)
	{
		FilledRectangle(single.x, single.y, 5, 5, { radarcolorr, radarcolorg, radarcolorb, 255 });
	}
}
//MiniMap Radar Stuff
void MiniMapRadar(D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist)
{
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowRounding = 0.2f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13529413f, 0.14705884f, 0.15490198f, 0.82f));
	ImGuiWindowFlags TargetFlags;
	//Radar Window Flags: No Move, Resize, Title bar, Background etc. makes it so you can change it once set.

	//slash out  | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove to move the minimap
	TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;

	if (!firstS)
	{
		ImGui::SetNextWindowPos(ImVec2{ 1200, 60 }, ImGuiCond_Once);
		firstS = true;
	}
	if (RadarSettings::Radar == true)
	{
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

			DrawRadarPoint(EneamyPos, LocalPos, LocalPlayerY, eneamyDist, DrawPos.x, DrawPos.y, DrawSize.x, DrawSize.y, { 255, 255, 255, 255 });
		}
		ImGui::End();
	}
	ImGui::PopStyleColor();
}

//Bigger Radar Stuff, Hotket T
void BigMiniMapRadar(D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist)
{
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowRounding = 0.2f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13529413f, 0.14705884f, 0.15490198f, 0.82f));
	ImGuiWindowFlags TargetFlags;
	//Radar Window Flags: No Move, Resize, Title bar, Background etc. makes it so you can change it once set.

	//slash out  | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove
	TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar/* | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove*/;

	if (!firstS)
	{
		ImGui::SetNextWindowPos(ImVec2{ 1200, 60 }, ImGuiCond_Once);
		firstS = true;
	}
	if (RadarSettings::Radar == true)
	{
		ImGui::SetNextWindowSize({ 500, 500 });
		ImGui::Begin(("Bigger Radar"), 0, TargetFlags);
		//if (ImGui::Begin(xorstr("Radar", 0, ImVec2(200, 200), -1.f, TargetFlags))) {
		{
			ImDrawList* Draw = ImGui::GetWindowDrawList();
			ImVec2 DrawPos = ImGui::GetCursorScreenPos();
			ImVec2 DrawSize = ImGui::GetContentRegionAvail();
			ImVec2 midRadar = ImVec2(DrawPos.x + (DrawSize.x / 2), DrawPos.y + (DrawSize.y / 2));

			//unslash to set to minimap, it helps line it up
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(DrawPos.x + DrawSize.x / 2, DrawPos.y + DrawSize.y / 2), circradarsize100, IM_COL32(255, 255, 255, 255));
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(DrawPos.x + DrawSize.x / 2, DrawPos.y + DrawSize.y / 2), circradarsize200, IM_COL32(255, 255, 255, 255));
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(DrawPos.x + DrawSize.x / 2, DrawPos.y + DrawSize.y / 2), circradarsize300, IM_COL32(255, 255, 255, 255));
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(DrawPos.x + DrawSize.x / 2, DrawPos.y + DrawSize.y / 2), circradarsize400, IM_COL32(255, 255, 255, 255));
			ImGui::GetWindowDrawList()->AddCircle(ImVec2(DrawPos.x + DrawSize.x / 2, DrawPos.y + DrawSize.y / 2), circradarsize500, IM_COL32(255, 255, 255, 255));
			//ImGui::GetWindowDrawList()->AddLine(ImVec2(midRadar.x, midRadar.y - DrawSize.y / 2.f), ImVec2(midRadar.x, midRadar.y + DrawSize.y / 2.f), IM_COL32(255, 255, 255, 255));

			DrawRadarPoint2(EneamyPos, LocalPos, LocalPlayerY, eneamyDist, DrawPos.x, DrawPos.y, DrawSize.x, DrawSize.y, { 255, 255, 255, 255 });
		}
		ImGui::End();
	}
	ImGui::PopStyleColor();
}

bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

player players[100];

/*
//Full map radar test, Broken atm
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
//These values only work with 1920x1080 fullscreen
//Battel Royal
world KingsCanyon(ImVec2(0, 0), ImVec2(0, 0), ImVec2(0, 0), ImVec2(0, 0)); //to be measured
world WorldsEdge(ImVec2(-9190.608398, 8443.554688), ImVec2(824, 412), ImVec2(-19529.794922, -8933.173828), ImVec2(707, 608));
world Olympus(ImVec2(0, 0), ImVec2(0, 0), ImVec2(0, 0), ImVec2(0, 0)); //to be measured
world StormPoint(ImVec2(-21264.427734, -47086.878906), ImVec2(711, 983), ImVec2(40298.070313, 21163.728516), ImVec2(1321, 306));

//Arena
world Overflow(ImVec2(-3344.994629, -4018.093018), ImVec2(552, 431), ImVec2(5039.592773, -4639.289063), ImVec2(1322, 489));
world DropOff(ImVec2(3135.113281, 1654.107666), ImVec2(1151, 603), ImVec2(-2920.918701, 811.240479), ImVec2(722, 663));
world Habitat4(ImVec2(4482.470215, -604.362854), ImVec2(1205, 544), ImVec2(-4464.019043, 593.067688), ImVec2(650, 470));
world Encore(ImVec2(4144.926270, 468.957611), ImVec2(1184, 472), ImVec2(-3791.070313, 3.092307), ImVec2(692, 501));
world PartyCrasher(ImVec2(-3275.972900, 3646.970703), ImVec2(589, 197), ImVec2(1085.708740, -3869.658936), ImVec2(1022, 943));

ImVec2 worldToScreenMap(D3DXVECTOR3 origin) {
		float ratioX;
		float ratioY;
		ImVec2 w1;
		ImVec2 s1;
		
		if (stormmap == true) { //Storm Point
			ratioX = StormPoint.ratioX;
			ratioY = StormPoint.ratioY;
			w1 = StormPoint.w1;
			s1 = StormPoint.s1;
		}
		else if (strncmp(mapname, "mp_rr_aqueduct", 14) == 0) { //arena Overflow
			ratioX = Overflow.ratioX;
			ratioY = Overflow.ratioY;
			w1 = Overflow.w1;
			s1 = Overflow.s1;
		}
		else if (strncmp(mapname, "mp_rr_arena_composite", 21) == 0) { //arena DropOff
			ratioX = DropOff.ratioX;
			ratioY = DropOff.ratioY;
			w1 = DropOff.w1;
			s1 = DropOff.s1;
		}
		else if (strncmp(mapname, "mp_rr_arena_habitat", 19) == 0) { //arena Habitat4
			ratioX = Habitat4.ratioX;
			ratioY = Habitat4.ratioY;
			w1 = Habitat4.w1;
			s1 = Habitat4.s1;
		}
		else if (strncmp(mapname, "mp_rr_arena_skygarden", 21) == 0) { //arena Encore
			ratioX = Encore.ratioX;
			ratioY = Encore.ratioY;
			w1 = Encore.w1;
			s1 = Encore.s1;
		}
		else if (strncmp(mapname, "mp_rr_party_crasher", 19) == 0) { //arena PartyCrasher
			ratioX = PartyCrasher.ratioX;
			ratioY = PartyCrasher.ratioY;
			w1 = PartyCrasher.w1;
			s1 = PartyCrasher.s1;
		}
		else if (strncmp(mapname, "mp_rr_canyonlands_mu", 20) == 0) { //KingsCanyon
			ratioX = KingsCanyon.ratioX;
			ratioY = KingsCanyon.ratioY;
			w1 = KingsCanyon.w1;
			s1 = KingsCanyon.s1;
		}
		if (worldsedge == true) { //WorldsEdge
			ratioX = WorldsEdge.ratioX;
			ratioY = WorldsEdge.ratioY;
			w1 = WorldsEdge.w1;
			s1 = WorldsEdge.s1;
		}
		else if (strncmp(mapname, "mp_rr_olympus", 13) == 0) { //Olympus
			ratioX = Olympus.ratioX;
			ratioY = Olympus.ratioY;
			w1 = Olympus.w1;
			s1 = Olympus.s1;
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

		FilledRectangle(pos_x, pos_y, 5, 5, { radarcolorr, radarcolorg, radarcolorb, 255 });
		
	
}
*/


void Overlay::RenderEsp()
{
	//Fov Circle Stuff
	if (fovcircle && zoomf1 == 0)
	{



		//ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddCircle(ImVec2(1920 / 2, 1080 / 2), fovsize, IM_COL32(fovcolor1, fovcolor2, fovcolor3, 255), 100, fovthick);
		//ImGui::End();
	}

	else if (fovcircle && zoomf1 == 1)
	{
		//ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddCircle(ImVec2(1920 / 2, 1080 / 2), fovsize2, IM_COL32(fovcolor1, fovcolor2, fovcolor3, 255), 100, fovthick);
		//ImGui::End();
	}
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
							MiniMapRadar(players[i].EntityPosition, players[i].LocalPlayerPosition, players[i].localviewangle.y, radardistance);
					}
					if (bigradar)
					{
						BigMiniMapRadar(players[i].EntityPosition, players[i].LocalPlayerPosition, players[i].localviewangle.y, radardistance);
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
					/*
					//Full Radar map, broken atm
					if (mainradarmap == true)

						worldToScreenMap(players[i].EntityPosition);

						//String(ImVec2(players[i].boxMiddle, (players[i].b_y - players[i].height - 15)), WHITE, players[i].name);
					*/

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
	add[18] = (uintptr_t)&chargerifle;
	add[19] = (uintptr_t)&shooting;
	add[20] = (uintptr_t)&glowr;
	add[21] = (uintptr_t)&glowg;
	add[22] = (uintptr_t)&glowb;
	add[23] = (uintptr_t)&firing_range;
	add[24] = (uintptr_t)&glowtype;
	add[25] = (uintptr_t)&glowtype2;
	
	printf(XorStr("Game Version 3.0.11.32 |-| Radar Ver With Color Test |-| Add me offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));

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
			std::ifstream config("Config.txt");

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
				config >> glowtype;
				config >> glowtype2;
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
				config >> fovcircle;
				config >> fovsize;
				config >> fovsize2;
				config >> fovcolor1;
				config >> fovcolor2;
				config >> fovcolor3;
				config >> fovcolorset[0];
				config >> fovcolorset[1];
				config >> fovcolorset[2];
				config >> fovcolorset[3];
				config >> fovthick;
				//config >> item_current; // no idea how to imput a string of words 


				config.close();
			}
		}
		//Charge Rifle Hotkey is ~
		if (IsKeyDown(VK_OEM_3) && chargeriflekp == 0)
		{
			chargerifle = true;
			chargeriflekp = 1;
		}

		else if (IsKeyDown(VK_OEM_3) && chargeriflekp == 1)
		{
			chargerifle = false;
			chargeriflekp = 0;
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

		if (IsKeyDown(VK_F9) && k_f100 == 1)
		{
			k_f100 = 0;
			k_f20 = 1;
			k_f8 = 1;
		}
		else  if (IsKeyDown(VK_F10) && k_f100 == 0)
		{
			k_f20 = 0;
			k_f100 = 1;
			k_f8 = 1;

		}

		if (IsKeyDown(VK_F11))
		{
			k_f20 = 0;
			k_f100 = 0;
			bone = 1;
			smooth = 100;

		}
		//Big Radar Hokey is T
		//Main Map Radar
		if (IsKeyDown(0x54) && biggerradartoggle == 0)
		{
			biggerradartoggle = 1;
			switch (bigradar)
			{
			case 0:
				bigradar = true;
				break;
			case 1:
				bigradar = false;
				break;
			default:
				break;
			}
		}
		else if (!IsKeyDown(0x54) && biggerradartoggle == 1)
		{
			biggerradartoggle = 0;
		}

		/*
		//Main Map Radar, Broken atm
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
		*/
		if (IsKeyDown(aim_key))
		{
			aiming = true;
			zoomf1 = 1;
		}

		else if (IsKeyDown(aim_key2))
			aiming = true;
		else
		{
			aiming = false;
			zoomf1 = 0;
		}
		if (IsKeyDown(shoot_key))
		{
			shooting = true;

		}

		if (IsKeyDown(shoot_key2))
		{
			shooting = true;

		}

	}
	ready = false;
	ov1.Clear();
	if (!use_nvidia)
		system(XorStr("taskkill /F /T /IM Nvspcaps64.exe")); //custom overlay process name
	return 0;
}



