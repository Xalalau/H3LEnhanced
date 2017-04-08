#include "extdll.h"
#include "util.h"

#include "Relationship.h"

const char* RelationshipToString( const Relationship relationship )
{
#define TO_STRING( constant ) case constant: return #constant;

	switch( relationship )
	{
	TO_STRING( R_AL )
	TO_STRING( R_FR )
	TO_STRING( R_NO )
	TO_STRING( R_DL )
	TO_STRING( R_HT )
	TO_STRING( R_NM )
	default: break;
	}

#undef TO_STRING

	ASSERT( false );

	return "Unknown";
}

Relationship RelationshipFromString( const char* pszString, const Relationship defaultValue )
{
	if( pszString )
	{
#define FROM_STRING( constant ) if( !stricmp( pszString, #constant ) ) return constant;

		FROM_STRING( R_AL )
		else FROM_STRING( R_FR )
		else FROM_STRING( R_NO )
		else FROM_STRING( R_DL )
		else FROM_STRING( R_HT )
		else FROM_STRING( R_NM )

#undef FROM_STRING
	}

	return defaultValue;
}

const char* RelationshipToPrettyString( const Relationship relationship )
{
	switch( relationship )
	{
	case R_AL:	return "Ally";
	case R_FR:	return "Fear";
	case R_NO:	return "None";
	case R_DL:	return "Dislike";
	case R_HT:	return "Hate";
	case R_NM:	return "Nemesis";
	default: break;
	}

	ASSERT( false );

	return "Unknown";
}
