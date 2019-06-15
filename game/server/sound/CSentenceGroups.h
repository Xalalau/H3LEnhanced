#ifndef GAME_SERVER_SOUND_CSENTENCEGROUPS_H
#define GAME_SERVER_SOUND_CSENTENCEGROUPS_H

// sentence groups
#define CBSENTENCENAME_MAX 16
/**
*	Max number of elements per sentence group
*/
#define CSENTENCE_LRU_MAX 32

/**
*	Max number of sentences in game. NOTE: this must match
*	CVOXFILESENTENCEMAX in engine\sound.h!!!
*/
#define CVOXFILESENTENCEMAX 1536

/**
*	Max number of sentence groups
*/
#define CSENTENCEG_MAX 200

/**
*	Maintains the list of sentence groups.
*/
class CSentenceGroups
{
public:
	/**
	*	Used to indicate that a sentence could not be found.
	*/
	static const int INVALID_SENTENCE_INDEX;

	/**
	*	Maximum length of a sentence name, including null terminator.
	*/
	static const size_t MAX_SENTENCE_NAME = 64;

	/**
	*	Maximum length of a sentence number name, including null terminator.
	*/
	static const size_t MAX_SENTENCENUM_NAME = 32;

	struct CSentenceGroup
	{
		char szgroupname[ CBSENTENCENAME_MAX ];
		int count;
		unsigned char rgblru[ CSENTENCE_LRU_MAX ];
	};

public:
	CSentenceGroups() = default;

	/**
	*	Formats a sentence name. Format is "!%s%d", where %s is the group name, and %d is the sentence that was picked.
	*	@param[ out ] pszBuffer Destination buffer. Must be valid.
	*	@param uiBufferSize Size of the buffer, in characters.
	*	@param pszSentenceGroup Sentence group to use. Must be valid.
	*	@param iPick Sentence in the group to pick.
	*	@return Whether the name was successfully formatted.
	*/
	static bool FormatSentenceName( char* pszBuffer, const size_t uiBufferSize, const char* const pszSentenceGroup, const int iPick );

	/**
	*	Secure overload to automatically infer the buffer size.
	*	@see FormatSentenceName( char* pszBuffer, const size_t uiBufferSize, const char* const pszSentenceGroup, const int iPick )
	*/
	template<size_t SIZE>
	static bool FormatSentenceName( char ( & szBuffer )[ SIZE ], const char* const pszSentenceGroup, const int iPick )
	{
		return FormatSentenceName( szBuffer, SIZE, pszSentenceGroup, iPick );
	}

	/**
	*	Gets a sentence name by index.
	*	@return If the index is valid, the sentence name. Otherwise, an empty string.
	*/
	const char* GetSentenceName( const size_t uiIndex ) const;

	/**
	*	Open sentences.txt, scan for groups, build m_SentenceGroups
	*	Should be called from world spawn, only works on the
	*	first call and is ignored subsequently.
	*/
	void Initialize();

	/**
	*	For this entity, for the given sentence within the sentence group, stop
	*	the sentence.
	*/
	void Stop( CBaseEntity* pEntity, int isentenceg, int ipick );

	/**
	*	Given sentence group index, play random sentence for given entity.
	*	Returns ipick - which sentence was picked to 
	*	play from the group. Ipick is only needed if you plan on stopping
	*	the sound before playback is done (@see Stop).
	*/
	int PlayRndI( CBaseEntity* pEntity, int isentenceg, float volume, float attenuation, int flags, int pitch );

	/**
	*	Same as above, but takes sentence group name instead of index
	*	@see PlayRndI( CBaseEntity* pEntity, int isentenceg, float volume, float attenuation, int flags, int pitch )
	*/
	int PlayRndSz( CBaseEntity* pEntity, const char* szrootname, float volume, float attenuation, int flags, int pitch );

	/**
	*	Play sentences in sequential order from sentence group. Reset after last sentence.
	*/
	int PlaySequentialSz( CBaseEntity* pEntity, const char* szrootname, float volume, float attenuation, int flags, int pitch, int ipick, const bool bReset );

	/**
	*	Given sentence group rootname (name without number suffix),
	*	get sentence group index (isentenceg). Returns -1 if no such name.
	*/
	int GetIndex( const char* szrootname );

	/**
	*	Convert sentence (sample) name to !sentencenum, return !sentencenum
	*/
	int Lookup( const char* sample, char* sentencenum );

private:
	/**
	*	Randomize list of sentence name indices
	*/
	void InitLRU( unsigned char* plru, int count );

	/*
	*	Pick a random sentence from rootname0 to rootnameX.
	*	Picks from the m_SentenceGroups[isentenceg] least
	*	recently used, modifies lru array. returns the sentencename.
	*	Note, lru must be seeded with 0-n randomized sentence numbers, with the
	*	rest of the lru filled with -1. The first integer in the lru is
	*	actually the size of the list.  
	*	@return ipick, the ordinal of the picked sentence within the group.
	*/
	int Pick( int isentenceg, char* szfound );

	/*
	*	Ignore lru. Pick next sentence from sentence group. Go in order until we hit the last sentence, 
	*	then repeat list if bReset is true.  If bReset is false, then repeat last sentence.
	*	ipick is passed in as the requested sentence ordinal.
	*	ipick 'next' is returned.  
	*	@return If an error has occurred, returns INVALID_SENTENCE_INDEX. Otherwise, returns the next sentence to play after the picked sentence.
	*/

	int PickSequential( int isentenceg, char* szfound, int ipick, const bool bReset );

private:
	char m_szAllSentenceNames[ CVOXFILESENTENCEMAX ][ CBSENTENCENAME_MAX ] = {};
	size_t m_uiSentenceCount = 0;

	CSentenceGroup m_SentenceGroups[ CSENTENCEG_MAX ] = {};

	bool m_bInitialized = false;

private:
	CSentenceGroups( const CSentenceGroups& ) = delete;
	CSentenceGroups& operator=( const CSentenceGroups& ) = delete;
};

#endif //GAME_SERVER_SOUND_CSENTENCEGROUPS_H
