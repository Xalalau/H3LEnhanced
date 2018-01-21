#
# Finds the VGUI1 library
# Expects to be called from the SDK root
#

# On Windows we need to link with the import library, on other systems we link with the shared library distributed with the game
if( MSVC )
	find_library( VGUI1 vgui${CMAKE_STATIC_LIBRARY_SUFFIX} PATHS ${vgui_DIR} NO_DEFAULT_PATH )
else()
	find_library( VGUI1 vgui${CMAKE_SHARED_LIBRARY_SUFFIX} PATHS ${vgui_DIR} NO_DEFAULT_PATH )
endif()

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( VGUI1 DEFAULT_MSG )

if( VGUI1 )
	add_library( vgui SHARED IMPORTED )
	if( MSVC )
		set_property( TARGET vgui PROPERTY IMPORTED_IMPLIB ${VGUI1} )
	else()
		set_property( TARGET vgui PROPERTY IMPORTED_LOCATION ${VGUI1} )
	endif()
endif()
