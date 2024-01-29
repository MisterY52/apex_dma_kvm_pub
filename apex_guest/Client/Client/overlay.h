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

typedef struct E01C6Dvisuals
{
	bool E01C6Dbox = true;
	bool E01C6Dline = true;
	bool E01C6Ddistance = true;
	bool E01C6Dhealthbar = true;
	bool E01C6Dshieldbar = true;
	bool E01C6Drenderxp = true;
}E01C6Dvisuals;


class Overlay
{
public:
	void StartE01C6D();
	DWORD CreateE01C6DOverlay();
	void ClearE01C6D();
	int getE01C6DWidth();
	int getE01C6DHeight();
	void RenderE01C6DInfo();
	void RenderE01C6DMenu();
	void RenderE01C6DEsp();
	void ProcessE01C6DEsp();
	void RenderE01C6DSpectator();
	void ClickE01C6DThrough(bool v);
	void DrawE01C6DLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawE01C6DBox(ImColor color, float x, float y, float w, float h);
	void E01C6DText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect);
	void E01C6DText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect,int font);
	void RectE01C6DFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgressE01C6DBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor);
	void StringE01C6D(ImVec2 pos, ImColor color, const char* text);
	void StringE01C6D(ImVec2 pos, ImColor color, const char* text, int font);
	//Seer
	void DrawE01C6DHealth(float E01C6Dx, float E01C6Dy, int E01C6Dshield, int E01C6Dmax_shield, int E01C6DarmorType, int E01C6Dhealth);
private:
	bool E01C6Drunning;
	HWND overlayE01C6DHWND;
};