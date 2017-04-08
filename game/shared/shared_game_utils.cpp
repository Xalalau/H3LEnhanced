#include <cassert>
#include <climits>
#include <cstdarg>

#include "extdll.h"
#include "util.h"

#ifdef CLIENT_DLL
#include "hud.h"
#include "cl_util.h"
#include "event_api.h"
#endif

#include "cbase.h"

#include "shared_game_utils.h"

DLL_GLOBAL const Vector	g_vecZero = Vector( 0, 0, 0 );

cvar_t* g_pDeveloper = nullptr;

CBaseEntity* GET_PRIVATE( edict_t* pent )
{
	if( pent )
		return reinterpret_cast<CBaseEntity*>( pent->pvPrivateData );

	return nullptr;
}

CHashStringPool g_StringPool;

edict_t* ENT( CBaseEntity* pEntity )
{
	//The other ENT overloads don't check for null, so neither does this one - Solokiller
	return pEntity->edict();
}

entvars_t* VARS( CBaseEntity* pEntity )
{
	return pEntity ? pEntity->pev : nullptr;
}

void MESSAGE_BEGIN( int iMsgType, int iMsgID, const float* pVecOrigin, CBaseEntity* pPlayer )
{
	MESSAGE_BEGIN( iMsgType, iMsgID, pVecOrigin, pPlayer ? pPlayer->edict() : nullptr );
}

bool FNullEnt( const CBaseEntity* pEntity )
{
	return pEntity == nullptr || FNullEnt( pEntity->pev );
}

static unsigned int glSeed = 0;

unsigned int seed_table[ 256 ] =
{
	28985, 27138, 26457, 9451, 17764, 10909, 28790, 8716, 6361, 4853, 17798, 21977, 19643, 20662, 10834, 20103,
	27067, 28634, 18623, 25849, 8576, 26234, 23887, 18228, 32587, 4836, 3306, 1811, 3035, 24559, 18399, 315,
	26766, 907, 24102, 12370, 9674, 2972, 10472, 16492, 22683, 11529, 27968, 30406, 13213, 2319, 23620, 16823,
	10013, 23772, 21567, 1251, 19579, 20313, 18241, 30130, 8402, 20807, 27354, 7169, 21211, 17293, 5410, 19223,
	10255, 22480, 27388, 9946, 15628, 24389, 17308, 2370, 9530, 31683, 25927, 23567, 11694, 26397, 32602, 15031,
	18255, 17582, 1422, 28835, 23607, 12597, 20602, 10138, 5212, 1252, 10074, 23166, 19823, 31667, 5902, 24630,
	18948, 14330, 14950, 8939, 23540, 21311, 22428, 22391, 3583, 29004, 30498, 18714, 4278, 2437, 22430, 3439,
	28313, 23161, 25396, 13471, 19324, 15287, 2563, 18901, 13103, 16867, 9714, 14322, 15197, 26889, 19372, 26241,
	31925, 14640, 11497, 8941, 10056, 6451, 28656, 10737, 13874, 17356, 8281, 25937, 1661, 4850, 7448, 12744,
	21826, 5477, 10167, 16705, 26897, 8839, 30947, 27978, 27283, 24685, 32298, 3525, 12398, 28726, 9475, 10208,
	617, 13467, 22287, 2376, 6097, 26312, 2974, 9114, 21787, 28010, 4725, 15387, 3274, 10762, 31695, 17320,
	18324, 12441, 16801, 27376, 22464, 7500, 5666, 18144, 15314, 31914, 31627, 6495, 5226, 31203, 2331, 4668,
	12650, 18275, 351, 7268, 31319, 30119, 7600, 2905, 13826, 11343, 13053, 15583, 30055, 31093, 5067, 761,
	9685, 11070, 21369, 27155, 3663, 26542, 20169, 12161, 15411, 30401, 7580, 31784, 8985, 29367, 20989, 14203,
	29694, 21167, 10337, 1706, 28578, 887, 3373, 19477, 14382, 675, 7033, 15111, 26138, 12252, 30996, 21409,
	25678, 18555, 13256, 23316, 22407, 16727, 991, 9236, 5373, 29402, 6117, 15241, 27715, 19291, 19888, 19847
};

unsigned int U_Random( void )
{
	glSeed *= 69069;
	glSeed += seed_table[ glSeed & 0xff ];

	return ( ++glSeed & 0x0fffffff );
}

void U_Srand( unsigned int seed )
{
	glSeed = seed_table[ seed & 0xff ];
}

/*
=====================
UTIL_SharedRandomLong
=====================
*/
int UTIL_SharedRandomLong( unsigned int seed, int low, int high )
{
	unsigned int range;

	U_Srand( ( int ) seed + low + high );

	range = high - low + 1;
	if( !( range - 1 ) )
	{
		return low;
	}
	else
	{
		int offset;
		int rnum;

		rnum = U_Random();

		offset = rnum % range;

		return ( low + offset );
	}
}

/*
=====================
UTIL_SharedRandomFloat
=====================
*/
float UTIL_SharedRandomFloat( unsigned int seed, float low, float high )
{
	//
	unsigned int range;

	U_Srand( ( int ) seed + *( int * ) &low + *( int * ) &high );

	U_Random();
	U_Random();

	range = high - low;
	if( !range )
	{
		return low;
	}
	else
	{
		int tensixrand;
		float offset;

		tensixrand = U_Random() & 65535;

		offset = ( float ) tensixrand / 65536.0;

		return ( low + offset * range );
	}
}

bool UTIL_GetGameDir( char* pszBuffer, const size_t uiBufferSize )
{
	ASSERT( pszBuffer );
	ASSERT( uiBufferSize > 0 );

#ifdef CLIENT_DLL
	const char* pszDir = gEngfuncs.pfnGetGameDirectory();

	strncpy( pszBuffer, pszDir, uiBufferSize );
	pszBuffer[ uiBufferSize - 1 ] = '\0';

	return strcmp( pszBuffer, pszDir ) == 0;

#else
	char szBuffer[ MAX_PATH ];

	GET_GAME_DIR( szBuffer );

	strncpy( pszBuffer, szBuffer, uiBufferSize );
	pszBuffer[ uiBufferSize - 1 ] = '\0';

	return strcmp( pszBuffer, szBuffer ) == 0;
#endif
}

void UTIL_StringToVector( Vector& vecOut, const char *pString )
{
	char *pstr, *pfront, tempString[ 128 ];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;

	for( j = 0; j < 3; j++ )			// lifted from pr_edict.c
	{
		vecOut[ j ] = atof( pfront );

		while( *pstr && *pstr != ' ' )
			pstr++;
		if( !*pstr )
			break;
		pstr++;
		pfront = pstr;
	}
	if( j < 2 )
	{
		/*
		ALERT( at_error, "Bad field in entity!! %s:%s == \"%s\"\n",
		pkvd->szClassName, pkvd->szKeyName, pkvd->szValue );
		*/
		for( j = j + 1; j < 3; j++ )
			vecOut[ j ] = 0;
	}
}

void UTIL_StringToIntArray( int *pVector, int count, const char *pString )
{
	char *pstr, *pfront, tempString[ 128 ];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;

	for( j = 0; j < count; j++ )			// lifted from pr_edict.c
	{
		pVector[ j ] = atoi( pfront );

		while( *pstr && *pstr != ' ' )
			pstr++;
		if( !*pstr )
			break;
		pstr++;
		pfront = pstr;
	}

	for( j++; j < count; j++ )
	{
		pVector[ j ] = 0;
	}
}

void UTIL_StringToColor( Color& color, const size_t uiCount, const char* const pszString )
{
	ASSERT( pszString );
	ASSERT( uiCount <= Color::NUM_COMPONENTS );

	//strtoul's second param is char**, so need to use that here. - Solokiller
	char* pszNext = const_cast<char*>( pszString );

	size_t uiIndex;

	for( uiIndex = 0; uiIndex < uiCount; ++uiIndex )
	{
		color[ uiIndex ] = strtoul( pszNext, &pszNext, 10 );

		if( !( *pszNext ) )
			break;
	}

	//Colors default to white.
	for( ++uiIndex; uiIndex < uiCount; ++uiIndex )
	{
		color[ uiIndex ] = 255;
	}
}

char* UTIL_VarArgs( const char* pszFormat, ... )
{
	static char szBuffers[ NUM_STATIC_BUFFERS ][ 1024 ];
	static size_t uiBufferIndex = 0;

	uiBufferIndex = ( uiBufferIndex + 1 ) % NUM_STATIC_BUFFERS;

	va_list argptr;

	va_start( argptr, pszFormat );

	vsprintf( szBuffers[ uiBufferIndex ], pszFormat, argptr );

	va_end( argptr );

	return szBuffers[ uiBufferIndex ];
}

char* UTIL_dtos( const int iValue )
{
	//This buffer size calculation determines the number of characters needed for an int, plus a null terminator.
	//See http://stackoverflow.com/questions/3919995/determining-sprintf-buffer-size-whats-the-standard/3920025#3920025
	//The old buffer size used by the SDK functions was 8.
	static char szBuffers[ NUM_STATIC_BUFFERS ][ ( ( ( sizeof( int ) * CHAR_BIT ) / 3 ) + 3 ) + 1 ];
	static size_t uiBufferIndex = 0;

	uiBufferIndex = ( uiBufferIndex + 1 ) % NUM_STATIC_BUFFERS;

	snprintf( szBuffers[ uiBufferIndex ], sizeof( szBuffers[ uiBufferIndex ] ), "%d", iValue );

	return szBuffers[ uiBufferIndex ];
}

//=========================================================
// UTIL_StripToken - for redundant keynames
//=========================================================
void UTIL_StripToken( const char *pKey, char *pDest )
{
	int i = 0;

	while( pKey[ i ] && pKey[ i ] != '#' )
	{
		pDest[ i ] = pKey[ i ];
		i++;
	}
	pDest[ i ] = 0;
}

char *safe_strcpy( char *dst, const char *src, const size_t len_dst )
{
	if( len_dst <= 0 )
	{
		return nullptr; // this is bad
	}

	strncpy( dst, src, len_dst );
	dst[ len_dst - 1 ] = '\0';

	return dst;
}

int safe_sprintf( char *dst, const size_t len_dst, const char *format, ... )
{
	if( len_dst <= 0 )
	{
		return -1; // this is bad
	}

	va_list v;

	va_start( v, format );

	vsnprintf( dst, len_dst, format, v );

	va_end( v );

	dst[ len_dst - 1 ] = '\0';

	return 0;
}

#ifdef	DEBUG
void DBG_AssertFunction(
	const bool	fExpr,
	const char*	szExpr,
	const char*	szFile,
	int			szLine,
	const char*	szMessage )
{
	if( fExpr )
		return;
	char szOut[ 512 ];
	if( szMessage != NULL )
		sprintf( szOut, "ASSERT FAILED:\n %s \n(%s@%d)\n%s\n", szExpr, szFile, szLine, szMessage );
	else
		sprintf( szOut, "ASSERT FAILED:\n %s \n(%s@%d)\n", szExpr, szFile, szLine );
	ALERT( at_console, szOut );
}
#endif	// DEBUG

char com_token[ MAX_COM_TOKEN ];

const char* COM_SkipWhitespace( const char* pszData )
{
	if( !pszData )
		return nullptr;

	char c;

	while( ( c = *pszData ) <= ' ' )
	{
		if( c == '\0' )
			return nullptr;                    // end of file;
		++pszData;
	}

	return pszData;
}

const char* COM_SkipComments( const char* pszData, bool& bWasComment )
{
	if( !pszData )
	{
		bWasComment = false;
		return nullptr;
	}

	char c = *pszData;

	// skip // comments
	if( c == '/' && pszData[ 1 ] == '/' )
	{
		while( *pszData && *pszData != '\n' )
			++pszData;
		bWasComment = true;
	}
	else
		bWasComment = false;

	if( !( *pszData ) )
		return nullptr;

	return pszData;
}

const char* COM_Parse( const char* pszData, char* pszBuffer, const size_t uiBufferSize )
{
	ASSERT( pszBuffer );
	ASSERT( uiBufferSize > 0 );

	if( !pszBuffer || !uiBufferSize )
		return nullptr;

	size_t len = 0;
	pszBuffer[ 0 ] = '\0';

	if( !pszData )
		return nullptr;

	{
		bool bWasComment;

		do
		{
			// skip whitespace
			pszData = COM_SkipWhitespace( pszData );

			if( !pszData )
				return nullptr;

			// skip // comments
			pszData = COM_SkipComments( pszData, bWasComment );

			if( !pszData )
				return nullptr;
		}
		while( bWasComment );
	}

	char c = *pszData;

	// handle quoted strings specially
	if( c == '\"' )
	{
		++pszData;
		while( 1 )
		{
			if( len + 1 >= uiBufferSize )
			{
				Alert( at_error, "COM_Parse: Buffer not large enough to contain token!\n" );
				pszBuffer[ len ] = '\0';
				return nullptr;
			}

			c = *pszData++;
			if( c == '\"' || !c )
			{
				pszBuffer[ len ] = '\0';
				return pszData;
			}
			pszBuffer[ len ] = c;
			++len;
		}
	}

	// parse single characters
	if( c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',' )
	{
		if( len + 1 >= uiBufferSize )
		{
			Alert( at_error, "COM_Parse: Buffer not large enough to contain token!\n" );
			pszBuffer[ len ] = '\0';
			return nullptr;
		}

		pszBuffer[ len ] = c;
		++len;
		pszBuffer[ len ] = '\0';
		return pszData + 1;
	}

	// parse a regular word
	do
	{
		if( len + 1 >= uiBufferSize )
		{
			Alert( at_error, "COM_Parse: Buffer not large enough to contain token!\n" );
			pszBuffer[ len ] = '\0';
			return nullptr;
		}

		pszBuffer[ len ] = c;
		++pszData;
		++len;
		c = *pszData;
		if( c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',' )
			break;
	}
	while( c > ' ' );

	pszBuffer[ len ] = '\0';
	return pszData;
}

const char* COM_Parse( const char* pszData )
{
	return COM_Parse( pszData, com_token, sizeof( com_token ) );
}

bool COM_TokenWaiting( const char* const pszBuffer )
{
	const char* p = pszBuffer;

	while( *p && *p != '\n' )
	{
		if( !isspace( *p ) || isalnum( *p ) )
			return true;

		++p;
	}

	return false;
}

char *memfgets( byte *pMemFile, int fileSize, int &filePos, char *pBuffer, int bufferSize )
{
	// Bullet-proofing
	if( !pMemFile || !pBuffer )
		return NULL;

	if( filePos >= fileSize )
		return NULL;

	int i = filePos;
	int last = fileSize;

	// fgets always NULL terminates, so only read bufferSize-1 characters
	if( last - filePos > ( bufferSize - 1 ) )
		last = filePos + ( bufferSize - 1 );

	int stop = 0;

	// Stop at the next newline (inclusive) or end of buffer
	while( i < last && !stop )
	{
		if( pMemFile[ i ] == '\n' )
			stop = 1;
		i++;
	}


	// If we actually advanced the pointer, copy it over
	if( i != filePos )
	{
		// We read in size bytes
		int size = i - filePos;
		// copy it out
		memcpy( pBuffer, pMemFile + filePos, sizeof( byte )*size );

		// If the buffer isn't full, terminate (this is always true)
		if( size < bufferSize )
			pBuffer[ size ] = 0;

		// Update file pointer
		filePos = i;
		return pBuffer;
	}

	// No data read, bail
	return NULL;
}

cvar_t* CVarGetPointer( const char* const pszName )
{
	return
#ifndef CLIENT_DLL
	CVAR_GET_POINTER( pszName )
#else
	gEngfuncs.pfnGetCvarPointer( pszName )
#endif
		;
}

void Alert( ALERT_TYPE aType, const char* const pszFormat, ... )
{
	assert( pszFormat );

	va_list list;

	va_start( list, pszFormat );

	char szBuffer[ 4096 ];

	const int iResult = vsnprintf( szBuffer, sizeof( szBuffer ), pszFormat, list );

	if( iResult >= 0 && static_cast<size_t>( iResult ) < sizeof( szBuffer ) )
	{
#ifndef CLIENT_DLL
		ALERT( aType, "%s", szBuffer );
#else
		//Mimic the server version.
		if( g_pDeveloper->value != 0 || gEngfuncs.GetMaxClients() <= 1 )
		{
			const char* pszPrefix = nullptr;

			bool bContinue = true;

			switch( aType )
			{
			case at_notice:
				{
					pszPrefix = "NOTE: ";
					break;
				}

			case at_aiconsole:
				{
					if( g_pDeveloper->value < 2 )
						bContinue = false;
					break;
				}

			case at_warning:
				{
					pszPrefix = "WARNING: ";
					break;
				}

			case at_error:
				{
					pszPrefix = "ERROR: ";
					break;
				}

			case at_logged:
				{
					//No actual logging, just tell the client it was a logged value.
					pszPrefix = "LOGGED: ";
					break;
				}

			case at_console:
			default: break;
			}

			if( bContinue )
			{
				if( pszPrefix )
				{
					gEngfuncs.Con_Printf( "%s%s", pszPrefix, szBuffer );
				}
				else
				{
					gEngfuncs.Con_Printf( "%s", szBuffer );
				}
			}
		}
#endif
	}

	va_end( list );
}

void Con_Printf( const char* const pszFormat, ... )
{
	assert( pszFormat );

	va_list list;

	va_start( list, pszFormat );

	char szBuffer[ 4096 ];

	const int iResult = vsnprintf( szBuffer, sizeof( szBuffer ), pszFormat, list );

	if( iResult >= 0 && static_cast<size_t>( iResult ) < sizeof( szBuffer ) )
	{
#ifdef CLIENT_DLL
		gEngfuncs.Con_Printf( "%s", szBuffer );
#else
		g_engfuncs.pfnServerPrint( szBuffer );
#endif
	}
}

void Con_DPrintf( const char* const pszFormat, ... )
{
	if( g_pDeveloper->value != 0 )
	{
		assert( pszFormat );

		va_list list;

		va_start( list, pszFormat );

		char szBuffer[ 4096 ];

		const int iResult = vsnprintf( szBuffer, sizeof( szBuffer ), pszFormat, list );

		if( iResult >= 0 && static_cast<size_t>( iResult ) < sizeof( szBuffer ) )
		{
#ifdef CLIENT_DLL
			gEngfuncs.Con_Printf( "%s", szBuffer );
#else
			g_engfuncs.pfnServerPrint( szBuffer );
#endif
		}
	}
}

Contents UTIL_PointContents( const Vector &vec )
{
	return static_cast<Contents>(
#ifndef CLIENT_DLL
	POINT_CONTENTS( vec )
#else
	gEngfuncs.PM_PointContents( vec, nullptr )
#endif
	);
}

float UTIL_WaterLevel( const Vector &position, float minz, float maxz )
{
	Vector midUp = position;
	midUp.z = minz;

	if( UTIL_PointContents( midUp ) != CONTENTS_WATER )
		return minz;

	midUp.z = maxz;
	if( UTIL_PointContents( midUp ) == CONTENTS_WATER )
		return maxz;

	float diff = maxz - minz;
	while( diff > 1.0 )
	{
		midUp.z = minz + diff / 2.0;
		if( UTIL_PointContents( midUp ) == CONTENTS_WATER )
		{
			minz = midUp.z;
		}
		else
		{
			maxz = midUp.z;
		}
		diff = maxz - minz;
	}

	return midUp.z;
}

#ifndef CLIENT_DLL
extern DLL_GLOBAL short g_sModelIndexBubbles;// holds the index for the bubbles model
#endif

void UTIL_Bubbles( const Vector& mins, const Vector& maxs, int count, const float flSpeed )
{
	Vector mid = ( mins + maxs ) * 0.5;

	float flHeight = UTIL_WaterLevel( mid, mid.z, mid.z + 1024 );
	flHeight = flHeight - mins.z;

#ifndef CLIENT_DLL
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, mid );
		WRITE_BYTE( TE_BUBBLES );
		WRITE_COORD( mins.x );	// mins
		WRITE_COORD( mins.y );
		WRITE_COORD( mins.z );
		WRITE_COORD( maxs.x );	// maxz
		WRITE_COORD( maxs.y );
		WRITE_COORD( maxs.z );
		WRITE_COORD( flHeight );			// height
		WRITE_SHORT( g_sModelIndexBubbles );
		WRITE_BYTE( count ); // count
		WRITE_COORD( flSpeed ); // speed
	MESSAGE_END();
#else
	gEngfuncs.pEfxAPI->R_Bubbles( mins, maxs, flHeight, gEngfuncs.pEventAPI->EV_FindModelIndex( BUBBLES_MODEL ), count, flSpeed );
#endif
}

void UTIL_BubbleTrail( const Vector& from, const Vector& to, int count, const float flSpeed )
{
	float flHeight = UTIL_WaterLevel( from, from.z, from.z + 256 );
	flHeight = flHeight - from.z;

	if( flHeight < 8 )
	{
		flHeight = UTIL_WaterLevel( to, to.z, to.z + 256 );
		flHeight = flHeight - to.z;
		if( flHeight < 8 )
			return;

		// UNDONE: do a ploink sound
		flHeight = flHeight + to.z - from.z;
	}

	if( count > 255 )
		count = 255;

#ifndef CLIENT_DLL
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BUBBLETRAIL );
		WRITE_COORD( from.x );	// mins
		WRITE_COORD( from.y );
		WRITE_COORD( from.z );
		WRITE_COORD( to.x );	// maxz
		WRITE_COORD( to.y );
		WRITE_COORD( to.z );
		WRITE_COORD( flHeight );			// height
		WRITE_SHORT( g_sModelIndexBubbles );
		WRITE_BYTE( count ); // count
		WRITE_COORD( flSpeed ); // speed
	MESSAGE_END();
#else
	gEngfuncs.pEfxAPI->R_BubbleTrail( from, to, flHeight, gEngfuncs.pEventAPI->EV_FindModelIndex( BUBBLES_MODEL ), count, flSpeed );
#endif
}

Vector UTIL_VecToAngles( const Vector& vec )
{
	Vector vecAngles;

	//Used to call VEC_TO_ANGLES, which is a server engine interface function.
	//Same code, different source. Dll agnostic. - Solokiller
	VectorAngles( vec, vecAngles );

	return vecAngles;
}

Vector VecVelocityForDamage( const float flDamage )
{
	Vector vec( RANDOM_FLOAT( -100, 100 ), RANDOM_FLOAT( -100, 100 ), RANDOM_FLOAT( 200, 300 ) );

	if( flDamage > -50 )
		vec = vec * 0.7;
	else if( flDamage > -200 )
		vec = vec * 2;
	else
		vec = vec * 10;

	return vec;
}

void UTIL_MakeVectors( const Vector& vecAngles )
{
#ifdef CLIENT_DLL
	//This is what the engine does. - Solokiller
	AngleVectors( vecAngles, gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up );
#else
	MAKE_VECTORS( vecAngles );
#endif
}

#define SWAP(a,b,temp)	((temp)=(a),(a)=(b),(b)=(temp))

void UTIL_MakeInvVectors( const Vector& vec, globalvars_t* pgv )
{
	UTIL_MakeVectors( vec );

	float tmp;
	pgv->v_right = pgv->v_right * -1;

	SWAP( pgv->v_forward.y, pgv->v_right.x, tmp );
	SWAP( pgv->v_forward.z, pgv->v_up.x, tmp );
	SWAP( pgv->v_right.z, pgv->v_up.y, tmp );
}

void UTIL_Sparks( const Vector& position )
{
#ifdef CLIENT_DLL
	//The engine uses these constants - Solokiller
	gEngfuncs.pEfxAPI->R_SparkEffect( position, 8, -200, 200 );
#else
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, position );
		WRITE_BYTE( TE_SPARKS );
		WRITE_COORD( position.x );
		WRITE_COORD( position.y );
		WRITE_COORD( position.z );
	MESSAGE_END();
#endif
}

void UTIL_Ricochet( const Vector& position, float scale )
{
#ifdef CLIENT_DLL
	//The engine uses this sprite. - Solokiller
	gEngfuncs.pEfxAPI->R_RicochetSprite( position, gEngfuncs.hudGetModelByIndex( gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/richo1.spr" ) ), 0.1f, scale );
	gEngfuncs.pEfxAPI->R_RicochetSound( position );
#else
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, position );
		WRITE_BYTE( TE_ARMOR_RICOCHET );
		WRITE_COORD( position.x );
		WRITE_COORD( position.y );
		WRITE_COORD( position.z );
		WRITE_BYTE( ( int ) ( scale * 10 ) );
	MESSAGE_END();
#endif
}

float UTIL_WeaponTimeBase()
{
#if defined( CLIENT_WEAPONS ) || defined( CLIENT_DLL )
	//Always 0.0 on client, even if not predicting weapons ( won't get called in that case )
	return 0.0;
#else
	return gpGlobals->time;
#endif
}

void UTIL_DestructEntity( CBaseEntity* pEntity )
{
	ASSERT( pEntity );

	pEntity->OnDestroy();
	pEntity->~CBaseEntity();
}

#ifdef SERVER_DLL
namespace
{
static void UTIL_RemoveCleanup( CBaseEntity* pEntity )
{
	pEntity->UpdateOnRemove();
	pEntity->GetFlags() |= FL_KILLME;
	pEntity->ClearTargetname();
}
}

void UTIL_Remove( CBaseEntity *pEntity )
{
	if( !pEntity )
		return;

	UTIL_RemoveCleanup( pEntity );
}
#endif

void UTIL_RemoveNow( CBaseEntity* pEntity )
{
	if( !pEntity )
		return;

#ifdef SERVER_DLL
	//Let UTIL_Remove's stuff happen even when removing right away. - Solokiller
	UTIL_RemoveCleanup( pEntity );

	REMOVE_ENTITY( pEntity->edict() );
#else
	UTIL_DestructEntity( pEntity );

	//On the client, entities are allocated using byte arrays. - Solokiller
	delete[] reinterpret_cast<byte*>( pEntity );
#endif
}

bool COM_FileBase( const char *in, char *out, size_t uiSizeInCharacters )
{
	ASSERT( out );

	if( uiSizeInCharacters == 0 )
		return false;

	size_t uiLength = strlen( in );

	// scan backward for '.'
	size_t end = uiLength - 1;
	while( end && in[ end ] != '.' && in[ end ] != '/' && in[ end ] != '\\' )
		--end;

	if( in[ end ] != '.' )	// no '.', copy to end
		end = uiLength - 1;
	else
		--end;				// Found ',', copy to left of '.'


	// Scan backward for '/'
	size_t start = uiLength;
	while( start > 0 && in[ start - 1 ] != '/' && in[ start - 1 ] != '\\' )
		--start;

	// Length of new string
	uiLength = end - start + 1;

	if( uiSizeInCharacters < uiLength )
		return false;

	// Copy partial string
	strncpy( out, &in[ start ], uiLength );
	// Terminate it
	out[ uiLength ] = '\0';

	return true;
}

bool UTIL_IsGame( const char* game )
{
	char szGameDir[ MAX_PATH ];

	UTIL_GetGameDir( szGameDir, sizeof( szGameDir ) );

	if( szGameDir[ 0 ] )
	{
		char gd[ 1024 ];

		if( COM_FileBase( szGameDir, gd ) && !stricmp( gd, game ) )
			return true;
	}
	return false;
}

static cvar_t* sv_cheats = nullptr;

bool UTIL_CheatsAllowed()
{
	if( !sv_cheats )
		sv_cheats = CVAR_GET_POINTER( "sv_cheats" );

	ASSERT( sv_cheats );

	return sv_cheats->value != 0;
}
