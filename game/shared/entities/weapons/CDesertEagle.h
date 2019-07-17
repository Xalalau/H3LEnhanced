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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CDESERTEAGLE_H
#define GAME_SHARED_ENTITIES_WEAPONS_CDESERTEAGLE_H

class CDesertEagleLaser;

enum DesertEagleAnim
{
	DEAGLE_IDLE1 = 0,
	DEAGLE_IDLE2,
	DEAGLE_IDLE3,
	DEAGLE_IDLE4,
	DEAGLE_IDLE5,
	DEAGLE_SHOOT,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD_NOSHOT,
	DEAGLE_RELOAD,
	DEAGLE_DRAW,
	DEAGLE_HOLSTER
};

class CDesertEagle : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS(CDesertEagle, CBasePlayerWeapon);

	CDesertEagle();

	void Precache() override;

	void Spawn() override;

	bool AddToPlayer(CBasePlayer* pPlayer) override;

	bool Deploy() override;

	void Holster() override;

	void WeaponIdle() override;

	void PrimaryAttack() override;

	void SecondaryAttack() override;

	void Reload() override;

	void GetWeaponData(weapon_data_t& data) override;

	void SetWeaponData(const weapon_data_t& data) override;

	// ############ hu3lifezado ############ //
	// Chance da arma quebrar e sair voando
	bool RandomlyBreak();
	// Chance da arma travar e nao atirar mais
	bool RandomlyJammed();
	// Definir a qualidade da arma
	void SetQuality();
	// Dano por estilhacos
	void ShrapnelDamage(int chance, int min_damage, int max_damage);
	// Perder toda a municao
	bool RandomlyLostAllAmmo();
	// ############ //

private:
	void UpdateLaser();

private:
	int m_iShell;
	unsigned short m_usFireEagle;

	// ############ hu3lifezado ############ //
	// Tempo ate processar a nova chance da arma atirar sozinha
	float m_nextbadshootchance;
	// Nos nao podemos imprimir mensagens assim que o jogo comeca
	float m_waitforthegametobeready;
	// Arma travada
	bool m_jammedweapon;
	// Imprimir mensagem quando o jogador coleta a arma
	bool m_firstmessage;
	// Comando para copiarmos valores de qualidade do server para o client
	cvar_t	*hu3_touros_gambiarra_qualidade;
	// Cada defeito da arma tem um bônus que é adicionado de 0 até 100% dependendo dessa qualidade 9 até 1;
	float m_qualitypercentageeffect;

public:
	// Qualidade da arma (de 1 ate 9, nunca 10)
	int m_quality;
	// Recuperar numero inicial de balas / Controlar retirada de balas no caso de arma jogada
	int m_iClip2;
	// ############ //


};
#endif //GAME_SHARED_ENTITIES_WEAPONS_CDESERTEAGLE_H

#endif //USE_OPFOR