#pragma once

#include <algorithm>
#include <sstream>

typedef int ( WINAPI *keyword )( DWORD );
static keyword NtGetAsyncKeyState;

typedef HRESULT( *present )( IDXGISwapChain *, UINT, UINT );
inline present c_original {  };

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,		// A fake button which is 'pressed' and 'released' when the wheel is moved up 
	MOUSE_WHEEL_DOWN,	// A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,
};
typedef unsigned long CRC32_t;

struct CUserCmd
{
	int32_t command_number; //0x0000 
	int32_t tick_count; //0x0004 
	float_t curtime; //0x0008 
	D3DXVECTOR2 viewangles; //0x000C 
	char pad_0x0018[0xC]; //0x0018
	float_t forwardmove; //0x0024 
	float_t sidemove; //0x0028 
	float_t upmove; //0x002C 
	int32_t buttons; //0x0030 
	char pad_0x0034[0x150]; //0x0034
	int32_t randomseed; //0x0184 
	char pad_0x0188[0x8]; //0x0188
	D3DXVECTOR2 headposition; //0x0190 
	float_t maxpitch; //0x019C 
	char pad_0x01A0[0xE0]; //0x01A0
};//size 0x0280;

class CInput
{
public:
	virtual void Function0(); // 0
	virtual void Function1(); // 8
	virtual void Function2(); // 10
	virtual void Function3(); // 18
	virtual void Function4(); // 20
	virtual void Function5(); // 28
	virtual void Function6(); // 30
	virtual void Function7(); // 38
	virtual void Function8(); // 40
	virtual CUserCmd *GetUserCmd( int uk, int sequence_number ); //	48

	class CVerifiedUserCmd
	{
	public:
		CUserCmd	    m_cmd;
		CRC32_t         m_crc;
	};
};




typedef __int64( __stdcall *_CreateMove )( CInput *pInput, int sequence_number, float input_sample_frametime, bool active );
 _CreateMove OLD_CreateMove;

 //typedef __int64( __thiscall *_GetChecksum )( void *pThis );
 uintptr_t checksum;

float valuee = 1.0f;
static float Height = 0;
static float Width = 0;

ImFont *m_pFont;
ImFont *esp_pFont;

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

inline float dist( const D3DXVECTOR3 p1, const D3DXVECTOR3 p2 )
{
	float x = p1.x - p2.x;
	float y = p1.y - p2.y;
	float z = p1.z - p2.z;
	return sqrt( x * x + y * y + z * z );
}

bool unload = false;

static void HelpMarker( const char *desc )
{
	ImGui::TextDisabled( x("(?)") );
	if ( ImGui::IsItemHovered() )
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
		ImGui::TextUnformatted( desc );
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

namespace cfg
{
	bool menu;
	
	bool no_recoil = true;
	bool silentaim = false;
	bool skydivespeed = true;

	bool skin_changer = false;
	bool third_person = false;
	float weapon_skin = 0.0f;
	float player_skin = 0.0f;
	float player_body = 0.0f;

	bool Radar = true;

	bool esp_enabled = true;
	bool box = true;
	bool lines = false;
	bool distance = true;
	bool name = true;
	bool shield = false;
	bool health = false;
	bool aimbotvischeck = false;
	bool skeleton = true;
	bool chams = true;
	bool knocked_check = true;
	bool visible_check = true;
	bool weapon_held = true;
	float rcsIntensity = 75.f; // recoil control system intensity in %

	float esp_distance = 300.0f;

	bool aimbot_enabled = true;
	bool fov_circle = true;
	float fov = 200.0f;
	float skydive_speed = 1500.0f;
	float smoothness = 3.0f;
}

namespace objectCfg
{
	bool objects_esp = true;
	float renderDistance = 300.0f;

	bool deathbox = false;
	bool vault_key_roller = false;
	bool throwables = false;
	bool decoycheck = false;
	bool vehicle = false;
	bool ziplines = false;
}

namespace itemCfg
{
	bool item_esp_enabled = false;
	float renderDistance = 300.0f;

	bool heals = false;
	bool guns = false;
	bool attachment = false;
	bool ammo = false;
	bool scopes = false;
	bool throwables = false;
	bool armor = false;
	bool backpacks = false;
	bool evoarmor = false;
	bool knockdownshield = false;
	bool helmets = false;

	namespace rareCheck
	{
		bool common = true;
		bool rare = true;
		bool epic = true;
		bool legendary = true;
		bool evo = true;
		bool Heirloom = true;
	}

	static bool selectedRarity[6];
	static bool selectedTypes[11];

	static const char *Rarity[] = { "Common", "Rare", "Epic", "Legendary", "Evo", "Heirloom" };
	static const char *Type[] = { "Heal", "Guns", "Attachments", "Ammo", "Scopes", "Throwables", "Armor", "Evo Armor", "Knockdown Shield", "Backpacks", "Helmets"};
}

struct Color
{
	float r, g, b;
	Color operator+( Color addedColor )
	{
		return { r + addedColor.r, g + addedColor.g, b + addedColor.b };
	}

	Color operator-( Color addedColor )
	{
		return { r - addedColor.r, g - addedColor.g, b - addedColor.b };
	}

	Color operator*( float multiplier )
	{
		return { r * multiplier, g * multiplier, b * multiplier };
	}

	Color operator/( float dividend )
	{
		return { r / dividend, g / dividend, b / dividend };
	}
	Color floor()
	{
		return { floorf( r ), floorf( g ), floorf( b ) };
	}
};

namespace detail
{
	extern "C" void *_spoofer_stub();
	template <typename Ret, typename... Args>
	static inline auto shellcode_stub_helper(
		const void *shell,
		Args... args
	) -> Ret
	{
		auto fn = ( Ret( * )( Args... ) )( shell );
		return fn( args... );
	}
	template <std::size_t Argc, typename>
	struct argument_remapper
	{
		template<
			typename Ret,
			typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack
		>
			static auto do_call( const void *shell, void *shell_param, First first, Second second,
				Third third, Fourth fourth, Pack... pack ) -> Ret
		{
			return shellcode_stub_helper< Ret, First, Second, Third, Fourth, void *, void *, Pack... >( shell, first, second, third, fourth, shell_param, nullptr, pack... );
		}
	};
	template <std::size_t Argc>
	struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		template<
			typename Ret,
			typename First = void *,
			typename Second = void *,
			typename Third = void *,
			typename Fourth = void *
		>
			static auto do_call(
				const void *shell,
				void *shell_param,
				First first = First {},
				Second second = Second {},
				Third third = Third {},
				Fourth fourth = Fourth {}
			) -> Ret
		{
			return shellcode_stub_helper<
				Ret,
				First,
				Second,
				Third,
				Fourth,
				void *,
				void *
			>(
				shell,
				first,
				second,
				third,
				fourth,
				shell_param,
				nullptr
				);
		}
	};
}
inline uint64_t SkrrAdrrs = ( uint64_t ) GetModuleHandle( NULL );

ImGuiWindow &createscene()
{
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
	ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 0 ) );
	ImGui::Begin( xorstr( "##createscene" ), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar );
	auto &io = ImGui::GetIO();
	ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
	ImGui::SetWindowSize( ImVec2( io.DisplaySize.x, io.DisplaySize.y ), ImGuiCond_Always );
	return *ImGui::GetCurrentWindow();
}



VOID destroyscene( ImGuiWindow &window )
{

	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar( 2 );
	ImGui::Render();
}

class utility
{
public:
	/* Getkeystate */


	bool debug;

	template <typename Ret, typename... Args>
	inline auto call( Ret( *fn )( Args... ), Args... args ) -> Ret
	{
		static const void *jmprbx = nullptr;
		if ( !jmprbx )
		{
			const auto ntdll = reinterpret_cast< const unsigned char * >( SkrrAdrrs );
			const auto dos = reinterpret_cast< const IMAGE_DOS_HEADER * >( ntdll );
			const auto nt = reinterpret_cast< const IMAGE_NT_HEADERS * >( ntdll + dos->e_lfanew );
			const auto sections = IMAGE_FIRST_SECTION( nt );
			const auto num_sections = nt->FileHeader.NumberOfSections;
			constexpr char section_name[5] { '.', 't', 'e', 'x', 't' };
			const auto     section = std::find_if( sections, sections + num_sections, [&] ( const auto &s )
			{
				return std::equal( s.Name, s.Name + 5, section_name );
			} );
			constexpr unsigned char instr_bytes[2] { 0xFF, 0x26 };
			const auto              va = ntdll + section->VirtualAddress;
			jmprbx = std::search( va, va + section->Misc.VirtualSize, instr_bytes, instr_bytes + 2 );
		}
		struct shell_params
		{
			const void *trampoline;
			void *function;
			void *rdx;
		};
		shell_params p
		{
			jmprbx,
			reinterpret_cast< void * >( fn )
		};
		using mapper = detail::argument_remapper<sizeof...( Args ), void>;
		return mapper::template do_call<Ret, Args...>( ( const void * ) &detail::_spoofer_stub, &p, args... );
	}

	inline const void *gadget( wchar_t *moduleName )
	{
		static const void *jmprbx = nullptr;
		if ( !jmprbx )
		{
			const auto ntdll = reinterpret_cast< const unsigned char * >( ::GetModuleHandleW( moduleName ) );
			const auto dos = reinterpret_cast< const IMAGE_DOS_HEADER * >( ntdll );
			const auto nt = reinterpret_cast< const IMAGE_NT_HEADERS * >( ntdll + dos->e_lfanew );
			const auto sections = IMAGE_FIRST_SECTION( nt );
			const auto num_sections = nt->FileHeader.NumberOfSections;

			constexpr char section_name[5] { '.', 't', 'e', 'x', 't' };
			const auto     section = std::find_if( sections, sections + num_sections, [&] ( const auto &s )
			{
				return std::equal( s.Name, s.Name + 5, section_name );
			} );

			constexpr unsigned char instr_bytes[2] { 0xFF, 0x26 };
			const auto              va = ntdll + section->VirtualAddress;
			jmprbx = std::search( va, va + section->Misc.VirtualSize, instr_bytes, instr_bytes + 2 );
		}
		return jmprbx;
	}

	BOOL isValid( DWORD64 address )
	{
		if ( !call( IsBadWritePtr, ( LPVOID ) address, ( UINT_PTR ) 8 ) ) return TRUE;
		else return FALSE;

	}

};

void ReadMemory( const void *address, void *buffer, size_t size )
{
	utility *utils { };

	DWORD back = NULL;

	DWORD instruction = 0x04;

	if ( utils->call( VirtualProtect, ( LPVOID ) address, size, instruction, &back ) )
	{
		utils->call( memcpy, buffer, address, size );

		utils->call( VirtualProtect, ( LPVOID ) address, size, back, &back );
	}
}

std::string ReadASCII128( uint64_t adr )
{
	char buf[128];
	ReadMemory( ( const void * ) adr, &buf, 128 );
	return buf;
}



template<typename T>
T Read( DWORD_PTR address, const T &def = T() )
{
	utility *utilities { };

	if ( utilities->isValid( address ) )
		return *( T * ) address;
	else
		return T();
}

std::string ReadASCII( uint64_t adr )
{
	utility *util { };
	char buf[64];
	if ( util->isValid( adr ))
	{
		ReadMemory( ( const void * ) adr, &buf, 64 );
		return buf;
	}

	else
	{
		return "";
	}
}

template<typename T>
T Write( DWORD_PTR address, DWORD_PTR value, const T &def = T() )
{
	utility *utilities { };
//	utilities *utilities {  };


	if ( utilities->isValid( address ) )
		return *( T * ) address = value;
	else
		return T();
}
void  WINAPIV DebugOut( const TCHAR *fmt, ... )
{
	utility *utility { };
	TCHAR  s[1025];
	va_list  args;
	va_start( args, fmt );
	wvsprintf( s, fmt, args );
	va_end( args );
	utility->call( OutputDebugString, ( LPCSTR ) s );
}

#define apex_output(a) DebugOut( x("apex-sdk : %d : %s"), __LINE__, a);

//#define apex_output(a) /*utility::call(MessageBoxA, (HWND)0, (LPCSTR)a, (LPCSTR)0, (UINT)0);*/ DebugOut( x("[apexsdk] %s : %d"), __LINE__, a);



#define RELATIVE_ADDR(addr, size) ((uint64_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

namespace scanner
{
	uint64_t find( uint64_t pModuleBaseAddress, const char *sSignature, size_t nSelectResultIndex = 0 );
	uint64_t find( const char *sSignature, int rva = 0, size_t nSelectResultIndex = 0 );
	uint64_t find( uint64_t pModuleBaseAddress, const char *sSignature, size_t nSelectResultIndex )
	{
		static auto patternToByte = [] ( const char *pattern )
		{
			auto       bytes = std::vector<int> {};
			const auto start = const_cast< char * >( pattern );
			const auto end = const_cast< char * >( pattern ) + strlen( pattern );
			for ( auto current = start; current < end; ++current )
			{
				if ( *current == '?' )
				{
					++current;
					if ( *current == '?' )
						++current;
					bytes.push_back( -1 );
				}
				else
					bytes.push_back( strtoul( current, &current, 16 ) );
			}
			return bytes;
		};

		const auto dosHeader = ( PIMAGE_DOS_HEADER ) pModuleBaseAddress;
		const auto ntHeaders = ( PIMAGE_NT_HEADERS ) ( ( std::uint8_t * ) pModuleBaseAddress + dosHeader->e_lfanew );
		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto       patternBytes = patternToByte( sSignature );
		const auto scanBytes = reinterpret_cast< std::uint8_t * >( pModuleBaseAddress );
		const auto s = patternBytes.size();
		const auto d = patternBytes.data();
		size_t nFoundResults = 0;

		for ( auto i = 0ul; i < sizeOfImage - s; ++i )
		{
			bool found = true;

			for ( auto j = 0ul; j < s; ++j )
			{
				if ( scanBytes[i + j] != d[j] && d[j] != -1 )
				{
					found = false;
					break;
				}
			}

			if ( found )
			{
				if ( nSelectResultIndex != 0 )
				{
					if ( nFoundResults < nSelectResultIndex )
					{
						nFoundResults++;
						found = false;
					}
					else
						return reinterpret_cast< uint64_t >( &scanBytes[i] );
				}
				else
					return reinterpret_cast< uint64_t >( &scanBytes[i] );
			}
		}
		return NULL;
	}

	uint64_t find( const char *sSignature, int rva, size_t nSelectResultIndex )
	{
		utility *utility { };
		static bool bIsSetted = false;
		static MODULEINFO info = { 0 };
		if ( !bIsSetted )
		{

			//GetModuleInformation, ( HANDLE ) GetCurrentProcess(), ( HMODULE ) GetModuleHandle( NULL ), ( LPMODULEINFO ) &info, ( DWORD ) sizeof( info )
			utility->call( GetModuleInformation, (HANDLE)GetCurrentProcess(), ( HMODULE )GetModuleHandle(NULL), ( LPMODULEINFO )&info, ( DWORD )sizeof( info ));

			bIsSetted = true;
		}

		if ( rva != 0 )
		{
			return RELATIVE_ADDR( find( ( uint64_t ) info.lpBaseOfDll, sSignature, nSelectResultIndex ), rva );
		}
		else if ( rva == 0 )
			return find( ( uint64_t ) info.lpBaseOfDll, sSignature, nSelectResultIndex );
	}
}

typedef struct
{

	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;


std::string string_To_UTF8( const std::string &str )
{
	int nwLen = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

	wchar_t *pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory( pwBuf, nwLen * 2 + 2 );

	::MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen );

	int nLen = ::WideCharToMultiByte( CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL );

	char *pBuf = new char[nLen + 1];
	ZeroMemory( pBuf, nLen + 1 );

	::WideCharToMultiByte( CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL );

	std::string retStr( pBuf );

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

/* RGBA Structure (Red, Green, Blue, Alpha) */

static void FilledRectangle( int x, int y, int w, int h, RGBA color )
{
	ImGui::GetOverlayDrawList()->AddRectFilled( ImVec2( x, y ), ImVec2( x + w, y + h ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), 0, 0 );
}

void Box( int x, int y, int w, int h, RGBA color )
{
	ImGui::GetOverlayDrawList()->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), 0, 0 );
}

void Rectangle( int x, int y, int w, int h, RGBA color, int thickness )
{
	ImGui::GetOverlayDrawList()->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), 0, 0, thickness );
}

void FilledCircle( int x, int y, int radius, RGBA color, int segments )
{
	ImGui::GetOverlayDrawList()->AddCircleFilled( ImVec2( x, y ), radius, ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), segments );
}

void Circle( int x, int y, int radius, RGBA color, int segments )
{
	ImGui::GetOverlayDrawList()->AddCircle( ImVec2( x, y ), radius, ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), segments );
}

void Triangle( int x1, int y1, int x2, int y2, int x3, int y3, RGBA color, float thickne )
{
	ImGui::GetOverlayDrawList()->AddTriangle( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), thickne );
}

void FilledTriangle( int x1, int y1, int x2, int y2, int x3, int y3, RGBA color )
{
	ImGui::GetOverlayDrawList()->AddTriangleFilled( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ) );
}

void Line( int x1, int y1, int x2, int y2, RGBA color, int thickness )
{
	if ( x2 != 0 && y2 != 0 )
	{
		ImGui::GetOverlayDrawList()->AddLine( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), thickness );
	}
}

void BottomHealthBar( int x, int y, int w, int h, int borderPx, RGBA color )
{
	FilledRectangle( x + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w - w + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
}

void CornerBox( int x, int y, int w, int h, int borderPx, RGBA color )
{
	int WidthThird = w / 4;
	int HeightThird = WidthThird;

	//topleft 
	FilledRectangle( x - w / 2 - ( 1 ), y - ( 1 ), WidthThird + 2, 3, { 0, 0,0, 255 } ); //h
	FilledRectangle( x - w / 2 - ( 1 ), y - ( 1 ), 3, HeightThird + 2, { 0, 0,0, 255 } ); //v

	//topright
	FilledRectangle( ( x + w / 2 ) - WidthThird - ( 1 ), y - ( 1 ), WidthThird + 2, 3, { 0, 0,0, 255 } ); //h
	FilledRectangle( x + w / 2 - ( 1 ), y - ( 1 ), 3, HeightThird + 2, { 0, 0,0, 255 } ); //v

	//bottom left
	FilledRectangle( x - w / 2 - ( 1 ), y + h - 4, WidthThird + 2, 3, { 0, 0,0, 255 } ); //h
	FilledRectangle( x - w / 2 - ( 1 ), ( y + h ) - HeightThird - 4, 3, HeightThird + 2, { 0, 0,0, 255 } );//v

	//bottom right
	FilledRectangle( ( x + w / 2 ) - WidthThird - ( 1 ), y + h - 4, WidthThird + 2, 3, { 0, 0,0, 255 } );//h
	FilledRectangle( x + w / 2 - ( 1 ), ( y + h ) - HeightThird - 4, 3, HeightThird + 3, { 0, 0,0, 255 } ); //v

	/* draw eight lines */
	//topleft 
	FilledRectangle( x - w / 2, y, WidthThird, 1, color ); //h
	FilledRectangle( x - w / 2, y, 1, HeightThird, color ); //v

	//topright
	FilledRectangle( ( x + w / 2 ) - WidthThird, y, WidthThird, 1, color ); //h
	FilledRectangle( x + w / 2, y, 1, HeightThird, color ); //v

	//bottom left
	FilledRectangle( x - w / 2, y + h - 3, WidthThird, 1, color ); //h
	FilledRectangle( x - w / 2, ( y + h ) - HeightThird - 3, 1, HeightThird, color );//v

	//bottom right
	FilledRectangle( ( x + w / 2 ) - WidthThird, y + h - 3, WidthThird, 1, color );//h
	FilledRectangle( x + w / 2, ( y + h ) - HeightThird - 3, 1, HeightThird + 1, color ); //v		
}

void BorderBox( int x, int y, int x2, int y2, int thickness, RGBA color )
{
	FilledRectangle( x, y, x2, thickness, color ); // top
	FilledRectangle( x, y + y2, x2, thickness, color ); // bottom
	FilledRectangle( x, y, thickness, y2, color ); // left
	FilledRectangle( x + x2, y, thickness, y2 + thickness, color ); // right
}

void NormalBox( int x, int y, int w, int h, int borderPx, RGBA color )
{
	FilledRectangle( x + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x + w - w + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x, y, borderPx, h, color ); //left 
	FilledRectangle( x, y + h - h + borderPx * 2, borderPx, h, color ); //left 
	FilledRectangle( x + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w - w + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w + borderPx, y, borderPx, h, color );//right 
	FilledRectangle( x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color );//right 
}

void NormalFilledBox( int x, int y, int w, int h, int borderPx, RGBA color )
{
	FilledRectangle( x + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x + w - w + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x, y, borderPx, h, color ); //left 
	FilledRectangle( x, y + h - h + borderPx * 2, borderPx, h, color ); //left 
	FilledRectangle( x + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w - w + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w + borderPx, y, borderPx, h, color );//right 
	FilledRectangle( x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color );//right 
}
void SideHealthBar( int x, int y, int height, int health, int shield, RGBA ColHealth, bool shieldbar, bool healthbar )
{
	int box_h = ( height * health ) / 100;
	int box_s = ( height * shield ) / 100;

	RGBA ShiledBlackBar = { 0 , 0 , 0 , 255 };

	if ( shield < 50 )
	{
		ShiledBlackBar = { 0 , 0 , 0 , 0 };
	}
	else
	{
		ShiledBlackBar = { 0 , 0 , 0 , 255 };
	}

	if ( healthbar )
	{
		FilledRectangle( x + 7 + 1, y, 4 + 1, height + 2, RGBA { 0, 0, 0, 255 } );
		FilledRectangle( x + 7 + 1, y, 4 + 1, height + 2, RGBA { 0, 0, 0, 255 } );
		FilledRectangle( x + 7, y, 4, box_h, ColHealth );
	}

	if ( shieldbar )
	{
		if ( !healthbar )
		{
			FilledRectangle( x + 7 + 1, y, 4 + 1, height + 2, ShiledBlackBar );
			FilledRectangle( x + 7 + 1, y, 4 + 1, height + 2, ShiledBlackBar );
			FilledRectangle( x + 7 + 1, y, 4, box_s, RGBA { 0, 255, 255, 255 } );
		}
		if ( healthbar )
		{
			FilledRectangle( x + 7 + 5, y, 4 + 1, height + 2, ShiledBlackBar );
			FilledRectangle( x + 7 + 5, y, 4 + 1, height + 2, ShiledBlackBar );
			FilledRectangle( x + 7 + 5, y, 4, box_s, RGBA { 0, 255, 255, 255 } );
		}

	}
}
//
void OutlinedText( ImFont *pFont, const std::string &text, const ImVec2 &pos, float size, RGBA color, bool center )
{
	if ( !pFont )
		return;

	std::stringstream stream( text );
	std::string line;


	float y = 0.0f;
	int i = 0;

	ImGui::PushFont( pFont );
	while ( std::getline( stream, line ) )
	{
		ImVec2 textSize = pFont->CalcTextSizeA( size, FLT_MAX, 0.0f, line.c_str() );

		if ( center )
		{

			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x - textSize.x / 2.0f ) + 1, ( pos.y + textSize.y * i ) + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x - textSize.x / 2.0f ) - 1, ( pos.y + textSize.y * i ) - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			//	ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			//	ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( pos.x - textSize.x / 2.0f, pos.y + textSize.y * i ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), line.c_str() );
		}
		else
		{//
			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x ) + 1, ( pos.y + textSize.y * i ) + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x ) - 1, ( pos.y + textSize.y * i ) - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			//	ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				//ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( pos.x, pos.y + textSize.y * i ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), line.c_str() );

		}

		y = pos.y + textSize.y * ( i + 1 );
		i++;
	}
	ImGui::PopFont();
	//return y;
}

D3DXVECTOR3 AbsOrgin(uintptr_t ent, bool Update = false )
{
	if ( !ent ) return D3DXVECTOR3 { 0.f, 0.f, 0.f };

	return *(D3DXVECTOR3*)( ent + 0x14C );
}

int BoneByHitBox(uintptr_t ent, int HitBox )
{
	//get model ptr
	DWORD64 Model = *(DWORD64*)( ent + 0x1138 );
	if ( !Model ) return -1;

	//get studio hdr
	DWORD64 StudioHdr = *(DWORD64*)( Model + 8 );
	if ( !StudioHdr ) return -1;

	//get hitbox array
	DWORD64 HitBoxsArray = StudioHdr + *(int*)( StudioHdr + 0xB4 );
	if ( !HitBoxsArray ) return -1;

	//get bone index
	int Bone = *(int*)( HitBoxsArray + *(int*)( HitBoxsArray + 8 ) + ( HitBox * 0x2C ) );

	return ( ( Bone < 0 ) || ( Bone > 255 ) ) ? -1 : Bone;
}


void DrawNewTextImGui( int x, int y, RGBA color, const char *str )
{
	if ( x != 0 && y != 0 )
	{


		ImFont a;
		std::string utf_8_1 = std::string( str );
		std::string utf_8_2 = string_To_UTF8( utf_8_1 );
		ImGui::GetOverlayDrawList()->AddText( ImVec2( x, y ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), utf_8_2.c_str() );
	}
}


Color FromHSB( float hue, float saturation, float brightness )
{
	float h = hue == 1.0f ? 0 : hue * 6.0f;
	float f = h - ( int ) h;
	float p = brightness * ( 1.0f - saturation );
	float q = brightness * ( 1.0f - saturation * f );
	float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

	int hh = floor( h );
	switch ( hh )
	{
		case 0:
			return Color( brightness, t, p );
		case 1:
			return Color( q, brightness, p );
		case 2:
			return Color( p, brightness, t );
		case 3:
			return Color( p, q, brightness );
		case 4:
			return Color( t, p, brightness );
		case 5:
			return Color( brightness, p, q );
	}
	return Color( 0.0f, 0.0f, 0.0f ); //Never happens. Its here just to shut up the compiler
}

void HealthBar( int Health, int x, int y, int height )
{
	auto health = Health;



	float flBoxes = std::ceil( health / 10.f );
	float flX = x - height / 6.f;
	float flY = y - 1;
	float flHeight = height / 10.f;
	float flMultiplier = 12 / 360.f; flMultiplier *= flBoxes - 1;
	//ImGui::GetOverlayDrawList()->AddRectFilled( ImVec2( flX, flY ), ImVec2(  6, height + 4 ), IM_COL32( 0, 0, 0, 80 ) );

	FilledRectangle( flX, flY, 4, height + 2, { 79, 79, 79, 255 } );
	Box( flX, flY, 4, height + 2, { 1,1,1,1 } );


	Color ColHealth = FromHSB( flMultiplier, 1, 1 );
	FilledRectangle( flX + 1, flY, 3, flHeight * flBoxes + 2, { 0, 0, 0, 230 } );

	FilledRectangle( flX + 1, flY, 2, flHeight * flBoxes + 1, { ( unsigned long ) ( ColHealth.r * 255 ), ( unsigned long ) ( ColHealth.g * 255 ), ( unsigned long ) ( ColHealth.b * 255 ), 255 } );
	//for ( int i = 0; i < 10; i++ )
	//	Line( flX, flY + i * flHeight, flX + 4, flY + i * flHeight, {1, 1, 1, 255}, 1 );
}


void Shieldbar( int Shield, int maxshield, int x, int y, int height )
{
	auto health = 0;

	if ( maxshield > 100 )
		health = 100 * Shield / maxshield;
	else if (Shield < 100 )
		health = Shield;
	

	float flBoxes = std::ceil( health / 10.f );
	float flX = x - height / 6.f;
	float flY = y - 1;
	float flHeight = height / 10.f;
	float flMultiplier = 12 / 360.f; flMultiplier *= flBoxes - 1;

	FilledRectangle( flX, flY, 4, height + 2, { 79, 79, 79, 255 } );
	Box( flX, flY, 4, height + 2, { 1,1,1,1 } );


	Color ColHealth = { 30,144,255 };
	FilledRectangle( flX + 1, flY, 3, flHeight * flBoxes + 2, { 0, 0, 0, 230 } );

	FilledRectangle( flX + 1, flY, 2, flHeight * flBoxes + 1, { 46,139,192,255} );
	//for ( int i = 0; i < 10; i++ )
	//	Line( flX, flY + i * flHeight, flX + 4, flY + i * flHeight, { 1, 1, 1, 255 }, 1 );
}


ImGuiStyle *dst;
ImFont *icons = nullptr;
ImFont *big = nullptr;
ImFont *bigger = nullptr;
ImFont *g_font = nullptr;

bool Tab( const char *icon, const char *label, const ImVec2 &size_arg, const bool selected )
{


	ImGuiWindow *window = ImGui::GetCurrentWindow();
	if ( window->SkipItems )
		return false;

	static float sizeplus = 0.f;

	ImGuiContext &g = *GImGui;
	const ImGuiStyle &style = g.Style;
	const ImGuiID id = window->GetID( label );
	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize( size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f );

	const ImRect bb( pos, ImVec2( pos.x + size.x, pos.y + size.y ) );
	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	auto animcolor3 = ImColor( 27, 27, 27, 255 );
	auto animcolor2 = ImColor( 255, 255, 255, 255 );
	auto animcolor = ImColor( 24, 24, 24, 255 );

	window->DrawList->AddRectFilled( { bb.Min.x,bb.Max.y }, { bb.Max.x,bb.Min.y }, animcolor );

	if ( selected )
		window->DrawList->AddRectFilled( { bb.Min.x,bb.Max.y }, { bb.Min.x + 2,bb.Min.y }, animcolor2 );

	if ( selected )
		window->DrawList->AddRectFilled( { bb.Min.x + 2,bb.Max.y }, { bb.Min.x + 148,bb.Min.y }, animcolor3 );

	ImGui::PushFont( big );

	if ( selected )
		window->DrawList->AddText( { bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize( label ).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize( label ).y / 2 }, ImColor( 225 / 255.f, 225 / 255.f, 225 / 255.f, 255.f / 255.f ), label );

	if ( !selected )
		window->DrawList->AddText( { bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize( label ).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize( label ).y / 2 }, ImColor( 75 / 255.f, 75 / 255.f, 75 / 255.f, 255.f / 255.f ), label );


	ImGui::PopFont();

	return pressed;
}
bool Radar = true;
bool teamRadar = false;
bool enemyRadar = true;
int xAxis_Radar = 150;
int yAxis_Radar = 150;
int radartype = 1;
int width_Radar = 250;
int height_Radar = 250;
float distance_Radar = 400.0f;

static D3DXVECTOR3 RotatePoint( D3DXVECTOR3 EntityPos, D3DXVECTOR3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool *viewCheck )
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -( EntityPos.y - LocalPlayerPos.y );
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * ( float ) ( M_PI / 180.0F );
	x_1 = ( float ) ( r_2 * ( float ) cos( ( double ) ( yawToRadian ) ) - r_1 * sin( ( double ) ( yawToRadian ) ) ) / 20;
	y_1 = ( float ) ( r_2 * ( float ) sin( ( double ) ( yawToRadian ) ) + r_1 * cos( ( double ) ( yawToRadian ) ) ) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if ( x_1 < 5 )
		x_1 = 5;

	if ( x_1 > sizeX - 5 )
		x_1 = sizeX - 5;

	if ( y_1 < 5 )
		y_1 = 5;

	if ( y_1 > sizeY - 5 )
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return D3DXVECTOR3( x_1, y_1, 0 );
}




void DrawRadarPoint( D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist, int xAxis, int yAxis, int width, int height, D3DXCOLOR color )
{
	bool out = false;
	D3DXVECTOR3 siz;
	siz.x = width;
	siz.y = height;
	D3DXVECTOR3 pos;
	pos.x = xAxis;
	pos.y = yAxis;
	bool ck = false;

	//FilledRectangle( pos.x, pos.y, siz.x, siz.y, { 0,0,0, 43 } );

	D3DXVECTOR3 single = RotatePoint( EneamyPos, LocalPos, pos.x, pos.y, siz.x, siz.y, LocalPlayerY, 2.f, &ck );
	if ( eneamyDist >= 0.f && eneamyDist < distance_Radar )
	{
		//if (radartype == 0)
		//	Drawing::DrawOutlinedText(font, std::to_string((int)eneamyDist), ImVec2(single.x, single.y), 11, { 255, 255, 255, 255 }, true);
		//else
		FilledRectangle( single.x, single.y, 7, 7, { 255, 255, 255, 255 } );

	}
}

bool menu = true;

bool firstS = false;

void pkRadar( D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist )
{
	ImGuiStyle *style = &ImGui::GetStyle();
	style->WindowRounding = 0.2f;
	ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.13529413f, 0.14705884f, 0.15490198f, 0.82f ) );
	ImGuiWindowFlags TargetFlags;
	if ( menu )
		TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	else
		TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;

	if ( !firstS )
	{
		ImGui::SetNextWindowPos( ImVec2 { 1200, 60 }, ImGuiCond_Once );
		firstS = true;
	}
	if ( cfg::Radar )
	{
		ImGui::SetNextWindowSize( { 250, 250 } );
		ImGui::Begin( xorstr( "Radar" ), 0, TargetFlags );
		//if (ImGui::Begin(xorstr("Radar", 0, ImVec2(200, 200), -1.f, TargetFlags))) {
		{
			ImDrawList *Draw = ImGui::GetOverlayDrawList();
			ImVec2 DrawPos = ImGui::GetCursorScreenPos();
			ImVec2 DrawSize = ImGui::GetContentRegionAvail();
			ImVec2 midRadar = ImVec2( DrawPos.x + ( DrawSize.x / 2 ), DrawPos.y + ( DrawSize.y / 2 ) );
			ImGui::GetWindowDrawList()->AddLine( ImVec2( midRadar.x - DrawSize.x / 2.f, midRadar.y ), ImVec2( midRadar.x + DrawSize.x / 2.f, midRadar.y ), IM_COL32( 255, 255, 255, 255 ) );
			ImGui::GetWindowDrawList()->AddLine( ImVec2( midRadar.x, midRadar.y - DrawSize.y / 2.f ), ImVec2( midRadar.x, midRadar.y + DrawSize.y / 2.f ), IM_COL32( 255, 255, 255, 255 ) );

			DrawRadarPoint( EneamyPos, LocalPos, LocalPlayerY, eneamyDist, DrawPos.x, DrawPos.y, DrawSize.x, DrawSize.y, { 255, 255, 255, 255 } );
		}
		ImGui::End();
	}
	ImGui::PopStyleColor();
}

class UserCmd
{
public:
	int command_number;
	int tick_count;
	float command_time;
	D3DXVECTOR2 viewangles;
	BYTE pad1[0xC];
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	BYTE pad2[0x24C];
};

