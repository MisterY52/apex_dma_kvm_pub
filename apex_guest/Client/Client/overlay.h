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

typedef struct placeholdervisuals
{
	bool placeholderbox = true;
	bool placeholderline = true;
	bool placeholderdistance = true;
	bool placeholderhealthbar = true;
	bool placeholdershieldbar = true;
	bool placeholdername = true;
	bool placeholderrenderxp = true;
}placeholdervisuals;


class Overlay
{
public:
	void Startplaceholder();
	DWORD CreateplaceholderOverlay();
	void Clearplaceholder();
	int getplaceholderWidth();
	int getplaceholderHeight();
	void RenderplaceholderInfo();
	void RenderplaceholderMenu();
	void RenderplaceholderEsp();
	void RenderplaceholderSpectator();
	void ClickplaceholderThrough(bool v);
	void DrawplaceholderLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawplaceholderBox(ImColor color, float x, float y, float w, float h);
	void placeholderText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect);
	void placeholderText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect,int font);
	void RectplaceholderFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgressplaceholderBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor);
	void Stringplaceholder(ImVec2 pos, ImColor color, const char* text);
	void Stringplaceholder(ImVec2 pos, ImColor color, const char* text, int font);
	//Seer
	void DrawplaceholderHealth(float placeholderx, float placeholdery, int placeholdershield, int placeholdermax_shield, int placeholderarmorType, int placeholderhealth);
private:
	bool placeholderrunning;
	HWND overlayplaceholderHWND;
};