#include "hud.h"
#include "cl_util.h"

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Weapons.h"

#include "ammohistory.h"

#include "CWeaponHUDInfo.h"

CWeaponHUDInfo::CWeaponHUDInfo()
{
	memset( this, 0, sizeof( *this ) );
}

bool CWeaponHUDInfo::LoadFromFile( const char* const pszWeaponName )
{
	ASSERT( pszWeaponName );
	ASSERT( *pszWeaponName );

	if( !pszWeaponName || !( *pszWeaponName ) )
		return false;

	const int iRes = ScreenWidth < 640 ? 320 : 640;

	char sz[ MAX_PATH ];

	struct WeaponSpriteData
	{
		WeaponHUDSprite& sprite;
		const char* const pszName;

		const WeaponHUDSprite* const pFallback;

		const bool bAdjustHistoryGap;
	};

	WeaponSpriteData weaponData[] = 
	{
		{ m_Crosshair, "crosshair", nullptr, false },
		{ m_AutoAim, "autoaim", nullptr, false },

		//default to non-zoomed crosshair
		{ m_ZoomedCrosshair, "zoom", &m_Crosshair, false },

		//default to zoomed crosshair
		{ m_ZoomedAutoAim, "zoom_autoaim", &m_ZoomedCrosshair, false },
		{ m_Inactive, "weapon", nullptr, false },
		{ m_Active, "weapon_s", nullptr, true },
		{ m_PrimaryAmmo, "ammo", nullptr, false },
		{ m_SecondaryAmmo, "ammo2", nullptr, false }
	};

	for( auto& data : weaponData )
	{
		memset( &data.sprite.rect, 0, sizeof( wrect_t ) );
		data.sprite.hSprite = INVALID_HSPRITE;
	}

	sprintf( sz, "sprites/%s.txt", pszWeaponName );

	int i;
	client_sprite_t *pList = SPR_GetList( sz, &i );

	if( !pList )
		return false;

	client_sprite_t* p;

	for( auto data : weaponData )
	{
		if( ( p = GetSpriteList( pList, data.pszName, iRes, i ) ) != nullptr )
		{
			sprintf( sz, "sprites/%s.spr", p->szSprite );
			data.sprite.hSprite = SPR_Load( sz );
			data.sprite.rect = p->rc;

			if( data.bAdjustHistoryGap )
			{
				gHR.iHistoryGap = max( gHR.iHistoryGap, data.sprite.rect.bottom - data.sprite.rect.top );
			}
		}
		else if( data.pFallback )
		{
			data.sprite = *data.pFallback;
		}
	}

	return true;
}