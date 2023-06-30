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

typedef struct serial - stringvisuals
{
	bool serial - stringbox = true;
	bool serial - stringline = true;
	bool serial - stringdistance = true;
	bool serial - stringhealthbar = true;
	bool serial - stringshieldbar = true;
	bool serial - stringname = true;
	bool serial - stringrenderrank = true;
}serial - stringvisuals;


class Overlay
{
public:
	void Startserial - string();
	DWORD Createserial - stringOverlay();
	void Clearserial - string();
	int getserial - stringWidth();
	int getserial - stringHeight();
	void Renderserial - stringInfo();
	void Renderserial - stringMenu();
	void Renderserial - stringEsp();
	void Clickserial - stringThrough(bool v);
	void Drawserial - stringLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void Drawserial - stringBox(ImColor color, float x, float y, float w, float h);
	void serial - stringText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect);
	void Rectserial - stringFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void Progressserial - stringBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor);
	void Stringserial - string(ImVec2 pos, ImColor color, const char* text);
	//Seer
	void Drawserial - stringHealth(float serial - stringx, float serial - stringy, int serial - stringshield, int serial - stringmax_shield, int serial - stringarmorType, int serial - stringhealth);
private:
	bool serial - stringrunning;
	HWND overlayserial - stringHWND;
};