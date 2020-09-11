#pragma once

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

class Direct
{
public:
	IDirect3D9Ex* dx_Object;
	IDirect3DDevice9Ex* dx_Device;
	D3DPRESENT_PARAMETERS dx_Params;
	LPD3DXLINE dx_line;
	LPD3DXFONT dx_font;

	static bool D3DInit(HWND hwnd,Direct& result);
	void D3DShutdown();
	void Fill(int x, int y, int w, int h, int r, int g, int b, int a);
	void DrawBox(int x,int y,int x2,int y2, int r, int g, int b, int a);
	void DrawBox2(int x, int y, int width, int height, int r, int g, int b, int a);
	void DrawString(int x, int y, int a, int r, int g, int b, LPCWSTR text);
	void DrawLine(float x, float y, float xx, float yy, int a, int r, int g, int b);
};