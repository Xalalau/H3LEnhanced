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
// Arma Touros quebrada voadora e tiro secundario, codigos adaptados de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
#include "entities/weapons/CFlyingTouros.h"
// Para balancar a tela:
#include "shake.h"
// Imprimir mensagens
#ifdef CLIENT_DLL
#include "ui/hud/CHudTextMessage.h"
#endif
// ############ //

#include "CDesertEagle.h"

// ############ hu3lifezado ############ //
#ifdef CLIENT_DLL
// Sincronida da qualidade inicial
cvar_t	*hu3_touros_qualidade_inicial;
// Sincronida da municao primaria inicial
cvar_t	*hu3_touros_municao_inicial;
// Sincronida da municao primaria continuamente
cvar_t	*hu3_touros_municao_sync;
#endif
// ############ //

LINK_ENTITY_TO_CLASS(weapon_eagle, CDesertEagle);

CDesertEagle::CDesertEagle()
	: BaseClass(WEAPON_DESERT_EAGLE)
{
}

void CDesertEagle::Precache()
{
	BaseClass::Precache();

	PRECACHE_MODEL("models/v_desert_eagle.mdl");
	PRECACHE_MODEL("models/w_desert_eagle.mdl");
	PRECACHE_MODEL("models/p_desert_eagle.mdl");
	m_iShell = PRECACHE_MODEL("models/shell.mdl");
	PRECACHE_SOUND("weapons/desert_eagle_fire.wav");
	PRECACHE_SOUND("weapons/desert_eagle_reload.wav");
	PRECACHE_SOUND("weapons/desert_eagle_sight.wav");
	PRECACHE_SOUND("weapons/desert_eagle_sight2.wav");
	m_usFireEagle = PRECACHE_EVENT(1, "events/eagle.sc");
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

	SetModel("models/w_desert_eagle.mdl");

	// ############ hu3lifezado ############ //
	// Tempo ate processar a nova chance da arma atirar sozinha
	m_nextbadshootchance = gpGlobals->time + 6;
	// Nos nao podemos imprimir mensagens assim que o jogo comeca
	m_waitforthegametobeready = gpGlobals->time + 12;
	// Arma travada
	m_jammedweapon = false;
	// Qualidade da arma e porcentagem extra de problemas
	m_quality = 0;
	m_qualitypercentageeffect = 0;
	// Imprimir mensagem quando o jogador coleta a arma
	m_firstmessage = true;
	// Sincronização de balas inicial (server) s / Controlar retirada de balas no caso de arma jogada
	m_iClip2 = -1;
#ifdef CLIENT_DLL
	// Server -> client: Comando para copiarmos valores de qualidade inicial 
	hu3_touros_qualidade_inicial = gEngfuncs.pfnRegisterVariable("hu3_touros_qualidade_inicial", "", 0);
	// Server -> client: Comando para copiarmos valores de municao inicial 
	hu3_touros_municao_inicial = gEngfuncs.pfnRegisterVariable("hu3_touros_municao_inicial", "-1", 0);
	// Manter a sincronia de balas entre server e client mesmo que forcado
	hu3_touros_municao_sync = gEngfuncs.pfnRegisterVariable("hu3_touros_municao_sync", "-1", 0);
#endif
	// ############ //

	FallInit();
}

bool CDesertEagle::AddToPlayer(CBasePlayer* pPlayer)
{
	if (BaseClass::AddToPlayer(pPlayer))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, nullptr, pPlayer);
		WRITE_BYTE(m_iId);
		MESSAGE_END();
		return true;
	}

	return false;
}

bool CDesertEagle::Deploy()
{
	// ############ hu3lifezado ############ //
	// Sincronizacao do numero de balas inicial
	// Cliente
#ifdef CLIENT_DLL
	if (hu3_touros_municao_inicial->value != -1)
	{
		m_iClip = hu3_touros_municao_inicial->value;
		hu3_touros_municao_inicial->value = -1;
	}
#endif
	// Servidor
	if (m_iClip2 != -1)
	{
		m_iClip = m_iClip2;
		m_iClip2 = -1;
	}

	// Manter a sincronia de balas entre server e client mesmo que forcado
#ifdef CLIENT_DLL
	if (hu3_touros_municao_sync->value == -1)
		hu3_touros_municao_sync->value = m_iClip;
#endif

	// Inicializo o controle da animação de reload
	m_reloaded = false;
	// ############ //

	return DefaultDeploy(
		"models/v_desert_eagle.mdl", "models/p_desert_eagle.mdl",
		DEAGLE_DRAW,
		"onehanded");
}

void CDesertEagle::Holster()
{
	m_fInReload = false;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10.0, 15.0);

	SendWeaponAnim(DEAGLE_HOLSTER);
}

void CDesertEagle::WeaponIdle()
{
	ResetEmptySound();

	// ############ hu3lifezado ############ //
	// Gambiarra: a animacao de reload demora 3.7 segundos, e aqui faco os 2 segundos finais.
	// Se estiver voltando de um reload, termino jogando a arma fora
	if (m_reloaded)
	{
		ThrowWeapon(m_reloaded);

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.8;
		m_flNextSecondaryAttack = gpGlobals->time + 1.8;

		m_reloaded = false;

		return;
	}

	// Entre 4 e 7 segundos tem entre 10% e 30% de chance da arma atirar sozinha (leva 12 segundos para comecar a rodar inicialmente)
	if (m_nextbadshootchance <= gpGlobals->time && m_waitforthegametobeready <= gpGlobals->time)
	{
		m_nextbadshootchance = gpGlobals->time + UTIL_SharedRandomLong(m_pPlayer->random_seed, 4, 7);

		if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, 99) >= (90 - 20 * m_qualitypercentageeffect) && !m_jammedweapon && m_iClip != 0)
		{
			UTIL_Sparks(m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 22 + gpGlobals->v_right * 10);
#ifndef CLIENT_DLL
			UTIL_SayText("Sua arma Cornus atirou sozinha ou balas sairam voando!|r", m_pPlayer);

			// 30% de chance de levar dano de 1 a 5 por estilhacos
			ShrapnelDamage(30, 1, 5);
#endif
			PrimaryAttack();

			return;
		}
	}
	// ############ //

	if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip)
	{
		const float flNextIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0.0, 1.0);

		int iAnim;

		if (flNextIdle <= 0.3)
		{
			iAnim = DEAGLE_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextIdle + 2.5;
		}
		else
		{
			if (flNextIdle > 0.6)
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

		SendWeaponAnim(iAnim);
	}
}

// ############ hu3lifezado ############ //
// De 3% a 17% de chance da arma travar e nao atirar mais
bool CDesertEagle::RandomlyJammed()
{
	bool print = false;

	if (m_jammedweapon)
		print = true;
	else if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, 99) >= (97 - 14 * m_qualitypercentageeffect))
	{
		m_jammedweapon = true;
		print = true;
	}

	if (print)
	{
#ifndef CLIENT_DLL
		UTIL_SayText("Sua arma Cornus travou! Recarregue-a!|r", m_pPlayer);
#endif
		return true;
	}

	return false;
}

// De 1% a 9% de chance da arma quebrar e sair voando
bool CDesertEagle::RandomlyBreak()
{
	if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, 99) >= (99 - 8 * m_qualitypercentageeffect))
	{
		if (m_pPlayer->RemovePlayerItem(this)) // Only throw if we were able to detatch from player.
		{
			// Como a arma quebra, o jogador perde a municao que esta nela, e eu preciso fazer isso antes de tudo!
			if (m_iClip != 0)
				m_iClip = 0;

#ifndef CLIENT_DLL
			// Dano de estilhaco de 10 a 25 no jogador, efeitos da tela e tremer, som da arma quebrando
			ShrapnelDamage(100, 10, 25);
			UTIL_ScreenShake(m_pPlayer->GetAbsOrigin(), 6.0, 4.0, 0.6, 750);
			EMIT_SOUND_DYN(this, CHAN_WEAPON, "weapons/desert_eagle_fuck.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 0xF));
#endif

			// Get the origin, direction, and fix the angle of the throw.
			Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 8 + gpGlobals->v_forward * 16;
			Vector vecDir = gpGlobals->v_forward;
			Vector vecAng = UTIL_VecToAngles(vecDir);
			vecAng.z = vecDir.z - 90;

#ifndef CLIENT_DLL
			// Create a flying Touros.
			CFlyingTouros *pFTouros = (CFlyingTouros *)Create("flying_touros", vecSrc, Vector(0, 0, 0), m_pPlayer->edict());

			// Give the Touros its velocity, angle, and spin. 
			// Lower the gravity a bit, so it flys. 
			pFTouros->pev->velocity = vecDir * 500 + m_pPlayer->pev->velocity;
			pFTouros->pev->angles = vecAng;
			pFTouros->pev->avelocity.x = -1000;
			pFTouros->pev->gravity = .5;
			pFTouros->m_pPlayer = m_pPlayer;

			// Nao deixar o jogador pegar a arma do chao
			pFTouros->SetMode(1);
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
			UTIL_SayText("Sua arma Cornus quebrou!|r", m_pPlayer);
#endif
			// Zero a qualidade
			m_quality = 0;
#ifdef CLIENT_DLL
			hu3_touros_qualidade_inicial->value = 0;
#endif
		}
		return true;
	}
	return false;
}

// De 1% a 8% de chance da arma perder toda a municao
bool CDesertEagle::RandomlyLostAllAmmo()
{
	if (UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 100) <= 1 + 7 * m_qualitypercentageeffect)
	{
		int i;
#ifndef CLIENT_DLL
		UTIL_SayText("Infelizmente a sua arma Cornus estragou toda a municao dela...|r", m_pPlayer);
#endif
		// Tomar dano de estilhacos
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

// Dano por estilhaco
void CDesertEagle::ShrapnelDamage(int chance, int min_damage, int max_damage)
{
#ifndef CLIENT_DLL
	if (RANDOM_LONG(1, 100) <= chance)
	{
		UTIL_SayText("Voce foi ferido por estilhacos de bala da sua arma Cornus...|r", m_pPlayer);

		float damage = RANDOM_LONG(min_damage, max_damage);
		TraceResult tr = UTIL_GetGlobalTrace();
		g_MultiDamage.Clear();
		m_pPlayer->TraceAttack(CTakeDamageInfo(m_pPlayer, damage, DMG_BLAST), GetAbsVelocity().Normalize(), tr);
		g_MultiDamage.ApplyMultiDamage(this, m_pPlayer);
		UTIL_ScreenFade(m_pPlayer, Vector(255, 0, 0), 0.2, 0.1, 128, FFADE_IN);
	}
#endif
}

// Definir a qualidade da arma
void CDesertEagle::SetQuality()
{
	// Verifico se o servidor enviou algum valor de qualidade para ser sincronizado no cliente
#ifdef CLIENT_DLL
	if (hu3_touros_qualidade_inicial->value != 0)
		m_quality = hu3_touros_qualidade_inicial->value;
#endif

	// Se nao houver a qualidade definida, precisamos de uma
	if (m_quality == 0)
		m_quality = UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 9);

	// Cada defeito da arma tem um bonus que e adicionado de 0 até 100% dependendo dessa qualidade 9 ate 1;
	m_qualitypercentageeffect = 1.0 - (m_quality - 1 + (m_quality - 1) * 1 / 8) * 1.0 / 9;

	// Mensagem inicial, sempre no primeiro tiro
#ifndef CLIENT_DLL
	if (m_firstmessage)
	{
		char mensagem_i[70] = "Pelo uso voce percebeu a qualidade dessa Cornus:";
		char mensagem_m[15];
		snprintf(mensagem_m, 9, " %d/10", m_quality);
		char mensagem_f[5] = "|g";
		strcat(mensagem_i, strcat(mensagem_m, mensagem_f));
		UTIL_SayText(mensagem_i, m_pPlayer);
		m_firstmessage = false;
	}
#endif
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

	// Nao deixo a arma que

	// BUG BUG!!!!
	// CORRECAO!!! Eu descobri que quando a municao acaba o servidor tem se alterado
	// instantaneamente para algo entre 7 e 10 balas!!! Esse numero so eh corrigido
	// depois que o jogador termina de recarregar. A fim de evitar erros de sincronia, de
	// animacao e estresse procurando o que esta gerando esse bug no HL1, eu simplesmente 
	// forco o codigo a nao rodar ate que ele esteja normalizado. Mas isso eg solucao
	// parcial, pois nao cobre o caso dela vir com 7 balas... Esse problema deve ser
	// para o Vanheer.
#ifndef CLIENT_DLL
	if (m_iClip > 7)
		return;
#endif
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

	if (m_iClip <= 0)
	{
		if (!m_fInReload)
		{
			if (m_bFireOnEmpty)
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

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_pPlayer->GetEffects() |= EF_MUZZLEFLASH;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// Quantidade de balas atiradas de uma vez segundo a qualidade da arma
	int i, j = 0;

	if (m_qualitypercentageeffect <= 0.25)
	{
		j = 1;
	}
	else if (m_qualitypercentageeffect > 0.25 && m_qualitypercentageeffect <= 0.50)
	{
		j = UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 2);
	}
	else if (m_qualitypercentageeffect > 0.50 && m_qualitypercentageeffect <= 0.75)
	{
		j = UTIL_SharedRandomLong(m_pPlayer->random_seed, 1, 4);
	}
	else if (m_qualitypercentageeffect > 0.75 && m_qualitypercentageeffect <= 1.0)
	{
		j = UTIL_SharedRandomLong(m_pPlayer->random_seed, 2, 7);
	}

	// Movi essas linhas para fora do loop
	UTIL_MakeVectors(m_pPlayer->GetViewAngle() + m_pPlayer->GetPunchAngle());
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	// Entre 2% e 10% de chance de levar dano de 1 a 5 por estilhacos (caso a arma nao esteja travada)
	if (!m_jammedweapon)
		ShrapnelDamage(2 + 8 * m_qualitypercentageeffect, 1, 5);

	for (i = 0; i < j; i++)
	{
		// Chance de travar a arma
		if (RandomlyJammed())
		{
			if (m_iClip == 7)
				--m_iClip;

			SendWeaponAnim(DEAGLE_SHOOT);
			break;
		}

		// Chance de quebrar a arma
		if (RandomlyLostAllAmmo())
			break;

		--m_iClip;

		// Novo espalhamento todo zoado (distancia do centro)
		float flSpread = UTIL_SharedRandomFloat(m_pPlayer->random_seed + i, 0.01, 0.91 + 0.49 * m_qualitypercentageeffect);

		Vector vecSpread;

		vecSpread = m_pPlayer->FireBulletsPlayer(
			1,
			vecSrc, vecAiming, Vector(flSpread, flSpread, flSpread),
			8192.0, BULLET_PLAYER_DEAGLE, 0, 0,
			m_pPlayer, UTIL_SharedRandomLong(m_pPlayer->random_seed + i, 1, 10)); // Mais espalhamento aqui (angulos ao redor do centro)

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

	/*
	// Para checar a porcaria da sincronia de balas
	#ifdef CLIENT_DLL
	ALERT( at_console, "-------- j / m_iClip: %d / % d\n", j, m_iClip);
	#endif
	#ifndef CLIENT_DLL
	char mensagem_i[70] = "[HU3 DEBUG] (servidor) j / m_iClip:";
	char mensagem_m[15];
	snprintf(mensagem_m, 9, " %d / %d", j, m_iClip);
	char mensagem_f[5] = "|b";
	strcat(mensagem_i, strcat(mensagem_m, mensagem_f));
	UTIL_SayText(mensagem_i, m_pPlayer);
	#endif
	*/

	// Forco a sincronia de balas com isso
#ifndef CLIENT_DLL
	char command[35] = "hu3_touros_municao_sync ";
	char value[2];
	snprintf(value, 2, "%d", m_iClip);
	strcat(strcat(command, value), "\n");
	CLIENT_COMMAND(ENT(m_pPlayer), command);
#else
	if (m_iClip != hu3_touros_municao_sync->value)
		m_iClip = hu3_touros_municao_sync->value;
#endif

	// Novos tempos
	//m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + (m_bLaserActive ? 0.5 : 0.22);
	//m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10.0, 15.0);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 4.0, 8.0);
	// ############ //
}

// ############ hu3lifezado ############ //
// Tiro secundario:
void CDesertEagle::SecondaryAttack()
{
	ThrowWeapon(false);
}

// Varejar a arma, adaptado de:
// http://web.archive.org/web/20020717063241/http://lambda.bubblemod.org/tuts/crowbar/
void CDesertEagle::ThrowWeapon(bool isReloading)
{
	if (isReloading || m_pPlayer->RemovePlayerItem(this))
	{
		// Don't throw if we were able to detatch from player.

		// Como a arma eh jogada fora, o jogador perde a municao que esta nela, e eu preciso fazer isso antes de tudo!
		if (!isReloading && m_iClip != 0)
		{
			m_iClip2 = m_iClip;
			m_iClip = 0;
		}

		// Get the origin, direction, and fix the angle of the throw.
		Vector vecSrc = m_pPlayer->GetGunPosition();
		if (!isReloading)
			vecSrc = vecSrc + gpGlobals->v_right * 8 + gpGlobals->v_forward * 16;
		else
			vecSrc = vecSrc + gpGlobals->v_right * 16 + gpGlobals->v_forward * 3 - gpGlobals->v_up * 27;
		Vector vecDir = gpGlobals->v_forward + gpGlobals->v_right * 0.35;
		Vector vecAng = UTIL_VecToAngles(vecDir);
		vecAng.z = vecDir.z - 90;

#ifndef CLIENT_DLL
		// Create a flying Touros.
		CFlyingTouros *pFTouros = (CFlyingTouros *)Create("flying_touros", vecSrc, Vector(0, 0, 0), m_pPlayer->edict());

		// Give the Touros its velocity, angle, and spin. 
		// Lower the gravity a bit, so it flys. 
		pFTouros->pev->velocity = vecDir * 200 + m_pPlayer->pev->velocity;
		pFTouros->pev->angles = vecAng;
		pFTouros->pev->avelocity.x = -500;
		pFTouros->pev->gravity = .9;
		pFTouros->m_pPlayer = m_pPlayer;
		int mode = 0;

		// Definir se o jogador vai poder pegar a arma do chao ou nao
		if (isReloading)
			mode = 1;
		pFTouros->SetMode(mode);

		// Salvo a qualidade da arma e a quantidade de balas inicial
		if (!isReloading)
		{
			if (m_quality == 0) // Caso o jogador jogue a arma fora antes de dar o primeiro tiro
				m_quality = RANDOM_LONG(1, 9);

			pFTouros->SetQuality(m_quality, m_iClip2);
		}

		// Do player weapon anim and sound effect. 
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		EMIT_SOUND_DYN(this, CHAN_WEAPON, "weapons/cbar_miss1.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 0xF));
#endif

		if (!isReloading)
		{
			// Zero a qualidade
			m_quality = 0;
#ifdef CLIENT_DLL
			hu3_touros_qualidade_inicial->value = 0;
#endif

			// Just for kicks, set this. 
			// But we destroy this weapon anyway so... thppt. 
			m_flNextSecondaryAttack = gpGlobals->time + 0.75;

			// Mensagem
#ifndef CLIENT_DLL
			UTIL_SayText("Voce jogou sua arma Cornus fora!|g", m_pPlayer);
#endif

			// take item off hud
			m_pPlayer->pev->weapons &= ~(1 << this->m_iId);

			// Destroy this weapon
			DestroyItem();
		}
	}
}
// ############ //

void CDesertEagle::Reload()
{
	// ############ hu3lifezado ############ //
	// Se a arma estiver travada, liberar
	if (m_jammedweapon)
		m_jammedweapon = false;

	if (m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] > 0 && !m_reloaded)
	{
		// Tempo reajustado (1.5)
		const bool bResult = DefaultReload(m_iClip ? DEAGLE_RELOAD : DEAGLE_RELOAD_NOSHOT, 1.9, 1);

		if (bResult)
		{
			// Gambiarra: a animacao demora 3.7 segundos, mas aqui faco so 1.9. Na parte de idle eu jogo a arma fora e atraso mais 1.8 segundos. Nao me importo, eh mais facil fazer logo assim
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;
			// Indico que estou em um reload
			m_reloaded = true;
		}
	}
	// ############ //
}

void CDesertEagle::GetWeaponData(weapon_data_t& data)
{
	BaseClass::GetWeaponData(data);
}

void CDesertEagle::SetWeaponData(const weapon_data_t& data)
{
	BaseClass::SetWeaponData(data);
}
#endif //USE_OPFOR