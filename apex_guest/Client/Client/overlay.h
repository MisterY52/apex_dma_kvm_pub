#pragma once


#include <Windows.h>
#include <WinUser.h>
#include <Dwmapi.h> 
#pragma comment(lib, "dwmapi.lib")
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <cwchar>
#include <thread>
#include <string>
#include <codecvt>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")
#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 0, 0)
#define BLUE ImColor(0, 0, 255)
#define ORANGE ImColor(255, 165, 0)
#define WHITE ImColor(255, 255, 255)

struct Process_Informations
{
	HWND overlayHWND;
};

typedef struct visuals
{
	bool box = true;
	bool line = true;
	bool distance = true;
	bool healthbar = true;
	bool shieldbar = true;
}visuals;

class Overlay
{
public:
	void Start();
	DWORD CreateOverlay();
	void Clear();
	int getWidth();
	int getHeight();
	void RenderInfo();
	void RenderMenu();
	void RenderEsp();
	void ClickThrough(bool v);
	void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawBox(ImColor color, float x, float y, float w, float h);
	void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect);
	void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgressBar(float x, float y, float w, float h, int value, int v_max);
	void String(ImVec2 pos, ImColor color, const char* text);
private:
	bool running;
	Process_Informations proc;
};

