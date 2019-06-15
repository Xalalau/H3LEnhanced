add_sources(
	CClientGameInterface.h
	CClientGameInterface.cpp
	cdll_int.cpp
	cl_dll.h
	cl_util.h
	cl_util.cpp
	com_weapons.h
	com_weapons.cpp
	CWeaponHUDInfo.h
	CWeaponHUDInfo.cpp
	demo.h
	demo.cpp
	entity.cpp
	ev_common.cpp
	ev_hldm.h
	ev_hldm.cpp
	events.cpp
	eventscripts.h
	Exports.h
	global_consts.h
	KeyValuesCompat.h
	KeyValuesCompat.cpp
	MessageHandler.h
	MessageHandler.cpp
	player_info.h
	voice_banmgr.h
	voice_banmgr.cpp
	voice_status.h
	voice_status.cpp
)

add_subdirectory( effects )
add_subdirectory( entities )
add_subdirectory( hl )
add_subdirectory( input )
add_subdirectory( renderer )
add_subdirectory( ui )