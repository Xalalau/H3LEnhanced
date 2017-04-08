add_sources(
	CClientVGUI.h
	CClientVGUI.cpp
	HudDefs.h
	HudDefs.cpp
)

add_subdirectory( hud )
add_subdirectory( shared )
add_subdirectory( vgui )

if( USE_VGUI2 )
	add_subdirectory( vgui2 )
endif()
