// Client side entity management functions

#include <memory.h>

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_types.h"
#include "studio_event.h" // def. of mstudioevent_t
#include "r_efx.h"
#include "event_api.h"
#include "pm_defs.h"
#include "pmtrace.h"	
#include "pm_shared.h"
#include "bench.h"
#include "Exports.h"
#include "com_weapons.h"

#include "ScriptEvent.h"

#include "mathlib.h"

#include "CHudSpectator.h"

#include "particleman.h"
extern IParticleMan *g_pParticleMan;

void Game_AddObjects( void );

extern Vector v_origin;

int g_iAlive = 1;

/*
========================
HUD_AddEntity
	Return 0 to filter entity from visible list for rendering
========================
*/
int DLLEXPORT HUD_AddEntity( int type, cl_entity_t *ent, const char *modelname )
{
	switch ( type )
	{
	case ET_NORMAL:
		Bench_CheckEntity( type, ent, modelname );
		break;
	case ET_PLAYER:
	case ET_BEAM:
	case ET_TEMPENTITY:
	case ET_FRAGMENTED:
	default:
		break;
	}
	// each frame every entity passes this function, so the overview hooks it to filter the overview entities
	// in spectator mode:
	// each frame every entity passes this function, so the overview hooks 
	// it to filter the overview entities

	if ( g_iUser1 )
	{
		if( auto pSpectator = GETHUDCLASS( CHudSpectator ) )
		{
			pSpectator->AddOverviewEntity( type, ent, modelname );

			if ( (	g_iUser1 == OBS_IN_EYE || pSpectator->m_pip->value == INSET_IN_EYE ) &&
					ent->index == g_iUser2 )
				return 0;	// don't draw the player we are following in eye
		}
	}

	return 1;
}

/*
=========================
HUD_TxferLocalOverrides

The server sends us our origin with extra precision as part of the clientdata structure, not during the normal
playerstate update in entity_state_t.  In order for these overrides to eventually get to the appropriate playerstate
structure, we need to copy them into the state structure at this point.
=========================
*/
void DLLEXPORT HUD_TxferLocalOverrides( entity_state_t *state, const clientdata_t *client )
{
	state->origin = client->origin;

	// Spectator
	state->iuser1 = client->iuser1;
	state->iuser2 = client->iuser2;

	// Duck prevention
	state->iuser3 = client->iuser3;

	// Fire prevention
	state->iuser4 = client->iuser4;
}

/*
=========================
HUD_ProcessPlayerState

We have received entity_state_t for this player over the network.  We need to copy appropriate fields to the
playerstate structure
=========================
*/
void DLLEXPORT HUD_ProcessPlayerState( entity_state_t *dst, const entity_state_t *src )
{
	// Copy in network data
	dst->origin = src->origin;
	dst->angles = src->angles;

	dst->velocity = src->velocity;

	dst->frame					= src->frame;
	dst->modelindex				= src->modelindex;
	dst->skin					= src->skin;
	dst->effects				= src->effects;
	dst->weaponmodel			= src->weaponmodel;
	dst->movetype				= src->movetype;
	dst->sequence				= src->sequence;
	dst->animtime				= src->animtime;
	
	dst->solid					= src->solid;
	
	dst->rendermode				= src->rendermode;
	dst->renderamt				= src->renderamt;	
	dst->rendercolor.r			= src->rendercolor.r;
	dst->rendercolor.g			= src->rendercolor.g;
	dst->rendercolor.b			= src->rendercolor.b;
	dst->renderfx				= src->renderfx;

	dst->framerate				= src->framerate;
	dst->body					= src->body;

	memcpy( &dst->controller[0], &src->controller[0], 4 * sizeof( byte ) );
	memcpy( &dst->blending[0], &src->blending[0], 2 * sizeof( byte ) );

	dst->basevelocity = src->basevelocity;

	dst->friction				= src->friction;
	dst->gravity				= src->gravity;
	dst->gaitsequence			= src->gaitsequence;
	dst->spectator				= src->spectator;
	dst->usehull				= src->usehull;
	dst->playerclass			= src->playerclass;
	dst->team					= src->team;
	dst->colormap				= src->colormap;

#if defined( _TFC )
	dst->fuser1					= src->fuser1;
#endif

	// Save off some data so other areas of the Client DLL can get to it
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();	// Get the local player's index
	if ( dst->number == player->index )
	{
		g_iPlayerClass = dst->playerclass;
		g_iTeamNumber = dst->team;

		g_iUser1 = src->iuser1;
		g_iUser2 = src->iuser2;
		g_iUser3 = src->iuser3;
	}
}

/*
=========================
HUD_TxferPredictionData

Because we can predict an arbitrary number of frames before the server responds with an update, we need to be able to copy client side prediction data in
 from the state that the server ack'd receiving, which can be anywhere along the predicted frame path ( i.e., we could predict 20 frames into the future and the server ack's
 up through 10 of those frames, so we need to copy persistent client-side only state from the 10th predicted frame to the slot the server
 update is occupying.
=========================
*/
void DLLEXPORT HUD_TxferPredictionData ( entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd )
{
	ps->oldbuttons				= pps->oldbuttons;
	ps->flFallVelocity			= pps->flFallVelocity;
	ps->iStepLeft				= pps->iStepLeft;
	ps->playerclass				= pps->playerclass;

	pcd->viewmodel				= ppcd->viewmodel;
	pcd->m_iId					= ppcd->m_iId;
	pcd->ammo_shells			= ppcd->ammo_shells;
	pcd->ammo_nails				= ppcd->ammo_nails;
	pcd->ammo_cells				= ppcd->ammo_cells;
	pcd->ammo_rockets			= ppcd->ammo_rockets;
	pcd->m_flNextAttack			= ppcd->m_flNextAttack;
	pcd->fov					= ppcd->fov;
	pcd->weaponanim				= ppcd->weaponanim;
	pcd->tfstate				= ppcd->tfstate;
	pcd->maxspeed				= ppcd->maxspeed;

	pcd->deadflag				= ppcd->deadflag;

	// Spectating or not dead == get control over view angles.
	g_iAlive = ( ppcd->iuser1 || ( pcd->deadflag == DEAD_NO ) ) ? 1 : 0;

	// Spectator
	pcd->iuser1					= ppcd->iuser1;
	pcd->iuser2					= ppcd->iuser2;

	// Duck prevention
	pcd->iuser3 = ppcd->iuser3;

	if ( gEngfuncs.IsSpectateOnly() )
	{
		// in specator mode we tell the engine who we want to spectate and how
		// iuser3 is not used for duck prevention (since the spectator can't duck at all)
		pcd->iuser1 = g_iUser1;	// observer mode
		pcd->iuser2 = g_iUser2; // first target
		pcd->iuser3 = g_iUser3; // second target
	}

	// Fire prevention
	pcd->iuser4 = ppcd->iuser4;

	pcd->fuser2					= ppcd->fuser2;
	pcd->fuser3					= ppcd->fuser3;

	pcd->vuser1 = ppcd->vuser1;
	pcd->vuser2 = ppcd->vuser2;
	pcd->vuser3 = ppcd->vuser3;
	pcd->vuser4 = ppcd->vuser4;

	memcpy( wd, pwd, MAX_WEAPONS * sizeof( weapon_data_t ) );
}

#if defined( BEAM_TEST )
// Note can't index beam[ 0 ] in Beam callback, so don't use that index
// Room for 1 beam ( 0 can't be used )
static cl_entity_t beams[ 2 ];

void BeamEndModel( void )
{
	cl_entity_t *player, *model;
	int modelindex;
	model_t *mod;

	// Load it up with some bogus data
	player = gEngfuncs.GetLocalPlayer();
	if ( !player )
		return;

	mod = gEngfuncs.CL_LoadModel( "models/sentry3.mdl", &modelindex );
	if ( !mod )
		return;

	// Slot 1
	model = &beams[ 1 ];

	*model = *player;
	model->player = 0;
	model->model = mod;
	model->curstate.modelindex = modelindex;
		
	// Move it out a bit
	model->origin[0] = player->origin[0] - 100;
	model->origin[1] = player->origin[1];

	model->attachment[0] = model->origin;
	model->attachment[1] = model->origin;
	model->attachment[2] = model->origin;
	model->attachment[3] = model->origin;

	gEngfuncs.CL_CreateVisibleEntity( ET_NORMAL, model );
}

void Beams( void )
{
	static float lasttime;
	float curtime;
	model_t *mod;
	int index;

	BeamEndModel();
	
	curtime = gEngfuncs.GetClientTime();
	Vector end;

	if ( ( curtime - lasttime ) < 10.0 )
		return;

	mod = gEngfuncs.CL_LoadModel( "sprites/laserbeam.spr", &index );
	if ( !mod )
		return;

	lasttime = curtime;

	end [ 0 ] = v_origin.x + 100;
	end [ 1 ] = v_origin.y + 100;
	end [ 2 ] = v_origin.z;

	BEAM *p1;
	p1 = gEngfuncs.pEfxAPI->R_BeamEntPoint( -1, end, index,
		10.0, 2.0, 0.3, 1.0, 5.0, 0.0, 1.0, 1.0, 1.0, 1.0 );
}
#endif

/*
=========================
HUD_CreateEntities
	
Gives us a chance to add additional entities to the render this frame
=========================
*/
void DLLEXPORT HUD_CreateEntities( void )
{
#if defined( BEAM_TEST )
	Beams();
#endif

	Bench_AddObjects();

	// Add in any game specific objects
	Game_AddObjects();

	GetClientVoiceMgr()->CreateEntities();
}

#if defined( _TFC )
extern int g_bACSpinning[ MAX_CLIENTS + 1 ];
#endif 

/*
=========================
HUD_StudioEvent

The entity's studio model description indicated an event was
fired during this frame, handle the event by it's tag ( e.g., muzzleflash, sound )
=========================
*/
void DLLEXPORT HUD_StudioEvent( const mstudioevent_t *event, const cl_entity_t *entity )
{
	int iMuzzleFlash = 1;

#if defined( _TFC )

	if ( g_bACSpinning[ entity->index - 1 ] )
		iMuzzleFlash = 0;

#endif 

	switch( event->event )
	{
	case SCRIPT_EVENT_MUZZLEFLASH_ATTACH0:
		if ( iMuzzleFlash )
			gEngfuncs.pEfxAPI->R_MuzzleFlash( entity->attachment[0], atoi( event->options) );
		break;
	case SCRIPT_EVENT_MUZZLEFLASH_ATTACH1:
		if ( iMuzzleFlash )
			gEngfuncs.pEfxAPI->R_MuzzleFlash( entity->attachment[1], atoi( event->options) );
		break;
	case SCRIPT_EVENT_MUZZLEFLASH_ATTACH2:
		if ( iMuzzleFlash )
			gEngfuncs.pEfxAPI->R_MuzzleFlash( entity->attachment[2], atoi( event->options) );
		break;
	case SCRIPT_EVENT_MUZZLEFLASH_ATTACH3:
		if ( iMuzzleFlash )
			gEngfuncs.pEfxAPI->R_MuzzleFlash( entity->attachment[3], atoi( event->options) );
		break;
	case SCRIPT_EVENT_SPARK:
		gEngfuncs.pEfxAPI->R_SparkEffect( entity->attachment[0], atoi( event->options), -100, 100 );
		break;
	// Client side sound
	case SCRIPT_EVENT_CLIENT_PLAYSOUND:
		gEngfuncs.pfnPlaySoundByNameAtLocation( event->options, 1.0, entity->attachment[0] );
		break;
	default:
		break;
	}
}

/*
=================
CL_UpdateTEnts

Simulation and cleanup of temporary entities
=================
*/
void DLLEXPORT HUD_TempEntUpdate (
	double frametime,   // Simulation time
	double client_time, // Absolute time on client
	double cl_gravity,  // True gravity on client
	TEMPENTITY **ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY **ppTempEntActive, // List 
	Callback_AddVisibleEntity pAddVisibleEnt, 
	Callback_TempEntPlaySound pTempPlaySound )
{
	static int gTempEntFrame = 0;

	Vector		vAngles;

	if( g_iUser1 != 0 && g_iUser1 != OBS_ROAMING )
	{
		vAngles = v_angles;
	}
	else
	{
		gEngfuncs.GetViewAngles( vAngles );
	}

	//TODO: is this supposed to happen here or in HUD_DrawTransparentTriangles? - Solokiller
	if ( g_pParticleMan )
		 g_pParticleMan->SetVariables( cl_gravity, vAngles );

	// Nothing to simulate
	if ( !*ppTempEntActive )		
		return;

	// in order to have tents collide with players, we have to run the player prediction code so
	// that the client has the player list. We run this code once when we detect any COLLIDEALL 
	// tent, then set this bool to true so the code doesn't get run again if there's more than
	// one COLLIDEALL ent for this update. (often are).
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( -1 );	

	// !!!BUGBUG	-- This needs to be time based
	gTempEntFrame = (gTempEntFrame+1) & 31;

	TEMPENTITY* pTemp = *ppTempEntActive;

	// !!! Don't simulate while paused....  This is sort of a hack, revisit.
	if ( frametime <= 0 )
	{
		while ( pTemp )
		{
			if ( !(pTemp->flags & FTENT_NOMODEL ) )
			{
				pAddVisibleEnt( &pTemp->entity );
			}
			pTemp = pTemp->next;
		}
	}
	else
	{
		int			i;
		TEMPENTITY *pnext;
		float		life;

		TEMPENTITY* pprev = NULL;
		//const float freq = client_time * 0.01;
		const float fastFreq = client_time * 5.5;
		const float gravity = -frametime * cl_gravity;
		const float gravitySlow = gravity * 0.5;

		while ( pTemp )
		{
			int active;

			active = 1;

			life = pTemp->die - client_time;
			pnext = pTemp->next;
			if ( life < 0 )
			{
				if ( pTemp->flags & FTENT_FADEOUT )
				{
					if (pTemp->entity.curstate.rendermode == kRenderNormal)
						pTemp->entity.curstate.rendermode = kRenderTransTexture;
					pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt * ( 1 + life * pTemp->fadeSpeed );
					if ( pTemp->entity.curstate.renderamt <= 0 )
						active = 0;

				}
				else 
					active = 0;
			}
			if ( !active )		// Kill it
			{
				pTemp->next = *ppTempEntFree;
				*ppTempEntFree = pTemp;
				if ( !pprev )	// Deleting at head of list
					*ppTempEntActive = pnext;
				else
					pprev->next = pnext;
			}
			else
			{
				pprev = pTemp;
			
				pTemp->entity.prevstate.origin = pTemp->entity.origin;

				if ( pTemp->flags & FTENT_SPARKSHOWER )
				{
					// Adjust speed if it's time
					// Scale is next think time
					if ( client_time > pTemp->entity.baseline.scale )
					{
						// Show Sparks
						gEngfuncs.pEfxAPI->R_SparkEffect( pTemp->entity.origin, 8, -200, 200 );

						// Reduce life
						pTemp->entity.baseline.framerate -= 0.1;

						if ( pTemp->entity.baseline.framerate <= 0.0 )
						{
							pTemp->die = client_time;
						}
						else
						{
							// So it will die no matter what
							pTemp->die = client_time + 0.5;

							// Next think
							pTemp->entity.baseline.scale = client_time + 0.1;
						}
					}
				}
				else if ( pTemp->flags & FTENT_PLYRATTACHMENT )
				{
					cl_entity_t *pClient;

					pClient = gEngfuncs.GetEntityByIndex( pTemp->clientIndex );

					pTemp->entity.origin = pClient->origin + pTemp->tentOffset;
				}
				else if ( pTemp->flags & FTENT_SINEWAVE )
				{
					pTemp->x += pTemp->entity.baseline.origin[0] * frametime;
					pTemp->y += pTemp->entity.baseline.origin[1] * frametime;

					pTemp->entity.origin[0] = pTemp->x + sin( pTemp->entity.baseline.origin[2] + client_time * pTemp->entity.prevstate.frame ) * (10*pTemp->entity.curstate.framerate);
					pTemp->entity.origin[1] = pTemp->y + sin( pTemp->entity.baseline.origin[2] + fastFreq + 0.7 ) * (8*pTemp->entity.curstate.framerate);
					pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
				}
				else if ( pTemp->flags & FTENT_SPIRAL )
				{
					//const float s = sin( pTemp->entity.baseline.origin[2] + fastFreq );
					//const float c = cos( pTemp->entity.baseline.origin[2] + fastFreq );

					pTemp->entity.origin[0] += pTemp->entity.baseline.origin[0] * frametime + 8 * sin( client_time * 20 + (int)pTemp );
					pTemp->entity.origin[1] += pTemp->entity.baseline.origin[1] * frametime + 4 * sin( client_time * 30 + (int)pTemp );
					pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
				}
			
				else 
				{
					for ( i = 0; i < 3; i++ ) 
						pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * frametime;
				}
			
				if ( pTemp->flags & FTENT_SPRANIMATE )
				{
					pTemp->entity.curstate.frame += frametime * pTemp->entity.curstate.framerate;
					if ( pTemp->entity.curstate.frame >= pTemp->frameMax )
					{
						pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);

						if ( !(pTemp->flags & FTENT_SPRANIMATELOOP) )
						{
							// this animating sprite isn't set to loop, so destroy it.
							pTemp->die = client_time;
							pTemp = pnext;
							continue;
						}
					}
				}
				else if ( pTemp->flags & FTENT_SPRCYCLE )
				{
					pTemp->entity.curstate.frame += frametime * 10;
					if ( pTemp->entity.curstate.frame >= pTemp->frameMax )
					{
						pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);
					}
				}
	// Experiment
	#if 0
				if ( pTemp->flags & FTENT_SCALE )
					pTemp->entity.curstate.framerate += 20.0 * (frametime / pTemp->entity.curstate.framerate);
	#endif

				if ( pTemp->flags & FTENT_ROTATE )
				{
					pTemp->entity.angles[0] += pTemp->entity.baseline.angles[0] * frametime;
					pTemp->entity.angles[1] += pTemp->entity.baseline.angles[1] * frametime;
					pTemp->entity.angles[2] += pTemp->entity.baseline.angles[2] * frametime;

					pTemp->entity.latched.prevangles = pTemp->entity.angles;
				}

				if ( pTemp->flags & (FTENT_COLLIDEALL | FTENT_COLLIDEWORLD) )
				{
					Vector	traceNormal;
					float	traceFraction = 1;

					if ( pTemp->flags & FTENT_COLLIDEALL )
					{
						pmtrace_t pmtrace;
						physent_t *pe;
				
						gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );

						gEngfuncs.pEventAPI->EV_PlayerTrace( pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX, -1, &pmtrace );


						if ( pmtrace.fraction != 1 )
						{
							pe = gEngfuncs.pEventAPI->EV_GetPhysent( pmtrace.ent );

							if ( !pmtrace.ent || ( pe->info != pTemp->clientIndex ) )
							{
								traceFraction = pmtrace.fraction;
								traceNormal = pmtrace.plane.normal;

								if ( pTemp->hitcallback )
								{
									(*pTemp->hitcallback)( pTemp, &pmtrace );
								}
							}
						}
					}
					else if ( pTemp->flags & FTENT_COLLIDEWORLD )
					{
						pmtrace_t pmtrace;
					
						gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );

						gEngfuncs.pEventAPI->EV_PlayerTrace( pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX | PM_WORLD_ONLY, -1, &pmtrace );					

						if ( pmtrace.fraction != 1 )
						{
							traceFraction = pmtrace.fraction;
							traceNormal = pmtrace.plane.normal;

							if ( pTemp->flags & FTENT_SPARKSHOWER )
							{
								// Chop spark speeds a bit more
								//
								pTemp->entity.baseline.origin = pTemp->entity.baseline.origin * 0.6;

								if ( pTemp->entity.baseline.origin.Length() < 10 )
								{
									pTemp->entity.baseline.framerate = 0.0;								
								}
							}

							if ( pTemp->hitcallback )
							{
								(*pTemp->hitcallback)( pTemp, &pmtrace );
							}
						}
					}
				
					if ( traceFraction != 1 )	// Decent collision now, and damping works
					{
						float  proj, damp;

						// Place at contact point
						VectorMA( pTemp->entity.prevstate.origin, traceFraction*frametime, pTemp->entity.baseline.origin, pTemp->entity.origin );
						// Damp velocity
						damp = pTemp->bounceFactor;
						if ( pTemp->flags & (FTENT_GRAVITY|FTENT_SLOWGRAVITY) )
						{
							damp *= 0.5;
							if ( traceNormal[2] > 0.9 )		// Hit floor?
							{
								if ( pTemp->entity.baseline.origin[2] <= 0 && pTemp->entity.baseline.origin[2] >= gravity*3 )
								{
									damp = 0;		// Stop
									pTemp->flags &= ~(FTENT_ROTATE|FTENT_GRAVITY|FTENT_SLOWGRAVITY|FTENT_COLLIDEWORLD|FTENT_SMOKETRAIL);
									pTemp->entity.angles[0] = 0;
									pTemp->entity.angles[2] = 0;
								}
							}
						}

						if (pTemp->hitSound)
						{
							pTempPlaySound(pTemp, damp);
						}

						if (pTemp->flags & FTENT_COLLIDEKILL)
						{
							// die on impact
							pTemp->flags &= ~FTENT_FADEOUT;	
							pTemp->die = client_time;			
						}
						else
						{
							// Reflect velocity
							if ( damp != 0 )
							{
								proj = DotProduct( pTemp->entity.baseline.origin, traceNormal );
								VectorMA( pTemp->entity.baseline.origin, -proj*2, traceNormal, pTemp->entity.baseline.origin );
								// Reflect rotation (fake)

								pTemp->entity.angles[1] = -pTemp->entity.angles[1];
							}
						
							if ( damp != 1 )
							{
								pTemp->entity.baseline.origin = pTemp->entity.baseline.origin * damp;
								pTemp->entity.angles = pTemp->entity.angles * 0.9;
							}
						}
					}
				}


				if ( (pTemp->flags & FTENT_FLICKER) && gTempEntFrame == pTemp->entity.curstate.effects )
				{
					dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight (0);
					dl->origin = pTemp->entity.origin;
					dl->radius = 60;
					dl->color.r = 255;
					dl->color.g = 120;
					dl->color.b = 0;
					dl->die = client_time + 0.01;
				}

				if ( pTemp->flags & FTENT_SMOKETRAIL )
				{
					gEngfuncs.pEfxAPI->R_RocketTrail (pTemp->entity.prevstate.origin, pTemp->entity.origin, RocketTrailType::SMOKE );
				}

				if ( pTemp->flags & FTENT_GRAVITY )
					pTemp->entity.baseline.origin[2] += gravity;
				else if ( pTemp->flags & FTENT_SLOWGRAVITY )
					pTemp->entity.baseline.origin[2] += gravitySlow;

				if ( pTemp->flags & FTENT_CLIENTCUSTOM )
				{
					if ( pTemp->callback )
					{
						( *pTemp->callback )( pTemp, frametime, client_time );
					}
				}

				// Cull to PVS (not frustum cull, just PVS)
				if ( !(pTemp->flags & FTENT_NOMODEL ) )
				{
					if ( !pAddVisibleEnt( &pTemp->entity ) )
					{
						if ( !(pTemp->flags & FTENT_PERSIST) ) 
						{
							pTemp->die = client_time;			// If we can't draw it this frame, just dump it.
							pTemp->flags &= ~FTENT_FADEOUT;	// Don't fade out, just die
						}
					}
				}
			}
			pTemp = pnext;
		}
	}

	// Restore state info
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

/*
=================
HUD_GetUserEntity

If you specify negative numbers for beam start and end point entities, then
  the engine will call back into this function requesting a pointer to a cl_entity_t 
  object that describes the entity to attach the beam onto.

Indices must start at 1, not zero.
=================
*/
cl_entity_t DLLEXPORT *HUD_GetUserEntity( int index )
{
#if defined( BEAM_TEST )
	// None by default, you would return a valic pointer if you create a client side
	//  beam and attach it to a client side entity.
	if ( index > 0 && index <= 1 )
	{
		return &beams[ index ];
	}
	else
	{
		return NULL;
	}
#else
	return NULL;
#endif
}

