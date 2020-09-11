#include "DirectX.h"

bool Direct::D3DInit(HWND hwnd,Direct& result)
{
	Direct res = Direct();

    if(FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &res.dx_Object)))
        return false;

	res.dx_Params.hDeviceWindow			= hwnd;
	res.dx_Params.MultiSampleQuality	= D3DMULTISAMPLE_NONE;
	res.dx_Params.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	res.dx_Params.BackBufferCount		= 1;
	res.dx_Params.BackBufferFormat		= D3DFMT_A8R8G8B8;
	res.dx_Params.BackBufferWidth		= 0;
	res.dx_Params.BackBufferHeight		= 0;
	res.dx_Params.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
	res.dx_Params.Windowed				= true;

    if(FAILED(res.dx_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &res.dx_Params, NULL, &res.dx_Device)))
        return false;

	D3DXCreateLine(res.dx_Device, &res.dx_line);
	if (FAILED(D3DXCreateFontA(res.dx_Device, 13, 0, 400, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Courier", &res.dx_font))) {
		return false;
	}

	result = res;
    return true;
}

void Direct::D3DShutdown()
{
	if(dx_Device != NULL)
		dx_Device->Release();
	dx_Device = NULL;
	if(dx_Object != NULL)
		dx_Object->Release();
	dx_Object = NULL;
}

void Direct::Fill(int x, int y, int w, int h, int r, int g, int b, int a)
{
	D3DCOLOR col = D3DCOLOR_ARGB(a,r,g,b);
	D3DRECT rec = { x, y, x+w, y+h };
	dx_Device->Clear(1, &rec, D3DCLEAR_TARGET, col, 0, 0);
}

void Direct::DrawBox(int x,int y,int x2,int y2, int r, int g, int b, int a)
{
	Fill(x-1, y-1, x2+2, 1, r, g, b, a);
	Fill(x-1, y, 1, y2-1, r, g, b, a);
	Fill(x+x2, y, 1, y2-1, r, g, b, a);
	Fill(x-1, y+y2-1, x2+2, 1, r, g, b, a);
}

void Direct::DrawBox2(int x, int y, int width, int height, int r, int g, int b, int a)
{
	D3DCOLOR color = D3DCOLOR_ARGB(a, r, g, b);
	D3DCOLOR clear = D3DCOLOR_ARGB(0, 0, 0, 0);
	D3DRECT rec_paint = { x, y, x+ width, y+ height };
	D3DRECT rec_clear = { x + 1, y + 1, x + width - 1, y + height - 1 };
	dx_Device->Clear(1, &rec_paint, D3DCLEAR_TARGET, color, 0, 0);
	dx_Device->Clear(1, &rec_clear, D3DCLEAR_TARGET, clear, 0, 0);

}

void Direct::DrawString(int x, int y,int a,int r,int g,int b, LPCWSTR text)
{
	D3DCOLOR col = D3DCOLOR_ARGB(a, r, g, b);
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+wcslen(text) * 10;
	rect.bottom = y+30;
	dx_font->DrawTextW(NULL, text, -1, &rect, DT_LEFT, col);
}

void Direct::DrawLine(float x, float y, float xx, float yy, int a,int r,int g,int b) {
	D3DXVECTOR2 line[2];
	dx_line->SetWidth(1);
	line[0] = D3DXVECTOR2(x, y);
	line[1] = D3DXVECTOR2(xx, yy);
	dx_line->Draw(line, 2, D3DCOLOR_ARGB(a, r, g, b));
}