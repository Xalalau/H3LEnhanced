#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"
// ############ hu3lifezado ############ //
// Usado para copiarmos valores de qualidade da Tauros do server para o client
#include "entities/weapons/CDesertEagle.h"
// ############ //


#include "CWeaponBox.h"

//*********************************************************
// weaponbox code:
//*********************************************************

BEGIN_DATADESC( CWeaponBox )
	DEFINE_ARRAY( m_rgAmmo, FIELD_INTEGER, CAmmoTypes::MAX_AMMO_TYPES ),
	DEFINE_ARRAY( m_rgiszAmmo, FIELD_STRING, CAmmoTypes::MAX_AMMO_TYPES ),
	DEFINE_ARRAY( m_rgpPlayerItems, FIELD_CLASSPTR, MAX_WEAPON_SLOTS ),
	DEFINE_FIELD( m_cAmmoTypes, FIELD_INTEGER ),
	DEFINE_THINKFUNC( Kill ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( weaponbox, CWeaponBox );

//=========================================================
//
//=========================================================
void CWeaponBox::Precache()
{
	PRECACHE_MODEL( "models/w_weaponbox.mdl" );
}

//=========================================================
// CWeaponBox - Spawn 
//=========================================================
void CWeaponBox::Spawn()
{
	Precache();

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	SetSize( g_vecZero, g_vecZero );

	SetModel( "models/w_weaponbox.mdl" );
}

//=========================================================
// CWeaponBox - Touch: try to add my contents to the toucher
// if the toucher is a player.
//=========================================================
void CWeaponBox::Touch( CBaseEntity *pOther )
{
	if( !( pev->flags & FL_ONGROUND ) )
	{
		return;
	}

	if( !pOther->IsPlayer() )
	{
		// only players may touch a weaponbox.
		return;
	}

	if( !pOther->IsAlive() )
	{
		// no dead guys.
		return;
	}

	CBasePlayer *pPlayer = ( CBasePlayer * ) pOther;
	int i;

	// dole out ammo
	for( i = 0; i < CAmmoTypes::MAX_AMMO_TYPES; i++ )
	{
		if( !FStringNull( m_rgiszAmmo[ i ] ) )
		{
			// there's some ammo of this type. 
			pPlayer->GiveAmmo( m_rgAmmo[ i ], ( char * ) STRING( m_rgiszAmmo[ i ] ) );

			//ALERT ( at_console, "Gave %d rounds of %s\n", m_rgAmmo[i], STRING(m_rgiszAmmo[i]) );

			// now empty the ammo from the weaponbox since we just gave it to the player
			m_rgiszAmmo[ i ] = iStringNull;
			m_rgAmmo[ i ] = 0;
		}
	}

	// go through my weapons and try to give the usable ones to the player. 
	// it's important the the player be given ammo first, so the weapons code doesn't refuse 
	// to deploy a better weapon that the player may pick up because he has no ammo for it.
	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		if( m_rgpPlayerItems[ i ] )
		{
			CBasePlayerWeapon *pItem;

			// have at least one weapon in this slot
			while( m_rgpPlayerItems[ i ] )
			{
				//ALERT ( at_console, "trying to give %s\n", m_rgpPlayerItems[ i ]->GetClassname() );

				pItem = m_rgpPlayerItems[ i ];

				// ############ hu3lifezado ############ //
				// Tiro secundario, adaptado de:
				// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
				// Hack for flying_crowbar. Dont pickup if player already has crowbar. 
				if (pItem->m_iId == WEAPON_CROWBAR)
				{
					int j;
					// check if the player already has this weapon
					for (j = 0; j < MAX_WEAPON_SLOTS; j++)
					{
						CBasePlayerWeapon *it = pPlayer->m_rgpPlayerItems[j];

						while (it != NULL)
						{
							if (it->m_iId == WEAPON_CROWBAR)
								return;

							it = it->m_pNext;
						}
					}
				}

				// Comando para copiarmos valores de qualidade da Tauros do server para o client
				CDesertEagle * pItem_hu3 = (CDesertEagle *) pItem;
				if (pItem_hu3->m_quality)
				{
					char command[35] = "hu3_touros_gambiarra_qualidade ";
					char value[2];
					snprintf(value, 2, "%d", pItem_hu3->m_quality);
					strcat(strcat(command, value), "\n");
					CLIENT_COMMAND( ENT( pPlayer ), command);
				}
				// ############ //

				m_rgpPlayerItems[ i ] = m_rgpPlayerItems[ i ]->m_pNext;// unlink this weapon from the box

				

				if( pPlayer->AddPlayerItem( pItem ) )
				{
					pItem->AttachToPlayer( pPlayer );
				}
			}
		}
	}

	EMIT_SOUND( pOther, CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM );
	SetTouch( NULL );
	UTIL_Remove( this );
}

//=========================================================
//=========================================================
void CWeaponBox::KeyValue( KeyValueData *pkvd )
{
	if( m_cAmmoTypes < CAmmoTypes::MAX_AMMO_TYPES )
	{
		PackAmmo( ALLOC_STRING( pkvd->szKeyName ), atoi( pkvd->szValue ) );
		m_cAmmoTypes++;// count this new ammo type.

		pkvd->fHandled = true;
	}
	else
	{
		ALERT( at_console, "WeaponBox too full! only %d ammotypes allowed\n", CAmmoTypes::MAX_AMMO_TYPES );
	}
}

//=========================================================
// CWeaponBox::IsEmpty - is there anything in this box?
//=========================================================
bool CWeaponBox::IsEmpty() const
{
	int i;

	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		if( m_rgpPlayerItems[ i ] )
		{
			return false;
		}
	}

	for( i = 0; i < CAmmoTypes::MAX_AMMO_TYPES; i++ )
	{
		if( !FStringNull( m_rgiszAmmo[ i ] ) )
		{
			// still have a bit of this type of ammo
			return false;
		}
	}

	return true;
}

//=========================================================
// CWeaponBox - GiveAmmo
//=========================================================
int CWeaponBox::GiveAmmo( int iCount, char *szName, int iMax, int *pIndex/* = NULL*/ )
{
	int i;

	for( i = 1; i < CAmmoTypes::MAX_AMMO_TYPES && !FStringNull( m_rgiszAmmo[ i ] ); i++ )
	{
		if( stricmp( szName, STRING( m_rgiszAmmo[ i ] ) ) == 0 )
		{
			if( pIndex )
				*pIndex = i;

			int iAdd = min( iCount, iMax - m_rgAmmo[ i ] );
			if( iCount == 0 || iAdd > 0 )
			{
				m_rgAmmo[ i ] += iAdd;

				return i;
			}
			return -1;
		}
	}
	if( i < CAmmoTypes::MAX_AMMO_TYPES )
	{
		if( pIndex )
			*pIndex = i;

		m_rgiszAmmo[ i ] = MAKE_STRING( szName );
		m_rgAmmo[ i ] = iCount;

		return i;
	}
	ALERT( at_console, "out of named ammo slots\n" );
	return i;
}

//=========================================================
//=========================================================
void CWeaponBox::SetObjectCollisionBox()
{
	pev->absmin = GetAbsOrigin() + Vector( -16, -16, 0 );
	pev->absmax = GetAbsOrigin() + Vector( 16, 16, 16 );
}

//=========================================================
// CWeaponBox - Kill - the think function that removes the
// box from the world.
//=========================================================
void CWeaponBox::Kill()
{
	CBasePlayerWeapon *pWeapon;
	int i;

	// destroy the weapons
	for( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		pWeapon = m_rgpPlayerItems[ i ];

		while( pWeapon )
		{
			pWeapon->SetThink( &CBasePlayerWeapon::SUB_Remove );
			pWeapon->pev->nextthink = gpGlobals->time + 0.1;
			pWeapon = pWeapon->m_pNext;
		}
	}

	// remove the box
	UTIL_Remove( this );
}

//=========================================================
// CWeaponBox::HasWeapon - is a weapon of this type already
// packed in this box?
//=========================================================
bool CWeaponBox::HasWeapon( CBasePlayerWeapon *pCheckItem ) const
{
	CBasePlayerWeapon *pItem = m_rgpPlayerItems[ pCheckItem->iItemSlot() ];

	while( pItem )
	{
		if( pItem->ClassnameIs( pCheckItem->GetClassname() ) )
		{
			return true;
		}
		pItem = pItem->m_pNext;
	}

	return false;
}

//=========================================================
// CWeaponBox - PackWeapon: Add this weapon to the box
//=========================================================
bool CWeaponBox::PackWeapon( CBasePlayerWeapon *pWeapon )
{
	// is one of these weapons already packed in this box?
	if( HasWeapon( pWeapon ) )
	{
		return false;// box can only hold one of each weapon type
	}

	if( pWeapon->m_pPlayer )
	{
		if( !pWeapon->m_pPlayer->RemovePlayerItem( pWeapon ) )
		{
			// failed to unhook the weapon from the player!
			return false;
		}
	}

	int iWeaponSlot = pWeapon->iItemSlot();

	if( m_rgpPlayerItems[ iWeaponSlot ] )
	{
		// there's already one weapon in this slot, so link this into the slot's column
		pWeapon->m_pNext = m_rgpPlayerItems[ iWeaponSlot ];
		m_rgpPlayerItems[ iWeaponSlot ] = pWeapon;
	}
	else
	{
		// first weapon we have for this slot
		m_rgpPlayerItems[ iWeaponSlot ] = pWeapon;
		pWeapon->m_pNext = NULL;
	}

	pWeapon->pev->spawnflags |= SF_NORESPAWN;// never respawn
	pWeapon->pev->movetype = MOVETYPE_NONE;
	pWeapon->pev->solid = SOLID_NOT;
	pWeapon->pev->effects = EF_NODRAW;
	pWeapon->pev->modelindex = 0;
	pWeapon->pev->model = iStringNull;
	pWeapon->pev->owner = edict();
	pWeapon->SetThink( NULL );// crowbar may be trying to swing again, etc.
	pWeapon->SetTouch( NULL );
	pWeapon->m_pPlayer = NULL;

	//ALERT ( at_console, "packed %s\n", pWeapon->GetClassname() );

	// ############ hulifezado ############ //
	// Tiro secundario, adaptado de:
	// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
	// If we are packing a crowbar, change the model to the world crowbar. 
	if (pWeapon->m_iId == WEAPON_CROWBAR)
	{
		SET_MODEL(ENT(pev), "models/w_crowbar.mdl");
	}
	// ############ //

	return true;
}

//=========================================================
// CWeaponBox - PackAmmo
//=========================================================
bool CWeaponBox::PackAmmo( int iszName, int iCount )
{
	if( FStringNull( iszName ) )
	{
		// error here
		ALERT( at_console, "NULL String in PackAmmo!\n" );
		return false;
	}

	const int iMaxCarry = g_AmmoTypes.GetMaxCarryByName( STRING( iszName ) );

	if( iMaxCarry != -1 && iCount > 0 )
	{
		//ALERT ( at_console, "Packed %d rounds of %s\n", iCount, STRING(iszName) );
		GiveAmmo( iCount, ( char * ) STRING( iszName ), iMaxCarry );
		return true;
	}

	return false;
}