#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"

#include "Decals.h"

#include "CSprayCan.h"

LINK_ENTITY_TO_CLASS( spray_can, CSprayCan );

void CSprayCan::Spawn( CBaseEntity* pOwner )
{
	pev->origin = pOwner->GetAbsOrigin() + Vector( 0, 0, 32 );
	pev->angles = pOwner->pev->v_angle;
	pev->owner = pOwner->edict();
	pev->frame = 0;

	pev->nextthink = gpGlobals->time + 0.1;
	EMIT_SOUND( this, CHAN_VOICE, "player/sprayer.wav", 1, ATTN_NORM );
}

void CSprayCan::Think()
{
	TraceResult	tr;
	int playernum;
	int nFrames;
	CBasePlayer *pPlayer;

	pPlayer = ( CBasePlayer * ) GET_PRIVATE( pev->owner );

	if( pPlayer )
		nFrames = pPlayer->GetCustomDecalFrames();
	else
		nFrames = -1;

	playernum = ENTINDEX( pev->owner );

	// ALERT(at_console, "Spray by player %i, %i of %i\n", playernum, (int)(pev->frame + 1), nFrames);

	UTIL_MakeVectors( pev->angles );
	UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, &tr );

	// No customization present.
	if( nFrames == -1 )
	{
		UTIL_DecalTrace( &tr, DECAL_LAMBDA6 );
		UTIL_Remove( this );
	}
	else
	{
		UTIL_PlayerDecalTrace( &tr, playernum, pev->frame, true );
		// Just painted last custom frame.
		if( pev->frame++ >= ( nFrames - 1 ) )
			UTIL_Remove( this );
	}

	pev->nextthink = gpGlobals->time + 0.1;
}
