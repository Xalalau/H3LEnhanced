#ifndef GAME_SERVER_ENTITIES_TRIGGERS_CWEAPONCONDITION_H
#define GAME_SERVER_ENTITIES_TRIGGERS_CWEAPONCONDITION_H

class CWeaponCondition : public CBaseTrigger
{
public:
	DECLARE_CLASS( CWeaponCondition, CBaseTrigger );
	DECLARE_DATADESC();

	void Spawn( void );

	void KeyValue( KeyValueData *pkvd );
	virtual void Use( CBaseEntity pActivator, CBaseEntity pCaller, USE_TYPE useType, float value );
	void Touch ( CBaseEntity *pOther );
		
	void ProcessConditions();

	string_t m_TargetHasWpnAndAmmo;
	string_t m_TargetHasWpn;
	string_t m_TargetDisarmed;
};


#endif //GAME_SERVER_ENTITIES_TRIGGERS_CWEAPONCONDITION_H