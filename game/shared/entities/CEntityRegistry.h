#ifndef GAME_SHARED_ENTITIES_CENTITYREGISTRY_H
#define GAME_SHARED_ENTITIES_CENTITYREGISTRY_H

struct entvars_t;
struct edict_t;

class CBaseEntity;

/**
*	Represents an entity class.
*/
class CBaseEntityRegistry
{
public:
	/**
	*	Constructor.
	*	@param pszEntityName Name of the entity as used in maps.
	*	@param pszClassName C++ Class name.
	*/
	CBaseEntityRegistry( const char* const pszEntityName, const char* const pszClassName );

	/**
	*	@return The entity's class name as used in maps.
	*/
	const char* GetEntityname() const { return m_pszEntityName; }

	/**
	*	@return The C++ class name.
	*/
	const char* GetClassname() const { return m_pszClassName; }

	/**
	*	@return Size of the C++ class, in bytes. Used for diagnostics only.
	*/
	virtual size_t GetSize() const = 0;

	/**
	*	Creates a new instance of the entity, using the given entvars_t instance.
	*	@param pev Entvars instance that will be assigned to the entity.
	*	@return Entity instance.
	*/
	virtual CBaseEntity* CreateInstance( entvars_t* pev ) = 0;

	/**
	*	Creates a new instance of the entity, using the given edict_t instance.
	*	@param pEdict Edict instance that will be assigned to the entity.
	*	@return Entity instance.
	*/
	virtual CBaseEntity* CreateInstance( edict_t* pEdict ) = 0;

	/**
	*	Creates a new instance of the entity. Allocates a new edict.
	*	@return Entity instance.
	*/
	virtual CBaseEntity* CreateInstance() = 0;

	/**
	*	Destroys the given entity instance.
	*/
	virtual void DestroyInstance( CBaseEntity* pInstance ) = 0;

private:
	const char* const m_pszEntityName;
	const char* const m_pszClassName;

private:
	CBaseEntityRegistry( const CBaseEntityRegistry& ) = delete;
	CBaseEntityRegistry& operator=( const CBaseEntityRegistry& ) = delete;
};

/**
*	Template class used to implement the entity class actions that depend on the entity's C++ class type.
*	@tparam T Entity's C++ class type.
*/
template<typename T>
class CEntityRegistry : public CBaseEntityRegistry
{
public:
	using CBaseEntityRegistry::CBaseEntityRegistry;

	size_t GetSize() const override { return sizeof( T ); }

	CBaseEntity* CreateInstance( entvars_t* pev ) override
	{
		// allocate private data 
		CBaseEntity* pEntity = new( pev ) T;
		pEntity->pev = pev;
		pEntity->SetClassname( GetEntityname() );
		//Now calls OnCreate - Solokiller
		pEntity->OnCreate();

		return pEntity;
	}

	CBaseEntity* CreateInstance( edict_t* pEdict ) override
	{
		return CreateInstance( &pEdict->v );
	}

	CBaseEntity* CreateInstance() override
	{
		return CreateInstance( CREATE_ENTITY() );
	}

	void DestroyInstance( CBaseEntity* pInstance ) override
	{
		UTIL_Remove( pInstance );
	}

private:
	CEntityRegistry( const CEntityRegistry& ) = delete;
	CEntityRegistry& operator=( const CEntityRegistry& ) = delete;
};

#endif //GAME_SHARED_ENTITIES_CENTITYREGISTRY_H