#include "overlay.h"
#include <fstream>
#include <iomanip>


using namespace std;
extern int aim;
extern bool esp;
//extern bool item_glow;
extern bool player_glow;
extern bool aim_no_recoil;
extern bool ready;
extern bool use_nvidia;
extern float max_dist;
extern float smooth;
extern float max_fov;
extern int bone;
extern int spectators;
extern int allied_spectators;

extern bool onevone;

//extern float esp_distance;

extern int index;
//extern int xp_level;

//Dynamic Distance Stuff
extern float DDS;

bool k_del = false;
static bool fov = false;
static bool showing = false;
extern float cfsize;

//glow color and type
extern float glowr; //Red Value
extern float glowg; //Green Value
extern float glowb; //Blue Value
extern float glowcolor[3];
//glow visable
extern float glowrviz;
extern float glowgviz;
extern float glowbviz;
extern float glowcolorviz[3];
//knocked
extern float glowrknocked;
extern float glowgknocked;
extern float glowbknocked;
extern float glowcolorknocked[3];

//DDS
extern float min_max_fov;
extern float max_max_fov;
extern float min_smooth;
extern float max_smooth;

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
void DrawHexagon(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col, float thickness);
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
	ImGui::SetNextWindowSize(ImVec2(490, 440));
	ImGui::Begin(XorStr("##title"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	if (ImGui::BeginTabBar(XorStr("Tab")))
	{
		if (ImGui::BeginTabItem(XorStr("Main")))
		{
			ImGui::Checkbox(XorStr("ESP"), &esp);

			ImGui::Checkbox(XorStr("AIM"), &aim_enable);

			if (aim_enable)
			{
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("Visibility check"), &vis_check);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr("No recoil/sway"), &aim_no_recoil);
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

			//ImGui::Checkbox(XorStr("Glow items"), &item_glow);
			ImGui::Checkbox(XorStr("Glow players"), &player_glow);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr("Config")))
		{
			ImGui::Text(XorStr("Max distance:"));
			ImGui::SliderFloat(XorStr("##1"), &max_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text("%d meters", (int)(max_dist / 40));

			ImGui::Text(XorStr("Smooth aim value:"));
			ImGui::SliderFloat(XorStr("##2"), &smooth, 12.0f, 250.0f, "%.2f");

			ImGui::Text(XorStr("Max FOV:"));
			ImGui::SliderFloat(XorStr("##3"), &max_fov, 3.80f, 250.0f, "%.2f");
			
			ImGui::Text(XorStr("Aim at (bone id):"));
			ImGui::SliderInt(XorStr("##4"), &bone, 0, 175);
			//TEST DDS
			ImGui::Text(XorStr("Dynamic Distance Stuff:"));
			ImGui::SliderFloat(XorStr("##DDS"), &DDS, 40.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			//ImGui::TextColored(GREEN, "%.0f ", DDS / 39.62);
			ImGui::Text("%d meters", (int)(DDS / 40));
			//TEST CONFIG DDS
			//ImGui::Text(XorStr("EBD:"));
			ImGui::SliderFloat(XorStr("##min_max_fov"), &min_max_fov, 3.80f, 250.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Min Fov");
			ImGui::SliderFloat(XorStr("##max_max_fov"), &max_max_fov, 3.80f, 250.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Max Fov");
			ImGui::SliderFloat(XorStr("##min_smooth"), &min_smooth, 12.0f, 250.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Min Smooth");
			ImGui::SliderFloat(XorStr("##max_smooth"), &max_smooth, 12.0f, 250.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Max Smooth");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr("Visuals")))
		{
			ImGui::Text(XorStr("ESP options:"));
			//ImGui::SliderFloat("ESP Distance", &esp_distance, 50.0f, 500.0f);
			ImGui::Checkbox(XorStr("Box"), &v.box);
			ImGui::SameLine(0, 75.0f);
			ImGui::Checkbox(XorStr("Name"), &v.name);
			ImGui::Checkbox(XorStr("Circle fov"), &fov);
			ImGui::SameLine();
			ImGui::SliderFloat(XorStr("fov"), &cfsize, 2.0f, 250.0f, "%.2f size");
			ImGui::Checkbox(XorStr("Line"), &v.line);
			//ImGui::Checkbox(XorStr("Level"), &v.player_level);
			ImGui::Checkbox(XorStr("Distance"), &v.distance);
			ImGui::Checkbox(XorStr("Health bar"), &v.healthbar);
			ImGui::Checkbox(XorStr("Shield bar"), &v.shieldbar);
			ImGui::Checkbox(XorStr("Skeleton"), &v.skeleton);
			//test glow
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Text(XorStr("Player Glow Visable:"));
			ImGui::ColorEdit3("##Glow Color Picker Visable", glowcolorviz);
			{
				glowrviz = glowcolorviz[0] * 250;
				glowgviz = glowcolorviz[1] * 250;
				glowbviz = glowcolorviz[2] * 250;
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Text(XorStr("Player Glow Not Visable:"));
			ImGui::ColorEdit3("##Glow Color Not Visable", glowcolor);
			{
				glowr = glowcolor[0] * 250;
				glowg = glowcolor[1] * 250;
				glowb = glowcolor[2] * 250;
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Text(XorStr("Player Glow Knocked:"));
			ImGui::ColorEdit3("##Glow Color Knocked", glowcolorknocked);
			{
				glowrknocked = glowcolorknocked[0] * 250;
				glowgknocked = glowcolorknocked[1] * 250;
				glowbknocked = glowcolorknocked[2] * 250;
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Text(XorStr("Saving and Loading. Need to Save Once to make the file."));
			//Saving
			if (ImGui::Button("Save Config"))
			{
				ofstream config("Settings.txt");
				if (config.is_open())
				{
					//config << std::boolalpha << firing_range << "\n";
					config << aim << "\n";
					config << std::boolalpha << esp << "\n";
					//config << std::boolalpha << item_glow << "\n";
					config << std::boolalpha << player_glow << "\n";
					//config << std::boolalpha << aim_no_recoil << "\n";
					config << max_dist << "\n";
					//config << smooth << "\n";
					//config << max_fov << "\n";
					//config << bone << "\n";
					config << v.healthbar << "\n";
					config << v.shieldbar << "\n";
					config << v.distance << "\n";
					//config << v.player_level << "\n";
					config << v.line << "\n";
					config << v.skeleton << "\n";
					//config << cfsize << "\n";
					//config << DDS << "\n";
					//config << aiming << "\n";
					config << glowr << "\n";
					config << glowg << "\n";
					config << glowb << "\n";
					config << glowcolor[0] << "\n";
					config << glowcolor[1] << "\n";
					config << glowcolor[2] << "\n";
					//glow visable
					config << glowrviz << "\n";
					config << glowgviz << "\n";
					config << glowbviz << "\n";
					config << glowcolorviz[0] << "\n";
					config << glowcolorviz[1] << "\n";
					config << glowcolorviz[2] << "\n";
					//glow knocked
					config << glowrknocked << "\n";
					config << glowgknocked << "\n";
					config << glowbknocked << "\n";
					config << glowcolorknocked[0] << "\n";
					config << glowcolorknocked[1] << "\n";
					config << glowcolorknocked[2] << "\n";
					//config << std::boolalpha << onevone;
					config << min_max_fov << "\n";
					config << max_max_fov << "\n";
					config << min_smooth << "\n";
					config << max_smooth << "\n";
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
					//config >> std::boolalpha >> firing_range >> "\n";
					config >> aim;
					config >> std::boolalpha >> esp;
					//config >> std::boolalpha >> item_glow;
					config >> std::boolalpha >> player_glow;
					//config >> std::boolalpha >> aim_no_recoil;
					config >> max_dist;
					//config >> smooth;
					//config >> max_fov;
					//config >> bone;
					config >> v.healthbar;
					config >> v.shieldbar;
					config >> v.distance;
					//config >> v.player_level;
					config >> v.line;
					config >> v.skeleton;
					//config >> cfsize;
					//config >> DDS;
					//config >> aiming;
					config >> glowr;
					config >> glowg;
					config >> glowb;
					config >> glowcolor[0];
					config >> glowcolor[1];
					config >> glowcolor[2];
					//glow visable
					config >> glowrviz;
					config >> glowgviz;
					config >> glowbviz;
					config >> glowcolorviz[0];
					config >> glowcolorviz[1];
					config >> glowcolorviz[2];
					//glow knocked
					config >> glowrknocked;
					config >> glowgknocked;
					config >> glowbknocked;
					config >> glowcolorknocked[0];
					config >> glowcolorknocked[1];
					config >> glowcolorknocked[2];
					//config >> std::boolalpha >> onevone;
					config >> min_max_fov;
					config >> max_max_fov;
					config >> min_smooth;
					config >> max_smooth;
					config.close();
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::Text(XorStr("Overlay FPS: %.3f ms/frame (%.1f FPS)"), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void Overlay::RenderInfo()
{
	ImGui::SetNextWindowPos(ImVec2(300, 0));
	ImGui::SetNextWindowSize(ImVec2(170, 100));
	ImGui::Begin(XorStr("##info"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	DrawLine(ImVec2(1, 2), ImVec2(170, 2), RED, 2);;
	ImGui::Text(XorStr("DDS :"));
	ImGui::SameLine();
	ImGui::Text("%d meters", (int)(DDS / 40));
	ImGui::SameLine();
	ImGui::TextColored(RED, "%d", spectators);
	ImGui::SameLine();
	ImGui::Text("-");
	ImGui::SameLine();
	ImGui::TextColored(GREEN, "%d", allied_spectators);
	ImGui::Checkbox(XorStr("Glow"), &player_glow);
	ImGui::SameLine();
	ImGui::Checkbox(XorStr("ESP"), &esp);
	ImGui::SameLine();
	ImGui::Checkbox(XorStr("1V1"), &onevone);
	//ImGui::SameLine();
	//ImGui::TextColored(GREEN, "%.0f", esp_distance / 39.62); //meters
	ImGui::Text(XorStr("SMT"));
	ImGui::SameLine();
	ImGui::SliderFloat(XorStr("##2"), &smooth, 12.0f, 250.0f, "%.2f");
	ImGui::Text(XorStr("FOV"));
	ImGui::SameLine();
	ImGui::SliderFloat(XorStr("##3"), &max_fov, 3.80f, 250.0f, "%.2f");

	// Get the end position of the FOV slider
	ImVec2 fovSliderEnd = ImGui::GetItemRectMax();
	ImVec2 squarePos = ImVec2(fovSliderEnd.x + 5, fovSliderEnd.y - 10); // Adjust the position as needed
	ImVec2 squareSize = ImVec2(10, 10);

	bool isConnected = ready; // Use the 'ready' variable to determine connection status

	if (isConnected)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(squarePos, ImVec2(squarePos.x + squareSize.x, squarePos.y + squareSize.y), IM_COL32(0, 255, 0, 255)); // Green if ready
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRectFilled(squarePos, ImVec2(squarePos.x + squareSize.x, squarePos.y + squareSize.y), IM_COL32(255, 0, 0, 255)); // Red if not ready
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
		printf(XorStr("Can't find the overlay\n"));
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

		if (IsKeyDown(VK_DELETE) && !k_del)
		{
			showing = !showing;

			k_del = true;
		}
		else if (!IsKeyDown(VK_DELETE) && k_del)
		{
			k_del = false;
		}

		if (show_menu)
			RenderMenu();
		else
			RenderInfo();
			//RenderSpectator();

		if (fov)
		{
			ImGui::Begin("##circlefov", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
			auto draw = ImGui::GetBackgroundDrawList();
			draw->AddCircle(ImVec2(2560 / 2, 1440         / 2), cfsize, IM_COL32(255, 0, 0, 255), 100, 1.0f);
			ImGui::End();
		}
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
	ImVec2 points[6] = { p1, p2, p3, p4, p5, p6 };
	ImGui::GetWindowDrawList()->AddPolyline(points, 6, col, ImDrawFlags_Closed, thickness);
}
void DrawHexagonFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col)
{
	ImVec2 points[6] = { p1, p2, p3, p4, p5, p6 };
	ImGui::GetWindowDrawList()->AddConvexPolyFilled(points, 6, col);
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