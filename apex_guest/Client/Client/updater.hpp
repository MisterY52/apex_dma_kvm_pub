#pragma once
#include <utility.hpp>
#include <xstring>
#include <iomanip>

inline uintptr_t BaseAddress = ( uintptr_t ) GetModuleHandle( NULL );

#define ENTITY_MAX_COUNT 15000

enum classes : int32_t
{
	pEntityList,
	pCInput,
	pLocalPlayer,
	pNameList,
	pViewRender,
	pViewMatrix,
	pLastVisibleTime,
};

class update
{
public:
	static auto GetClass( classes offset_name )
	{
		uint64_t offset;

		switch ( offset_name )
		{
			case pEntityList:
				offset = scanner::find( x( "4C 8D 05 ? ? ? ? 4C 8B 25 ? ? ? ?" ) );
				break;
			case pViewRender:
				offset = scanner::find( x( "48 8B 0D ? ? ? ? 48 8B 01 FF 50 40 48 8B 0D ? ? ? ?" ) );
				break;
			case pCInput:
				offset = scanner::find( x( "4C 8B 05 ? ? ? ? 48 8D 0D ? ? ? ? 49 8B 80 80 00 00 00" ) );
				break;
			case pLocalPlayer:
				offset = scanner::find( x( "48 8B 05 ? ? ? ? 48 0F 44 C7" ) );
				break;
			case pNameList:
				offset = scanner::find( x( "48 8D 05 ? ? ? ? FF CA" ) );
				break;
			case pViewMatrix:
				offset = scanner::find( x( "48 8D 1D ? ? ? ? 66 0F 1F 84 00 00 00 00 00 48 8B 01 48 0F BE F7 49 3B C6 0F 85" ) );
				break;
			case pLastVisibleTime:
				offset = scanner::find( x( "C0 03 00 00 00 00 00" ) );
				break;
			default:
				break;
		}

		offset -= ( uint64_t ) GetModuleHandle( NULL );

		return offset;
	}
};