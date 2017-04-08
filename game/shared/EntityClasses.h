#ifndef GAME_SHARED_ENTITYCLASSES_H
#define GAME_SHARED_ENTITYCLASSES_H

#include <cstdint>
#include <climits>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include "Relationship.h"

/**
*	@file
*
*	Provides functionality to define classifications for entities, and define relationships between them
*/

/**
*	Identifies an entity classification.
*	TODO: should really be a uint8, but save/restore can't deal with those yet. - Solokiller
*/
using EntityClassification_t = uint32_t;

const EntityClassification_t MAX_ENTITY_CLASSIFICATIONS = UINT8_MAX;

const EntityClassification_t INVALID_ENTITY_CLASSIFICATION = 0;

/**
*	A relationship to a classification.
*/
struct CClassificationRelationship final
{
	CClassificationRelationship( EntityClassification_t classId, Relationship relationship )
		: m_ClassId( classId )
		, m_Relationship( relationship )
	{
	}

	CClassificationRelationship( const CClassificationRelationship& other ) = default;
	CClassificationRelationship& operator=( const CClassificationRelationship& other ) = default;

	CClassificationRelationship( CClassificationRelationship&& other ) = default;
	CClassificationRelationship& operator=( CClassificationRelationship&& other ) = default;

	//Needed for binary search and insert
	operator EntityClassification_t() const
	{
		return m_ClassId;
	}

	EntityClassification_t m_ClassId;
	Relationship m_Relationship;
};

/**
*	Stores data for an entity classification.
*/
class CEntityClassificationData final
{
public:
	using Relationships_t = std::vector<CClassificationRelationship>;

public:
	CEntityClassificationData( EntityClassification_t classId, std::string&& szName, 
							   Relationship defaultSourceRelationship, 
							   Relationship defaultTargetRelationship, bool bHasDefaultTargetRelationship )
		: m_ClassId( classId )
		, m_szName( std::move( szName ) )
		, m_DefaultSourceRelationship( defaultSourceRelationship )
		, m_DefaultTargetRelationship( defaultTargetRelationship )
		, m_bHasDefaultTargetRelationship( bHasDefaultTargetRelationship )
	{
	}

	const Relationships_t& GetRelationships() const { return m_Relationships; }

	bool HasRelationshipToClassification( EntityClassification_t targetClassId ) const;

	/**
	*	Gets the relationship to a given classification, if one is defined.
	*	@param targetClassId Id of the target classification
	*	@param[ out ] outRelationship The relationship between this class and the target class, or R_NO if no relationship is defined
	*	@return Whether a relationship was found between the classifications
	*/
	bool GetRelationshipToClassification( EntityClassification_t targetClassId, Relationship& outRelationship );

	void AddRelationship( EntityClassification_t targetClassId, Relationship relationship );

	void RemoveRelationship( EntityClassification_t targetClassId );

	const EntityClassification_t m_ClassId;

	const std::string m_szName;

	Relationship m_DefaultSourceRelationship;
	Relationship m_DefaultTargetRelationship;

	bool m_bHasDefaultTargetRelationship;

private:
	Relationships_t m_Relationships;

private:
	CEntityClassificationData( const CEntityClassificationData& ) = delete;
	CEntityClassificationData& operator=( const CEntityClassificationData& ) = delete;
};

/**
*	Manages the list of entity classifications, aliases to classifications and relationships between classifications.
*/
class CEntityClassificationsManager final
{
private:
	using ClassList_t = std::vector<std::unique_ptr<CEntityClassificationData>>;
	//first=index, second=is alias
	using ClassMap_t = std::unordered_map<std::string, std::pair<size_t, bool>>;

	static const EntityClassification_t FIRST_ID_OFFSET = 1;

	//TODO: should only need one of these globally - Solokiller
	static const std::string EMPTY_STRING;

public:
	CEntityClassificationsManager() = default;
	~CEntityClassificationsManager() = default;

	/**
	*	Called on server startup.
	*/
	void Initialize();

	/**
	*	Removes all classification, aliases and relationships. Add the NONE classification.
	*/
	void Reset();

	/**
	*	@return Whether classId refers to a valid, existing classification.
	*/
	bool IsClassIdValid( EntityClassification_t classId ) const;

	/**
	*	@see AddClassification( std::string&& szName, Relationship defaultSourceRelationship, Relationship defaultTargetRelationship )
	*/
	EntityClassification_t AddClassification( std::string&& szName, Relationship defaultSourceRelationship = R_NO );

	/**
	*	Adds or updates an entity classification.
	*	@param szName Name of the classification
	*	@param defaultSourceRelationship Default relationship to other classifications if not explicitly added
	*	@param defaultTargetRelationship Default relationship from other classifications to this classification if not explicitly added
	*	@return Classification Id, or INVALID_ENTITY_CLASSIFICATION
	*/
	EntityClassification_t AddClassification( std::string&& szName, Relationship defaultSourceRelationship, Relationship defaultTargetRelationship );

private:
	EntityClassification_t AddClassification( std::string&& szName, Relationship defaultSourceRelationship, Relationship defaultTargetRelationship, bool bHasDefaultTargetRelationship );

public:
	/**
	*	Removes a classification. Removes any aliases mapped to this classification.
	*	@return Whether the classification was present and removed.
	*/
	bool RemoveClassification( EntityClassification_t classId );

	/**
	*	Removes a classification. If bRemoveAliases is true, removes the classification if szName is an alias to it.
	*	@see RemoveClassification( EntityClassification_t classId )
	*/
	bool RemoveClassification( const std::string& szName, bool bRemoveAliases = true );

	/**
	*	@return The id for a named classification or alias.
	*/
	EntityClassification_t GetClassificationId( const std::string& szName ) const;

	/**
	*	Adds an alias mapping name to target. If The given target does not exist, it will be created with default settings.
	*	@return The Id for the target, or INVALID_ENTITY_CLASSIFICATION if the target couldn't be created.
	*/
	EntityClassification_t AddAlias( std::string&& szName, std::string&& szTarget );

	bool RemoveAlias( const std::string& szName );

	/**
	*	Adds a relationship between 2 classes.
	*	@param bBidirectional If true, the relationship is added for both classes. Otherwise, only the first class receives the relationship
	*/
	void AddRelationship( EntityClassification_t sourceClassId, EntityClassification_t targetClassId, Relationship relationship, bool bBidirectional = false );

	/**
	*	Convenience method to add relationships between named classifications.
	*	@see AddRelationship( EntityClassification_t sourceClassId, EntityClassification_t targetClassId, Relationship relationship, bool bBidirectional = false )
	*/
	void AddRelationship( const std::string& sourceClassName, const std::string& targetClassName, Relationship relationship, bool bBidirectional = false );

	/**
	*	Removes a relationship from source to target. If bBidirectional is true, also removes the relationship from target to source.
	*/
	void RemoveRelationship( EntityClassification_t sourceClassId, EntityClassification_t targetClassId, bool bBidirectional = false );

	void RemoveRelationship( const std::string& sourceClassName, const std::string& targetClassName, bool bBidirectional = false );

	/**
	*	Returns the relationship between 2 classifications. Returns R_NO if either class Id is invalid.
	*/
	Relationship GetRelationshipBetween( EntityClassification_t sourceClassId, EntityClassification_t targetClassId, bool bBidirectional = false ) const;

	Relationship GetRelationshipBetween( const std::string& sourceClassName, const std::string& targetClassName, bool bBidirectional = false ) const;

	/**
	*	@return The name for a classification.
	*/
	const std::string& GetClassificationName( EntityClassification_t classification ) const;

	/**
	*	The Id used for "None" classification. Commonly used, and used as a default in some cases.
	*/
	EntityClassification_t GetNoneId() const { return m_NoneId; }

	/**
	*	Can be used to override the "None" classification Id if needed.
	*/
	void SetNoneId( EntityClassification_t noneId )
	{
		m_NoneId = noneId;
	}

	/**
	*	Writes all classifications and relationships to a file.
	*/
	void WriteToFile( const char* pszFilename ) const;

private:
	static size_t IdToIndex( EntityClassification_t classId );

	static EntityClassification_t IndexToId( size_t index );

private:
	//TODO: use a CUtlDict - Solokiller
	ClassList_t m_ClassList;
	ClassMap_t m_ClassMap;

	EntityClassification_t m_NoneId = INVALID_ENTITY_CLASSIFICATION;

private:
	CEntityClassificationsManager( const CEntityClassificationsManager& ) = delete;
	CEntityClassificationsManager& operator=( const CEntityClassificationsManager& ) = delete;
};

CEntityClassificationsManager& EntityClassifications();

#endif //GAME_SHARED_ENTITYCLASSES_H
