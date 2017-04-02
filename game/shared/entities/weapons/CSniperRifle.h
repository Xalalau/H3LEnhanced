#if USE_OPFOR
#ifndef GAME_SHARED_ENTITIES_WEAPONS_CSNIPERRIFLE_H
#define GAME_SHARED_ENTITIES_WEAPONS_CSNIPERRIFLE_H

enum SniperRifleAnim
{
	SNIPERRIFLE_DRAW = 0,
	SNIPERRIFLE_SLOWIDLE,
	SNIPERRIFLE_FIRE,
	SNIPERRIFLE_FIRELASTROUND,
	SNIPERRIFLE_RELOAD1,
	SNIPERRIFLE_RELOAD2,
	SNIPERRIFLE_RELOAD3,
	SNIPERRIFLE_SLOWIDLE2,
	SNIPERRIFLE_HOLSTER
};

/**
*	Opposing force sniper rifle.
*	TODO: use base class for snipers?
*	- Solokiller
*/
class CSniperRifle : public CBasePlayerWeapon
{
public:
	DECLARE_CLASS( CSniperRifle, CBasePlayerWeapon );
	DECLARE_DATADESC();

	CSniperRifle();

	void Precache() override;
	void Spawn() override;

	bool AddToPlayer( CBasePlayer* pPlayer ) override;

	bool Deploy() override;

	void Holster() override;

	void WeaponIdle() override;

	void PrimaryAttack() override;

	void SecondaryAttack() override;

	void Reload() override;

	void ToggleZoom();

public:
	// ############ hu3lifezado ############ //
	// Funcoes para renderizar a mira em terceira pessoa
	void ItemPreFrame(void);
	void UpdateSpot();
	void RemoveSpot();
	// ############ //

private:
	unsigned short m_usSniper;

	bool m_bInZoom;
	bool m_bReloading;
	float m_flReloadStart;

	// ############ hu3lifezado ############ //
	// Mira em terceira pessoa
	CHu3XSpot* m_pLaser;
	// ############ //
};

#endif //GAME_SHARED_ENTITIES_WEAPONS_CSNIPERRIFLE_H
#endif //USE_OPFOR