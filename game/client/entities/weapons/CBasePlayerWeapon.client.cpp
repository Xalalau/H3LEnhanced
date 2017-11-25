#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "CBasePlayer.h"
#include "Weapons.h"

#include "hud.h"
#include "cl_util.h"

#include "com_weapons.h"

extern bool g_brunninggausspred;

bool CBasePlayerWeapon::AddToPlayer( CBasePlayer* pPlayer )
{
	return false;
}

bool CBasePlayerWeapon::AddDuplicate( CBasePlayerWeapon* pOriginal )
{
	return false;
}

bool CBasePlayerWeapon::ExtractAmmo( CBasePlayerWeapon* pWeapon )
{
	return false;
}

bool CBasePlayerWeapon::ExtractClipAmmo( CBasePlayerWeapon* pWeapon )
{
	return false;
}

bool CBasePlayerWeapon::AddPrimaryAmmo( int iCount, const char *szName, int iMaxClip )
{
	return true;
}

bool CBasePlayerWeapon::AddSecondaryAmmo( int iCount, const char *szName )
{
	return true;
}

/*
=====================
CBasePlayerWeapon::SendWeaponAnim

Animate weapon model
=====================
*/
void CBasePlayerWeapon::SendWeaponAnim( int iAnim, int body )
{
	m_pPlayer->SetWeaponAnim( iAnim );

	HUD_SendWeaponAnim( iAnim, body, 0 );
}

bool CBasePlayerWeapon::IsUseable()
{
	return true;
}

/*
=====================
CBasePlayerWeapon :: DefaultDeploy

=====================
*/
bool CBasePlayerWeapon::DefaultDeploy( const char* const pszViewModel, const char* const pszWeaponModel, int iAnim, const char* const pszAnimExt, int body )
{
	if( !CanDeploy() )
		return false;

	LoadVModel( pszViewModel, m_pPlayer );

	SendWeaponAnim( iAnim, body );

	g_brunninggausspred = false;
	m_pPlayer->m_flNextAttack = 0.5;
	m_flTimeWeaponIdle = 1.0;
	return true;
}

bool CBasePlayerWeapon::UpdateClientData( CBasePlayer* pPlayer )
{
	return false;
}

void CBasePlayerWeapon::RetireWeapon( void )
{
}

/*
=====================
CBasePlayerWeapon::Holster

Put away weapon
=====================
*/
void CBasePlayerWeapon::Holster()
{
	m_fInReload = false; // cancel any reload in progress.
	g_brunninggausspred = false;
	m_pPlayer->ClearViewModelIndex();
}

float CBasePlayerWeapon::GetNextAttackDelay( float flTime )
{
	return flTime;
}

void CBasePlayerWeapon::DestroyItem( void )
{
}

void CBasePlayerWeapon::DefaultTouch( CBaseEntity* pOther )
{
}

void CBasePlayerWeapon::FallThink( void )
{
}

void CBasePlayerWeapon::Materialize( void )
{
}

void CBasePlayerWeapon::AttemptToMaterialize( void )
{
}

CBaseEntity* CBasePlayerWeapon::Respawn( void )
{
	return NULL;
}

void CBasePlayerWeapon::FallInit( void )
{
}

void CBasePlayerWeapon::CheckRespawn( void )
{
}

void CBasePlayerWeapon::Drop( void )
{
}

void CBasePlayerWeapon::Kill( void )
{
}

void CBasePlayerWeapon::AttachToPlayer( CBasePlayer* pPlayer )
{
}