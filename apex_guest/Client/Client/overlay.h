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
#include "XorString.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#include <d3d10_1.h>
#include "D3DX10Math.h"
#pragma comment(lib, "d3d11.lib")

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 0, 0)
#define BLUE ImColor(0, 0, 255)
#define ORANGE ImColor(255, 165, 0)
#define WHITE ImColor(255, 255, 255)
#define TEAL ImColor(0, 128, 128)
#define YELLOW ImColor(255, 255, 0)

typedef struct visuals
{
	bool box = false;
	bool line = false;
	bool distance = true;
	bool healthbar = true;
	bool shieldbar = true;
	bool name = true;
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
	//Seer
	void DrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health);
private:
	bool running;
	HWND overlayHWND;
};
