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
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CKNIFE_H
#define GAME_SHARED_ENTITIES_WEAPONS_CKNIFE_H

enum KnifeAnim
{
	KNIFE_IDLE1 = 0,
	KNIFE_DRAW,
	KNIFE_HOLSTER,
	KNIFE_ATTACK1,
	KNIFE_ATTACK1MISS,
	KNIFE_ATTACK2,
	KNIFE_ATTACK2HIT,
	KNIFE_ATTACK3,
	KNIFE_ATTACK3HIT,
	KNIFE_IDLE2,
	KNIFE_IDLE3,
	KNIFE_CHARGE,
	KNIFE_STAB,
	KNIFE_PICHAVASIM,
    KNIFE_SELECTION
};

class CKnife : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CKnife, CBasePlayerWeapon );
	// ############ hu3lifezado ############ //
	// DECLARE_DATADESC(); removido
	// ############ //

	CKnife();

	void Precache() override;

	void Spawn() override;

	bool Deploy() override;

	void Holster() override;

	void PrimaryAttack() override;

	// ############ hu3lifezado ############ //
	// Funcoes para mexer com pichacao
	void WeaponIdle() override;

	void SecondaryAttack() override;

	void DamageAnimationAndSound();

	bool TraceSomeShit();

	void PlaceColor();
    
   	static const char *pSelectionSounds[];
	// ############ //

private:
	unsigned short m_usKnife;

	int m_iSwing;
	TraceResult m_trHit;

	// ############ hu3lifezado ############ //
	// Tempo ate processar novamente dano, animacoes e sons
	float m_nextthink;
	// Tempo ate proxima mudanca de cor ser liberada
	float m_nextcolorchange;
	// Tempo que quando ultrapassado forca a rechecagem do primeiro acerto
	float m_nextfirsthit;
	// Tempo para o proximo som de spray aplicado em parede
	float m_nextsprayonwallsound;
	// ############ //
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CKNIFE_H
#endif //USE_OPFOR
