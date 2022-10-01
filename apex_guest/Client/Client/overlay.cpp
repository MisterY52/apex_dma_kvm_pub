#pragma warning (disable : 4715)
#pragma warning (disable : 4005)
#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#include "overlay.h"
#include <fstream>
#include <iomanip>

using namespace std;

extern bool firing_range;
extern int aim;
extern bool esp;
extern bool item_glow;
extern bool player_glow;
extern bool aim_no_recoil;
extern bool ready;
extern bool use_nvidia;
extern float max_dist;
extern float smooth;
extern float max_fov;
extern int bone;
extern bool thirdperson;
extern int spectators;
extern int allied_spectators;
//Left and Right Aim key toggle
extern bool toggleaim;
extern bool toggleaim2;
int e = 0;
//glow color and type
extern float glowr; //Red Value
extern float glowg; //Green Value
extern float glowb; //Blue Value
extern float glowcolor[3];
//MiniMap Radar
int minimapradardotsize1 = 5;
int minimapradardotsize2 = 5;
extern bool minimapradar;
unsigned int radarcolorr = 0; //Red Value
unsigned int radarcolorg = 0; //Green Value
unsigned int radarcolorb = 0; //Blue Value
extern float radarcolor[3];
//Main Map Radar
extern bool mainradarmap;
int mainmapradardotsize1 = 5;
int mainmapradardotsize2 = 5;
//Ha think i was done ?
//Item Filter Brute Force!
extern bool lightbackpack;
extern bool medbackpack;
extern bool heavybackpack;
extern bool shieldupgrade;
extern bool shieldupgradehead;
extern bool accelerant;
extern bool phoenix;
extern bool healthlarge;
extern bool healthsmall;
extern bool shieldbattsmall;
extern bool shieldbattlarge;
extern bool ammosniper;
extern bool ammohc;
extern bool optic;
extern bool ammosc;;
extern bool ammonrg;
extern bool ammoshotgun;
extern bool lasersight;;
extern bool magsniper;
extern bool magenergy;
extern bool stocksniper;
extern bool stockregular;
extern bool shielddown;
extern bool lightammomag;
extern bool heavyammomag;
extern bool optic2x;
extern bool opticholo1x;
extern bool opticsniper6x;
extern bool opticsniper4x8x;
extern bool opticsniperthreat;
extern bool optic2x4x;
extern bool opticthreat;
extern bool optic3x;
extern bool opticholo1x2x;
extern bool suppressor;
extern bool weaponmod;
extern bool grenade_frag;
extern bool grenade_arc_star;
extern bool grenade_thermite;
extern bool shotgunbolt;
extern bool weapon_kraber;
extern bool weapon_mastiff;
extern bool weapon_lstar;
extern bool weapon_havoc;
extern bool weapon_devotion;
extern bool weapon_triple_take;
extern bool weapon_flatline;
extern bool weapon_hemlock;
extern bool weapon_g7_scout;
extern bool weapon_alternator;
extern bool weapon_r99;
extern bool weapon_prowler;
extern bool weapon_volt;
extern bool weapon_longbow;
extern bool weapon_charge_rifle;
extern bool weapon_spitfire;
extern bool weapon_r301;
extern bool weapon_eva8;
extern bool weapon_peacekeeper;
extern bool weapon_mozambique;
extern bool weapon_wingman;
extern bool weapon_p2020;
extern bool weapon_re45;
extern bool weapon_sentinel;
extern bool weapon_bow;
extern bool weapon_3030_repeater;
extern bool weapon_rampage;
extern bool weapon_car_smg;
//Aim Dist check
extern float aimdist;
//item glow brightness
extern int itemglowbrightness;
//Menu Stuff
int menu1 = 0;
int menu2 = 0;
int menu3 = 0;
int menu4 = 0;



int width;
int height;
bool k_leftclick = false;
bool k_ins = false;
bool show_menu = false;
visuals v;

extern bool IsKeyDown(int vk);

LONG nv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG nv_default_in_game = nv_default | WS_DISABLED;
LONG nv_edit = nv_default_in_game | WS_VISIBLE;

LONG nv_ex_default = WS_EX_TOOLWINDOW;
LONG nv_ex_edit = nv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
LONG nv_ex_edit_menu = nv_ex_default | WS_EX_TRANSPARENT;

static DWORD WINAPI StaticMessageStart(void* Param)
{
	Overlay* ov = (Overlay*)Param;
	ov->CreateOverlay();
	return 0;
}

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
	wchar_t className[255] = L"";
	GetClassName(hwnd, className, 255);
	if (use_nvidia)
	{
		if (wcscmp(XorStrW(L"CEF-OSC-WIDGET"), className) == 0) //Nvidia overlay
		{
			HWND* w = (HWND*)lParam;
			if (GetWindowLong(hwnd, GWL_STYLE) != nv_default && GetWindowLong(hwnd, GWL_STYLE) != nv_default_in_game)
				return TRUE;
			*w = hwnd;
			return TRUE;
		}
	}
	else
	{
		if (wcscmp(XorStrW(L"overlay"), className) == 0) //Custom overlay
		{
			HWND* w = (HWND*)lParam;
			*w = hwnd;
			return TRUE;
		}
	}
	return TRUE;
}

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void Overlay::RenderMenu()
{
	static bool aim_enable = false;
	static bool vis_check = false;
	static bool spec_disable = false;
	static bool all_spec_disable = false;

	if (aim > 0)
	{
		aim_enable = true;
		if (aim > 1)
		{
			vis_check = true;
		}
		else
		{
			vis_check = false;
		}
	}
	else
	{
		aim_enable = false;
		vis_check = false;
	}
	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(450, 860), ImGuiCond_Once);
	ImGui::Begin(XorStr("##title"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	//if (ImGui::BeginTabBar(XorStr("Tab")))
	//{
		//if (ImGui::BeginTabItem(XorStr("##")))
		//{
				if (ImGui::CollapsingHeader("Main Toggle Settings"))
				{
					menu1 = 1;
					ImGui::Sliderbox(XorStr("ESP On/Off"), &esp);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("AIM On/Off"), &aim_enable);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Glow Items"), &item_glow);
					ImGui::Sliderbox(XorStr("Glow Players"), &player_glow);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Thirdperson"), &thirdperson);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Mini-Map Radar"), &minimapradar);
					if (aim_enable)
					{
						ImGui::Sliderbox(XorStr("Visibility Check"), &vis_check);
						ImGui::SameLine();
						ImGui::Sliderbox(XorStr("No Recoil"), &aim_no_recoil);
						if (vis_check)
						{
							aim = 2;
						}
						else
						{
							aim = 1;
						}
					}
					else
					{
						aim = 0;
					}
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Firing Range"), &firing_range);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Aiming Distance:"));
					ImGui::SameLine();
					ImGui::TextColored(GREEN, "%.f meters", aimdist / 39.62);
					ImGui::SliderFloat(XorStr("##Aim Distance"), &aimdist, 10.0f * 39.62, 1600.0f * 39.62, "##");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Aiming Keys:"));
					ImGui::RadioButton("Left Mouse", &e, 1); ImGui::SameLine();
					ImGui::RadioButton("Right Mouse ", &e, 2); ImGui::SameLine();
					ImGui::RadioButton("Left/Right Mouse", &e, 3);
					//Setting one and unsetting the other
					if (e == 1)
					{
						toggleaim = true;
						toggleaim2 = false;
					}
					else if (e == 2)
					{
						toggleaim = false;
						toggleaim2 = true;
					}
					else if (e == 3)
					{
						toggleaim = true;
						toggleaim2 = true;
					}
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Max distance for everything:"));
					ImGui::SameLine();
					ImGui::TextColored(GREEN,"%d meters", (int)(max_dist / 40));
					ImGui::SliderFloat(XorStr("##1"), &max_dist, 100.0f * 40, 3800.0f * 40, "##");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Smooth Aim Value:"));
					ImGui::SameLine();
					ImGui::TextColored(GREEN, "%.f", smooth);
					ImGui::SliderFloat(XorStr("##2"), &smooth, 85.0f, 150.0f, "##");
					ImGui::SameLine();
					ImGui::Text(XorStr("85 To 100 Is Safe"));
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Max FOV:"));
					ImGui::SameLine();
					ImGui::TextColored(GREEN, "%.f", max_fov);
					ImGui::SliderFloat(XorStr("##3"), &max_fov, 1.0f, 50.0f, "##");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Aiming Bone:"));
					ImGui::Text(XorStr("0=Head, 1=Neck, 2=Chest, 3=Stomach"));
					ImGui::SliderInt(XorStr("##bone slider"), &bone, 0, 3);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("ESP Options:"));
					ImGui::Sliderbox(XorStr("Distance"), &v.distance);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Health bar"), &v.healthbar);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Shield bar"), &v.shieldbar);					
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Player Glow Color:"));
					ImGui::ColorEdit3("##Glow Color Picker", glowcolor);
					{
						glowr = glowcolor[0] * 250;
						glowg = glowcolor[1] * 250;
						glowb = glowcolor[2] * 250;
					}
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Saving and Loading. Need to Save Once to make the file."));
					//Saving
					if (ImGui::Button("Save Config"))
					{
						ofstream config("Settings.txt");
						if (config.is_open())
						{
							config << std::boolalpha << firing_range << "\n";
							config << aim << "\n";
							config << std::boolalpha << esp << "\n";
							config << std::boolalpha << item_glow << "\n";
							config << std::boolalpha << player_glow << "\n";
							config << std::boolalpha << aim_no_recoil << "\n";
							config << max_dist << "\n";
							config << smooth << "\n";
							config << max_fov << "\n";
							config << bone << "\n";
							config << glowr << "\n";
							config << glowg << "\n";
							config << glowb << "\n";
							config << glowcolor[0] << "\n";
							config << glowcolor[1] << "\n";
							config << glowcolor[2] << "\n";
							config << radarcolorr << "\n";
							config << radarcolorg << "\n";
							config << radarcolorb << "\n";
							config << radarcolor[0] << "\n";
							config << radarcolor[1] << "\n";
							config << radarcolor[2] << "\n";
							config << v.healthbar << "\n";
							config << v.shieldbar << "\n";
							config << v.distance << "\n";
							config << thirdperson << "\n";
							config << std::boolalpha << minimapradar << "\n";

							config << std::boolalpha << lightbackpack << "\n";
							config << std::boolalpha << medbackpack << "\n";
							config << std::boolalpha << heavybackpack << "\n";
							config << std::boolalpha << shieldupgrade << "\n";
							config << std::boolalpha << shieldupgradehead << "\n";
							config << std::boolalpha << accelerant << "\n";
							config << std::boolalpha << phoenix << "\n";
							config << std::boolalpha << healthlarge << "\n";
							config << std::boolalpha << healthsmall << "\n";
							config << std::boolalpha << shieldbattsmall << "\n";
							config << std::boolalpha << shieldbattlarge << "\n";
							config << std::boolalpha << ammosniper << "\n";
							config << std::boolalpha << ammohc << "\n";
							config << std::boolalpha << optic << "\n";
							config << std::boolalpha << ammosc << "\n";
							config << std::boolalpha << ammonrg << "\n";
							config << std::boolalpha << ammoshotgun << "\n";
							config << std::boolalpha << lasersight << "\n";
							config << std::boolalpha << magsniper << "\n";
							config << std::boolalpha << magenergy << "\n";
							config << std::boolalpha << stocksniper << "\n";
							config << std::boolalpha << stockregular << "\n";
							config << std::boolalpha << shielddown << "\n";
							config << std::boolalpha << lightammomag << "\n";
							config << std::boolalpha << heavyammomag << "\n";
							config << std::boolalpha << optic2x << "\n";
							config << std::boolalpha << opticholo1x << "\n";
							config << std::boolalpha << opticsniper6x << "\n";
							config << std::boolalpha << opticsniper4x8x << "\n";
							config << std::boolalpha << opticsniperthreat << "\n";
							config << std::boolalpha << optic2x4x << "\n";
							config << std::boolalpha << opticthreat << "\n";
							config << std::boolalpha << optic3x << "\n";
							config << std::boolalpha << opticholo1x2x << "\n";

							config << std::boolalpha << suppressor << "\n";
							config << std::boolalpha << weaponmod << "\n";
							config << std::boolalpha << grenade_frag << "\n";
							config << std::boolalpha << grenade_arc_star << "\n";
							config << std::boolalpha << grenade_thermite << "\n";
							config << std::boolalpha << shotgunbolt << "\n";
							config << std::boolalpha << weapon_kraber << "\n";
							config << std::boolalpha << weapon_mastiff << "\n";
							config << std::boolalpha << weapon_lstar << "\n";
							config << std::boolalpha << weapon_havoc << "\n";
							config << std::boolalpha << weapon_devotion << "\n";
							config << std::boolalpha << weapon_triple_take << "\n";
							config << std::boolalpha << weapon_flatline << "\n";
							config << std::boolalpha << weapon_hemlock << "\n";
							config << std::boolalpha << weapon_g7_scout << "\n";
							config << std::boolalpha << weapon_alternator << "\n";
							config << std::boolalpha << weapon_r99 << "\n";
							config << std::boolalpha << weapon_prowler << "\n";
							config << std::boolalpha << weapon_volt << "\n";
							config << std::boolalpha << weapon_longbow << "\n";
							config << std::boolalpha << weapon_charge_rifle << "\n";
							config << std::boolalpha << weapon_spitfire << "\n";
							config << std::boolalpha << weapon_r301 << "\n";
							config << std::boolalpha << weapon_eva8 << "\n";
							config << std::boolalpha << weapon_peacekeeper << "\n";
							config << std::boolalpha << weapon_mozambique << "\n";
							config << std::boolalpha << weapon_wingman << "\n";
							config << std::boolalpha << weapon_p2020 << "\n";
							config << std::boolalpha << weapon_re45 << "\n";
							config << std::boolalpha << weapon_sentinel << "\n";
							config << std::boolalpha << weapon_bow << "\n";
							config << std::boolalpha << weapon_3030_repeater << "\n";
							config << std::boolalpha << weapon_rampage << "\n";
							config << std::boolalpha << weapon_car_smg << "\n";
							config << toggleaim << "\n";
							config << toggleaim2 << "\n";
							config << e << "\n";
							config << minimapradardotsize1 << "\n";
							config << minimapradardotsize2 << "\n";
							config << aimdist << "\n";
							config << itemglowbrightness << "\n";
							config << mainmapradardotsize1 << "\n";
							config << mainmapradardotsize2;

							config.close();
						}
					}
					ImGui::SameLine();
					//Loading
					if (ImGui::Button("Load Config"))
					{
						ifstream config("Settings.txt");
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
							config >> minimapradardotsize1;
							config >> minimapradardotsize2;
							config >> aimdist;
							config >> itemglowbrightness;
							config >> mainmapradardotsize1;
							config >> mainmapradardotsize2;
							config.close();

						}
					}
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					if (menu1 == 1)
					{
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Weapone Filter Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Item Filter Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Radar Settings"), 0);
					}
					
				}
				if (ImGui::CollapsingHeader("Radar Settings"))
				{
					menu2 = 1;
					//Dot Size for both mini and main map
					ImGui::Text(XorStr("MiniMap Radar Dot Size"));
					ImGui::SliderInt(XorStr("MiniMap Dot Width"), &minimapradardotsize1, 1, 10);
					ImGui::SliderInt(XorStr("MiniMap Dot length"), &minimapradardotsize2, 1, 10);
					ImGui::Text(XorStr("Main Map Radar Dot Size"));
					ImGui::SliderInt(XorStr("Main Map Dot Width"), &mainmapradardotsize1, 1, 10);
					ImGui::SliderInt(XorStr("Main Map Dot length"), &mainmapradardotsize2, 1, 10);
					/*//Radar Color
					ImGui::Text(XorStr("Radar Color Picker:"));
					ImGui::ColorEdit3("##Radar Color Picker", radarcolor);
					{
						radarcolorr = radarcolor[0] * 250;
						radarcolorg = radarcolor[1] * 250;
						radarcolorb = radarcolor[2] * 250;
					}
					*/
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					if (menu2 == 1)
					{
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Weapone Filter Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Item Filter Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Main Toggle Settings"), 0);
					}
				}
				if (ImGui::CollapsingHeader("Item Filter Settings"))
				{
					menu3 = 1;
					ImGui::Text(XorStr("Ammo"));
					ImGui::Dummy(ImVec2(0.0f, 1.0f));
					ImGui::Sliderbox(XorStr("Sniper Ammo"), &ammosniper);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Heavy Ammo"), &ammohc);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Light Ammo"), &ammosc);
					ImGui::Sliderbox(XorStr("Energy Ammo"), &ammonrg);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Shotgun Ammo"), &ammoshotgun);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Ammo Mags"));
					ImGui::Dummy(ImVec2(0.0f, 1.0f));
					ImGui::Sliderbox(XorStr("Sniper"), &magsniper);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Heavy"), &heavyammomag);
					ImGui::Sliderbox(XorStr("Light"), &lightammomag);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Energy "), &magenergy);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("HCOGs"));
					ImGui::Dummy(ImVec2(0.0f, 1.0f));
					ImGui::Sliderbox(XorStr("1x HCOG"), &optic);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("2x HCOG"), &optic2x);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("3x HCOG"), &optic3x);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("2-4x"), &optic2x4x);
					ImGui::Text(XorStr("Snipers"));
					ImGui::Dummy(ImVec2(0.0f, 1.0f));
					ImGui::Sliderbox(XorStr("Sniper 6x"), &opticsniper6x);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Sniper 4-8x"), &opticsniper4x8x);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Sniper Threat"), &opticsniperthreat);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Holo's"));
					ImGui::Dummy(ImVec2(0.0f, 1.0f));
					ImGui::Sliderbox(XorStr("1x Holo"), &opticholo1x);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("2x Holo"), &opticholo1x2x);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("1x Threat"), &opticthreat);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Backpacks"));
					ImGui::Sliderbox(XorStr("Light Backpack"), &lightbackpack);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Medium Backpack"), &medbackpack);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Heavy Backpack"), &heavybackpack);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Shield's"));
					ImGui::Sliderbox(XorStr("Armor"), &shieldupgrade);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Helmets"), &shieldupgradehead);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Knockdown"), &shielddown);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::TextColored(GREEN, "Heals for Health");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));					
					ImGui::Sliderbox(XorStr("Large Health"), &healthlarge);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Small Health"), &healthsmall);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Phoenix Kit"), &phoenix);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::TextColored(BLUE, "Heals for Shields");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("Large Shield"), &shieldbattlarge);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Small Shield"), &shieldbattsmall);													
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Ultimate Accelerant"), &accelerant);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Attachements"));
					ImGui::Dummy(ImVec2(0.0f, 5.0f));
					ImGui::Sliderbox(XorStr("Lasersight"), &lasersight);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Sniper Stock"), &stocksniper);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Regular Stock"), &stockregular);
					ImGui::Sliderbox(XorStr("Suppressor"), &suppressor);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Weapon Mods"), &weaponmod);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Item Glow Brightness:"));
					ImGui::SliderInt(XorStr("##itemglowbright"), &itemglowbrightness, 2, 40, "%d");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					if (menu3 == 1)
					{
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Weapone Filter Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Radar Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Main Toggle Settings"), 0);
					}
				}
				if (ImGui::CollapsingHeader("Weapone Filter Settings"))
				{
					menu4 = 1;
					//Light Weapons
					ImGui::TextColored(ORANGE, "Light Weapons");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("P2020"), &weapon_p2020);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("RE-45"), &weapon_re45);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("R-99"), &weapon_r99);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("G7 Scout"), &weapon_g7_scout);
					ImGui::Sliderbox(XorStr("Spitfire"), &weapon_spitfire);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("R-301"), &weapon_r301);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Alternator "), &weapon_alternator);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					//Heavy Weapons
					ImGui::TextColored(TEAL, "Heavy Weapons");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("C.A.R."), &weapon_car_smg);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Flatline"), &weapon_flatline);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Hemlok"), &weapon_hemlock);
					ImGui::Sliderbox(XorStr("Prowler "), &weapon_prowler);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("30-30"), &weapon_3030_repeater);
					//Energy Weapons
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::TextColored(YELLOW, "Energy Weapons");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("Triple Take"), &weapon_triple_take);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("L-STAR"), &weapon_lstar);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Volt"), &weapon_volt);
					ImGui::Sliderbox(XorStr("Devotion "), &weapon_devotion);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("HAVOC"), &weapon_havoc);
					//Shotgun Weapons
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::TextColored(RED, "Shotgun Weapons");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("EVA-8"), &weapon_eva8);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Mozambique"), &weapon_mozambique);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Peacekeeper"), &weapon_peacekeeper);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Mastiff"), &weapon_mastiff);
					//Sniper Weapons
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::TextColored(BLUE, "Sniper Weapons");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("Charge Rifle"), &weapon_charge_rifle);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Longbow"), &weapon_longbow);
					ImGui::SameLine();
					ImGui::Sliderbox(XorStr("Sentinel"), &weapon_sentinel);
					ImGui::Sliderbox(XorStr("Wingman "), &weapon_wingman);
					//KRABER
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("The Kraber"));
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Sliderbox(XorStr("Kraber .50-Cal Sniper"), &weapon_kraber);
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					ImGui::Text(XorStr("Item Glow Brightness:"));
					ImGui::SliderInt(XorStr("##itemglowbright"), &itemglowbrightness, 2, 40, "%d");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					if (menu4 == 1)
					{
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Item Filter Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Radar Settings"), 0);
						ImGui::GetStateStorage()->SetInt(ImGui::GetID("Main Toggle Settings"), 0);
					}
				}
				//ImGui::EndTabItem();
			

		//}

		//ImGui::EndTabBar();
	//}
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text(XorStr("Overlay FPS: %.3f ms/frame (%.1f FPS)"), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}


void Overlay::RenderInfo()
{	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(154, 25));
	ImGui::Begin(XorStr("##info"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
	DrawLine(ImVec2(1, 5), ImVec2(154, 5), RED, 2);
	ImGui::TextColored(RED, "%d", spectators);
	ImGui::SameLine();
	ImGui::Text("--");
	ImGui::SameLine();
	ImGui::TextColored(GREEN, "%d", allied_spectators);
	ImGui::SameLine();
	ImGui::Text("--");
	ImGui::SameLine();
	//Aim is on = 2, On but No Vis Check = 1, Off = 0
	if (aim == 2)
	{
		ImGui::TextColored(GREEN, "Aim On %d", aim);
	}
	else
	{
		ImGui::TextColored(RED, "Aim Off %d", aim);
	}

	ImGui::End();
}

void Overlay::ClickThrough(bool v)
{
	if (v)
	{
		nv_edit = nv_default_in_game | WS_VISIBLE;
		if (GetWindowLong(overlayHWND, GWL_EXSTYLE) != nv_ex_edit)
			SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_edit);
	}
	else
	{
		nv_edit = nv_default | WS_VISIBLE;
		if (GetWindowLong(overlayHWND, GWL_EXSTYLE) != nv_ex_edit_menu)
			SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_edit_menu);
	}
}

DWORD Overlay::CreateOverlay()
{
	EnumWindows(EnumWindowsCallback, (LPARAM)&overlayHWND);
	Sleep(300);
	if (overlayHWND == 0)
	{
		printf(XorStr("Can't find the overlay, HIT ALT-Z\n"));
		Sleep(1000);
		exit(0);
	}

	HDC hDC = ::GetWindowDC(NULL);
	width = ::GetDeviceCaps(hDC, HORZRES);
	height = ::GetDeviceCaps(hDC, VERTRES);

	running = true;

	// Initialize Direct3D
	if (!CreateDeviceD3D(overlayHWND))
	{
		CleanupDeviceD3D();
		return 1;
	}

	// Show the window
	::ShowWindow(overlayHWND, SW_SHOWDEFAULT);
	::UpdateWindow(overlayHWND);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowMinSize = ImVec2(1, 1);

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(overlayHWND);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ClickThrough(true);
	while (running)
	{
		HWND wnd = GetWindow(GetForegroundWindow(), GW_HWNDPREV);
		if (use_nvidia)
		{
			if (GetWindowLong(overlayHWND, GWL_STYLE) != nv_edit)
				SetWindowLong(overlayHWND, GWL_STYLE, nv_edit);
			if (show_menu)
			{
				ClickThrough(false);
			}
			else
			{
				if (GetWindowLong(overlayHWND, GWL_EXSTYLE) != nv_ex_edit)
					SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_edit);
				ClickThrough(true);
			}
		}
		if (wnd != overlayHWND)
		{
			SetWindowPos(overlayHWND, wnd, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
			::UpdateWindow(overlayHWND);
		}

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (IsKeyDown(VK_LBUTTON) && !k_leftclick)
		{
			io.MouseDown[0] = true;
			k_leftclick = true;
		}
		else if (!IsKeyDown(VK_LBUTTON) && k_leftclick)
		{
			io.MouseDown[0] = false;
			k_leftclick = false;
		}

		if (IsKeyDown(VK_INSERT) && !k_ins && ready)
		{
			show_menu = !show_menu;
			ClickThrough(!show_menu);
			k_ins = true;
		}
		else if (!IsKeyDown(VK_INSERT) && k_ins)
		{
			k_ins = false;
		}

		if (show_menu)
			RenderMenu();
		else
			RenderInfo();

		RenderEsp();

		// Rendering
		ImGui::EndFrame();
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	ClickThrough(true);

	CleanupDeviceD3D();
	::DestroyWindow(overlayHWND);
	return 0;
}

void Overlay::Start()
{
	DWORD ThreadID;
	CreateThread(NULL, 0, StaticMessageStart, (void*)this, 0, &ThreadID);
}

void Overlay::Clear()
{
	running = 0;
	Sleep(50);
	if (use_nvidia)
	{
		SetWindowLong(overlayHWND, GWL_STYLE, nv_default);
		SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_default);
	}
}

int Overlay::getWidth()
{
	return width;
}

int Overlay::getHeight()
{
	return height;
}

// Helper functions

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer)
	{
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void Overlay::DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width)
{
	ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
}

void Overlay::DrawBox(ImColor color, float x, float y, float w, float h)
{
	DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.0f);
	DrawLine(ImVec2(x, y), ImVec2(x, y + h), color, 1.0f);
	DrawLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.0f);
	DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.0f);
}


void Overlay::Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
}

void Overlay::String(ImVec2 pos, ImColor color, const char* text)
{
	Text(pos, color, text, text + strlen(text), 200, 0);
}

void Overlay::RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

void Overlay::ProgressBar(float x, float y, float w, float h, int value, int v_max)
{
	ImColor barColor = ImColor(
		min(510 * (v_max - value) / 100, 255),
		min(510 * value / 100, 255),
		25,
		255
	);
	
	RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor, 0.0f, 0);
}


//Seer Hp and Shield bars (never re fixed the armor type so its set to max shield)

void DrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color) {
	ImGui::GetWindowDrawList()->AddQuadFilled(p1, p2, p3, p4, color);
}
void DrawHexagon(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col, float thickness)
{
	ImGui::GetWindowDrawList()->AddHexagon(p1, p2, p3, p4, p5, p6, col, thickness);
}
void DrawHexagonFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col)
{
	ImGui::GetWindowDrawList()->AddHexagonFilled(p1, p2, p3, p4, p5, p6, col);
}

void Overlay::DrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health) {

	int bg_offset = 3;
	int bar_width = 158;
	// 4steps...2*3=6
	// 38*4=152 152+6 = 158
	// 5steps...2*4=8
	// 30*5=150 150+8 = 158
	float max_health = 100.0f;
	float shield_step = 25.0f;

	int shield_25 = 30;
	int steps = 5;


	ImVec2 bg1(x - bar_width / 2 - bg_offset, y);
	ImVec2 bg2(bg1.x - 10, bg1.y - 16);
	ImVec2 bg3(bg2.x + 5, bg2.y - 7);
	ImVec2 bg4(bg3.x + bar_width + bg_offset, bg3.y);
	ImVec2 bg5(bg4.x + 11, bg4.y + 18);
	ImVec2 bg6(x + bar_width / 2 + bg_offset, y);
	DrawHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


	ImVec2 h1(bg1.x + 3, bg1.y - 4);
	ImVec2 h2(h1.x - 5, h1.y - 8);
	ImVec2 h3(h2.x + (float)health / max_health * bar_width, h2.y);
	ImVec2 h4(h1.x + (float)health / max_health * bar_width, h1.y);
	ImVec2 h3m(h2.x + bar_width, h2.y);
	ImVec2 h4m(h1.x + bar_width, h1.y);
	DrawQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
	DrawQuadFilled(h1, h2, h3, h4, WHITE);


	ImColor shieldCracked(97, 97, 97);
	ImColor shieldCrackedDark(67, 67, 67);

	ImColor shieldCol;
	ImColor shieldColDark; //not used, but the real seer q has shadow inside
	if (max_shield == 50) { //white
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	else if (max_shield == 75) { //blue
		shieldCol = ImColor(39, 178, 255);
		shieldColDark = ImColor(27, 120, 210);
	}
	else if (max_shield == 100) { //purple
		shieldCol = ImColor(206, 59, 255);
		shieldColDark = ImColor(136, 36, 220);
	}
	else if (max_shield == 100) { //gold
		shieldCol = ImColor(255, 255, 79);
		shieldColDark = ImColor(218, 175, 49);
	}
	else if (max_shield == 125) { //red
		shieldCol = ImColor(219, 2, 2);
		shieldColDark = ImColor(219, 2, 2);
	}
	else {
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	int shield_tmp = shield;
	int shield1 = 0;
	int shield2 = 0;
	int shield3 = 0;
	int shield4 = 0;
	int shield5 = 0;
	if (shield_tmp > 25) {
		shield1 = 25;
		shield_tmp -= 25;
		if (shield_tmp > 25) {
			shield2 = 25;
			shield_tmp -= 25;
			if (shield_tmp > 25) {
				shield3 = 25;
				shield_tmp -= 25;
				if (shield_tmp > 25) {
					shield4 = 25;
					shield_tmp -= 25;
					shield5 = shield_tmp;
				}
				else {
					shield4 = shield_tmp;
				}
			}
			else {
				shield3 = shield_tmp;
			}
		}
		else {
			shield2 = shield_tmp;
		}
	}
	else {
		shield1 = shield_tmp;
	}
	ImVec2 s1(h2.x - 1, h2.y - 2);
	ImVec2 s2(s1.x - 3, s1.y - 5);
	ImVec2 s3(s2.x + shield1 / shield_step * shield_25, s2.y);
	ImVec2 s4(s1.x + shield1 / shield_step * shield_25, s1.y);
	ImVec2 s3m(s2.x + shield_25, s2.y);
	ImVec2 s4m(s1.x + shield_25, s1.y);

	ImVec2 ss1(s4m.x + 2, s1.y);
	ImVec2 ss2(s3m.x + 2, s2.y);
	ImVec2 ss3(ss2.x + shield2 / shield_step * shield_25, s2.y);
	ImVec2 ss4(ss1.x + shield2 / shield_step * shield_25, s1.y);
	ImVec2 ss3m(ss2.x + shield_25, s2.y);
	ImVec2 ss4m(ss1.x + shield_25, s1.y);

	ImVec2 sss1(ss4m.x + 2, s1.y);
	ImVec2 sss2(ss3m.x + 2, s2.y);
	ImVec2 sss3(sss2.x + shield3 / shield_step * shield_25, s2.y);
	ImVec2 sss4(sss1.x + shield3 / shield_step * shield_25, s1.y);
	ImVec2 sss3m(sss2.x + shield_25, s2.y);
	ImVec2 sss4m(sss1.x + shield_25, s1.y);

	ImVec2 ssss1(sss4m.x + 2, s1.y);
	ImVec2 ssss2(sss3m.x + 2, s2.y);
	ImVec2 ssss3(ssss2.x + shield4 / shield_step * shield_25, s2.y);
	ImVec2 ssss4(ssss1.x + shield4 / shield_step * shield_25, s1.y);
	ImVec2 ssss3m(ssss2.x + shield_25, s2.y);
	ImVec2 ssss4m(ssss1.x + shield_25, s1.y);

	ImVec2 sssss1(ssss4m.x + 2, s1.y);
	ImVec2 sssss2(ssss3m.x + 2, s2.y);
	ImVec2 sssss3(sssss2.x + shield5 / shield_step * shield_25, s2.y);
	ImVec2 sssss4(sssss1.x + shield5 / shield_step * shield_25, s1.y);
	ImVec2 sssss3m(sssss2.x + shield_25, s2.y);
	ImVec2 sssss4m(sssss1.x + shield_25, s1.y);
	if (max_shield == 50) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield != 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
	}
	else if (max_shield == 75) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (shield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (shield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
	}
	else if (max_shield == 100) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (shield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (shield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (shield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (shield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
	}
	else if (max_shield == 125) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (shield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (shield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (shield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (shield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
		else if (shield <= 125) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
			if (shield < 125) {
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
		}
	}
}

