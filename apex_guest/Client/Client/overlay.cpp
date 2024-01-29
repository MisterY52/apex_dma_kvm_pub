#include "overlay.h"

extern int E01C6Daim;
extern bool E01C6Desp;
extern bool E01C6Daim_no_recoil;
extern bool E01C6Dready;
extern bool E01C6Duse_nvidia;
extern float E01C6Dmax_dist;
extern float E01C6Dseer_dist;
extern float E01C6Dsmooth;
extern float E01C6Dmax_fov;
extern float E01C6Dxp_dist;
extern float E01C6Dscale;
extern int E01C6Dbone;
extern bool E01C6Dthirdperson;
extern int E01C6Dspectators;
extern int E01C6Dallied_spectators;
extern bool E01C6Dchargerifle;
extern bool E01C6Dfreecam;
extern bool E01C6Dlockall_mode;
extern bool E01C6Dstart_rank_process;
extern bool E01C6Dfiring_range;
extern int E01C6Dindex;
extern float E01C6Drcs_pitch;
extern float E01C6Drcs_yaw;
extern bool E01C6Drefresh; 
extern bool E01C6Drender_plist;
int E01C6Dwidth;
int E01C6Dheight;
bool E01C6Dk_leftclick = false;
bool E01C6Dk_ins = false;
bool E01C6Dshow_menu = false;
E01C6Dvisuals E01C6Dv;

extern bool IsKeyDown(int vk);

LONG E01C6Dnv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG E01C6Dnv_default_in_game = E01C6Dnv_default | WS_DISABLED;
LONG E01C6Dnv_edit = E01C6Dnv_default_in_game | WS_VISIBLE;

LONG E01C6Dnv_ex_default = WS_EX_TOOLWINDOW;
LONG E01C6Dnv_ex_edit = E01C6Dnv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
LONG E01C6Dnv_ex_edit_menu = E01C6Dnv_ex_default | WS_EX_TRANSPARENT;

static DWORD WINAPI StaticMessageStart(void* E01C6DParam)
{
	Overlay* E01C6Dov = (Overlay*)E01C6DParam;
	E01C6Dov->CreateE01C6DOverlay();
	return 0;
}

BOOL CALLBACK EnumWindowsCallback(HWND E01C6Dhwnd, LPARAM lParam)
{
	wchar_t classE01C6DName[255] = L"";
	GetClassName(E01C6Dhwnd, classE01C6DName, 255);
	if (E01C6Duse_nvidia)
	{
		if (wcscmp(XorStrW(L"CEF-OSC-WIDGET"), classE01C6DName) == 0) //Nvidia overlay
		{
			HWND* w = (HWND*)lParam;
			if (GetWindowLong(E01C6Dhwnd, GWL_STYLE) != E01C6Dnv_default && GetWindowLong(E01C6Dhwnd, GWL_STYLE) != E01C6Dnv_default_in_game)
				return TRUE;
			*w = E01C6Dhwnd;
			return TRUE;
		}
	}
	else
	{
		if (wcscmp(XorStrW(L"E01C6Doverlay"), classE01C6DName) == 0) //Custom overlay
		{
			HWND* w = (HWND*)lParam;
			*w = E01C6Dhwnd;
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

void Overlay::RenderE01C6DMenu()
{
	static bool E01C6Daim_enable = false;
	static bool E01C6Dvis_check = false;
	static bool E01C6Dspec_disable = false;
	static bool E01C6Dall_spec_disable = false;

	if (E01C6Daim > 0)
	{
		E01C6Daim_enable = true;
		if (E01C6Daim > 1)
		{
			E01C6Dvis_check = true;
		}
		else
		{
			E01C6Dvis_check = false;
		}
	}
	else
	{
		E01C6Daim_enable = false;
		E01C6Dvis_check = false;
	}
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(490, 325));
	ImGui::Begin(XorStr("##title"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	if (ImGui::BeginTabBar(XorStr("Tab")))
	{
		if (ImGui::BeginTabItem(XorStr(u8"MainMenu")))
		{
			ImGui::Checkbox(XorStr(u8"ESP"), &E01C6Desp);
			ImGui::Checkbox(XorStr(u8"Aimbot"), &E01C6Daim_enable);

			if (E01C6Daim_enable)
			{
				ImGui::SameLine();
				ImGui::Checkbox(XorStr(u8"Visible check"), &E01C6Dvis_check);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr(u8"No recoil"), &E01C6Daim_no_recoil);
				if (E01C6Dvis_check)
				{
					E01C6Daim = 2;
				}
				else
				{
					E01C6Daim = 1;
				}
			}
			else
			{
				E01C6Daim = 0;
			}

			ImGui::Checkbox(XorStr(u8"freecamera"), &E01C6Dfreecam);
			ImGui::Checkbox(XorStr(u8"Lock on teammates"), &E01C6Dlockall_mode);
			ImGui::Checkbox(XorStr(u8"firing_range mode"), &E01C6Dfiring_range);//E01C6Dfiring_range
			//ImGui::InputInt(XorStr("index"), &E01C6Dindex);//E01C6Dindex
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr(u8"Aimbot")))
		{
			ImGui::Text(XorStr(u8"Max distence:"));
			ImGui::SliderFloat(XorStr("##1"), &E01C6Dmax_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(E01C6Dmax_dist / 40));
			ImGui::Text(u8"real dist :%d M", (int)(E01C6Dmax_dist / 40));

			ImGui::Text(XorStr(u8"Smooth"));
			ImGui::SliderFloat(XorStr("##2"), &E01C6Dsmooth, 76.0f, 100.0f, "%.2f");

			ImGui::Text(XorStr(u8"FOV:"));
			ImGui::SliderFloat(XorStr("##3"), &E01C6Dmax_fov, 5.0f, 250.0f, "%.2f");

			ImGui::Text(XorStr(u8"Bone:"));
			ImGui::SliderInt(XorStr("##4"), &E01C6Dbone, 0, 3);

			ImGui::Text(XorStr(u8"RCS PITCH:"));
			ImGui::SliderFloat(XorStr("##5"), &E01C6Drcs_pitch, 0, 1);

			ImGui::Text(XorStr(u8"RCS YAW:"));
			ImGui::SliderFloat(XorStr("##6"), &E01C6Drcs_yaw, 0, 1);

			//ImGui::Text(XorStr(u8"FOV Draw scale:"));
			//ImGui::SliderFloat(XorStr("##5"), &E01C6Dscale, 0.0f, 140.0f, "%.2f");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr(u8"ESPHACK")))
		{
			ImGui::Text(XorStr(u8"ESP"));
			//ImGui::Checkbox
			ImGui::Checkbox(XorStr(u8"BOX"), &E01C6Dv.E01C6Dbox);
			ImGui::SameLine(0, 70.0f);
			ImGui::Checkbox(XorStr(u8"xp level"), &E01C6Dv.E01C6Drenderxp);
			ImGui::Checkbox(XorStr(u8"line"), &E01C6Dv.E01C6Dline);
			ImGui::Checkbox(XorStr(u8"dist + teamID"), &E01C6Dv.E01C6Ddistance);
			ImGui::Checkbox(XorStr(u8"seer esp"), &E01C6Dv.E01C6Dhealthbar);

			ImGui::Text(XorStr(u8"seer esp distence:"));
			ImGui::SliderFloat(XorStr("##1"), &E01C6Dseer_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(E01C6Dseer_dist / 40));

			ImGui::Text(XorStr(u8"XP level display distence:"));
			ImGui::SliderFloat(XorStr("##2"), &E01C6Dxp_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(E01C6Dxp_dist / 40));

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr(u8"DEBUG")))
		{
			ImGui::Text(XorStr(u8"DEBUG & PlayerList"));
			if (ImGui::Button(u8"Refresh", ImVec2(40, 20)))
			{
				E01C6Drefresh = true;
				Sleep(200);
				E01C6Drefresh = false;
			}ImGui::SameLine();
			if (ImGui::Button(u8"GetRank", ImVec2(55, 20)))
			{
				E01C6Dstart_rank_process = true;
				Sleep(300);
				E01C6Dstart_rank_process = false;
			}
			ImGui::Checkbox(XorStr(u8"Display list(Replace SpectatorList)"), &E01C6Drender_plist);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::Text(XorStr("Overlay FPS: %.3f ms/frame (%.1f FPS)"), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void Overlay::RenderE01C6DInfo()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(210, 25));
	ImGui::Begin(XorStr("##info"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImGui::TextColored(RED, u8"Enemies %d", E01C6Dspectators);
	ImGui::SameLine();
	ImGui::Text(" / ");
	ImGui::SameLine();
	ImGui::TextColored(GREEN, u8"Team %d", E01C6Dallied_spectators);
	ImGui::End();
}

void Overlay::ClickE01C6DThrough(bool E01C6Dv)
{
	if (E01C6Dv)
	{
		E01C6Dnv_edit = E01C6Dnv_default_in_game | WS_VISIBLE;
		if (GetWindowLong(overlayE01C6DHWND, GWL_EXSTYLE) != E01C6Dnv_ex_edit)
			SetWindowLong(overlayE01C6DHWND, GWL_EXSTYLE, E01C6Dnv_ex_edit);
	}
	else
	{
		E01C6Dnv_edit = E01C6Dnv_default | WS_VISIBLE;
		if (GetWindowLong(overlayE01C6DHWND, GWL_EXSTYLE) != E01C6Dnv_ex_edit_menu)
			SetWindowLong(overlayE01C6DHWND, GWL_EXSTYLE, E01C6Dnv_ex_edit_menu);
	}
}

DWORD Overlay::CreateE01C6DOverlay()
{
	EnumWindows(EnumWindowsCallback, (LPARAM)&overlayE01C6DHWND);
	//Sleep(300);
	if (overlayE01C6DHWND == 0)
	{
		printf(XorStr("Can't find the overlay\n"));
		Sleep(1000);
		exit(0);
	}

	HDC hDC = ::GetWindowDC(NULL);
	E01C6Dwidth = ::GetDeviceCaps(hDC, HORZRES);
	E01C6Dheight = ::GetDeviceCaps(hDC, VERTRES);

	E01C6Drunning = true;

	// Initialize Direct3D
	if (!CreateDeviceD3D(overlayE01C6DHWND))
	{
		CleanupDeviceD3D();
		return 1;
	}

	// Show the window
	::ShowWindow(overlayE01C6DHWND, SW_SHOWDEFAULT);
	::UpdateWindow(overlayE01C6DHWND);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("C://windows//fonts/simhei.ttf", 12.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	ImGui::GetIO().FontDefault = font;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowMinSize = ImVec2(1, 1);

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(overlayE01C6DHWND);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ClickE01C6DThrough(true);
	while (E01C6Drunning)
	{
		HWND wnd = GetWindow(GetForegroundWindow(), GW_HWNDPREV);
		if (wnd != overlayE01C6DHWND)
		{
			SetWindowPos(overlayE01C6DHWND, wnd, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
			::UpdateWindow(overlayE01C6DHWND);
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

		if (IsKeyDown(VK_LBUTTON) && !E01C6Dk_leftclick)
		{
			io.MouseDown[0] = true;
			E01C6Dk_leftclick = true;
		}
		else if (!IsKeyDown(VK_LBUTTON) && E01C6Dk_leftclick)
		{
			io.MouseDown[0] = false;
			E01C6Dk_leftclick = false;
		}

		if (IsKeyDown(VK_INSERT) && !E01C6Dk_ins && E01C6Dready)
		{
			E01C6Dshow_menu = !E01C6Dshow_menu;
			ClickE01C6DThrough(!E01C6Dshow_menu);
			E01C6Dk_ins = true;
		}
		else if (!IsKeyDown(VK_INSERT) && E01C6Dk_ins)
		{
			E01C6Dk_ins = false;
		}
		if (E01C6Dshow_menu) {
			RenderE01C6DMenu();
		}
		else {
			RenderE01C6DInfo();
			RenderE01C6DSpectator();
		}

		ProcessE01C6DEsp();

		// Rendering
		ImGui::EndFrame();
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

<<<<<<< HEAD
		g_pSwapChain->Present(0, 0); // Present with vsync

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
=======
		g_pSwapChain->Present(0, 0); // Modified to increase FPS

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));// Modified to increase FPS
>>>>>>> 046f9c6ef56140fc611ab4d38f4c9f0426c98dfd
	}
	ClickE01C6DThrough(true);

	CleanupDeviceD3D();
	::DestroyWindow(overlayE01C6DHWND);
	return 0;
}

void Overlay::StartE01C6D()
{
	DWORD ThreadID;
	CreateThread(NULL, 0, StaticMessageStart, (void*)this, 0, &ThreadID);
}

void Overlay::ClearE01C6D()
{
	E01C6Drunning = 0;
	Sleep(1);
}

int Overlay::getE01C6DWidth()
{
	return E01C6Dwidth;
}

int Overlay::getE01C6DHeight()
{
	return E01C6Dheight;
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

void Overlay::DrawE01C6DLine(ImVec2 a, ImVec2 b, ImColor color, float width)
{
	ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
}

void Overlay::DrawE01C6DBox(ImColor color, float x, float y, float w, float h)
{
	DrawE01C6DLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.0f);
	DrawE01C6DLine(ImVec2(x, y), ImVec2(x, y + h), color, 1.0f);
	DrawE01C6DLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.0f);
	DrawE01C6DLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.0f);
}

void Overlay::E01C6DText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect)
{
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 14, pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
}

void Overlay::E01C6DText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect , int font)
{
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), font, pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
}

void Overlay::StringE01C6D(ImVec2 pos, ImColor color, const char* text)
{
	E01C6DText(pos, color, text, text + strlen(text), 200, 0);
}

void Overlay::StringE01C6D(ImVec2 pos, ImColor color, const char* text,int font)
{
	E01C6DText(pos, color, text, text + strlen(text), 200, 0 , font);
}

void Overlay::RectE01C6DFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

void Overlay::ProgressE01C6DBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor)
{
	RectE01C6DFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor, 0.0f, 0);
}


//seer esp
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

void Overlay::DrawE01C6DHealth(float E01C6Dx, float E01C6Dy, int E01C6Dshield, int E01C6Dmax_shield, int E01C6DarmorType, int E01C6Dhealth) {

	int E01C6Dbg_offset = 3;
	int E01C6Dbar_width = 105; //158
	// 4steps...2*3=6
	// 38*4=152 152+6 = 158
	// 5steps...2*4=8
	// 30*5=150 150+8 = 158
	float E01C6Dmax_health = 100.0f;//100
	float E01C6Dshield_step = 25.0f; //25

	int E01C6Dshield_25 = 14; //30
	int E01C6Dsteps = 5;


	ImVec2 bg1(E01C6Dx - E01C6Dbar_width / 2 - E01C6Dbg_offset, E01C6Dy);
	ImVec2 bg2(bg1.x - 10, bg1.y - 16);
	ImVec2 bg3(bg2.x + 5, bg2.y - 7);
	ImVec2 bg4(bg3.x + E01C6Dbar_width + E01C6Dbg_offset, bg3.y);
	ImVec2 bg5(bg4.x + 11, bg4.y + 18);
	ImVec2 bg6(E01C6Dx + E01C6Dbar_width / 2 + E01C6Dbg_offset, E01C6Dy);
	DrawHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


	ImVec2 h1(bg1.x + 3, bg1.y - 4);
	ImVec2 h2(h1.x - 5, h1.y - 8);
	ImVec2 h3(h2.x + (float)E01C6Dhealth / E01C6Dmax_health * E01C6Dbar_width, h2.y);
	ImVec2 h4(h1.x + (float)E01C6Dhealth / E01C6Dmax_health * E01C6Dbar_width, h1.y);
	ImVec2 h3m(h2.x + E01C6Dbar_width, h2.y);
	ImVec2 h4m(h1.x + E01C6Dbar_width, h1.y);
	DrawQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
	DrawQuadFilled(h1, h2, h3, h4, WHITE);


	ImColor shieldCracked(97, 97, 97);
	ImColor shieldCrackedDark(67, 67, 67);

	ImColor shieldCol;
	ImColor shieldColDark; //not used, but the real seer q has shadow inside
	if (E01C6Dmax_shield == 50) { //white
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	else if (E01C6Dmax_shield == 75) { //blue
		shieldCol = ImColor(39, 178, 255);
		shieldColDark = ImColor(27, 120, 210);
	}
	else if (E01C6Dmax_shield == 100) { //purple
		shieldCol = ImColor(206, 59, 255);
		shieldColDark = ImColor(136, 36, 220);
	}
	else if (E01C6Dmax_shield == 100) { //gold
		shieldCol = ImColor(255, 255, 79);
		shieldColDark = ImColor(218, 175, 49);
	}
	else if (E01C6Dmax_shield == 125) { //red
		shieldCol = ImColor(219, 2, 2);
		shieldColDark = ImColor(219, 2, 2);
	}
	else {
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	int shield_tmp = E01C6Dshield;
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
	ImVec2 s3(s2.x + shield1 / E01C6Dshield_step * E01C6Dshield_25, s2.y);
	ImVec2 s4(s1.x + shield1 / E01C6Dshield_step * E01C6Dshield_25, s1.y);
	ImVec2 s3m(s2.x + E01C6Dshield_25, s2.y);
	ImVec2 s4m(s1.x + E01C6Dshield_25, s1.y);

	ImVec2 ss1(s4m.x + 2, s1.y);
	ImVec2 ss2(s3m.x + 2, s2.y);
	ImVec2 ss3(ss2.x + shield2 / E01C6Dshield_step * E01C6Dshield_25, s2.y);
	ImVec2 ss4(ss1.x + shield2 / E01C6Dshield_step * E01C6Dshield_25, s1.y);
	ImVec2 ss3m(ss2.x + E01C6Dshield_25, s2.y);
	ImVec2 ss4m(ss1.x + E01C6Dshield_25, s1.y);

	ImVec2 sss1(ss4m.x + 2, s1.y);
	ImVec2 sss2(ss3m.x + 2, s2.y);
	ImVec2 sss3(sss2.x + shield3 / E01C6Dshield_step * E01C6Dshield_25, s2.y);
	ImVec2 sss4(sss1.x + shield3 / E01C6Dshield_step * E01C6Dshield_25, s1.y);
	ImVec2 sss3m(sss2.x + E01C6Dshield_25, s2.y);
	ImVec2 sss4m(sss1.x + E01C6Dshield_25, s1.y);

	ImVec2 ssss1(sss4m.x + 2, s1.y);
	ImVec2 ssss2(sss3m.x + 2, s2.y);
	ImVec2 ssss3(ssss2.x + shield4 / E01C6Dshield_step * E01C6Dshield_25, s2.y);
	ImVec2 ssss4(ssss1.x + shield4 / E01C6Dshield_step * E01C6Dshield_25, s1.y);
	ImVec2 ssss3m(ssss2.x + E01C6Dshield_25, s2.y);
	ImVec2 ssss4m(ssss1.x + E01C6Dshield_25, s1.y);

	ImVec2 sssss1(ssss4m.x + 2, s1.y);
	ImVec2 sssss2(ssss3m.x + 2, s2.y);
	ImVec2 sssss3(sssss2.x + shield5 / E01C6Dshield_step * E01C6Dshield_25, s2.y);
	ImVec2 sssss4(sssss1.x + shield5 / E01C6Dshield_step * E01C6Dshield_25, s1.y);
	ImVec2 sssss3m(sssss2.x + E01C6Dshield_25, s2.y);
	ImVec2 sssss4m(sssss1.x + E01C6Dshield_25, s1.y);
	if (E01C6Dmax_shield == 50) {
		if (E01C6Dshield <= 25) {
			if (E01C6Dshield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (E01C6Dshield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (E01C6Dshield != 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
	}
	else if (E01C6Dmax_shield == 75) {
		if (E01C6Dshield <= 25) {
			if (E01C6Dshield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (E01C6Dshield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (E01C6Dshield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (E01C6Dshield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (E01C6Dshield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
	}
	else if (E01C6Dmax_shield == 100) {
		if (E01C6Dshield <= 25) {
			if (E01C6Dshield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (E01C6Dshield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (E01C6Dshield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (E01C6Dshield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (E01C6Dshield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (E01C6Dshield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (E01C6Dshield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
	}
	else if (E01C6Dmax_shield == 125) {
		if (E01C6Dshield <= 25) {
			if (E01C6Dshield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (E01C6Dshield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (E01C6Dshield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (E01C6Dshield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (E01C6Dshield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (E01C6Dshield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (E01C6Dshield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
		else if (E01C6Dshield <= 125) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
			if (E01C6Dshield < 125) {
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (E01C6Dshield != 0)
				DrawQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
		}
	}
}
