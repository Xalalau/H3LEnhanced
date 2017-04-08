#ifndef GAME_SHARED_RELATIONSHIP_H
#define GAME_SHARED_RELATIONSHIP_H

/**
*	Monster to monster relationship types.
*/
enum Relationship
{
	/**
	*	(ALLY) pals. Good alternative to R_NO when applicable.
	*/
	R_AL = -2,

	/**
	*	(FEAR)will run.
	*/
	R_FR = -1,

	/**
	*	(NO RELATIONSHIP) disregard.
	*/
	R_NO = 0,

	/**
	*	(DISLIKE) will attack.
	*/
	R_DL = 1,

	/**
	*	(HATE) will attack this character instead of any visible DISLIKEd characters.
	*/
	R_HT = 2,

	/**
	*	(NEMESIS)  A monster Will ALWAYS attack its nemsis, no matter what.
	*/
	R_NM = 3,

	//Used for enumeration of relationship constants.
	R_FIRST = R_AL,
	R_LAST = R_NM,
};

const char* RelationshipToString( const Relationship relationship );

Relationship RelationshipFromString( const char* pszString, const Relationship defaultValue = R_NO );

const char* RelationshipToPrettyString( const Relationship relationship );

/**
*	The length of the longest relationship pretty string.
*/
const size_t LONGEST_RELATIONSHIP_PRETTY_STRING =
[]()
{
	size_t uiLongest = 0;

	for( int relationship = R_FIRST; relationship <= R_LAST; ++relationship )
	{
		const auto uiLength = strlen( RelationshipToPrettyString( static_cast<Relationship>( relationship ) ) );

		if( uiLength > uiLongest )
			uiLongest = uiLength;
	}

	return uiLongest;
}();

#endif //GAME_SHARED_RELATIONSHIP_H
