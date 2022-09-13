#pragma once

#include <cheat/internal/core.hpp>
#include <cheat/internal/utility.hpp>
#include <cheat/internal/updater/updater.hpp>
#include <cheat/internal/actor/actors.hpp>

#include <impl/render/font.h>

ID3D11Device *device = nullptr;
ID3D11DeviceContext *immediateContext = nullptr;
ID3D11RenderTargetView *renderTargetView = nullptr;
static IDXGISwapChain *g_pSwapChain;


HRESULT( *presenth )( IDXGISwapChain *swapChain, UINT syncInterval, UINT flags ) = nullptr;
HRESULT( *resizeh )( IDXGISwapChain *swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags ) = nullptr;
WNDPROC oriWndProc = NULL;
HWND hwnd = NULL;

void Insert()
{
	if ( NtGetAsyncKeyState( VK_INSERT ) & 0x8000 )
	{
		cfg::menu = !cfg::menu;
	}	
	if ( NtGetAsyncKeyState( VK_DELETE ) & 0x8000 )
	{
		unload = !unload;
	}
}

void pkRender()
{
	ImGuiStyle *style = &ImGui::GetStyle();
	ImVec4 *colors = style->Colors;

	//editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	//editor.SetPalette(TextEditor::GetDarkPalette());
	//editor.SetShowDefines(false);

	style->WindowRounding = 0;
	style->WindowTitleAlign = ImVec2( 0.01, 0.5 );

	style->GrabRounding = 1;
	style->GrabMinSize = 20;
	style->FrameRounding = 0;
	style->FramePadding = ImVec2( 5, 5 );
	style->TouchExtraPadding = ImVec2( 5, 5 );
	style->WindowPadding = ImVec2( 5, 5 );
	style->DisplaySafeAreaPadding = ImVec2( 5, 5 );
	style->DisplayWindowPadding = ImVec2( 5, 5 );

	colors[ImGuiCol_Text] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_TextDisabled] = ImVec4( 0.00f, 0.40f, 0.41f, 1.00f );
	colors[ImGuiCol_WindowBg] = ImVec4( 0.021f, 0.021f, 0.021f, 1.00f );
	colors[ImGuiCol_ChildWindowBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ImGuiCol_PopupBg] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_Border] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ImGuiCol_FrameBg] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.44f, 0.80f, 0.80f, 0.27f );
	colors[ImGuiCol_FrameBgActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_TitleBg] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f, 0.00f, 0.00f, 0.54f );
	colors[ImGuiCol_TitleBgActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_MenuBarBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.20f );
	colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.125f, 0.125f, 0.125f, 0.36f );
	colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.125f, 0.125f, 0.125f, 0.36f );
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_CheckMark] = ImVec4( 1.00f, 1.00f, 1.00f, 0.68f );
	colors[ImGuiCol_SliderGrab] = ImVec4( 1.00f, 1.00f, 1.00f, 0.36f );
	colors[ImGuiCol_SliderGrabActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.76f );
	colors[ImGuiCol_Button] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_ButtonHovered] = ImVec4( 0.125f, 0.125f, 0.125f, 0.36f );
	colors[ImGuiCol_ButtonActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_Header] = ImVec4( 1.00f, 1.00f, 1.00f, 0.33f );
	colors[ImGuiCol_HeaderHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.42f );
	colors[ImGuiCol_HeaderActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.54f );
	colors[ImGuiCol_Column] = ImVec4( 0.00f, 0.50f, 0.50f, 0.33f );
	colors[ImGuiCol_ColumnHovered] = ImVec4( 0.00f, 0.50f, 0.50f, 0.47f );
	colors[ImGuiCol_ColumnActive] = ImVec4( 0.00f, 0.70f, 0.70f, 1.00f );
	colors[ImGuiCol_ResizeGrip] = ImVec4( 1.00f, 1.00f, 1.00f, 0.54f );
	colors[ImGuiCol_ResizeGripHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.74f );
	colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotLines] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotHistogram] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4( 0.04f, 0.10f, 0.09f, 0.51f );
	
	// removed watermark

	if ( cfg::fov_circle )
		Circle( Width / 2, Height / 2, cfg::fov, { 255, 255, 255, 255 }, 150 );

	ImGui::SetWindowSize( ImVec2( 398.000, 477.000 ) );

	if ( cfg::menu )
	{

		// removed menu & P2C name
		if ( ImGui::Begin( x( "UC pasta" ), NULL, ImGuiWindowFlags_NoResize) )
		{
		}
	}
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	utility *utility {};

	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) && menu )
	{
		return true;
	}

	return utility->call( CallWindowProcW, oriWndProc, hWnd, msg, wParam, lParam );
}


HRESULT pkHook( IDXGISwapChain *swapchain, UINT sync, UINT flags )
{
	utility *utility { };

	g_pSwapChain = swapchain;

	if ( !device )
	{

		ID3D11Texture2D *renderTarget = 0;
		ID3D11Texture2D *backBuffer = 0;
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		swapchain->GetDevice( __uuidof( device ), ( PVOID * ) &device );
		device->GetImmediateContext( &immediateContext );

		swapchain->GetBuffer( 0, __uuidof( renderTarget ), ( PVOID * ) &renderTarget );
		device->CreateRenderTargetView( renderTarget, nullptr, &renderTargetView );
		renderTarget->Release();
		swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( PVOID * ) &backBuffer );
		backBuffer->GetDesc( &backBufferDesc );

		hwnd = utility->call( FindWindowW, ( L"Respawn001" ), ( L"Apex Legends" ) );
		backBuffer->Release();
		ImFontConfig font_config;
		font_config.OversampleH = 1; 
		font_config.OversampleV = 1;
		font_config.PixelSnapH = 1;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x044F, // Cyrillic
			0,
		};

		ImGuiIO &io = ImGui::GetIO(); ( void ) io;
		//m_pFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF( ( void * ) menufont, sizeof( menufont ), 15, &font_config, ranges );
		m_pFont = io.Fonts->AddFontFromFileTTF( x( "C:\\Windows\\Fonts\\Arial.ttf" ), 15.0f );//ImGui::GetIO().Fonts->AddFontFromMemoryTTF( ( void * ) menufont, sizeof( menufont ), 15, &font_config, ranges );
		esp_pFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF( ( void * ) apexEngineFont, sizeof( apexEngineFont ), 15, &font_config, ranges );

		if ( m_pFont == NULL or esp_pFont == NULL)
		{

			utility->call( MessageBoxA, ( HWND ) 0, ( LPCSTR ) xorstr( "Failed to load font from memory" ), ( LPCSTR ) 0, ( UINT ) 0 );
			utility->call( exit, 0 );
		}

		ImGui_ImplDX11_Init( hwnd, device, immediateContext );
		ImGui_ImplDX11_CreateDeviceObjects();

		Width = ( float ) backBufferDesc.Width;
		Height = ( float ) backBufferDesc.Height;

	}
	immediateContext->OMSetRenderTargets( 1, &renderTargetView, nullptr );
	auto &window = createscene();


	//unloadCheat();
	pkRender();

	cWorld->SetCameraMatrix();

	cPlayerLoop();
	cItemLoop();

    cObjectLoop();
	//Insert();

	destroyscene( window ); //Destroy


	return utility->call( c_original, swapchain, sync, flags );
}