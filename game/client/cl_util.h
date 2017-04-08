/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// cl_util.h
//
#ifndef GAME_CLIENT_CL_UTIL_H
#define GAME_CLIENT_CL_UTIL_H

#include <type_traits>

#include "cvardef.h"

#include <stdio.h> // for safe_sprintf()
#include <stdarg.h>  // "
#include <string.h> // for strncpy()

#include "shared_game_utils.h"

// Macros to hook function calls into the HUD object
/**
*	Gets the Hud class instance of the given class name.
*/
#define GETHUDCLASS( className )									\
( static_cast<className*>( Hud().GetHud().HudList().GetElementByName( #className ) ) )

#define __HOOK_MESSAGE_ON_HANDLER( handler, object, messageName )				\
handler.Add( 																	\
	#messageName, 																\
	object, 																	\
	&std::remove_reference<decltype( object )>::type::MsgFunc_##messageName )

/**
*	Hooks a message for an object on the global message handlers.
*/
#define HOOK_GLOBAL_MESSAGE( object, messageName )						\
__HOOK_MESSAGE_ON_HANDLER( MessageHandlers(), object, messageName )

/**
*	Hooks a message for an object on a specific Hud.
*/
#define HOOK_OBJECT_MESSAGE( hud, object, messageName )						\
__HOOK_MESSAGE_ON_HANDLER( hud.GetMessageHandlers(), object, messageName )

/**
*	Same as HOOK_MESSAGE, but for classes deriving from CBaseHud. Uses the local message handlers.
*/
#define HOOK_HUD_MESSAGE( messageName )				\
HOOK_OBJECT_MESSAGE( ( *this ), *this, messageName )

/**
*	Hooks a network message function.
*	@param messageName Name of the message whose function should be hooked. The function name is ThisClass::MsgFunc_<messageName>.
*/
#define HOOK_MESSAGE( messageName )					\
HOOK_OBJECT_MESSAGE( GetHud(), *this, messageName )

/**
*	Declares a function that calls the HUD class method for the given command.
*	@param className HUD class name.
*	@param commandFuncName Name of the command. The HUD class method should be named UserCmd_<commandFuncName>.
*/
#define DECLARE_COMMAND( className, commandFuncName )								\
void __CmdFunc_##commandFuncName()													\
{																					\
	if( auto pElement = Hud().GetHud().HudList().GetElementByName( #className ) )	\
	{																				\
		( static_cast<className*>( pElement ) )->UserCmd_##commandFuncName();		\
	}																				\
}

/**
*	Hooks a command function.
*	@param szCommandName Name of the command whose function should be hooked.
*	@param commandFuncName Name of the command function. The function name is __CmdFunc_<commandFuncName>.
*/
#define HOOK_COMMAND( szCommandName, commandFuncName ) gEngfuncs.pfnAddCommand( szCommandName, __CmdFunc_##commandFuncName );

//TODO: this fixes compilation issues caused by server and client code being in the same file, but isn't ideal. Clean up. - Solokiller
#ifndef CVAR_GET_FLOAT
inline float CVAR_GET_FLOAT( const char *x ) {	return gEngfuncs.pfnGetCvarFloat( (char*)x ); }
inline const char* CVAR_GET_STRING( const char *x ) {	return gEngfuncs.pfnGetCvarString( (char*)x ); }
#endif
inline cvar_t *CVAR_CREATE( const char *cv, const char *val, const int flags ) {	return gEngfuncs.pfnRegisterVariable( (char*)cv, (char*)val, flags ); }

/**
*	Represents the invalid HSPRITE handle.
*/
const HSPRITE INVALID_HSPRITE = 0;

#define SPR_Load (*gEngfuncs.pfnSPR_Load)
#define SPR_Set (*gEngfuncs.pfnSPR_Set)
#define SPR_Frames (*gEngfuncs.pfnSPR_Frames)
#define SPR_GetList (*gEngfuncs.pfnSPR_GetList)

// SPR_Draw  draws a the current sprite as solid
#define SPR_Draw (*gEngfuncs.pfnSPR_Draw)
// SPR_DrawHoles  draws the current sprites,  with color index255 not drawn (transparent)
#define SPR_DrawHoles (*gEngfuncs.pfnSPR_DrawHoles)
// SPR_DrawAdditive  adds the sprites RGB values to the background  (additive transulency)
#define SPR_DrawAdditive (*gEngfuncs.pfnSPR_DrawAdditive)

// SPR_EnableScissor  sets a clipping rect for HUD sprites.  (0,0) is the top-left hand corner of the screen.
#define SPR_EnableScissor (*gEngfuncs.pfnSPR_EnableScissor)
// SPR_DisableScissor  disables the clipping rect
#define SPR_DisableScissor (*gEngfuncs.pfnSPR_DisableScissor)

/**
*	Loads a resolution dependent HUD sprite.
*	@param pszName Sprite name. Must contain a %d to add the resolution specifier.
*								E.g. "sprites/%d_train.spr"
*	@return Handle to the sprite, or 0 if it couldn't be loaded.
*/
HSPRITE LoadSprite( const char* const pszName );

client_sprite_t *GetSpriteList( client_sprite_t *pList, const char *psz, int iRes, int iCount );

//
#define FillRGBA (*gEngfuncs.pfnFillRGBA)


// ScreenHeight returns the height of the screen, in pixels
#define ScreenHeight (Hud().ScreenInfo().iHeight)
// ScreenWidth returns the width of the screen, in pixels
#define ScreenWidth (Hud().ScreenInfo().iWidth)

// use this to project world coordinates to screen coordinates
#define XPROJECT(x)	( (1.0f+(x))*ScreenWidth*0.5f )
#define YPROJECT(y) ( (1.0f-(y))*ScreenHeight*0.5f )

/**
*	Converts a resolution independent X value to a resolution dependent one.
*/
#define XRES(x)					(x  * ((float)ScreenWidth / 640))

/**
*	Converts a resolution independent Y value to a resolution dependent one.
*/
#define YRES(y)					(y  * ((float)ScreenHeight / 480))

/**
*	@see cl_enginefunc_t::pfnGetScreenInfo
*/
#define GetScreenInfo (*gEngfuncs.pfnGetScreenInfo)

/**
*	@see cl_enginefunc_t::pfnServerCmd
*/
#define ServerCmd (*gEngfuncs.pfnServerCmd)

/**
*	@see cl_enginefunc_t::pfnClientCmd
*/
#define EngineClientCmd (*gEngfuncs.pfnClientCmd)

/**
*	@see cl_enginefunc_t::pfnSetCrosshair
*/
void SetCrosshair( HSPRITE hCrosshair, const wrect_t& crosshairRC, int r, int g, int b );

// Gets the height & width of a sprite,  at the specified frame
/**
*	@see cl_enginefunc_t::pfnSPR_Height
*/
inline int SPR_Height( HSPRITE x, int f )	{ return gEngfuncs.pfnSPR_Height( x, f ); }

/**
*	@see cl_enginefunc_t::pfnSPR_Width
*/
inline int SPR_Width( HSPRITE x, int f )	{ return gEngfuncs.pfnSPR_Width( x, f ); }

/**
*	@see cl_enginefunc_t::pfnTextMessageGet
*/
inline client_textmessage_t* TextMessageGet( const char* const pszName ) { return gEngfuncs.pfnTextMessageGet( pszName ); }

/**
*	@see cl_enginefunc_t::pfnDrawCharacter
*/
inline int TextMessageDrawChar( int x, int y, int number, int r, int g, int b ) 
{ 
	return gEngfuncs.pfnDrawCharacter( x, y, number, r, g, b ); 
}

/**
*	@see cl_enginefunc_t::pfnDrawConsoleString
*/
inline int DrawConsoleString( int x, int y, const char* const pszString )
{
	return gEngfuncs.pfnDrawConsoleString( x, y, pszString );
}

/**
*	@see cl_enginefunc_t::pfnDrawConsoleStringLen
*/
inline void GetConsoleStringSize( const char* const pszString, int* piWidth, int* piHeight )
{
	gEngfuncs.pfnDrawConsoleStringLen( pszString, piWidth, piHeight );
}

/**
*	Gets the width in pixels of a string if it were drawn onscreen.
*	@param pszString String to check.
*	@return Width in pixels.
*/
inline int ConsoleStringLen( const char* const pszString )
{
	int _width, _height;
	GetConsoleStringSize( pszString, &_width, &_height );
	return _width;
}

/**
*	@see cl_enginefunc_t::pfnConsolePrint
*/
inline void ConsolePrint( const char* const pszString )
{
	gEngfuncs.pfnConsolePrint( pszString );
}

/**
*	@see cl_enginefunc_t::pfnCenterPrint
*/
inline void CenterPrint( const char* const pszString )
{
	gEngfuncs.pfnCenterPrint( pszString );
}

// sound functions
/**
*	@see cl_enginefunc_t::pfnPlaySoundByName
*/
inline void PlaySound( const char* const pszSound, float vol ) { gEngfuncs.pfnPlaySoundByName( pszSound, vol ); }

/**
*	@see cl_enginefunc_t::pfnPlaySoundByIndex
*/
inline void PlaySound( int iSound, float vol ) { gEngfuncs.pfnPlaySoundByIndex( iSound, vol ); }

#include "MinMax.h"

/**
*	Scales RGB colors by the alpha value.
*	@param[ in, out ] r Red color. [ 0, 255 ].
*	@param[ in, out ] g Green color. [ 0, 255 ].
*	@param[ in, out ] b Blue color. [ 0, 255 ].
*	@param a Alpha value. [ 0, 255 ].
*/
void ScaleColors( int& r, int& g, int& b, const int a );

// disable 'possible loss of data converting float to int' warning message
#pragma warning( disable: 4244 )
// disable 'truncation from 'const double' to 'float' warning message
#pragma warning( disable: 4305 )

/**
*	Unpacks an RGB color into individual components.
*	@param[ out ] r Red color. [ 0, 255 ].
*	@param[ out ] g Green color. [ 0, 255 ].
*	@param[ out ] b Blue color. [ 0, 255 ].
*	@param ulRGB Packed RGB color.
*	<pre>
*	Layout:
*	| unused | R | G | B |
*	</pre>
*/
inline void UnpackRGB( int& r, int& g, int& b, unsigned long ulRGB )
{
	r = ( ulRGB & 0xFF0000 ) >> 16;
	g = ( ulRGB & 0xFF00 ) >> 8;
	b = ulRGB & 0xFF;
}

/**
*	@copydoc cl_enginefunc_t::GetPlayerUniqueID
*/
bool UTIL_GetPlayerUniqueID( int iPlayer, char playerID[ PLAYERID_BUFFER_SIZE ] );

#define MSG_BUF_SIZE 128

/**
*	Prints a text message with up to 4 string arguments.
*/
void UTIL_TextMsg( const ClientPrintDest msgDest, 
				   const char* pszMessage, 
				   const char* pszString1 = nullptr, const char* pszString2 = nullptr, const char* pszString3 = nullptr, const char* pszString4 = nullptr );

/**
*	Localized version of UTIL_TextMsg.
*	@see UTIL_TextMsg
*/
void UTIL_LocalizedTextMsg( const ClientPrintDest msgDest,
				   const char* pszMessage,
				   const char* pszString1 = nullptr, const char* pszString2 = nullptr, const char* pszString3 = nullptr, const char* pszString4 = nullptr );

/**
*	Gets the name of the current map, without maps/ and .bsp.
*	The destination buffer is only modified if the function returns true.
*	@param[ out ] pszBuffer Destination buffer.	Should ideally be MAX_PATH or greater.
*	@param uiSizeInBytes Size of the destination buffer, in bytes.
*	@return Whether the map name was successfully extracted.
*/
bool UTIL_GetMapName( char* pszBuffer, const size_t uiSizeInBytes );

#endif //GAME_CLIENT_CL_UTIL_H
