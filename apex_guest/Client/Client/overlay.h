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

typedef struct changeableplacevisuals
{
	bool changeableplacebox = true;
	bool changeableplaceline = true;
	bool changeableplacedistance = true;
	bool changeableplacehealthbar = true;
	bool changeableplaceshieldbar = true;
	bool changeableplacename = true;
	bool changeableplacerenderxp = true;
}changeableplacevisuals;


class Overlay
{
public:
	void Startchangeableplace();
	DWORD CreatechangeableplaceOverlay();
	void Clearchangeableplace();
	int getchangeableplaceWidth();
	int getchangeableplaceHeight();
	void RenderchangeableplaceInfo();
	void RenderchangeableplaceMenu();
	void RenderchangeableplaceEsp();
	void ClickchangeableplaceThrough(bool v);
	void DrawchangeableplaceLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawchangeableplaceBox(ImColor color, float x, float y, float w, float h);
	void changeableplaceText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect);
	void RectchangeableplaceFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgresschangeableplaceBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor);
	void Stringchangeableplace(ImVec2 pos, ImColor color, const char* text);
	//Seer
	void DrawchangeableplaceHealth(float changeableplacex, float changeableplacey, int changeableplaceshield, int changeableplacemax_shield, int changeableplacearmorType, int changeableplacehealth);
private:
	bool changeableplacerunning;
	HWND overlaychangeableplaceHWND;
};