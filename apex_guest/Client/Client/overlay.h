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
#pragma comment(lib, "d3d11.lib")

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 75, 0)
#define BLUE ImColor(51, 153, 255)
#define ORANGE ImColor(255, 165, 0)
#define WHITE ImColor(255, 255, 255)
#define PURPLE ImColor(255, 0 , 255)

typedef struct visuals
{
	bool box = true;
	bool line = true;
	bool distance = true;
	bool healthbar = true;
	bool shieldbar = true;
	bool renderxp = true;
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
	void ProcessEsp();
	void RenderSpectator();
	void ClickThrough(bool v);
	void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawBox(ImColor color, float x, float y, float w, float h);
	void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect);
	void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect,int font);
	void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgressBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor);
	void String(ImVec2 pos, ImColor color, const char* text);
	void String(ImVec2 pos, ImColor color, const char* text, int font);
	//Seer
	void DrawHealth(float x, float y, int shield, int max_shield, int armorType, int health);
private:
	bool running;
	HWND overlayHWND;
};