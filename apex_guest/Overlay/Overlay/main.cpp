#include <Windows.h>
#include <chrono>
#include <Dwmapi.h> 
#pragma comment(lib, "dwmapi.lib")

#define WDA_EXCLUDEFROMCAPTURE 0x00000011
const MARGINS margins = { -1 ,-1, -1, -1 };
const wchar_t g_szClassName[] = L"serial-stringoverlay";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND serial - stringhwnd;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(RGB(0, 0, 0));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);

	serial - stringhwnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT,
		g_szClassName,
		g_szClassName,
		WS_POPUP | WS_VISIBLE,
		0, 0, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	SetLayeredWindowAttributes(serial - stringhwnd, RGB(0, 0, 0), 175, LWA_ALPHA);
	SetWindowDisplayAffinity(serial - stringhwnd, WDA_EXCLUDEFROMCAPTURE);
	DwmExtendFrameIntoClientArea(serial - stringhwnd, &margins);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	exit(0);
	return Msg.wParam;
}