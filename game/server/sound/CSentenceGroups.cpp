#include "extdll.h"
#include "util.h"

#include "CSentenceGroups.h"

const int CSentenceGroups::INVALID_SENTENCE_INDEX = -1;

bool CSentenceGroups::FormatSentenceName( char* pszBuffer, const size_t uiBufferSize, const char* const pszSentenceGroup, const int iPick )
{
	ASSERT( pszBuffer );
	ASSERT( uiBufferSize > 0 );
	ASSERT( pszSentenceGroup );

	const int iResult = snprintf( pszBuffer, uiBufferSize, "!%s%d", pszSentenceGroup, iPick );

	return PrintfSuccess( iResult, uiBufferSize );
}

const char* CSentenceGroups::GetSentenceName( const size_t uiIndex ) const
{
	ASSERT( m_bInitialized );
	ASSERT( uiIndex < m_uiSentenceCount );

	return uiIndex < m_uiSentenceCount ? m_szAllSentenceNames[ uiIndex ] : "";
}

void CSentenceGroups::Initialize()
{
	if( m_bInitialized )
		return;

	int filePos = 0, fileSize;

	byte* pMemFile = g_engfuncs.pfnLoadFileForMe( "sound/sentences.txt", &fileSize );

	if( !pMemFile )
		return;

	char buffer[ 512 ] = {};
	char szgroup[ MAX_SENTENCE_NAME ] = {};
	int i, j;

	int isentencegs = -1;

	// for each line in the file...
	while( memfgets( pMemFile, fileSize, filePos, buffer, sizeof( buffer ) - 1 ) != nullptr )
	{
		// skip whitespace
		i = 0;
		while( buffer[ i ] && buffer[ i ] == ' ' )
			++i;

		if( !buffer[ i ] )
			continue;

		if( buffer[ i ] == '/' || !isalpha( buffer[ i ] ) )
			continue;

		// get sentence name
		j = i;
		while( buffer[ j ] && buffer[ j ] != ' ' )
			++j;

		if( !buffer[ j ] )
			continue;

		if( m_uiSentenceCount > CVOXFILESENTENCEMAX )
		{
			ALERT( at_error, "Too many sentences in sentences.txt!\n" );
			break;
		}

		// null-terminate name and save in sentences array
		buffer[ j ] = '\0';
		const char *pString = buffer + i;

		if( strlen( pString ) >= CBSENTENCENAME_MAX )
			ALERT( at_warning, "Sentence %s longer than %d letters\n", pString, CBSENTENCENAME_MAX - 1 );

		strcpy( m_szAllSentenceNames[ m_uiSentenceCount++ ], pString );

		--j;
		if( j <= i )
			continue;
		if( !isdigit( buffer[ j ] ) )
			continue;

		// cut out suffix numbers
		while( j > i && isdigit( buffer[ j ] ) )
			--j;

		if( j <= i )
			continue;

		buffer[ j + 1 ] = '\0';

		// if new name doesn't match previous group name, 
		// make a new group.

		if( strcmp( szgroup, &( buffer[ i ] ) ) )
		{
			// name doesn't match with prev name,
			// copy name into group, init count to 1
			++isentencegs;
			if( isentencegs >= CSENTENCEG_MAX )
			{
				ALERT( at_error, "Too many sentence groups in sentences.txt!\n" );
				break;
			}

			strcpy( m_SentenceGroups[ isentencegs ].szgroupname, &( buffer[ i ] ) );
			m_SentenceGroups[ isentencegs ].count = 1;

			strcpy( szgroup, &( buffer[ i ] ) );

			continue;
		}
		else
		{
			//name matches with previous, increment group count
			if( isentencegs >= 0 )
				++m_SentenceGroups[ isentencegs ].count;
		}
	}

	g_engfuncs.pfnFreeFile( pMemFile );

	m_bInitialized = true;

	// init lru lists

	i = 0;

	while( m_SentenceGroups[ i ].count && i < CSENTENCEG_MAX )
	{
		InitLRU( &( m_SentenceGroups[ i ].rgblru[ 0 ] ), m_SentenceGroups[ i ].count );
		++i;
	}
}

void CSentenceGroups::Stop( CBaseEntity* pEntity, int isentenceg, int ipick )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return;

	if( isentenceg < 0 || ipick < 0 )
		return;

	char szBuffer[ MAX_SENTENCE_NAME ];

	FormatSentenceName( szBuffer, m_SentenceGroups[ isentenceg ].szgroupname, ipick );

	//TODO: make channel controllable. - Solokiller
	STOP_SOUND( pEntity, CHAN_VOICE, szBuffer );
}

int CSentenceGroups::PlayRndI( CBaseEntity* pEntity, int isentenceg,
						float volume, float attenuation, int flags, int pitch )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return INVALID_SENTENCE_INDEX;

	char szName[ MAX_SENTENCE_NAME ] = { '\0' };

	const int ipick = Pick( isentenceg, szName );

	if( ipick > 0 && *szName )
		EMIT_SOUND_DYN( pEntity, CHAN_VOICE, szName, volume, attenuation, flags, pitch );

	return ipick;
}

int CSentenceGroups::PlayRndSz( CBaseEntity* pEntity, const char* szgroupname,
						 float volume, float attenuation, int flags, int pitch )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return INVALID_SENTENCE_INDEX;

	const int isentenceg = GetIndex( szgroupname );

	if( isentenceg < 0 )
	{
		ALERT( at_console, "No such sentence group %s\n", szgroupname );
		return INVALID_SENTENCE_INDEX;
	}

	char name[ MAX_SENTENCE_NAME ] = { '\0' };

	const int ipick = Pick( isentenceg, name );

	if( ipick >= 0 && *name )
		EMIT_SOUND_DYN( pEntity, CHAN_VOICE, name, volume, attenuation, flags, pitch );

	return ipick;
}

int CSentenceGroups::PlaySequentialSz( CBaseEntity* pEntity, const char* szgroupname,
								float volume, float attenuation, int flags, int pitch, int ipick, const bool bReset )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return INVALID_SENTENCE_INDEX;

	const int isentenceg = GetIndex( szgroupname );

	if( isentenceg < 0 )
		return INVALID_SENTENCE_INDEX;

	char name[ MAX_SENTENCE_NAME ] = { '\0' };

	const int ipicknext = PickSequential( isentenceg, name, ipick, bReset );

	if( ipicknext >= 0 && *name )
		EMIT_SOUND_DYN( pEntity, CHAN_VOICE, name, volume, attenuation, flags, pitch );

	return ipicknext;
}

int CSentenceGroups::GetIndex( const char* szgroupname )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized || !szgroupname )
		return INVALID_SENTENCE_INDEX;

	// search m_SentenceGroups for match on szgroupname

	for( int i = 0; m_SentenceGroups[ i ].count; ++i )
	{
		if( !strcmp( szgroupname, m_SentenceGroups[ i ].szgroupname ) )
			return i;
	}

	return INVALID_SENTENCE_INDEX;
}

int CSentenceGroups::Lookup( const char* sample, char* sentencenum )
{
	// this is a sentence name; lookup sentence number
	// and give to engine as string.
	for( size_t i = 0; i < m_uiSentenceCount; ++i )
	{
		if( !stricmp( m_szAllSentenceNames[ i ], sample + 1 ) )
		{
			if( sentencenum )
			{
				snprintf( sentencenum, MAX_SENTENCENUM_NAME, "!%zu", i );
			}

			return static_cast<int>( i );
		}
	}

	// sentence name not found!
	return INVALID_SENTENCE_INDEX;
}

void CSentenceGroups::InitLRU( unsigned char* plru, int count )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return;

	if( count > CSENTENCE_LRU_MAX )
		count = CSENTENCE_LRU_MAX;

	for( int i = 0; i < count; ++i )
		plru[ i ] = ( unsigned char ) i;

	int j, k;
	unsigned char temp;

	// randomize array
	for( int i = 0; i < ( count * 4 ); ++i )
	{
		j = RANDOM_LONG( 0, count - 1 );
		k = RANDOM_LONG( 0, count - 1 );
		temp = plru[ j ];
		plru[ j ] = plru[ k ];
		plru[ k ] = temp;
	}
}

int CSentenceGroups::Pick( int isentenceg, char* szfound )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return INVALID_SENTENCE_INDEX;

	if( isentenceg < 0 )
		return INVALID_SENTENCE_INDEX;

	const char* szGroupName = m_SentenceGroups[ isentenceg ].szgroupname;
	const unsigned char count = m_SentenceGroups[ isentenceg ].count;
	unsigned char* plru = m_SentenceGroups[ isentenceg ].rgblru;

	unsigned char ipick;
	bool bFound = false;

	while( !bFound )
	{
		for( unsigned char i = 0; i < count; i++ )
			if( plru[ i ] != 0xFF )
			{
				ipick = plru[ i ];
				plru[ i ] = 0xFF;
				bFound = true;
				break;
			}

		if( !bFound )
			InitLRU( plru, count );
		else
		{
			FormatSentenceName( szfound, MAX_SENTENCE_NAME, szGroupName, ipick );
			return ipick;
		}
	}

	return INVALID_SENTENCE_INDEX;
}

int CSentenceGroups::PickSequential( int isentenceg, char* szfound, int ipick, const bool bReset )
{
	ASSERT( m_bInitialized );

	if( !m_bInitialized )
		return INVALID_SENTENCE_INDEX;

	if( isentenceg < 0 )
		return INVALID_SENTENCE_INDEX;

	const char* szgroupname = m_SentenceGroups[ isentenceg ].szgroupname;
	const unsigned char count = m_SentenceGroups[ isentenceg ].count;

	if( count == 0 )
		return INVALID_SENTENCE_INDEX;

	if( ipick >= count )
		ipick = count - 1;

	//TODO: put this in a helper function. - Solokiller
	FormatSentenceName( szfound, MAX_SENTENCE_NAME, szgroupname, ipick );

	if( ipick >= count )
	{
		if( bReset )
			// reset at end of list
			return 0;
		else
			return count;
	}

	return ipick + 1;
}
