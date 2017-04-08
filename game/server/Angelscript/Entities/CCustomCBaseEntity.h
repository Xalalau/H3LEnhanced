#ifndef GAME_SERVER_ANGELSCRIPT_ENTITIES_CCUSTOMCBASEENTITY_H
#define GAME_SERVER_ANGELSCRIPT_ENTITIES_CCUSTOMCBASEENTITY_H

#include <angelscript.h>

#include <Angelscript/util/CASObjPtr.h>
#include <Angelscript/util/CASRefPtr.h>
#include <Angelscript/util/CASExtendAdapter.h>

#include "IASCustomEntity.h"

#include "CASCustomEntities.h"

#undef GetObject

/**
*	Extension class for custom entities inheriting from BASECLASS.
*	Provides extension methods for CBaseEntity.
*/
template<typename BASECLASS>
class CBaseCustomCBaseEntity : public BASECLASS, public IASCustomEntity
{
public:
	DECLARE_CLASS( CBaseCustomCBaseEntity, BASECLASS );

	CASObjPtr& GetObject() const override final
	{
		//This cast is needed because you can't return non-const references to class members.
		return const_cast<CBaseCustomCBaseEntity*>( this )->CBaseCustomCBaseEntity::m_Instance;
	}

	void SetObject( CASObjPtr&& instance ) override final
	{
		CBaseCustomCBaseEntity::m_Instance = std::move( instance );
	}

	void SetClass( const CCustomEntityClass* pClass ) override final
	{
		m_pClass = pClass;
	}

	/**
	*	Sets a function. Can be either a function of the CBaseEntity class or the script class.
	*	@param function Function to set.
	*	@param pFunction Function to set. Can be null.
	*	@param pszName Name of the fundamental function being set.
	*/
	void SetFunction( CASRefPtr<asIScriptFunction>& function, asIScriptFunction* pFunction, const char* const pszName )
	{
		if( !pFunction )
		{
			function = nullptr;
			return;
		}

		bool bSuccess = false;

		if( pFunction->GetDelegateFunction() )
		{
			auto pDelegateType = pFunction->GetDelegateObjectType();

			ASSERT( pDelegateType );

			//Note: when checking for inheritance, check if our concrete type inherits from the given type.
			//When checking if our C++ type matches, check if the given type inherits from it.
			//This prevents subclasses from being
			const bool bIsScriptFunction = m_Instance.GetTypeInfo()->DerivesFrom( pDelegateType );

			auto pCPPType = pDelegateType->GetEngine()->GetTypeInfoByName( m_pClass->GetBaseClassData().szCPPClassName.c_str() );

			if( bIsScriptFunction ||
				pCPPType && pCPPType->DerivesFrom( pDelegateType ) )
			{
				bool bIsValid = false;

				if( bIsScriptFunction )
				{
					bIsValid = pFunction->GetDelegateObject() == m_Instance.Get();
				}
				else
				{
					bIsValid = pFunction->GetDelegateObject() == this;
				}

				if( bIsValid )
				{
					function = pFunction;

					bSuccess = true;
				}
			}
		}

		if( !bSuccess )
		{
			auto pActualFunction = pFunction->GetDelegateFunction();
			auto pActualObject = pFunction->GetDelegateObjectType();

			if( !pActualFunction )
				pActualFunction = pFunction;

			const std::string szCompleteName = pActualObject ? 
				std::string( pActualObject->GetNamespace() ) + "::" + pActualObject->GetName() + "::" + pActualFunction->GetName() : 
				std::string( pActualFunction->GetNamespace() ) + "::" + pActualFunction->GetName();

			Alert( at_warning, "Couldn't set custom entity \"%s\" (class %s) function %s to \"%s\"\n", 
				   ThisClass::GetTargetname(), ThisClass::GetClassname(), pszName, szCompleteName.c_str() );
		}

		pFunction->Release();
	}

	void OnCreate() override
	{
		CALL_EXTEND_FUNC( OnCreate, "()" );
	}

	void OnDestroy() override
	{
		CALL_EXTEND_FUNC( OnDestroy, "()" );

		m_Instance.Reset();
	}

	void UpdateOnRemove() override
	{
		CALL_EXTEND_FUNC( UpdateOnRemove, "()" );
	}

	void KeyValue( KeyValueData* pkvd ) override
	{
		CALL_EXTEND_FUNC( KeyValue, "(KeyValueData@)", pkvd );
	}

	void Precache() override
	{
		CALL_EXTEND_FUNC( Precache, "()" );
	}

	void Spawn() override
	{
		CALL_EXTEND_FUNC( Spawn, "()" );
	}

	void Activate() override
	{
		CALL_EXTEND_FUNC( Activate, "()" );
	}

	int ObjectCaps() const override
	{
		CALL_EXTEND_FUNC_RET( int, ObjectCaps, "()" );
	}

	void SetObjectCollisionBox() override
	{
		CALL_EXTEND_FUNC( SetObjectCollisionBox, "()" );
	}

	CBaseEntity* Respawn() override
	{
		//TODO: need helper macros for different return types.
		CBaseEntity* pResult = nullptr;

		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "CBaseEntity@ Respawn()" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			if( method.Call( CallFlag::NONE ) )
			{
				method.GetReturnValue( &pResult );
			}
		}
		else
		{
			pResult = BaseClass::Respawn();
		}

		return pResult;
	}

	void DefaultScriptThink()
	{
		//Script didn't override Think, so call think function that was set, if any.

		if( m_ThinkFunc )
		{
			//This is a delegate so we don't pass 'this'.
			as::Call( m_ThinkFunc.Get() );
		}
	}

	void SetScriptThink( asIScriptFunction* pFunction ) override final
	{
		SetFunction( m_ThinkFunc, pFunction, "Think" );
	}

	void Think() override
	{
		CALL_EXTEND_FUNC_DIFFFUNC( Think, DefaultScriptThink, "()" );
	}

	void DefaultScriptTouch( CBaseEntity* pOther )
	{
		//Script didn't override Touch, so call think function that was set, if any.

		if( m_TouchFunc )
		{
			//This is a delegate so we don't pass 'this'.
			as::Call( m_TouchFunc.Get(), pOther );
		}
	}

	void SetScriptTouch( asIScriptFunction* pFunction ) override final
	{
		SetFunction( m_TouchFunc, pFunction, "Touch" );
	}

	void Touch( CBaseEntity* pOther ) override
	{
		CALL_EXTEND_FUNC_DIFFFUNC( Touch, DefaultScriptTouch, "(CBaseEntity@)", pOther );
	}

	void DefaultScriptUse( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float flValue )
	{
		//Script didn't override Use, so call think function that was set, if any.

		if( m_UseFunc )
		{
			//This is a delegate so we don't pass 'this'.
			as::Call( m_UseFunc.Get(), pActivator, pCaller, useType, flValue );
		}
	}

	void SetScriptUse( asIScriptFunction* pFunction ) override final
	{
		SetFunction( m_UseFunc, pFunction, "Use" );
	}

	void Use( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float flValue ) override
	{
		CALL_EXTEND_FUNC_DIFFFUNC( Use, DefaultScriptUse, "(CBaseEntity@, CBaseEntity@, USE_TYPE, float)", pActivator, pCaller, useType, flValue );
	}

	void DefaultScriptBlocked( CBaseEntity* pOther )
	{
		//Script didn't override Blocked, so call think function that was set, if any.

		if( m_BlockedFunc )
		{
			//This is a delegate so we don't pass 'this'.
			as::Call( m_BlockedFunc.Get(), pOther );
		}
	}

	void SetScriptBlocked( asIScriptFunction* pFunction ) override final
	{
		SetFunction( m_BlockedFunc, pFunction, "Blocked" );
	}

	void Blocked( CBaseEntity* pOther ) override
	{
		CALL_EXTEND_FUNC_DIFFFUNC( Blocked, DefaultScriptBlocked, "(CBaseEntity@)", pOther );
	}

	EntityClassification_t GetClassification() override
	{
		CALL_EXTEND_FUNC_RET( EntityClassification_t, GetClassification, "()" );
	}

	int BloodColor() const override
	{
		CALL_EXTEND_FUNC_RET( int, BloodColor, "() const" );
	}

	void TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult* ptr ) override
	{
		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "void TraceAttack(const CTakeDamageInfo& in, Vector, TraceResult& in)" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			method.Call( CallFlag::NONE, &info, &vecDir, &ptr );
		}
		else
		{
			BaseClass::TraceAttack( info, vecDir, ptr );
		}
	}

	void TraceBleed( const CTakeDamageInfo& info, Vector vecDir, TraceResult* ptr ) override
	{
		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "void TraceBleed(const CTakeDamageInfo& in, Vector, TraceResult& in)" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			method.Call( CallFlag::NONE, &info, &vecDir, &ptr );
		}
		else
		{
			BaseClass::TraceBleed( info, vecDir, ptr );
		}
	}

	void OnTakeDamage( const CTakeDamageInfo& info ) override
	{
		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "void OnTakeDamage(const CTakeDamageInfo& in)" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			method.Call( CallFlag::NONE, &info );
		}
		else
		{
			BaseClass::OnTakeDamage( info );
		}
	}

	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override
	{
		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "void Killed(const CTakeDamageInfo& in, GibAction)" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			method.Call( CallFlag::NONE, &info, gibAction );
		}
		else
		{
			BaseClass::Killed( info, gibAction );
		}
	}

	float GiveHealth( float flHealth, int bitsDamageType ) override
	{
		CALL_EXTEND_FUNC_RET( float, GiveHealth, "(float, int)", flHealth, bitsDamageType );
	}

	bool IsTriggered( const CBaseEntity* const pActivator ) const override
	{
		CALL_EXTEND_FUNC_RET( bool, IsTriggered, "(const CBaseEntity@) const", pActivator );
	}

	//TODO: MyMonsterPointer. Allows scripts to return null if monsters need that kind of behavior. - Solokiller

	bool IsMoving() const override
	{
		CALL_EXTEND_FUNC_RET( bool, IsMoving, "() const" );
	}

	void OverrideReset() override
	{
		CALL_EXTEND_FUNC( OverrideReset, "()" );
	}

	int DamageDecal( int bitsDamageType ) const override
	{
		CALL_EXTEND_FUNC_RET( int, DamageDecal, "(int) const", bitsDamageType );
	}

	//TODO: temporary - Solokiller

	bool OnControls( const CBaseEntity* const pTest ) const override
	{
		CALL_EXTEND_FUNC_RET( bool, OnControls, "(const CBaseEntity@) const", pTest );
	}

	bool IsAlive() const override
	{
		CALL_EXTEND_FUNC_RET( bool, IsAlive, "() const" );
	}

	bool IsBSPModel() const override
	{
		CALL_EXTEND_FUNC_RET( bool, IsBSPModel, "() const" );
	}

	bool ReflectGauss() const override
	{
		CALL_EXTEND_FUNC_RET( bool, ReflectGauss, "() const" );
	}

	bool HasTarget( string_t targetname ) const override
	{
		bool bResult = false;

		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "bool HasTarget(const string_t& in) const" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			if( method.Call( CallFlag::NONE, &targetname ) )
			{
				method.GetReturnValue( &bResult );
			}
		}
		else
		{
			bResult = BaseClass::HasTarget( targetname );
		}

		return bResult;
	}

	bool IsInWorld() const override
	{
		CALL_EXTEND_FUNC_RET( bool, IsInWorld, "() const" );
	}

	//Do not override IsPlayer or IsNetClient: code assumes it's a CBasePlayer/CBaseSpectator in that case. - Solokiller

	const char* TeamID() const override
	{
		std::string szString;

		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "string TeamID() const" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			if( method.Call( CallFlag::NONE ) )
			{
				method.GetReturnValue( &szString );
			}
		}
		else
		{
			szString = BaseClass::TeamID();
		}

		return STRING( ALLOC_STRING( szString.c_str() ) );
	}

	CBaseEntity *GetNextTarget() override
	{
		CBaseEntity* pResult = nullptr;

		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "CBaseEntity@ GetNextTarget()" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			if( method.Call( CallFlag::NONE ) )
			{
				method.GetReturnValue( &pResult );
			}
		}
		else
		{
			pResult = BaseClass::GetNextTarget();
		}

		return pResult;
	}

	bool IsLockedByMaster() const override
	{
		CALL_EXTEND_FUNC_RET( bool, IsLockedByMaster, "() const" );
	}

	void DeathNotice( CBaseEntity* pChild ) override
	{
		CALL_EXTEND_FUNC( DeathNotice, "(CBaseEntity@)", pChild );
	}

	bool BarnacleVictimGrabbed( CBaseEntity* pBarnacle ) override
	{
		CALL_EXTEND_FUNC_RET( bool, BarnacleVictimGrabbed, "(CBaseEntity@)", pBarnacle );
	}

	Vector Center() const override
	{
		CALL_EXTEND_FUNC_RET( Vector, Center, "() const" );
	}

	Vector EyePosition() const override
	{
		CALL_EXTEND_FUNC_RET( Vector, EyePosition, "() const" );
	}

	Vector EarPosition() const override
	{
		CALL_EXTEND_FUNC_RET( Vector, EarPosition, "() const" );
	}

	Vector BodyTarget( const Vector &posSrc ) const override
	{
		Vector vecResult;

		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "Vector BodyTarget(const Vector& in) const" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			if( method.Call( CallFlag::NONE, &posSrc ) )
			{
				method.GetReturnValue( &vecResult );
			}
		}
		else
		{
			vecResult = BaseClass::BodyTarget( posSrc );
		}

		return vecResult;
	}

	int Illumination() const override
	{
		CALL_EXTEND_FUNC_RET( int, Illumination, "() const" );
	}

	bool FVisible( const CBaseEntity *pEntity ) const override
	{
		CALL_EXTEND_FUNC_RET( bool, FVisible, "(const CBaseEntity@) const", pEntity );
	}

	bool FVisible( const Vector &vecOrigin ) const override
	{
		bool bResult;

		if( auto pFunction = GetObject().GetTypeInfo()->GetMethodByDecl( "Vector BodyTarget(const Vector& in) const" ) )
		{
			CASOwningContext ctx( *pFunction->GetEngine() );

			CASMethod method( *pFunction, ctx, GetObject().Get() );

			if( method.Call( CallFlag::NONE, &vecOrigin ) )
			{
				method.GetReturnValue( &bResult );
			}
		}
		else
		{
			bResult = BaseClass::FVisible( vecOrigin );
		}

		return bResult;
	}

private:
	CASObjPtr m_Instance;

	const CCustomEntityClass* m_pClass = nullptr;

	CASRefPtr<asIScriptFunction> m_ThinkFunc;
	CASRefPtr<asIScriptFunction> m_TouchFunc;
	CASRefPtr<asIScriptFunction> m_UseFunc;
	CASRefPtr<asIScriptFunction> m_BlockedFunc;
};

/**
*	C++ Class type for custom CBaseEntity entities.
*/
using CCustomCBaseEntity = CBaseCustomCBaseEntity<CBaseEntity>;

#endif //GAME_SERVER_ANGELSCRIPT_ENTITIES_CCUSTOMCBASEENTITY_H
