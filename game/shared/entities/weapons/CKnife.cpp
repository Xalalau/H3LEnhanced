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
#include "Skill.h"

#include "gamerules/GameRules.h"

#include "CKnife.h"

// ############ hu3lifezado ############ //
// DATADESC() removida
// ############ //

LINK_ENTITY_TO_CLASS( weapon_knife, CKnife );

CKnife::CKnife()
	: BaseClass( WEAPON_KNIFE )
{
}

const char *CKnife::pSelectionSounds[] = 
{
	"weapons/spray_selection1.wav",
	"weapons/spray_selection2.wav",
	"weapons/spray_selection3.wav",
	"weapons/spray_selection4.wav",    
};

void CKnife::Precache()
{
	BaseClass::Precache();

	PRECACHE_MODEL( "models/v_knife.mdl" );
	PRECACHE_MODEL( "models/w_knife.mdl" );
	PRECACHE_MODEL( "models/p_knife.mdl" );

	// ############ hu3lifezado ############ //
	// Alguns audios foram removidos e os seguintes renomeados
	PRECACHE_SOUND( "weapons/spray_hit_flesh1.wav" );
	PRECACHE_SOUND( "weapons/spray_hit_flesh2.wav" );
	PRECACHE_SOUND( "weapons/spray_hit_wall1.wav" );
	PRECACHE_SOUND( "weapons/spray_hit_wall2.wav" );
	PRECACHE_SOUND( "weapons/spray_eupichavasim.wav" );
    PRECACHE_SOUND_ARRAY(pSelectionSounds);
	// ############ //

	m_usKnife = PRECACHE_EVENT( 1, "events/knife.sc" );
}

void CKnife::Spawn()
{
	Precache();

	SetModel( "models/w_knife.mdl" );

	m_iClip = WEAPON_NOCLIP;

	// ############ hu3lifezado ############ //
	// Tempo para controlar mudanca de cor no botao secundario do mouse
	m_nextcolorchange = 0;
	// Tempo que quando ultrapassado forca a rechecagem do primeiro acerto
	m_nextfirsthit = 0;
	// Tempo para o proximo som de spray aplicado em parede
	m_nextsprayonwallsound = 0;
	// ############ //

	FallInit();
}

bool CKnife::Deploy()
{
	// ############ hu3lifezado ############ //
	// Tempo entre chamadas DamageAnimationAndSound()
#ifndef CLIENT_DLL
	m_nextthink = gpGlobals->time + 0.1;
#endif
	// ############ //

	return DefaultDeploy( "models/v_knife.mdl", "models/p_knife.mdl", KNIFE_DRAW, "crowbar" );
}

void CKnife::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	SendWeaponAnim( KNIFE_HOLSTER );
}

// ############ hu3lifezado ############ //
// Pichar!
// Funcionamentos
// - Tenta colocar decals 40 vezes por segundo;
// - Tenta aplicar danos, animacoes e sons em intervalos minimos de 1 segundo;
// - Impede um novo ataque por 0.35s caso o jogador erre o alvo.
void CKnife::PrimaryAttack()
{
	// Trace generico (obs: o codigo da arma todo pode acessar o resultado desse trace pela variavel "m_trHit")
	bool bDidHit = TraceSomeShit();

	// Cagar cores no mapa
	if (bDidHit)
		PlaceColor();

	// Processamento de dano, animacoes e sons
	if (m_nextthink < pev->nextthink)
	{
		// Aplicamos as animacoes e, se necessario, dano e sons
		DamageAnimationAndSound();

		if ( bDidHit )
			// Se acertou algo os proximos ataques dentro do intervalo de tempo a seguir nao podem ser considerados o primeiro de uma sequencia deles
			m_nextfirsthit = gpGlobals->time + 0.35;
		else
			// Caso contrario eh necessario esperar um tempo ate poder dar o proximo ataque
			m_flNextPrimaryAttack = GetNextAttackDelay(0.35);

		// Tempo entre chamadas do DamageAnimationAndSound()
		m_nextthink = gpGlobals->time + 0.1;
	}

	// Delay caso o player esteja utilizando os decals de fundo ou de assinatura
	if (CVAR_GET_FLOAT("hu3_spray_color") > 10)
		m_flNextPrimaryAttack = GetNextAttackDelay(0.5);

	// Tempo minimo entre chamadas do PrimaryAttack()
	pev->nextthink = gpGlobals->time + 0.0025; // Tempo para o resto dos casos (rapido!)
}

// Trocar as cores
void CKnife::SecondaryAttack()
{
#ifndef CLIENT_DLL
	// Verifica o tempo para sabermos se eh possivel mudar a cor
	if (m_nextcolorchange > gpGlobals->time)
		return;

	// 10 cores + 2 fundos + 1 Carlos Adao no Decals.cpp
	if (CVAR_GET_FLOAT("hu3_spray_color") < 13)
		CVAR_SET_FLOAT("hu3_spray_color", CVAR_GET_FLOAT("hu3_spray_color") + 1);
	else
		CVAR_SET_FLOAT("hu3_spray_color", 1);

    EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, RANDOM_SOUND_ARRAY(pSelectionSounds), 1, ATTN_IDLE, 0, PITCH_NORM );
    
	// Animacao e seu tempo:

    SendWeaponAnim(KNIFE_SELECTION);

	// Faz a mudanca de cor ficar desativada durante tempo da animacao
	m_nextcolorchange = gpGlobals->time + 0.35;

	// Idle tambem so volta depois desse tempo
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.35;
#endif
	return;
}
// ############ //

void CKnife::DamageAnimationAndSound()
{
	// ############ hu3lifezado ############ //
	// !!! Parte de tracing foi movida para a funcao TraceSomeShit()
	TraceResult tr = m_trHit;

	// Caso o ataque em questao seja o primeiro de uma sequencia, devemos rodar a funcao de evento
	if (m_nextfirsthit < gpGlobals->time)
		PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usKnife, 0.0, g_vecZero, g_vecZero, 0, 0, 0, 0.0, 0, 0.0);

	// Animacoes simplificadas:
	switch (((m_iSwing++) % 2) + 1)
	{
	case 0:
		SendWeaponAnim(KNIFE_ATTACK1); break;
	case 1:
		SendWeaponAnim(KNIFE_ATTACK2HIT); break;
	case 2:
		SendWeaponAnim(KNIFE_ATTACK3HIT); break;
	}

	// Animacao "de tiro" do jogador
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	// Dano e sons mais abaixo...
	// ############ //

	if ( tr.flFraction < 1.0 )
	{
#ifndef CLIENT_DLL
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

		if( pEntity )
		{
			g_MultiDamage.Clear();

			if( ( m_flNextPrimaryAttack + 1 < UTIL_WeaponTimeBase() ) || g_pGameRules->IsMultiplayer() )
			{
				// first swing does full damage
				pEntity->TraceAttack( CTakeDamageInfo( m_pPlayer, gSkillData.GetPlrDmgKnife(), DMG_CLUB ), gpGlobals->v_forward, &tr );
			}
			else
			{
				// subsequent swings do half
				pEntity->TraceAttack( CTakeDamageInfo( m_pPlayer, gSkillData.GetPlrDmgKnife() / 2, DMG_CLUB ), gpGlobals->v_forward, &tr );
			}
			g_MultiDamage.ApplyMultiDamage( m_pPlayer, m_pPlayer );
		}

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		bool bHitWorld = true;

		if( pEntity )
		{
			if( pEntity->Classify() != EntityClassifications().GetNoneId() && pEntity->Classify() != EntityClassifications().GetClassificationId( classify::MACHINE ) )
			{
				// ############ hu3lifezado ############ //
				// Novos nomes de audios
				// play thwack or smack sound
				switch( RANDOM_LONG( 0, 1 ) )
				{
				case 0:
					EMIT_SOUND( m_pPlayer, CHAN_ITEM, "weapons/spray_hit_flesh1.wav", 1, ATTN_NORM ); break;
				case 1:
					EMIT_SOUND( m_pPlayer, CHAN_ITEM, "weapons/spray_hit_flesh2.wav", 1, ATTN_NORM ); break;
				}
				// ############ //

				m_pPlayer->m_iWeaponVolume = MELEE_BODYHIT_VOLUME;
				if( !pEntity->IsAlive() )
					// ############ hu3lifezado ############ //
					// Nao retorna mais nada
					return;
					// ############ //
				else
					flVol = 0.1;

				bHitWorld = false;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		// ############ hu3lifezado ############ //
		// Novos nomes de audios e adicionado tempo de espera
		if ( m_nextsprayonwallsound < gpGlobals->time )
		{
			if( bHitWorld )
			{
				// Desnecessario
				/*
				float fvolbar = TEXTURETYPE_PlaySound( tr, vecSrc, vecSrc + ( vecEnd - vecSrc ) * 2, BULLET_PLAYER_CROWBAR );

				if( g_pGameRules->IsMultiplayer() )
				{
					// override the volume here, cause we don't play texture sounds in multiplayer,
					// and fvolbar is going to be 0 from the above call.

					fvolbar = 1;
				}
				*/

				float fvolbar = 1;

				// Novos nomes de audios
				// also play crowbar strike
				switch( RANDOM_LONG( 0, 1 ) )
				{
				case 0:
					EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/spray_hit_wall1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
					break;
				case 1:
					EMIT_SOUND_DYN( m_pPlayer, CHAN_ITEM, "weapons/spray_hit_wall2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
					break;
				}

				// Nao tem mais utilidade aqui, comentado...
				// delay the decal a bit
				//m_trHit = tr;
			}

			// Delay ate a proxima reproducao de som. Deve ser o tempo do som menos o atraso de entrada
			// em DamageAnimationAndSound() feito em PrimaryAttack()
			m_nextsprayonwallsound = gpGlobals->time + 0.3;
		}

		m_pPlayer->m_iWeaponVolume = flVol * MELEE_WALLHIT_VOLUME;

		// Codigo de controle de frames tirado daqui e refeito no PrimaryAttack()
		// ############ //
#endif
	}
}

// ############ hu3lifezado ############ //
// Movi a parte de tracing para ca para ter mais controle e disponibilidade
bool CKnife::TraceSomeShit()
{
	TraceResult * pTrace;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 120;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &m_trHit);

#ifndef CLIENT_DLL
	if (m_trHit.flFraction >= 1.0)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, Hull::HEAD, ENT(m_pPlayer->pev), &m_trHit);
		if (m_trHit.flFraction < 1.0)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(m_trHit.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, m_trHit, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer);
			vecEnd = m_trHit.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	pTrace = &m_trHit;

	if (pTrace->pHit)
		return true;

	return false;
}

// Posicionar ponto de cor
void CKnife::PlaceColor()
{
	TraceResult * pTrace = &m_trHit;

	if (!pTrace->pHit)
		return;

	// Pega a entidade, a superficie acertada
	CBaseEntity* pHit = CBaseEntity::Instance(pTrace->pHit);

#ifndef CLIENT_DLL
	// A entidade eh valida?
	if (!UTIL_IsValidEntity(pHit))
		return;
#endif

	// A entidade eh mapa ou objeto puxavel?
	if (pHit->GetSolidType() == SOLID_BSP || pHit->GetMoveType() == MOVETYPE_PUSHSTEP)
	{
#ifndef CLIENT_DLL
		// Desenhar decal sobre essa entidade
		// Os decals estao dentro do arquivo decals.wad e sao listados em Decals.cpp
		UTIL_DecalTrace(pTrace, 50 + CVAR_GET_FLOAT("hu3_spray_color"));
#endif
	}
}

// Animacoes e sons de idle
void CKnife::WeaponIdle()
{
	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// 3% de chance de tocar EU PICHAVA SIM E CURTIA MUITO!
	if (RANDOM_LONG(0, 99) >= 97)
	{
		iAnim = KNIFE_PICHAVASIM;
		EMIT_SOUND(m_pPlayer, CHAN_WEAPON, "weapons/spray_eupichavasim.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 8;

		// Impedir a troca de cores de funcionar por pouco tempo para impedir que o jogador corte bruscamente a animacao
		m_nextcolorchange = gpGlobals->time + 3;
	}
	// 97% de chance de executar essa parte
	else
	{
		// 15% de chance de olhar o rotulo caso o jogador nao tenha acabado de trocar a cor da arma
		if (flRand <= 0.15 && m_nextcolorchange + 0.35 <= gpGlobals->time)
		{
			iAnim = KNIFE_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
		}
		// 50% de chance para segurar normalmente mas com algum movimento - Tipo 1
		else if (flRand <= 0.50)
		{
			iAnim = KNIFE_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
		}
		// 35% de chance para segurar normalmente mas com algum movimento - Tipo 2
		else
		{
			iAnim = KNIFE_IDLE3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
		}
	}

	SendWeaponAnim(iAnim);
}
// ############ //

#endif //USE_OPFOR
