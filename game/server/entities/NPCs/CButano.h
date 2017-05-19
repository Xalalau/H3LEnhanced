#ifndef GAME_SERVER_ENTITIES_NPCS_CBUTANO_H
#define GAME_SERVER_ENTITIES_NPCS_CBUTANO_H

// Definimos o tempo de vazamento do gas
// Para explodir depois de um tmepo
#define VAZAMENTO_BUTANO 15.0

class CButano : public CZombie
{
public:
	DECLARE_CLASS(CButano, CZombie);

	void Spawn(void) override;
	void Precache() override;

	void SetNewSpawn(void);

	void HandleAnimEvent(AnimEvent_t& event) override;
	void Killed(const CTakeDamageInfo& info, GibAction gibAction) override;

	void AtaqueCabuloso(void);
	void ExplodeButano(int dano, int magn);

	void PainSound(void) override;
	void AlertSound(void) override;
	void IdleSound(void) override;
	void AttackSound(void);

	static const char *pAttackSounds[];
	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];
};

#endif //GAME_SERVER_ENTITIES_NPCS_CBUTANO_H
