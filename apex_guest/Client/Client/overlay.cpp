#include "overlay.h"

extern int placeholderaim;
extern bool placeholderesp;
extern bool placeholderaim_no_recoil;
extern bool placeholderready;
extern bool placeholderuse_nvidia;
extern float placeholdermax_dist;
extern float placeholderseer_dist;
extern float placeholdersmooth;
extern float placeholdermax_fov;
extern float placeholderxp_dist;
extern float placeholderscale;
extern int placeholderbone;
extern bool placeholderthirdperson;
extern float placeholdername_dist;
extern int placeholderspectators;
extern int placeholderallied_spectators;
extern bool placeholderchargerifle;
extern bool placeholderfreecam;
extern bool placeholderlockall_mode;
extern bool placeholderfiring_range;
extern int placeholderindex;
extern float placeholderrcs_pitch;
extern float placeholderrcs_yaw;
int placeholderwidth;
int placeholderheight;
bool placeholderk_leftclick = false;
bool placeholderk_ins = false;
bool placeholdershow_menu = false;
placeholdervisuals placeholderv;

extern bool IsKeyDown(int vk);

LONG placeholdernv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG placeholdernv_default_in_game = placeholdernv_default | WS_DISABLED;
LONG placeholdernv_edit = placeholdernv_default_in_game | WS_VISIBLE;

LONG placeholdernv_ex_default = WS_EX_TOOLWINDOW;
LONG placeholdernv_ex_edit = placeholdernv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
LONG placeholdernv_ex_edit_menu = placeholdernv_ex_default | WS_EX_TRANSPARENT;

static DWORD WINAPI StaticMessageStart(void* placeholderParam)
{
	Overlay* placeholderov = (Overlay*)placeholderParam;
	placeholderov->CreateplaceholderOverlay();
	return 0;
}

BOOL CALLBACK EnumWindowsCallback(HWND placeholderhwnd, LPARAM lParam)
{
	wchar_t classplaceholderName[255] = L"";
	GetClassName(placeholderhwnd, classplaceholderName, 255);
	if (placeholderuse_nvidia)
	{
		if (wcscmp(XorStrW(L"CEF-OSC-WIDGET"), classplaceholderName) == 0) //Nvidia overlay
		{
			HWND* w = (HWND*)lParam;
			if (GetWindowLong(placeholderhwnd, GWL_STYLE) != placeholdernv_default && GetWindowLong(placeholderhwnd, GWL_STYLE) != placeholdernv_default_in_game)
				return TRUE;
			*w = placeholderhwnd;
			return TRUE;
		}
	}
	else
	{
		if (wcscmp(XorStrW(L"placeholderoverlay"), classplaceholderName) == 0) //Custom overlay
		{
			HWND* w = (HWND*)lParam;
			*w = placeholderhwnd;
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

void Overlay::RenderplaceholderMenu()
{
	static bool placeholderaim_enable = false;
	static bool placeholdervis_check = false;
	static bool placeholderspec_disable = false;
	static bool placeholderall_spec_disable = false;

	if (placeholderaim > 0)
	{
		placeholderaim_enable = true;
		if (placeholderaim > 1)
		{
			placeholdervis_check = true;
		}
		else
		{
			placeholdervis_check = false;
		}
	}
	else
	{
		placeholderaim_enable = false;
		placeholdervis_check = false;
	}
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(490, 275));
	ImGui::Begin(XorStr("##title"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	if (ImGui::BeginTabBar(XorStr("Tab")))
	{
		if (ImGui::BeginTabItem(XorStr(u8"MainMenu")))
		{
			ImGui::Checkbox(XorStr(u8"ESP"), &placeholderesp);
			ImGui::Checkbox(XorStr(u8"Aimbot"), &placeholderaim_enable);

			if (placeholderaim_enable)
			{
				ImGui::SameLine();
				ImGui::Checkbox(XorStr(u8"Visible check"), &placeholdervis_check);
				ImGui::SameLine();
				ImGui::Checkbox(XorStr(u8"No recoil"), &placeholderaim_no_recoil);
				if (placeholdervis_check)
				{
					placeholderaim = 2;
				}
				else
				{
					placeholderaim = 1;
				}
			}
			else
			{
				placeholderaim = 0;
			}

			ImGui::Checkbox(XorStr(u8"freecamera"), &placeholderfreecam);
			ImGui::Checkbox(XorStr(u8"Lock on teammates"), &placeholderlockall_mode);
			ImGui::Checkbox(XorStr(u8"firing_range mode"), &placeholderfiring_range);//placeholderfiring_range
			//ImGui::InputInt(XorStr("index"), &placeholderindex);//placeholderindex
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr(u8"Aimbot")))
		{
			ImGui::Text(XorStr(u8"Max distence:"));
			ImGui::SliderFloat(XorStr("##1"), &placeholdermax_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(placeholdermax_dist / 40));
			ImGui::Text(u8"real dist :%d M", (int)(placeholdermax_dist / 40));

			ImGui::Text(XorStr(u8"Smooth"));
			ImGui::SliderFloat(XorStr("##2"), &placeholdersmooth, 76.0f, 100.0f, "%.2f");

			ImGui::Text(XorStr(u8"FOV:"));
			ImGui::SliderFloat(XorStr("##3"), &placeholdermax_fov, 5.0f, 250.0f, "%.2f");

			ImGui::Text(XorStr(u8"Bone:"));
			ImGui::SliderInt(XorStr("##4"), &placeholderbone, 0, 3);

			ImGui::Text(XorStr(u8"RCS PITCH:"));
			ImGui::SliderFloat(XorStr("##5"), &placeholderrcs_pitch, 0, 1);

			ImGui::Text(XorStr(u8"RCS YAW:"));
			ImGui::SliderFloat(XorStr("##6"), &placeholderrcs_yaw, 0, 1);

			//ImGui::Text(XorStr(u8"FOV Draw scale:"));
			//ImGui::SliderFloat(XorStr("##5"), &placeholderscale, 0.0f, 140.0f, "%.2f");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(XorStr(u8"ESPHACK")))
		{
			ImGui::Text(XorStr(u8"ESP"));
			//ImGui::Checkbox
			ImGui::Checkbox(XorStr(u8"BOX"), &placeholderv.placeholderbox);
			ImGui::SameLine(0, 70.0f);
			ImGui::Checkbox(XorStr(u8"Name"), &placeholderv.placeholdername);
			ImGui::Checkbox(XorStr(u8"xp level"), &placeholderv.placeholderrenderxp);
			ImGui::Checkbox(XorStr(u8"line"), &placeholderv.placeholderline);
			ImGui::Checkbox(XorStr(u8"dist + teamID"), &placeholderv.placeholderdistance);
			ImGui::Checkbox(XorStr(u8"seer esp"), &placeholderv.placeholderhealthbar);

			ImGui::Text(XorStr(u8"seer esp distence:"));
			ImGui::SliderFloat(XorStr("##1"), &placeholderseer_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(placeholderseer_dist / 40));

			ImGui::Text(XorStr(u8"XP level display distence:"));
			ImGui::SliderFloat(XorStr("##2"), &placeholderxp_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(placeholderxp_dist / 40));

			ImGui::Text(XorStr(u8"name esp distence:"));
			ImGui::SliderFloat(XorStr("##3"), &placeholdername_dist, 100.0f * 40, 800.0f * 40, "%.2f");
			ImGui::SameLine();
			ImGui::Text(u8"(%d M)", (int)(placeholdername_dist / 40));
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void Overlay::RenderplaceholderInfo()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(210, 25));
	ImGui::Begin(XorStr("##info"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	ImGui::TextColored(RED, u8"Enemies %d", placeholderspectators);
	ImGui::SameLine();
	ImGui::Text(" / ");
	ImGui::SameLine();
	ImGui::TextColored(GREEN, u8"Team %d", placeholderallied_spectators);
	ImGui::End();
}

void Overlay::ClickplaceholderThrough(bool placeholderv)
{
	if (placeholderv)
	{
		placeholdernv_edit = placeholdernv_default_in_game | WS_VISIBLE;
		if (GetWindowLong(overlayplaceholderHWND, GWL_EXSTYLE) != placeholdernv_ex_edit)
			SetWindowLong(overlayplaceholderHWND, GWL_EXSTYLE, placeholdernv_ex_edit);
	}
	else
	{
		placeholdernv_edit = placeholdernv_default | WS_VISIBLE;
		if (GetWindowLong(overlayplaceholderHWND, GWL_EXSTYLE) != placeholdernv_ex_edit_menu)
			SetWindowLong(overlayplaceholderHWND, GWL_EXSTYLE, placeholdernv_ex_edit_menu);
	}
}

DWORD Overlay::CreateplaceholderOverlay()
{
	EnumWindows(EnumWindowsCallback, (LPARAM)&overlayplaceholderHWND);
	Sleep(300);
	if (overlayplaceholderHWND == 0)
	{
		printf(XorStr("Can't find the overlay\n"));
		Sleep(1000);
		exit(0);
	}

	HDC hDC = ::GetWindowDC(NULL);
	placeholderwidth = ::GetDeviceCaps(hDC, HORZRES);
	placeholderheight = ::GetDeviceCaps(hDC, VERTRES);

	placeholderrunning = true;

	// Initialize Direct3D
	if (!CreateDeviceD3D(overlayplaceholderHWND))
	{
		CleanupDeviceD3D();
		return 1;
	}

	// Show the window
	::ShowWindow(overlayplaceholderHWND, SW_SHOWDEFAULT);
	::UpdateWindow(overlayplaceholderHWND);

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
	ImGui_ImplWin32_Init(overlayplaceholderHWND);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ClickplaceholderThrough(true);
	while (placeholderrunning)
	{
		HWND wnd = GetWindow(GetForegroundWindow(), GW_HWNDPREV);
		if (wnd != overlayplaceholderHWND)
		{
			SetWindowPos(overlayplaceholderHWND, wnd, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
			::UpdateWindow(overlayplaceholderHWND);
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

		if (IsKeyDown(VK_LBUTTON) && !placeholderk_leftclick)
		{
			io.MouseDown[0] = true;
			placeholderk_leftclick = true;
		}
		else if (!IsKeyDown(VK_LBUTTON) && placeholderk_leftclick)
		{
			io.MouseDown[0] = false;
			placeholderk_leftclick = false;
		}

		if (IsKeyDown(VK_INSERT) && !placeholderk_ins && placeholderready)
		{
			placeholdershow_menu = !placeholdershow_menu;
			ClickplaceholderThrough(!placeholdershow_menu);
			placeholderk_ins = true;
		}
		else if (!IsKeyDown(VK_INSERT) && placeholderk_ins)
		{
			placeholderk_ins = false;
		}
		if (placeholdershow_menu) {
			RenderplaceholderMenu();
		}
		else {
			RenderplaceholderInfo();
			RenderplaceholderSpectator();
		}

		RenderplaceholderEsp();

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
	ClickplaceholderThrough(true);

	CleanupDeviceD3D();
	::DestroyWindow(overlayplaceholderHWND);
	return 0;
}

void Overlay::Startplaceholder()
{
	DWORD ThreadID;
	CreateThread(NULL, 0, StaticMessageStart, (void*)this, 0, &ThreadID);
}

void Overlay::Clearplaceholder()
{
	placeholderrunning = 0;
	Sleep(50);
}

int Overlay::getplaceholderWidth()
{
	return placeholderwidth;
}

int Overlay::getplaceholderHeight()
{
	return placeholderheight;
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

void Overlay::DrawplaceholderLine(ImVec2 a, ImVec2 b, ImColor color, float width)
{
	ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
}

void Overlay::DrawplaceholderBox(ImColor color, float x, float y, float w, float h)
{
	DrawplaceholderLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.0f);
	DrawplaceholderLine(ImVec2(x, y), ImVec2(x, y + h), color, 1.0f);
	DrawplaceholderLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.0f);
	DrawplaceholderLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.0f);
}

void Overlay::placeholderText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect)
{
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 14, pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
}

void Overlay::placeholderText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect , int font)
{
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), font, pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
}

void Overlay::Stringplaceholder(ImVec2 pos, ImColor color, const char* text)
{
	placeholderText(pos, color, text, text + strlen(text), 200, 0);
}

void Overlay::Stringplaceholder(ImVec2 pos, ImColor color, const char* text,int font)
{
	placeholderText(pos, color, text, text + strlen(text), 200, 0 , font);
}

void Overlay::RectplaceholderFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

void Overlay::ProgressplaceholderBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor)
{
	RectplaceholderFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor, 0.0f, 0);
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

void Overlay::DrawplaceholderHealth(float placeholderx, float placeholdery, int placeholdershield, int placeholdermax_shield, int placeholderarmorType, int placeholderhealth) {

	int placeholderbg_offset = 3;
	int placeholderbar_width = 105; //158
	// 4steps...2*3=6
	// 38*4=152 152+6 = 158
	// 5steps...2*4=8
	// 30*5=150 150+8 = 158
	float placeholdermax_health = 100.0f;//100
	float placeholdershield_step = 25.0f; //25

	int placeholdershield_25 = 14; //30
	int placeholdersteps = 5;


	ImVec2 bg1(placeholderx - placeholderbar_width / 2 - placeholderbg_offset, placeholdery);
	ImVec2 bg2(bg1.x - 10, bg1.y - 16);
	ImVec2 bg3(bg2.x + 5, bg2.y - 7);
	ImVec2 bg4(bg3.x + placeholderbar_width + placeholderbg_offset, bg3.y);
	ImVec2 bg5(bg4.x + 11, bg4.y + 18);
	ImVec2 bg6(placeholderx + placeholderbar_width / 2 + placeholderbg_offset, placeholdery);
	DrawHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


	ImVec2 h1(bg1.x + 3, bg1.y - 4);
	ImVec2 h2(h1.x - 5, h1.y - 8);
	ImVec2 h3(h2.x + (float)placeholderhealth / placeholdermax_health * placeholderbar_width, h2.y);
	ImVec2 h4(h1.x + (float)placeholderhealth / placeholdermax_health * placeholderbar_width, h1.y);
	ImVec2 h3m(h2.x + placeholderbar_width, h2.y);
	ImVec2 h4m(h1.x + placeholderbar_width, h1.y);
	DrawQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
	DrawQuadFilled(h1, h2, h3, h4, WHITE);


	ImColor shieldCracked(97, 97, 97);
	ImColor shieldCrackedDark(67, 67, 67);

	ImColor shieldCol;
	ImColor shieldColDark; //not used, but the real seer q has shadow inside
	if (placeholdermax_shield == 50) { //white
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	else if (placeholdermax_shield == 75) { //blue
		shieldCol = ImColor(39, 178, 255);
		shieldColDark = ImColor(27, 120, 210);
	}
	else if (placeholdermax_shield == 100) { //purple
		shieldCol = ImColor(206, 59, 255);
		shieldColDark = ImColor(136, 36, 220);
	}
	else if (placeholdermax_shield == 100) { //gold
		shieldCol = ImColor(255, 255, 79);
		shieldColDark = ImColor(218, 175, 49);
	}
	else if (placeholdermax_shield == 125) { //red
		shieldCol = ImColor(219, 2, 2);
		shieldColDark = ImColor(219, 2, 2);
	}
	else {
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	int shield_tmp = placeholdershield;
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
	ImVec2 s3(s2.x + shield1 / placeholdershield_step * placeholdershield_25, s2.y);
	ImVec2 s4(s1.x + shield1 / placeholdershield_step * placeholdershield_25, s1.y);
	ImVec2 s3m(s2.x + placeholdershield_25, s2.y);
	ImVec2 s4m(s1.x + placeholdershield_25, s1.y);

	ImVec2 ss1(s4m.x + 2, s1.y);
	ImVec2 ss2(s3m.x + 2, s2.y);
	ImVec2 ss3(ss2.x + shield2 / placeholdershield_step * placeholdershield_25, s2.y);
	ImVec2 ss4(ss1.x + shield2 / placeholdershield_step * placeholdershield_25, s1.y);
	ImVec2 ss3m(ss2.x + placeholdershield_25, s2.y);
	ImVec2 ss4m(ss1.x + placeholdershield_25, s1.y);

	ImVec2 sss1(ss4m.x + 2, s1.y);
	ImVec2 sss2(ss3m.x + 2, s2.y);
	ImVec2 sss3(sss2.x + shield3 / placeholdershield_step * placeholdershield_25, s2.y);
	ImVec2 sss4(sss1.x + shield3 / placeholdershield_step * placeholdershield_25, s1.y);
	ImVec2 sss3m(sss2.x + placeholdershield_25, s2.y);
	ImVec2 sss4m(sss1.x + placeholdershield_25, s1.y);

	ImVec2 ssss1(sss4m.x + 2, s1.y);
	ImVec2 ssss2(sss3m.x + 2, s2.y);
	ImVec2 ssss3(ssss2.x + shield4 / placeholdershield_step * placeholdershield_25, s2.y);
	ImVec2 ssss4(ssss1.x + shield4 / placeholdershield_step * placeholdershield_25, s1.y);
	ImVec2 ssss3m(ssss2.x + placeholdershield_25, s2.y);
	ImVec2 ssss4m(ssss1.x + placeholdershield_25, s1.y);

	ImVec2 sssss1(ssss4m.x + 2, s1.y);
	ImVec2 sssss2(ssss3m.x + 2, s2.y);
	ImVec2 sssss3(sssss2.x + shield5 / placeholdershield_step * placeholdershield_25, s2.y);
	ImVec2 sssss4(sssss1.x + shield5 / placeholdershield_step * placeholdershield_25, s1.y);
	ImVec2 sssss3m(sssss2.x + placeholdershield_25, s2.y);
	ImVec2 sssss4m(sssss1.x + placeholdershield_25, s1.y);
	if (placeholdermax_shield == 50) {
		if (placeholdershield <= 25) {
			if (placeholdershield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (placeholdershield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (placeholdershield != 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
	}
	else if (placeholdermax_shield == 75) {
		if (placeholdershield <= 25) {
			if (placeholdershield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (placeholdershield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (placeholdershield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (placeholdershield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (placeholdershield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
	}
	else if (placeholdermax_shield == 100) {
		if (placeholdershield <= 25) {
			if (placeholdershield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (placeholdershield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (placeholdershield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (placeholdershield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (placeholdershield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (placeholdershield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (placeholdershield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
	}
	else if (placeholdermax_shield == 125) {
		if (placeholdershield <= 25) {
			if (placeholdershield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (placeholdershield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (placeholdershield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (placeholdershield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (placeholdershield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (placeholdershield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (placeholdershield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
		else if (placeholdershield <= 125) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
			if (placeholdershield < 125) {
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (placeholdershield != 0)
				DrawQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
		}
	}
}