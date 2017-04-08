/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#ifndef GAME_SERVER_ENTITIES_NPCS_CSCIENTIST_H
#define GAME_SERVER_ENTITIES_NPCS_CSCIENTIST_H

#define		NUM_SCIENTIST_HEADS		4 // four heads available for scientist model
enum { HEAD_GLASSES = 0, HEAD_EINSTEIN = 1, HEAD_LUTHER = 2, HEAD_SLICK = 3 };

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		SCIENTIST_AE_HEAL		( 1 )
#define		SCIENTIST_AE_NEEDLEON	( 2 )
#define		SCIENTIST_AE_NEEDLEOFF	( 3 )

//=========================================================
// human scientist (passive lab worker)
//=========================================================
class CScientist : public CTalkMonster
{
public:
	DECLARE_CLASS( CScientist, CTalkMonster );
	DECLARE_DATADESC();

	void Spawn() override;
	void Precache() override;

	void SetYawSpeed() override;
	EntityClassification_t GetClassification() override;
	void HandleAnimEvent( AnimEvent_t& event ) override;
	void RunTask( const Task_t* pTask ) override;
	void StartTask( const Task_t* pTask ) override;
	int	ObjectCaps() const override { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
	void OnTakeDamage( const CTakeDamageInfo& info ) override;
	virtual int FriendNumber( int arrayNumber ) const override;
	void SetActivity( Activity newActivity ) override;
	Activity GetStoppedActivity() override;
	int ISoundMask() override;
	void DeclineFollowing() override;

	float	CoverRadius() override { return 1200; }		// Need more room for cover because scientists want to get far away!
	bool	DisregardEnemy( CBaseEntity *pEnemy ) const { return !pEnemy->IsAlive() || ( gpGlobals->time - m_fearTime ) > 15; }

	bool	CanHeal() const;
	void	Heal();
	void	Scream();

	// Override these to set behavior
	Schedule_t *GetScheduleOfType( int Type ) override;
	Schedule_t *GetSchedule( void ) override;
	MONSTERSTATE GetIdealState( void ) override;

	void DeathSound( void ) override;
	void PainSound( void ) override;

	void TalkInit( void );

	void Killed( const CTakeDamageInfo& info, GibAction gibAction ) override;

	DECLARE_SCHEDULES() override;

private:
	float m_painTime;
	float m_healTime;
	float m_fearTime;
};

#endif //GAME_SERVER_ENTITIES_NPCS_CSCIENTIST_H