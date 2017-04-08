#include "hud.h"
#include "cl_util.h"
#include "event_api.h"
#include "triangleapi.h"

#include "materials/Materials.h"

#include "pm_shared.h"
#include "pm_defs.h"
#include "com_model.h"

#include "r_studioint.h"

#include "particleman.h"

#include "CPartGrassPiece.h"
#include "CPartRainDrop.h"
#include "CPartSnowFlake.h"
#include "CPartWind.h"

#include "CEnvironment.h"

//TODO: move - Solokiller
extern engine_studio_api_t IEngineStudio;

extern Vector g_vPlayerVelocity;

CEnvironment g_Environment;

cvar_t* cl_weather = nullptr;

void CEnvironment::SetWeatherType( const WeatherType::WeatherType type )
{
	m_WeatherType = type;
}

void CEnvironment::RegisterParticleClasses()
{
	g_pParticleMan->AddCustomParticleClassSize( sizeof( CPartRainDrop ) );
}

void CEnvironment::Initialize()
{
	m_vecWeatherOrigin = g_vecZero;
	m_flWeatherTime = 0;
	m_flGrassTime = 0;
	m_vecGrassOrigin = g_vecZero;

	m_WeatherType = WeatherType::NONE;

	m_bGrassActive = false;

	SetupGrass();

	m_vecWind.x = UTIL_RandomFloat( -80.0f, 80.0f );
	m_vecWind.y = UTIL_RandomFloat( -80.0f, 80.0f );
	m_vecWind.z = 0;

	m_vecDesiredWindDirection.x = UTIL_RandomFloat( -80.0f, 80.0f );
	m_vecDesiredWindDirection.y = UTIL_RandomFloat( -80.0f, 80.0f );
	m_vecDesiredWindDirection.z = 0;

	m_flNextWindChangeTime = gEngfuncs.GetClientTime();

	m_pRainSprite	= const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/effects/rain.spr" ) ) );
	m_pSnowSprite	= const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/effects/snowflake.spr" ) ) );
	m_pRipple		= const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/effects/ripple.spr" ) ) );
	m_pRainSplash	= const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/wsplash3.spr" ) ) );

	m_pGasPuffSprite = const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/gas_puff_01.spr" ) ) );

	m_flWeatherValue = cl_weather->value;
}

void CEnvironment::SetupGrass()
{
	m_pGrassSprite1 = const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/grass_01.spr" ) ) );
	m_pGrassSprite2 = const_cast<model_t*>( gEngfuncs.GetSpritePointer( gEngfuncs.pfnSPR_Load( "sprites/grass_03.spr" ) ) );

	m_bGrassActive = mat::IsThereGrassTexture();
}

void CEnvironment::Update()
{
	Vector vecOrigin = Hud().GetOrigin();

	if( g_iUser1 > 0 && g_iUser1 != OBS_ROAMING )
	{
		if( cl_entity_t* pFollowing = gEngfuncs.GetEntityByIndex( g_iUser2 ) )
		{
			vecOrigin = pFollowing->origin;
		}
	}

	vecOrigin.z += 36.0f;

	if( cl_weather->value > 3.0 )
	{
		gEngfuncs.Cvar_SetValue( "cl_weather", 3.0 );
	}

	m_flWeatherValue = cl_weather->value;

	if( !IEngineStudio.IsHardware() )
		m_flWeatherValue = 0;

	m_vecWeatherOrigin = vecOrigin;

	UpdateWind();

	if( m_bGrassActive )
	{
		UpdateGrass();
	}

	if( m_flWeatherTime <= gEngfuncs.GetClientTime() )
	{
		switch( m_WeatherType )
		{
		case WeatherType::NONE: break;

		case WeatherType::RAIN:
			{
				UpdateRain();
				break;
			}

		case WeatherType::SNOW:
			{
				UpdateSnow();
				break;
			}
		}
	}

	m_flOldTime = gEngfuncs.GetClientTime();
}

void CEnvironment::UpdateWind()
{
	if( m_flNextWindChangeTime <= gEngfuncs.GetClientTime() )
	{
		m_vecDesiredWindDirection.x = UTIL_RandomFloat( -80.0f, 80.0f );
		m_vecDesiredWindDirection.y = UTIL_RandomFloat( -80.0f, 80.0f );
		m_vecDesiredWindDirection.z = 0;

		m_flNextWindChangeTime = gEngfuncs.GetClientTime() + UTIL_RandomFloat( 15.0f, 30.0f );

		Vector vecDir = m_vecDesiredWindDirection;

		m_flDesiredWindSpeed = vecDir.NormalizeInPlace();

		if( vecDir.x == 0.0f && vecDir.y == 0.0f )
		{
			m_flIdealYaw = 0;
		}
		else
		{
			m_flIdealYaw = floor( atan2( vecDir.y, vecDir.x ) * ( 180.0 / M_PI ) );

			if( m_flIdealYaw < 0.0f )
				m_flIdealYaw += 360.0f;
		}
	}

	Vector vecWindDir = m_vecWind;

	vecWindDir.NormalizeInPlace();

	Vector vecAngles;

	VectorAngles( vecWindDir, vecAngles );

	float flYaw;

	if( vecAngles.y < 0.0f )
	{
		flYaw = 120 * ( 3 * static_cast<int>( floor( vecAngles.y / 360.0 ) ) + 3 );
	}
	else
	{
		if( vecAngles.y < 360.0f )
		{
			flYaw = vecAngles.y;
		}
		else
		{
			flYaw = vecAngles.y - ( 360 * floor( vecAngles.y / 360.0 ) );
		}
	}

	if( m_flIdealYaw != flYaw )
	{
		const float flSpeed = ( gEngfuncs.GetClientTime() - m_flOldTime ) * 0.5 * 10.0;
		vecAngles.y = UTIL_ApproachAngle( m_flIdealYaw, flYaw, flSpeed );
	}

	Vector vecNewWind;

	AngleVectors( vecAngles, &vecNewWind, nullptr, nullptr );

	m_vecWind = vecNewWind * m_flDesiredWindSpeed;
}

void CEnvironment::UpdateGrass()
{
	if( m_flGrassTime > gEngfuncs.GetClientTime() )
	{
		return;
	}

	Vector vecDist = m_vecWeatherOrigin - m_vecGrassOrigin;

	const float flDist = vecDist.Length();

	if( flDist < 100.0 )
	{
		return;
	}

	m_vecGrassOrigin = m_vecWeatherOrigin;
	m_flGrassTime = gEngfuncs.GetClientTime() + 0.3;

	Vector vecOrigin;
	Vector vecEndPos;

	Vector vecWindOrigin;

	pmtrace_t trace;

	for( size_t uiIndex = 0; static_cast<float>( uiIndex ) < 35.0 * m_flWeatherValue; ++uiIndex )
	{
		vecOrigin = m_vecGrassOrigin;

		const float flXOffset = UTIL_RandomFloat( -800.0f, 850.0f );

		vecOrigin.x += ( UTIL_RandomLong( 0, 5 ) > 2 ) ? flXOffset : -flXOffset;
		vecOrigin.y += UTIL_RandomFloat( -800.0f, 800.0f );
		vecOrigin.z += 36.0f;

		vecEndPos.x = vecOrigin.x;
		vecEndPos.y = vecOrigin.y;
		vecEndPos.z = -8000.0f;

		gEngfuncs.pEventAPI->EV_SetTraceHull( Hull::LARGE );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecOrigin, vecEndPos, PM_WORLD_ONLY, -1, &trace );
		const char* pszTexture = gEngfuncs.pEventAPI->EV_TraceTexture( trace.ent, trace.endpos, vecEndPos );

		if( pszTexture )
		{
			//Skip the +/- and number.
			if( pszTexture[ 0 ] == '+' || 
				pszTexture[ 0 ] == '-' )
				pszTexture += 2;

			if( pszTexture[ 0 ] == '!' || 
				pszTexture[ 0 ] == '{' || 
				pszTexture[ 0 ] == '~' || 
				pszTexture[ 0 ] == ' ' )
				++pszTexture;

			if( PM_FindTextureType( pszTexture ) == CHAR_TEX_GRASS )
			{
				CreateGrassPiece( trace.endpos, Vector( 0, UTIL_RandomFloat( 0, 359.0 ), 0 ) );
			}
		}
	}
}

void CEnvironment::UpdateSnow()
{
	m_flWeatherTime = gEngfuncs.GetClientTime() + 0.7f;

	Vector vecPlayerDir = g_vPlayerVelocity;

	const float flSpeed = vecPlayerDir.NormalizeInPlace();

	if( 150.0f * m_flWeatherValue > 0.0f )
	{
		Vector vecOrigin;
		Vector vecEndPos;

		Vector vecWindOrigin;

		pmtrace_t trace;

		for( size_t uiIndex = 0; static_cast<float>( uiIndex ) < 150.0f * m_flWeatherValue; ++uiIndex )
		{
			vecOrigin = m_vecWeatherOrigin;

			vecOrigin.x += UTIL_RandomFloat( -300.0f, 300.0f );
			vecOrigin.y += UTIL_RandomFloat( -300.0f, 300.0f );
			vecOrigin.z += UTIL_RandomFloat( 100.0f, 300.0f );

			vecEndPos.x = vecOrigin.x + ( ( UTIL_RandomLong( 0, 5 ) > 2 ) ? g_vPlayerVelocity.x : -g_vPlayerVelocity.x );
			vecEndPos.y = vecOrigin.y + g_vPlayerVelocity.y;
			vecEndPos.z = 8000.0f;

			gEngfuncs.pEventAPI->EV_SetTraceHull( Hull::LARGE );
			gEngfuncs.pEventAPI->EV_PlayerTrace( vecOrigin, vecEndPos, PM_WORLD_ONLY, -1, &trace );
			const char* pszTexture = gEngfuncs.pEventAPI->EV_TraceTexture( trace.ent, vecOrigin, trace.endpos );

			if( pszTexture && strncmp( pszTexture, "sky", 3 ) == 0 )
			{
				CreateSnowFlake( vecOrigin );
			}
		}
	}
}

void CEnvironment::UpdateRain()
{
	m_flWeatherTime = gEngfuncs.GetClientTime() + 0.3f;

	Vector vecPlayerDir = g_vPlayerVelocity;

	const float flSpeed = vecPlayerDir.NormalizeInPlace();

	if( 150.0f * m_flWeatherValue > 0.0f )
	{
		int iWindParticle = 0;

		Vector vecOrigin;
		Vector vecEndPos;

		Vector vecWindOrigin;

		pmtrace_t trace;

		for( size_t uiIndex = 0; static_cast<float>( uiIndex ) < 150.0f * m_flWeatherValue; ++uiIndex )
		{
			vecOrigin = m_vecWeatherOrigin;

			vecOrigin.x += UTIL_RandomFloat( -400.0f, 400.0f );
			vecOrigin.y += UTIL_RandomFloat( -400.0f, 400.0f );
			vecOrigin.z += UTIL_RandomFloat( 100.0f, 300.0f );

			vecEndPos.x = vecOrigin.x + ( ( UTIL_RandomLong( 0, 5 ) > 2 ) ? g_vPlayerVelocity.x : -g_vPlayerVelocity.x );
			vecEndPos.y = vecOrigin.y + g_vPlayerVelocity.y;
			vecEndPos.z = 8000.0f;

			gEngfuncs.pEventAPI->EV_SetTraceHull( Hull::LARGE );
			gEngfuncs.pEventAPI->EV_PlayerTrace( vecOrigin, vecEndPos, PM_WORLD_ONLY, -1, &trace );
			const char* pszTexture = gEngfuncs.pEventAPI->EV_TraceTexture( trace.ent, vecOrigin, trace.endpos );
		
			if( pszTexture && strncmp( pszTexture, "sky", 3 ) == 0 )
			{
				CreateRaindrop( vecOrigin );

				if( iWindParticle == 15 )
				{
					iWindParticle = 1;

					vecWindOrigin.x = vecOrigin.x;
					vecWindOrigin.y = vecOrigin.y;
					vecWindOrigin.z = Hud().GetOrigin().z;

					if( gEngfuncs.pTriAPI->BoxInPVS( vecWindOrigin, vecWindOrigin ) )
					{
						vecEndPos.z = 8000.0f;

						gEngfuncs.pEventAPI->EV_SetTraceHull( Hull::LARGE );
						gEngfuncs.pEventAPI->EV_PlayerTrace( vecWindOrigin, vecEndPos, PM_WORLD_ONLY, -1, &trace );
						pszTexture = gEngfuncs.pEventAPI->EV_TraceTexture( trace.ent, vecOrigin, trace.endpos );
						
						if( pszTexture && strncmp( pszTexture, "sky", 3 ) == 0 )
						{
							vecEndPos.z = -8000.0f;

							gEngfuncs.pEventAPI->EV_SetTraceHull( Hull::LARGE );
							gEngfuncs.pEventAPI->EV_PlayerTrace( vecWindOrigin, vecEndPos, PM_WORLD_ONLY, -1, &trace );
						
							CreateWindParticle( trace.endpos );
						}
					}
				}
				else
				{
					++iWindParticle;
				}
			}
		}
	}
}

void CEnvironment::CreateGrassPiece( const Vector& vecOrigin, const Vector& vecNormal )
{
	model_t* pSprite = UTIL_RandomLong( 0, 1 ) != 0 ? m_pGrassSprite1 : m_pGrassSprite2;

	if( !pSprite )
	{
		return;
	}

	CPartGrassPiece* pParticle = new CPartGrassPiece();

	Vector vecPartOrigin = vecOrigin;
	vecPartOrigin.z += 10.0;

	const float flSize = UTIL_RandomFloat( 60.0, 75.0 ) * 0.5;

	pParticle->InitializeSprite( 
		vecPartOrigin, vecNormal, 
		pSprite, 
		flSize, 1.0 );
	
	//TODO: what does this do? - Solokiller
	pParticle->m_iAfterDampFlags = 32;

	pParticle->m_flGravity = 0;
	pParticle->m_iRendermode = kRenderTransAlpha;

	pParticle->SetCullFlag( RENDER_FACEPLAYER_ROTATEZ | CULL_PVS | CULL_FRUSTUM_PLANE );

	pParticle->m_vVelocity = g_vecZero;
	pParticle->m_vAVelocity = g_vecZero;

	pParticle->m_vAngles = vecNormal;

	pParticle->m_flFadeSpeed = 0;

	pParticle->m_flSize = flSize;

	pParticle->m_vColor.x = pParticle->m_vColor.y = pParticle->m_vColor.z = 125.0f;

	pParticle->m_flFadeSpeed = -1.0f;

	pParticle->m_flDieTime = gEngfuncs.GetClientTime() + 99999.0f;
}

void CEnvironment::CreateSnowFlake( const Vector& vecOrigin )
{
	if( !m_pSnowSprite )
	{
		return;
	}

	CPartSnowFlake* pParticle = new CPartSnowFlake();

	pParticle->InitializeSprite( 
		vecOrigin, g_vecZero, 
		m_pSnowSprite, 
		UTIL_RandomFloat( 2.0, 2.5 ), 1.0 );

	strcpy( pParticle->m_szClassname, "snow_particle" );

	pParticle->m_iNumFrames = m_pSnowSprite->numframes;

	pParticle->m_vVelocity.x = m_vecWind.x / UTIL_RandomFloat( 1.0, 2.0 );
	pParticle->m_vVelocity.y = m_vecWind.y / UTIL_RandomFloat( 1.0, 2.0 );
	pParticle->m_vVelocity.z = UTIL_RandomFloat( -100.0, -200.0 );

	pParticle->SetCollisionFlags( TRI_COLLIDEWORLD );

	const float flFrac = UTIL_RandomFloat( 0.0, 1.0 );

	if( flFrac >= 0.1 )
	{
		if( flFrac < 0.2 )
		{
			pParticle->m_vVelocity.z = -65.0;
		}
		else if( flFrac < 0.3 )
		{
			pParticle->m_vVelocity.z = -75.0;
		}
	}
	else
	{
		pParticle->m_vVelocity.x *= 0.5;
		pParticle->m_vVelocity.y *= 0.5;
	}

	pParticle->m_iRendermode = kRenderTransAdd;

	pParticle->SetCullFlag( RENDER_FACEPLAYER | LIGHT_NONE | CULL_PVS | CULL_FRUSTUM_SPHERE );

	pParticle->m_flScaleSpeed = 0;
	pParticle->m_flDampingTime = 0;
	pParticle->m_iFrame = 0;
	pParticle->m_flMass = 1.0;

	pParticle->m_flGravity = 0;
	pParticle->m_flBounceFactor = 0;

	pParticle->m_vColor.x = pParticle->m_vColor.y = pParticle->m_vColor.z = 128.0f;

	pParticle->m_flDieTime = gEngfuncs.GetClientTime() + 3.0;

	pParticle->m_bSpiral = UTIL_RandomLong( 0, 1 ) != 0;

	pParticle->m_flSpiralTime = gEngfuncs.GetClientTime() + UTIL_RandomLong( 2, 4 );
}

void CEnvironment::CreateRaindrop( const Vector& vecOrigin )
{
	if( !m_pRainSprite )
	{
		return;
	}

	CPartRainDrop* pParticle = new CPartRainDrop();

	pParticle->InitializeSprite( vecOrigin, g_vecZero, m_pRainSprite, 2.0f, 1.0f );

	strcpy( pParticle->m_szClassname, "particle_rain" );

	pParticle->m_flStretchY = 40.0f;

	pParticle->m_vVelocity.x = m_vecWind.x * UTIL_RandomFloat( 1.0f, 2.0f );
	pParticle->m_vVelocity.y = m_vecWind.y * UTIL_RandomFloat( 1.0f, 2.0f );
	pParticle->m_vVelocity.z = UTIL_RandomFloat( -500.0f, -1800.0f );

	pParticle->SetCollisionFlags( TRI_COLLIDEWORLD | TRI_COLLIDEKILL | TRI_WATERTRACE );

	pParticle->m_flGravity = 0;

	pParticle->SetCullFlag( ( LIGHT_NONE | CULL_PVS | CULL_FRUSTUM_PLANE ) );

	pParticle->m_iRendermode = kRenderTransAlpha;

	pParticle->m_vColor.x = pParticle->m_vColor.y = pParticle->m_vColor.z = 255.0f;

	pParticle->m_flDieTime = gEngfuncs.GetClientTime() + 1.0f;
}

void CEnvironment::CreateWindParticle( const Vector& vecOrigin )
{
	if( !m_pGasPuffSprite )
	{
		return;
	}

	CPartWind* pParticle = new CPartWind();

	Vector vecPartOrigin = vecOrigin;

	vecPartOrigin.z += 10.0;

	pParticle->InitializeSprite( 
		vecPartOrigin, g_vecZero, 
		m_pGasPuffSprite, 
		UTIL_RandomFloat( 50.0, 75.0 ), 1.0 );

	pParticle->m_iNumFrames = m_pGasPuffSprite->numframes;

	strcpy( pParticle->m_szClassname, "wind_particle" );

	//pParticle->m_iFrame = UTIL_RandomLong( m_pGasPuffSprite->numframes / 2, m_pGasPuffSprite->numframes );

	pParticle->m_vVelocity.x = m_vecWind.x / UTIL_RandomFloat( 1.0f, 2.0f );
	pParticle->m_vVelocity.y = m_vecWind.y / UTIL_RandomFloat( 1.0f, 2.0f );

	if( UTIL_RandomFloat( 0.0, 1.0 ) < 0.1 )
	{
		pParticle->m_vVelocity.x *= 0.5;
		pParticle->m_vVelocity.y *= 0.5;
	}

	pParticle->SetCollisionFlags( TRI_COLLIDEWORLD );
	pParticle->m_flGravity = 0;

	pParticle->m_iRendermode = kRenderTransAlpha;

	pParticle->SetCullFlag( RENDER_FACEPLAYER | LIGHT_NONE | CULL_PVS | CULL_FRUSTUM_SPHERE );

	pParticle->m_vAVelocity.z = UTIL_RandomFloat( -1.0, 1.0 );

	pParticle->m_flScaleSpeed = 0.4;
	pParticle->m_flDampingTime = 0;

	pParticle->m_iFrame = 0;

	pParticle->m_flMass = 1.0f;
	pParticle->m_flBounceFactor = 0;
	pParticle->m_vColor.x = pParticle->m_vColor.y = pParticle->m_vColor.z = 128.0;

	pParticle->m_flFadeSpeed = -1.0f;

	pParticle->m_flDieTime = gEngfuncs.GetClientTime() + 6.0;
}