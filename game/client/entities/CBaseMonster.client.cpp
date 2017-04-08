#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include	"nodes/Nodes.h"

CBaseEntity* CBaseMonster::CheckTraceHullAttack( float flDist, int iDamage, int iDmgType )
{
	return nullptr;
}

void CBaseMonster::Eat( float flFullDuration )
{
}

bool CBaseMonster::FShouldEat() const
{
	return true;
}

void CBaseMonster::BarnacleVictimBitten( CBaseEntity* pBarnacle )
{
}

void CBaseMonster::BarnacleVictimReleased()
{
}

void CBaseMonster::Listen()
{
}

float CBaseMonster::FLSoundVolume( CSound *pSound )
{
	return 0.0;
}

bool CBaseMonster::FValidateHintType( short sHint ) const
{
	return false;
}

void CBaseMonster::Look( int iDistance )
{
}

int CBaseMonster::ISoundMask()
{
	return 0;
}

CSound* CBaseMonster::PBestSound()
{
	return nullptr;
}

CSound* CBaseMonster::PBestScent()
{
	return nullptr;
}

void CBaseMonster::MonsterThink()
{
}

void CBaseMonster::MonsterUse( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value )
{
}

int CBaseMonster::IgnoreConditions()
{
	return 0;
}

void CBaseMonster::RouteClear()
{
}

void CBaseMonster::RouteNew()
{
}

bool CBaseMonster::FRouteClear() const
{
	return false;
}

bool CBaseMonster::FRefreshRoute()
{
	return false;
}

bool CBaseMonster::MoveToEnemy( Activity movementAct, float waitTime )
{
	return false;
}

bool CBaseMonster::MoveToLocation( Activity movementAct, float waitTime, const Vector& goal )
{
	return false;
}

bool CBaseMonster::MoveToTarget( Activity movementAct, float waitTime )
{
	return false;
}

bool CBaseMonster::MoveToNode( Activity movementAct, float waitTime, const Vector& goal )
{
	return false;
}

void CBaseMonster::RouteSimplify( const CBaseEntity* const pTargetEnt )
{
}

bool CBaseMonster::BarnacleVictimGrabbed( CBaseEntity* pBarnacle )
{
	return true;
}

bool CBaseMonster::CheckRangeAttack1( float flDot, float flDist )
{
	return false;
}

bool CBaseMonster::CheckRangeAttack2( float flDot, float flDist )
{
	return false;
}

bool CBaseMonster::CheckMeleeAttack1( float flDot, float flDist )
{
	return false;
}

bool CBaseMonster::CheckMeleeAttack2( float flDot, float flDist )
{
	return false;
}

void CBaseMonster::CheckAttacks( CBaseEntity* pTarget, float flDist )
{
}

bool CBaseMonster::FCanCheckAttacks() const
{
	return false;
}

bool CBaseMonster::CheckEnemy( CBaseEntity* pEnemy )
{
	return false;
}

void CBaseMonster::PushEnemy( CBaseEntity* pEnemy, Vector& vecLastKnownPos )
{
}

bool CBaseMonster::PopEnemy()
{
	return false;
}

void CBaseMonster::SetActivity( Activity NewActivity )
{
}

void CBaseMonster::SetSequenceByName( const char* const pszSequence )
{
}

int CBaseMonster::CheckLocalMove( const Vector &vecStart, const Vector &vecEnd, const CBaseEntity* const pTarget, float *pflDist )
{
	return 0;
}

float CBaseMonster::OpenDoorAndWait( CBaseEntity* pDoor )
{
	return 0.0;
}

void CBaseMonster::AdvanceRoute( float distance )
{
}

int CBaseMonster::RouteClassify( int iMoveFlag )
{
	return 0;
}

bool CBaseMonster::BuildRoute( const Vector &vecGoal, int iMoveFlag, const CBaseEntity* const pTarget )
{
	return false;
}

void CBaseMonster::InsertWaypoint( Vector vecLocation, int afMoveFlags )
{
}

bool CBaseMonster::FTriangulate( const Vector &vecStart, const Vector &vecEnd, float flDist, const CBaseEntity* const pTargetEnt, Vector *pApex )
{
	return false;
}

void CBaseMonster::Move( float flInterval )
{
}

bool CBaseMonster::ShouldAdvanceRoute( float flWaypointDist )
{
	return false;
}

void CBaseMonster::MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval )
{
}

void CBaseMonster::MonsterInit()
{
}

void CBaseMonster::MonsterInitThink()
{
}

void CBaseMonster::StartMonster()
{
}

void CBaseMonster::MovementComplete()
{
}

bool CBaseMonster::TaskIsRunning() const
{
	return false;
}

Relationship CBaseMonster::IRelationship( CBaseEntity* pTarget )
{
	return R_NO;
}

bool CBaseMonster::FindCover( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist )
{
	return false;
}

bool CBaseMonster::BuildNearestRoute( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist )
{
	return false;
}

CBaseEntity* CBaseMonster::BestVisibleEnemy()
{
	return nullptr;
}

bool CBaseMonster::FInViewCone( const CBaseEntity* pEntity ) const
{
	return false;
}

bool CBaseMonster::FInViewCone( const Vector& vecOrigin ) const
{
	return false;
}

void CBaseMonster::MakeIdealYaw( Vector vecTarget )
{
}

float CBaseMonster::FlYawDiff() const
{
	return 0.0;
}

float CBaseMonster::ChangeYaw( int yawSpeed )
{
	return 0;
}

float CBaseMonster::VecToYaw( Vector vecDir )
{
	return 0.0;
}

void CBaseMonster::SetEyePosition()
{
}

void CBaseMonster::HandleAnimEvent( AnimEvent_t& event )
{
}

Vector CBaseMonster::GetGunPosition()
{
	return g_vecZero;
}

void CBaseMonster::MakeDamageBloodDecal( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir )
{
}

bool CBaseMonster::FGetNodeRoute( const Vector& vecDest )
{
	return true;
}

int CBaseMonster::FindHintNode()
{
	return NO_NODE;
}

void CBaseMonster::ReportAIState()
{
}

void CBaseMonster::KeyValue( KeyValueData *pkvd )
{
}

bool CBaseMonster::FCheckAITrigger()
{
	return false;
}

bool CBaseMonster::CanPlaySequence( const bool fDisregardMonsterState, int interruptLevel ) const
{
	return false;
}

bool CBaseMonster::FindLateralCover( const Vector &vecThreat, const Vector &vecViewOffset )
{
	return false;
}

Vector CBaseMonster::ShootAtEnemy( const Vector &shootOrigin )
{
	return g_vecZero;
}

bool CBaseMonster::FacingIdeal() const
{
	return false;
}

bool CBaseMonster::FCanActiveIdle() const
{
	return false;
}

void CBaseMonster::PlaySentence( const char *pszSentence, float duration, float volume, float attenuation )
{
}

void CBaseMonster::PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, const bool bConcurrent, CBaseEntity *pListener )
{
}

void CBaseMonster::SentenceStop()
{
}

void CBaseMonster::CorpseFallThink()
{
}

void CBaseMonster::MonsterInitDead()
{
}

bool CBaseMonster::BBoxFlat() const
{
	return true;
}

bool CBaseMonster::GetEnemy()
{
	return false;
}

void CBaseMonster::TraceAttack( const CTakeDamageInfo& info, Vector vecDir, TraceResult *ptr )
{
}

CBaseEntity* CBaseMonster::DropItem( const char* const pszItemName, const Vector &vecPos, const Vector &vecAng )
{
	return nullptr;
}

void CBaseMonster::RadiusDamage( CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType )
{
}

void CBaseMonster::RadiusDamage( Vector vecSrc, CBaseEntity* pInflictor, CBaseEntity* pAttacker, float flDamage, EntityClassification_t iClassIgnore, int bitsDamageType )
{
}

void CBaseMonster::FadeMonster()
{
}

void CBaseMonster::GibMonster()
{
}

bool CBaseMonster::HasHumanGibs()
{
	return false;
}

bool CBaseMonster::HasAlienGibs()
{
	return false;
}

Activity CBaseMonster::GetDeathActivity()
{
	return ACT_DIE_HEADSHOT;
}

MONSTERSTATE CBaseMonster::GetIdealState()
{
	return MONSTERSTATE_ALERT;
}

Schedule_t* CBaseMonster::GetScheduleOfType( int Type )
{
	return nullptr;
}

Schedule_t *CBaseMonster::GetSchedule()
{
	return nullptr;
}

void CBaseMonster::RunTask( const Task_t* pTask )
{
}

void CBaseMonster::StartTask( const Task_t* pTask )
{
}

const Schedule_t *CBaseMonster::ScheduleFromName( const char* const pszName ) const
{
	return nullptr;
}

void CBaseMonster::BecomeDead()
{
}

void CBaseMonster::RunAI()
{
}

void CBaseMonster::Killed( const CTakeDamageInfo& info, GibAction gibAction )
{
}

void CBaseMonster::OnTakeDamage( const CTakeDamageInfo& info )
{
}

bool CBaseMonster::Restore( CRestore& )
{
	return true;
}