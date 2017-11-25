/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "animation.h"
#include "Effects.h"

#include "CActAnimating.h"

BEGIN_DATADESC(	CActAnimating )
	DEFINE_FIELD( m_Activity, FIELD_INTEGER ),
END_DATADESC()

void CActAnimating::SetActivity( Activity act ) 
{ 
	int sequence = LookupActivity( act ); 
	if ( sequence != ACTIVITY_NOT_AVAILABLE )
	{
		SetSequence( sequence );
		m_Activity = act; 
		SetFrame( 0 );
		ResetSequenceInfo( );
	}
}