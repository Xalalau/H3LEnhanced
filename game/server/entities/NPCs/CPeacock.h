#ifndef GAME_SERVER_ENTITIES_NPCS_CPEACOCK_H
#define GAME_SERVER_ENTITIES_NPCS_CPEACOCK_H

#define		HC_AE_JUMPATTACK	( 2 )

extern Schedule_t slHCRangeFlyFast[];
extern Schedule_t slHCRangeFly[];

class CPeacock : public CBaseMonster
{
public:
	DECLARE_CLASS(CPeacock, CBaseMonster );
	DECLARE_DATADESC();

	void Spawn( void ) override;
	void Precache( void ) override;
	void RunTask(const Task_t& task) override;
	void StartTask(const Task_t& task) override;
	void UpdateYawSpeed() override;
	void LeapTouch( CBaseEntity *pOther );
	Vector Center() const override;
	Vector BodyTarget( const Vector &posSrc ) const override;
	void PainSound( void ) override;
	void DeathSound( void ) override;
	void IdleSound( void ) override;
	void AlertSound( void ) override;
	void PrescheduleThink( void ) override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	bool CheckRangeAttack1( float flDot, float flDist ) override;
	bool CheckRangeAttack2( float flDot, float flDist ) override;
	void OnTakeDamage( const CTakeDamageInfo& info ) override;

	virtual float GetDamageAmount( void ) { return gSkillData.GetHeadcrabDmgBite(); }
	virtual int GetVoicePitch( void ) { return 100; }
	virtual float GetSoundVolume() const { return 1.0; }
	Schedule_t* GetScheduleOfType( int Type ) override;

	DECLARE_SCHEDULES() override;

	static const char *pIdleSounds[];
	static const char *pAlertSounds[];
	static const char *pPainSounds[];
	static const char *pFlySounds[];
	static const char *pDeathSounds[];
};

#endif //GAME_SERVER_ENTITIES_NPCS_CPEACOCK_H