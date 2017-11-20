#if USE_OPFOR
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
#include "Weapons.h"
#include "CBasePlayer.h"
// ############ hu3lifezado ############ //
// Arma Touros quebrada voadora, codigo adaptado de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
#include "entities/weapons/CFlyingTouros.h"
// Para balancar a tela:
#include "shake.h"
// Imprimir mensagens
#ifdef CLIENT_DLL
#include "ui/hud/CHudBase.h"
#include "ui/hud/CHudTextMessage.h"
#endif
// ############ //

#include "entities/weapons/CDesertEagleLaser.h"

#include "CDesertEagle.h"

LINK_ENTITY_TO_CLASS( weapon_eagle, CDesertEagle );

CDesertEagle::CDesertEagle()
	: BaseClass( WEAPON_DESERT_EAGLE )
{
}

void CDesertEagle::Precache()
{
	BaseClass::Precache();

	PRECACHE_MODEL( "models/v_desert_eagle.mdl" );
	PRECACHE_MODEL( "models/w_desert_eagle.mdl" );
	PRECACHE_MODEL( "models/p_desert_eagle.mdl" );
	m_iShell = PRECACHE_MODEL( "models/shell.mdl" );
	PRECACHE_SOUND( "weapons/desert_eagle_fire.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_reload.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_sight.wav" );
	PRECACHE_SOUND( "weapons/desert_eagle_sight2.wav" );
	m_usFireEagle = PRECACHE_EVENT( 1, "events/eagle.sc" );
	// ############ hu3lifezado ############ //
	// Arma Touros quebrada voadora, codigo adaptado de:
	// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
	UTIL_PrecacheOther("flying_touros");
	PRECACHE_SOUND("weapons/desert_eagle_fuck.wav");
	// ############ //
}

void CDesertEagle::Spawn()
{
	Precache();

	SetModel( "models/w_desert_eagle.mdl" );

	// ############ hu3lifezado ############ //
	// Entre 5 e 10 segundos tem 20% de chance da arma atirar sozinha
	m_nextbadshootchance = gpGlobals->time + RANDOM_LONG(5, 10);
	// Nos nao podemos imprimir mensagens assim que o jogo comeca
	m_waitforthegametobeready = gpGlobals->time + 12;
	// Arma travada
	m_jammedweapon = false;
	// Qualidade da arma e porcentagem extra de problemas (inicializadas com 0, alteradas no deploy)
	m_quality = 0;
	m_qualitypercentageeffect = 0;
	// Imprimir mensagem quando o jogador coleta a arma
	m_firstmessage = true;
	// ############ //

	FallInit();
}

bool CDesertEagle::AddToPlayer( CBasePlayer* pPlayer )
{
	if( BaseClass::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, nullptr, pPlayer );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return true;
	}

	return false;
}

bool CDesertEagle::Deploy()
{
	m_bSpotVisible = true;

	return DefaultDeploy( 
		"models/v_desert_eagle.mdl", "models/p_desert_eagle.mdl", 
		DEAGLE_DRAW,
		"onehanded" );
}

void CDesertEagle::Holster()
{
	m_fInReload = false;

#ifndef CLIENT_DLL
	if( m_pLaser )
	{
		m_pLaser->Killed( CTakeDamageInfo( nullptr, 0, 0 ), GIB_NEVER );
		m_pLaser = nullptr;
		m_bSpotVisible = false;
	}
#endif

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10.0, 15.0 );

	SendWeaponAnim( DEAGLE_HOLSTER );
}

void CDesertEagle::WeaponIdle()
{
#ifndef CLIENT_DLL
	UpdateLaser();
#endif

	ResetEmptySound();

	// ############ hu3lifezado ############ //
	// Entre 5 e 10 segundos tem no minimo 10% de chance da arma atirar sozinha (leva 12 segundos para comecar a rodar inicialmente)
	if (m_nextbadshootchance <= gpGlobals->time && m_waitforthegametobeready <= gpGlobals->time)
	{
		m_nextbadshootchance = gpGlobals->time + UTIL_SharedRandomLong(m_pPlayer->random_seed, 5, 10);

		if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, 99) >= (90 - 90 * m_qualitypercentageeffect) && ! m_jammedweapon)
		{
			UTIL_Sparks(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 22 + gpGlobals->v_right * 10);
#ifndef CLIENT_DLL
			UTIL_SayText("Sua arma Touros atirou sozinha ou balas sairam voado!|r", m_pPlayer);

			// 50% de chance de levar dano de 1 a 5 por estilhacos
			ShrapnelDamage(50, 1, 5);
#endif
			PrimaryAttack();

			return;
		}
	}
	// ############ //

	if( m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip )
	{
		const float flNextIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

		int iAnim;

		if( m_bLaserActive )
		{
			if( flNextIdle > 0.5 )
			{
				iAnim = DEAGLE_IDLE5;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextIdle + 2.0;
			}
			else
			{
				iAnim = DEAGLE_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextIdle + 2.5;
			}
		}
		else
		{
			if( flNextIdle <= 0.3 )
			{
				iAnim = DEAGLE_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextIdle + 2.5;
			}
			else
			{
				if( flNextIdle > 0.6 )
				{
					iAnim = DEAGLE_IDLE3;
					m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextIdle + 1.633;
				}
				else
				{
					iAnim = DEAGLE_IDLE2;
					m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextIdle + 2.5;
				}
			}
		}

		SendWeaponAnim( iAnim );
	}
}

// ############ hu3lifezado ############ //
// No minimo 3% de chance da arma travar e nao atirar mais
bool CDesertEagle::RandomlyJammed()
{
	bool print = false;
	
	if (m_jammedweapon)
		print = true;
	else if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, 99) >= (97 - 97 * m_qualitypercentageeffect))
	{
		m_jammedweapon = true;
		print = true;
	}

	if (print)
	{
#ifndef CLIENT_DLL
		UTIL_SayText("Sua arma Touros travou! Recarregue-a!|r", m_pPlayer);
#endif
		return true;
	}

	return false;
}

// No minimo 1% de chance da arma quebrar e sair voando
bool CDesertEagle::RandomlyBreak()
{
	if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, 99) >= (99 - 99 * m_qualitypercentageeffect))
	{
		// Only throw if we were able to detatch from player.
		if (m_pPlayer->RemovePlayerItem(this))
		{			
#ifndef CLIENT_DLL
			// Dano de 10 a 25 no jogador, efeitos da tela piscar vermelho e tremer, som da arma quebrando
			float damage = RANDOM_LONG(10, 25);
			TraceResult tr = UTIL_GetGlobalTrace();
			g_MultiDamage.Clear();
			m_pPlayer->TraceAttack(CTakeDamageInfo(m_pPlayer, damage, DMG_BLAST), pev->velocity.Normalize(), &tr);
			g_MultiDamage.ApplyMultiDamage(this, m_pPlayer);
			UTIL_ScreenShake(m_pPlayer->GetAbsOrigin(), 6.0, 4.0, 0.6, 750);
			UTIL_ScreenFade(m_pPlayer, Vector(255, 0, 0), 0.2, 0.1, 128, FFADE_IN);
			EMIT_SOUND_DYN(this, CHAN_WEAPON, "weapons/desert_eagle_fuck.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 0xF));
#endif

			// Get the origin, direction, and fix the angle of the throw.
			Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 8 + gpGlobals->v_forward * 16;
			Vector vecDir = gpGlobals->v_forward;
			Vector vecAng = UTIL_VecToAngles(vecDir);
			vecAng.z = vecDir.z - 90;

			// Create a flying Touros.
			CFlyingTouros *pFTouros = (CFlyingTouros *)Create("flying_touros", vecSrc, Vector(0, 0, 0), m_pPlayer->edict());

			// Give the Touros its velocity, angle, and spin. 
			// Lower the gravity a bit, so it flys. 
#ifndef CLIENT_DLL
			pFTouros->pev->velocity = vecDir * 500 + m_pPlayer->pev->velocity;
			pFTouros->pev->angles = vecAng;
			pFTouros->pev->avelocity.x = -1000;
			pFTouros->pev->gravity = .5;
			pFTouros->m_pPlayer = m_pPlayer;
#endif
			// Do player weapon anim.
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);

			// Just for kicks, set this. 
			m_flNextSecondaryAttack = gpGlobals->time + 0.75;

			// take item off hud
			m_pPlayer->pev->weapons &= ~(1 << this->m_iId);

			// Destroy this weapon
			DestroyItem();
			
			// Mensagem humilhante pessoal
#ifndef CLIENT_DLL
			UTIL_SayText("Sua arma Touros quebrou!|r", m_pPlayer);
#endif
			// Zero a qualidade
			m_quality = 0;
		}
		return true;
	}
	return false;
}

// Definir a qualidade da arma
void CDesertEagle::SetQuality()
{
	if (m_quality == 0)
	{
		// Qualidade da arma
		m_quality = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 1, 9);
		// A influencia da qualidade nos defeitos (30% a 0% de piora proporcional a qualidade de 1 ate 9)
		m_qualitypercentageeffect = 0.3 - (m_quality - 1 + (m_quality - 1) * 1 / 8) * 0.3 / 9;
		// Mensagem sobre a qualidade da arma
#ifndef CLIENT_DLL
		char mensagem_i[70] = "Pelo uso voce percebeu a qualidade dessa Touros:";
		char mensagem_m[15];
		snprintf(mensagem_m, 9, " %0.2f/10", m_quality);
		char mensagem_f[5] = "!|g";
		strcat(mensagem_i, strcat(mensagem_m, mensagem_f));
		UTIL_SayText(mensagem_i, m_pPlayer);
#endif
	}
}

// Dano por estilhaco
void CDesertEagle::ShrapnelDamage(int chance, int min_damage, int max_damage)
{
#ifndef CLIENT_DLL
	if (RANDOM_LONG(1, 100) <= chance)
	{
		UTIL_SayText("Voce foi ferido por estilhacos de bala da sua arma Touros...|r", m_pPlayer);

		// Dano de 10 a 25 no jogador, efeitos da tela piscar vermelho e tremer, som da arma quebrando
		float damage = RANDOM_LONG(min_damage, max_damage);
		TraceResult tr = UTIL_GetGlobalTrace();
		g_MultiDamage.Clear();
		m_pPlayer->TraceAttack(CTakeDamageInfo(m_pPlayer, damage, DMG_BLAST), pev->velocity.Normalize(), &tr);
		g_MultiDamage.ApplyMultiDamage(this, m_pPlayer);
		UTIL_ScreenFade(m_pPlayer, Vector(255, 0, 0), 0.2, 0.1, 128, FFADE_IN);
	}
#endif
}

// Perder toda a municao
bool CDesertEagle::RandomlyLostAllAmmo(float chance)
{
	if (RANDOM_FLOAT(1, 100) <= chance)
	{
		int i;
#ifndef CLIENT_DLL
		UTIL_SayText("Infelizmente a sua arma Touros estragou toda a municao dela...|r", m_pPlayer);
#endif
		// 100% de chance de tomar estilhacos
		ShrapnelDamage(100, 1, 5);
		// Um bando de faisca
		for (i = 0; i <= 5; i++)
			UTIL_Sparks(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 22 + gpGlobals->v_right * 10);
		// Tchau municao
		m_iClip = 0;
		m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] = 0;

		return true;
	}
	return false;
}
// ############ //

void CDesertEagle::PrimaryAttack()
{	
	// ############ hu3lifezado ############ //
	// Definir a qualidade da arma (uma unica vez)!
	// Estou fazendo isso aqui porque os valores aleatorios que eu preciso gerar pela funcao
	// UTIL_SharedRandomFloat estao diferentes no client e no server quando eu a chamo no inico
	// do carregamento da arma. Mesmo adicionando delay isso ainda acontece... Ja aqui nao.
	// Acredito que isso seja porque PrimaryAttack tem sua execucao mais posterior. Funciona.
	SetQuality();
	// Chance de quebrar
	if (!m_jammedweapon)
		if (RandomlyBreak())
			return;
	// ############ //

	if (m_pPlayer->GetWaterLevel() == WATERLEVEL_HEAD)
	{
		PlayEmptySound();

		// ############ hu3lifezado ############ //
		// Novo tempo
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		// ############ //
		return;
	}

	if( m_iClip <= 0 )
	{
		if( !m_fInReload )
		{
			if( m_bFireOnEmpty )
			{
				PlayEmptySound();
				// ############ hu3lifezado ############ //
				// Novo tempo
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
				// ############ //
			}
			else
			{
				Reload();
			}
		}

		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	// ############ hu3lifezado ############ //
	// Solta de 1 a 7 tiros por ataque primario e com mira pessima (variando com a qualidade da arma).

	// Evitar mexer nessas coisas caso a arma esteja travada
	int flags;
	if (!m_jammedweapon)
	{
		m_pPlayer->GetEffects() |= EF_MUZZLEFLASH;

		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

#if defined( CLIENT_WEAPONS )
		flags = FEV_NOTHOST;
#else
		flags = 0;
#endif
	}

#ifndef CLIENT_DLL
		if (m_pLaser && m_bLaserActive)
		{
			m_pLaser->GetEffects() |= EF_NODRAW;
			m_pLaser->SetThink(&CDesertEagleLaser::Revive);
			// Novo tempo (0.6)
			m_pLaser->SetNextThink(gpGlobals->time + 1.0);
		}
#endif


	// Quantidade de balas atiradas de uma vez segundo a qualidade da arma
	int i, j;

	if (m_qualitypercentageeffect <= 0.1)
	{
		j = UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 2);
	}
	else if (m_qualitypercentageeffect > 0.1 && m_qualitypercentageeffect <= 0.2)
	{
		j = UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 4);
	}
	else if (m_qualitypercentageeffect > 0.2 && m_qualitypercentageeffect <= 0.3)
	{
		j = UTIL_SharedRandomLong(m_pPlayer->random_seed, 3, 7);
	}

	// Movi essas linhas para fora do loop
	UTIL_MakeVectors(m_pPlayer->GetViewAngle() + m_pPlayer->GetPunchAngle());
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

#ifndef CLIENT_DLL
	// Entre 3% e 6% de chance de levar dano de 1 a 5 por estilhacos (caso a arma nao esteja travada)
	if (!m_jammedweapon)
		ShrapnelDamage(3 + 10*m_qualitypercentageeffect, 1, 5);
#endif

	for (i = 0; i < j; i++)
	{
		// Chance de travar a arma ou quebrar a arma (esta de 1 ate 4%)
		if (RandomlyJammed() || RandomlyLostAllAmmo(1 + 10 * m_qualitypercentageeffect))
		{
			SendWeaponAnim(DEAGLE_SHOOT);
			break;
		}
		else
			--m_iClip;

		// Novo espalhamento todo zoado (distancia do centro)
		float flSpread = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0.01, 0.4);

		Vector vecSpread;

		vecSpread = m_pPlayer->FireBulletsPlayer(
			1,
			vecSrc, vecAiming, Vector(flSpread, flSpread, flSpread),
			8192.0, BULLET_PLAYER_DEAGLE, 0, 0,
			m_pPlayer, UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 10)); // Mais espalhamento aqui (angulos ao redor do centro)

		PLAYBACK_EVENT_FULL(
			flags, m_pPlayer->edict(), m_usFireEagle, 0,
			g_vecZero, g_vecZero,
			vecSpread.x, vecSpread.y,
			0, 0,
			m_iClip == 0, 0);

		if (!m_iClip)
		{
			if (m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] <= 0)
				m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
			break;
		}
	}

#ifndef CLIENT_DLL
	UpdateLaser();
#endif

	// Novos tempos
	//m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + (m_bLaserActive ? 0.5 : 0.22);
	//m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10.0, 15.0);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 4.0, 8.0);
	// ############ //
}

void CDesertEagle::SecondaryAttack()
{
#ifndef CLIENT_DLL
	m_bLaserActive = !m_bLaserActive;

	if( !m_bLaserActive )
	{
		if( m_pLaser )
		{
			m_pLaser->Killed( CTakeDamageInfo( nullptr, 0, 0 ), GIB_NEVER );

			m_pLaser = nullptr;

			EMIT_SOUND_DYN( this, CHAN_WEAPON, "weapons/desert_eagle_sight2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
		}
	}
#endif

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
}

void CDesertEagle::Reload()
{
	// ############ hu3lifezado ############ //
	// Se a arma estiver travada, liberar
	if (m_jammedweapon)
		m_jammedweapon = false;
	// ############ //

	if( m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] > 0 )
	{
		// ############ hu3lifezado ############ //
		// Tempo reajustado (1.5)
		const bool bResult = DefaultReload( m_iClip ? DEAGLE_RELOAD : DEAGLE_RELOAD_NOSHOT, 3.7, 1 );
		// ############ //
	
#ifndef CLIENT_DLL
		if( m_pLaser && m_bLaserActive )
		{
			m_pLaser->GetEffects() |= EF_NODRAW;
			m_pLaser->SetThink( &CDesertEagleLaser::Revive );
			// ############ hu3lifezado ############ //
			// Tempos reajustados (1.6) (1.5)
			m_pLaser->SetNextThink( gpGlobals->time + 3.8 );

			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 3.7;
			// ############ //
		}
#endif

		if( bResult )
		{
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10.0, 15.0 );
		}
	}
}

void CDesertEagle::UpdateLaser()
{
#ifndef CLIENT_DLL
	if( m_bLaserActive && m_bSpotVisible )
	{
		if( !m_pLaser )
		{
			m_pLaser = CDesertEagleLaser::CreateSpot();

			EMIT_SOUND_DYN( this, CHAN_WEAPON, "weapons/desert_eagle_sight.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );
		}

		UTIL_MakeVectors( m_pPlayer->GetViewAngle() );

		Vector vecSrc = m_pPlayer->GetGunPosition();

		Vector vecEnd = vecSrc + gpGlobals->v_forward * 8192.0;

		TraceResult tr;

		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr );

		m_pLaser->SetAbsOrigin( tr.vecEndPos );
	}
#endif
}

void CDesertEagle::GetWeaponData( weapon_data_t& data )
{
	BaseClass::GetWeaponData( data );

	data.iuser1 = m_bLaserActive;
}

void CDesertEagle::SetWeaponData( const weapon_data_t& data )
{
	BaseClass::SetWeaponData( data );

	m_bLaserActive = data.iuser1 != 0;
}
#endif //USE_OPFOR
