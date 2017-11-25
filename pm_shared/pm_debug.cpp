/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include <cstring>

#include "mathlib.h"
#include "const.h"
#include "usercmd.h"
#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_movevars.h"
#include "pm_debug.h"

#include "com_model.h"

extern playermove_t *pmove;

// Expand debugging BBOX particle hulls by this many units.
#define BOX_GAP 0.0f               

static const int PM_boxpnt[6][4] =
{
	{ 0, 4, 6, 2 }, // +X
	{ 0, 1, 5, 4 }, // +Y
	{ 0, 2, 3, 1 }, // +Z
	{ 7, 5, 1, 3 }, // -X
	{ 7, 3, 2, 6 }, // -Y
	{ 7, 6, 4, 5 }, // -Z
};	

void PM_ShowClipBox()
{
#if defined( _DEBUG )
	Vector offset( 0, 0, 0 );

	if ( !pmove->runfuncs )
		return;

	// More debugging, draw the particle bbox for player and for the entity we are looking directly at.
	//  aslo prints entity info to the console overlay.
	//if ( !pmove->server )
	//	return;

	// Draw entity in center of view
	// Also draws the normal to the clip plane that intersects our movement ray.  Leaves a particle
	//  trail at the intersection point.
	PM_ViewEntity();

	Vector org = pmove->origin;

	if ( pmove->server )
	{
		org = org + offset;
	}
	else
	{
		org = org - offset;
	}

	// Show our BBOX in particles.
	PM_DrawBBox( pmove->player_mins[pmove->usehull], pmove->player_maxs[pmove->usehull], org, pmove->server ? 132 : 0, 0.1f );

	PM_ParticleLine( org, org, pmove->server ? 132 : 0, 0.1f, 5.0f );
/*
	{
		int i;
		for ( i = 0; i < pmove->numphysent; i++ )
		{
			if ( pmove->physents[ i ].info >= 1 && pmove->physents[ i ].info <= 4 )
			{
			 	PM_DrawBBox( pmove->player_mins[pmove->usehull], pmove->player_maxs[pmove->usehull], pmove->physents[i].origin, 132, 0.1 );
			}
		}
	}
*/
#endif
}

/*
===============
PM_ParticleLine(const Vector& start, const Vector& end, int color, float life)

================
*/
void PM_ParticleLine( const Vector& start, const Vector& end, int pcolor, float life, float vert )
{
	float linestep = 2.0f;
	float curdist;
	float len;
	Vector curpos;
	Vector diff;
	// Determine distance;

	diff = end - start;
	
	len = VectorNormalize(diff);

	curdist = 0;
	while (curdist <= len)
	{
		VectorMA( start, curdist, diff, curpos );
		
		pmove->PM_Particle( curpos, pcolor, life, 0, static_cast<int>( vert ) );
		curdist += linestep;
	}

}

/*
================
PM_DrawRectangle(const Vector& tl, const Vector& br)

================
*/
void PM_DrawRectangle( const Vector& tl, const Vector& bl, const Vector& tr, const Vector& br, int pcolor, float life )
{
	PM_ParticleLine(tl, bl, pcolor, life, 0);
	PM_ParticleLine(bl, br, pcolor, life, 0);
	PM_ParticleLine(br, tr, pcolor, life, 0);
	PM_ParticleLine(tr, tl, pcolor, life, 0);
}

/*
================
PM_DrawPhysEntBBox(int num)

================
*/
void PM_DrawPhysEntBBox( int num, int pcolor, float life )
{
	physent_t *pe;
	int j;
	Vector tmp;
	Vector p[8];
	float gap = BOX_GAP;
	Vector modelmins, modelmaxs;

	if (num >= pmove->numphysent ||
		num <= 0)
		return;

	pe = &pmove->physents[num];

	if (pe->model)
	{
		const Vector org = pe->origin;

		pmove->PM_GetModelBounds( pe->model, modelmins, modelmaxs );
		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? modelmins[0] - gap : modelmaxs[0] + gap;
			tmp[1] = (j & 2) ? modelmins[1] - gap : modelmaxs[1] + gap;
			tmp[2] = (j & 4) ? modelmins[2] - gap : modelmaxs[2] + gap;

			p[ j ] = tmp;
		}

		// If the bbox should be rotated, do that
		if (pe->angles[0] || pe->angles[1] || pe->angles[2])
		{
			Vector forward, right, up;

			AngleVectorsTranspose(pe->angles, forward, right, up);
			for (j = 0; j < 8; j++)
			{
				tmp = p[ j ];
				p[j][0] = DotProduct  ( tmp, forward );
				p[j][1] = DotProduct ( tmp, right );
				p[j][2] = DotProduct  ( tmp, up );
			}
		}

		// Offset by entity origin, if any.
		for (j = 0; j < 8; j++)
		{
			p[ j ] = p[ j ] + org;
		}

		for (j = 0; j < 6; j++)
		{
			PM_DrawRectangle(
				p[PM_boxpnt[j][1]],
				p[PM_boxpnt[j][0]],
				p[PM_boxpnt[j][2]],
				p[PM_boxpnt[j][3]],
				pcolor, life);
		}
	}
	else
	{
		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pe->mins[0] : pe->maxs[0];
			tmp[1] = (j & 2) ? pe->mins[1] : pe->maxs[1];
			tmp[2] = (j & 4) ? pe->mins[2] : pe->maxs[2];

			tmp = tmp + pe->origin;
			p[ j ] = tmp;
		}

		for (j = 0; j < 6; j++)
		{
			PM_DrawRectangle(
				p[PM_boxpnt[j][1]],
				p[PM_boxpnt[j][0]],
				p[PM_boxpnt[j][2]],
				p[PM_boxpnt[j][3]],
				pcolor, life);
		}

	}
}

/*
================
PM_DrawBBox(const Vector& mins, const Vector& maxs, const Vector& origin, int pcolor, float life)

================
*/
void PM_DrawBBox( const Vector& mins, const Vector& maxs, const Vector& origin, int pcolor, float life )
{
	int j;
	
	Vector tmp;
	Vector p[8];
	float gap = BOX_GAP;

	for (j = 0; j < 8; j++)
	{
		tmp[0] = (j & 1) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = (j & 2) ? mins[1] - gap : maxs[1] + gap ;
		tmp[2] = (j & 4) ? mins[2] - gap : maxs[2] + gap ;

		tmp = tmp + origin;
		p[ j ] = tmp;
	}

	for (j = 0; j < 6; j++)
	{
		PM_DrawRectangle(
			p[PM_boxpnt[j][1]],
			p[PM_boxpnt[j][0]],
			p[PM_boxpnt[j][2]],
			p[PM_boxpnt[j][3]],
			pcolor, life);
	}
}


#ifndef DEDICATED

/*
================
PM_ViewEntity

Shows a particle trail from player to entity in crosshair.
Shows particles at that entities bbox

Tries to shoot a ray out by about 128 units.
================
*/
void PM_ViewEntity()
{
	Vector forward, right, up;
	const float raydist = 256.0f;
	int pcolor = 77;

#if 0
	if ( !pm_showclip.value )
		return;
#endif

	AngleVectors (pmove->angles, forward, right, up);  // Determine movement angles

	const Vector origin = pmove->origin;

	float fup = 0.5f*( pmove->player_mins[pmove->usehull][2] + pmove->player_maxs[pmove->usehull][2] );
	fup += pmove->view_ofs[2];
	fup -= 4;

	const Vector end = origin + raydist * forward;

	pmtrace_t trace = pmove->PM_PlayerTrace( origin, end, PM_STUDIO_BOX, -1 );

	if (trace.ent > 0)  // Not the world
	{
		pcolor = 111;
	
		// Draw the hull or bbox.
		PM_DrawPhysEntBBox(trace.ent, pcolor, 0.3f);
	}
}

#endif
