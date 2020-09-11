#include "overlay.h"

extern int aim;
extern bool esp;
extern bool item_glow;
extern bool active;
extern bool use_nvidia;
extern int safe_level;
extern int spectators;
extern int allied_spectators;
extern float max_dist;
int width;
int height;

LONG nv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG nv_default_in_game = nv_default | WS_DISABLED;
LONG nv_edit = nv_default_in_game | WS_VISIBLE;

LONG nv_ex_default = WS_EX_TOOLWINDOW;
LONG nv_ex_edit = nv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;

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
		if (wcscmp(L"CEF-OSC-WIDGET", className) == 0) //Nvidia overlay
		{
			Process_Informations* proc = (Process_Informations*)lParam;
			if (GetWindowLong(hwnd, GWL_STYLE) != nv_default && GetWindowLong(hwnd, GWL_STYLE) != nv_default_in_game)
				return TRUE;
			proc->overlayHWND = hwnd;
			return TRUE;
		}
	}
	else
	{
		if (wcscmp(L"overlay", className) == 0) //Custom overlay
		{
			Process_Informations* proc = (Process_Informations*)lParam;
			proc->overlayHWND = hwnd;
			return TRUE;
		}
	}
	return TRUE;
}

DWORD Overlay::CreateOverlay()
{
	EnumWindows(EnumWindowsCallback, (LPARAM)&proc);
	Sleep(300);
	if (proc.overlayHWND == 0)
	{
		printf("Can't find the overlay\n");
		Sleep(1000);
		exit(0);
	}

	HDC hDC = ::GetWindowDC(NULL);
	width = ::GetDeviceCaps(hDC, HORZRES);
	height = ::GetDeviceCaps(hDC, VERTRES);

	if (!Direct::D3DInit(proc.overlayHWND, CurrentDirectX))
	{
		exit(0);
	}
		
	running = 1;
	int rs = 255;
	int gs = 255;
	int bs = 255;
	LPCWSTR txt = L"F5: ESP";
	LPCWSTR txt2 = L"F6: AIM";
	LPCWSTR txt3 = L"F8: ITEMS";

	while (running)
	{
		if (CurrentDirectX.dx_Device != 0)
		{
			std::wstring title = L"title (" + std::to_wstring(spectators) + L" - " + std::to_wstring(allied_spectators) + L")";
			std::wstring max_d = L"MAX DST: " + std::to_wstring((int)(max_dist / 40.0f));
			try 
			{
				HWND wnd = GetWindow(GetForegroundWindow(), GW_HWNDPREV);
				if (use_nvidia)
				{
					if(GetWindowLong(proc.overlayHWND, GWL_STYLE) != nv_edit)
						SetWindowLong(proc.overlayHWND, GWL_STYLE, nv_edit);
					if(GetWindowLong(proc.overlayHWND, GWL_STYLE) != nv_ex_edit)
						SetWindowLong(proc.overlayHWND, GWL_EXSTYLE, nv_ex_edit);
				}
				if (wnd != proc.overlayHWND)
				{
					SetWindowPos(proc.overlayHWND, wnd, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
					UpdateWindow(proc.overlayHWND);
				}
				CurrentDirectX.dx_Device->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, NULL);
				CurrentDirectX.dx_Device->BeginScene();
				switch (safe_level)
				{
				case 0:
					rs = 255;
					gs = 0;
					bs = 0;
					break;
				case 1:
					rs = 255;
					gs = 215;
					bs = 0;
					break;
				case 2:
					rs = 0;
					gs = 200;
					bs = 0;
					break;
				default:
					break;
				}
				CurrentDirectX.DrawString(10, 10, 255, rs, gs, bs, (LPCWSTR)title.c_str());

				if (esp)
				{
					CurrentDirectX.DrawString(10, 25, 255, 0, 255, 0, txt); //green
				}
				else
				{
					CurrentDirectX.DrawString(10, 25, 255, 255, 0, 0, txt); //red
				}

				if (aim==2)
				{
					std::wstring v = L"F6: AIM (V)";
					CurrentDirectX.DrawString(10, 38, 255, 0, 255, 0, (LPCWSTR)v.c_str()); //green
				}
				else if(aim==1)
				{
					CurrentDirectX.DrawString(10, 38, 255, 0, 255, 0, txt2); //green
				}
				else
				{
					CurrentDirectX.DrawString(10, 38, 255, 255, 0, 0, txt2); //red
				}
				
				if (item_glow)
				{
					CurrentDirectX.DrawString(10, 51, 255, 0, 255, 0, txt3); //green
				}
				else
				{
					CurrentDirectX.DrawString(10, 51, 255, 255, 0, 0, txt3); //red
				}
				
				CurrentDirectX.DrawString(10, 51, 255, 255, 215, 0, (LPCWSTR)max_d.c_str());
				Render();
				CurrentDirectX.dx_Device->EndScene();
				CurrentDirectX.dx_Device->Present(NULL, NULL, NULL, NULL);			
			}
			catch (...) {
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}

void Overlay::Start()
{
	DWORD ThreadID;
	CreateThread(NULL, 0, StaticMessageStart, (void*)this, 0, &ThreadID);
}

void Overlay::Render()
{
	(*(RenderCallbacks))(this);
}

void Overlay::SetRender(RenderCallback v)
{
	RenderCallbacks = v;
}

bool Overlay::isRunning()
{
	return running;
}

void Overlay::Clear()
{
	running = 0;
	Sleep(50);
	if (use_nvidia)
	{
		SetWindowLong(proc.overlayHWND, GWL_STYLE, nv_default);
		SetWindowLong(proc.overlayHWND, GWL_EXSTYLE, nv_ex_default);
	}
	CurrentDirectX.D3DShutdown();
}

int Overlay::getWidth()
{
	return width;
}

int Overlay::getHeight()
{
	return height;
}