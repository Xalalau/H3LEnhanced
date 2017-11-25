/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#include <cstdint>

#include "extdll.h"
#include "UTFUtils.h"

bool Q_IsValidUChar32( uchar32 uVal )
{
	// Values > 0x10FFFF are explicitly invalid; ditto for UTF-16 surrogate halves,
	// values ending in FFFE or FFFF, or values in the 0x00FDD0-0x00FDEF reserved range
	return ( static_cast<uint32_t>( uVal ) < 0x110000u ) &&
		( ( static_cast<uint32_t>( uVal ) - 0x00D800u ) > 0x7FFu ) &&
		( ( static_cast<uint32_t>( uVal ) & 0xFFFFu ) < 0xFFFEu ) &&
		( ( static_cast<uint32_t>( uVal ) - 0x00FDD0u ) > 0x1Fu );
}

int Q_UTF8ToUChar32( const char* pUTF8_, uchar32& uValueOut, bool& bErrorOut )
{
	const uint8 *pUTF8 = ( const uint8 * ) pUTF8_;

	int nBytes = 1;
	uint32 uValue = pUTF8[ 0 ];
	uint32 uMinValue = 0;

	// 0....... single byte
	if( uValue < 0x80 )
		goto decodeFinishedNoCheck;

	// Expecting at least a two-byte sequence with 0xC0 <= first <= 0xF7 (110...... and 11110...)
	if( ( uValue - 0xC0u ) > 0x37u || ( pUTF8[ 1 ] & 0xC0 ) != 0x80 )
		goto decodeError;

	uValue = ( uValue << 6 ) - ( 0xC0 << 6 ) + pUTF8[ 1 ] - 0x80;
	nBytes = 2;
	uMinValue = 0x80;

	// 110..... two-byte lead byte
	if( !( uValue & ( 0x20 << 6 ) ) )
		goto decodeFinished;

	// Expecting at least a three-byte sequence
	if( ( pUTF8[ 2 ] & 0xC0 ) != 0x80 )
		goto decodeError;

	uValue = ( uValue << 6 ) - ( 0x20 << 12 ) + pUTF8[ 2 ] - 0x80;
	nBytes = 3;
	uMinValue = 0x800;

	// 1110.... three-byte lead byte
	if( !( uValue & ( 0x10 << 12 ) ) )
		goto decodeFinishedMaybeCESU8;

	// Expecting a four-byte sequence, longest permissible in UTF-8
	if( ( pUTF8[ 3 ] & 0xC0 ) != 0x80 )
		goto decodeError;

	uValue = ( uValue << 6 ) - ( 0x10 << 18 ) + pUTF8[ 3 ] - 0x80;
	nBytes = 4;
	uMinValue = 0x10000;

	// 11110... four-byte lead byte. fall through to finished.

decodeFinished:
	if( uValue >= uMinValue && Q_IsValidUChar32( uValue ) )
	{
	decodeFinishedNoCheck:
		uValueOut = uValue;
		bErrorOut = false;
		return nBytes;
	}
decodeError:
	uValueOut = '?';
	bErrorOut = true;
	return nBytes;

decodeFinishedMaybeCESU8:
	// Do we have a full UTF-16 surrogate pair that's been UTF-8 encoded afterwards?
	// That is, do we have 0xD800-0xDBFF followed by 0xDC00-0xDFFF? If so, decode it all.
	if( ( uValue - 0xD800u ) < 0x400u && pUTF8[ 3 ] == 0xED && ( uint8 ) ( pUTF8[ 4 ] - 0xB0 ) < 0x10 && ( pUTF8[ 5 ] & 0xC0 ) == 0x80 )
	{
		uValue = 0x10000 + ( ( uValue - 0xD800u ) << 10 ) + ( ( uint8 ) ( pUTF8[ 4 ] - 0xB0 ) << 6 ) + pUTF8[ 5 ] - 0x80;
		nBytes = 6;
		uMinValue = 0x10000;
	}
	goto decodeFinished;
}

bool Q_UnicodeValidate( const char* pUTF8 )
{
	bool bError = false;
	while( *pUTF8 )
	{
		uchar32 uVal;
		// Our UTF-8 decoder silently fixes up 6-byte CESU-8 (improperly re-encoded UTF-16) sequences.
		// However, these are technically not valid UTF-8. So if we eat 6 bytes at once, it's an error.
		int nCharSize = Q_UTF8ToUChar32( pUTF8, uVal, bError );
		if( bError || nCharSize == 6 )
			return false;
		pUTF8 += nCharSize;
	}
	return true;
}