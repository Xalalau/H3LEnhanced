add_sources(
	AnimationEvent.h
	CActAnimating.h
	CActAnimating.cpp
	CBaseAnimating.h
	CBaseAnimating.cpp
	CBaseButton.h
	CBaseButton.cpp
	CBaseCombatCharacter.server.cpp
	CBaseDelay.h
	CBaseDelay.cpp
	CBaseDoor.h
	CBaseDoor.cpp
	CBaseEntity.combat.cpp
	CBaseEntity.server.cpp
	CBaseEntity.subs.cpp
	CBaseSpectator.h
	CBaseSpectator.cpp
	CBaseToggle.h
	CBaseToggle.cpp
	CBloodSplat.h
	CBloodSplat.cpp
	CBreakable.h
	CBreakable.cpp
	CButtonTarget.h
	CButtonTarget.cpp
	CCorpse.h
	CCorpse.cpp
	CDeadHEV.h
	CDeadHEV.cpp
	CDecal.h
	CDecal.cpp
	CEnvGlobal.h
	CEnvGlobal.cpp
	CFuncMortarField.h
	CFuncMortarField.cpp
	CFuncParent.h
	CFuncParent.cpp
	CGrenade.h
	CGrenade.cpp
	CInfoIntermission.h
	CInfoIntermission.cpp
	CLaserSpot.h
	CLaserSpot.cpp
	CMomentaryDoor.h
	CMomentaryDoor.cpp
	CMomentaryRotButton.h
	CMomentaryRotButton.cpp
	CMortar.h
	CMortar.cpp
	CMultiManager.h
	CMultiManager.cpp
	CMultiSource.h
	CMultiSource.cpp
	CNullEntity.h
	CNullEntity.cpp
	CPlayerHudColors.h
	CPlayerHudColors.cpp
	CPointCounter.h
	CPointCounter.cpp
	CPointCMD.h
	CPointCMD.cpp
	CPointEntity.h
	CPointEntity.cpp
	CPushable.h
	CPushable.cpp
	CRevertSaved.h
	CRevertSaved.cpp
	CRotButton.h
	CRotButton.cpp
	CRotDoor.h
	CRotDoor.cpp
	CRpgRocket.h
	CRpgRocket.cpp
	CSoundEnt.h
	CSoundEnt.cpp
	CSprayCan.h
	CSprayCan.cpp
	CStripWeapons.h
	CStripWeapons.cpp
	CWorld.h
	CWorld.cpp
	DoorConstants.h
)

add_subdirectory( ammo )
add_subdirectory( bmodels )
add_subdirectory( cycler )
add_subdirectory( effects )
add_subdirectory( items )
add_subdirectory( lights )
add_subdirectory( maprules )
add_subdirectory( NPCs )
add_subdirectory( plats )
add_subdirectory( player )
add_subdirectory( rope )
add_subdirectory( sound )
add_subdirectory( spawnpoints )
add_subdirectory( tanks )
add_subdirectory( triggers )
add_subdirectory( weapons )
add_subdirectory( weather )
add_subdirectory( xen )