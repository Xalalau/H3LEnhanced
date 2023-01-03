###################################################
#                                                 #
#   // ############ hu3lifezado ############ //   #
#            Hu3-Life CMake build file            #
#                // ############ //               #
#                                                 #
###################################################
cmake_minimum_required( VERSION 3.6 )

# Source additional modules from the "cmake" directory
list( APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_LIST_DIR}/cmake"
)

include( InputFilesList )
include( MSVCRuntime )
include( WinXPSupport )
include( Ternary )
############ hu3lifezado ############
# (Xalalau)
# Adicionado: pastas do Hu3-Life
#
# (R4to0)
# Removido parâmetro do C++ que estava sendo usado no compilador C. Corrige "invalid argument '-std=c++1y' not allowed with 'C/ObjC'" no Mac OSX.
# Removido a flag -g do compilador em modo release. Isso estava deixando as bibiliotecas com tamanho superior a 200MB!!!
# A flag -g agora é usada apenas quando compilado em modo DEBUG.
# Adicionado -m32 na exceção do Mac OSX por questões de segurança (este estava definido no SHARED_COMPILER_FLAGS que não é usado aqui por causa do -std=c++1y.
# Habilitado o CMAKE_SKIP_RPATH para que nãp grave caminhos fixos no linker (procura bibliotecas linkadas no $LD_LIBRARY_PATH).
# Melhorias nos comentários.
#
# Agradecimentos ao Duggles (Sven Co-op Team) por apontar o problema com o parâmetro -g, que fazia os .so ficarem com tamanho absurdo (Thanks a lot Duggles!).
project( Hu3-Life )

# Remove caminhos de diretorios no rpath
# https://cmake.org/cmake/help/latest/variable/CMAKE_SKIP_RPATH.html
set( CMAKE_SKIP_RPATH true )

#C++14 support
set( SHARED_COMPILER_FLAGS )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" )
	# Sempre compilar em modo 32 bits (-m32)
	set( SHARED_COMPILER_FLAGS "${SHARED_COMPILER_FLAGS} -m32" )

	# Não usar -g em modo release
	# Isso faz com que os binários compilados fiquem com tamanho muito grande!
	if( CMAKE_BUILD_TYPE MATCHES DEBUG)
		set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g" )
	endif()
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SHARED_COMPILER_FLAGS}" )

# No Mac OSX, o compilador C não aceita o parâmetro -std=c++1y que está no SHARED_COMPILER_FLAGS,
# então definimos os outros parâmetros manaulmente aqui sem este.
if( APPLE )
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32" )
else()
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SHARED_COMPILER_FLAGS}" )
endif()
############

unset( SHARED_COMPILER_FLAGS )

#Get the steam/steamapps/common directory
set( STEAMCOMMON "$ENV{SteamCommon}" CACHE PATH "Steam common directory path" )

if( "${STEAMCOMMON}" STREQUAL "" )

	message( FATAL_ERROR "Unable to get Steam common directory path" )

endif()

#Base dir for game outputs
set( GAME_BIN_PATH "${STEAMCOMMON}/Half-Life" )
############ hu3lifezado ############
set( GAME_BASE_PATH "${GAME_BIN_PATH}/Hu3-Life" )
############

message( STATUS "Base Game Path: ${GAME_BASE_PATH}" )

#Workaround to let the compiler find vgui.so and others without embedding absolute paths as library dependencies
link_directories( ${GAME_BIN_PATH} )

set( EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external )

option( USE_OPFOR "Whether to include Opposing Force related stuff" )
option( USE_VGUI2 "Whether to include VGUI2 features" )

#Some libraries that we use don't come with .a files (import libraries) for Cygwin compilation (Unix Makefiles on Windows).
#This isn't really supported, and since we only use Makefiles on Windows for the compile_commands.json file right now this isn't really an issue.
#Should we choose to support Cygwin we will need the import libraries though.
option( CLANG_DB_BUILD "Whether this is a Clang database build. If enabled, some settings are disabled in order to function on Windows" )

ternary( USE_OPFOR_DEFINE USE_OPFOR 1 0 )
ternary( USE_VGUI2_DEFINE USE_VGUI2 1 0 )

if( CLANG_DB_BUILD )
	message( STATUS "Exporting Compile Commands" )
	set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
endif()

#Shared preprocessor definitions
set( SHARED_DEFS 
)

if( MSVC )
	set( SHARED_DEFS
		${SHARED_DEFS}
		_CRT_SECURE_NO_WARNINGS
		_SCL_SECURE_NO_WARNINGS
	)
else()
	#All non-MSVC targets need this. That includes Cygwin and Mingw.
	#Match Source 2013 in defining GNUC for UNIX platforms.
	#Match Source 2013 in defining POSIX for UNIX platforms.
	set( SHARED_DEFS
		${SHARED_DEFS}
		GNUC
		POSIX
	)

	if( APPLE )
		#Define OSX so code can check for it.
		set( SHARED_DEFS
			${SHARED_DEFS}
			OSX
		)
	endif()

	if( UNIX )
		#Define LINUX so code can check for it.
		#Define _LINUX for Source SDK dependencies.
		set( SHARED_DEFS
			${SHARED_DEFS}
			LINUX
			_LINUX
		)
	endif()
endif()

#Shared definitions for the client & server libaries.
set( SHARED_GAME_DEFS
	CLIENT_WEAPONS
	NO_MALLOC_OVERRIDE
	USE_OPFOR=${USE_OPFOR_DEFINE}
)

#Shared linker flags
set( SHARED_GAME_LINKER_FLAGS
)

if( MSVC )
	#Set Windows subsystem
	set( SHARED_GAME_LINKER_FLAGS
		${SHARED_GAME_LINKER_FLAGS} "/SUBSYSTEM:WINDOWS "
)
elseif( UNIX )
	#From the Github 2013 Makefile, match the settings for Unix environments.
	# force 387 for FP math so the precision between win32 and linux and osx match
	# Trigger an error if any code tries to use an implicit return type
	# Default visibility is hidden unless explicitly altered with __attribute__( visibility() )
	set( SHARED_GAME_LINKER_FLAGS
		${SHARED_GAME_LINKER_FLAGS} "-mfpmath=387 -Werror=return-type -fvisibility=hidden "
	)

	############ hu3lifezado ############
	# (R4to0)
	# Removido parâmetros "-momit-leaf-frame-pointer" e "-mtune=core2" no Mac OSX. Corrige "clang: error: no such file or directory: ';<nome do parametro>'"
	# TODO - Investigate why this fails on Mac (JoelTroch)
	#if( APPLE )
	#	set( SHARED_GAME_LINKER_FLAGS
	#		${SHARED_GAME_LINKER_FLAGS} "-momit-leaf-frame-pointer -mtune=core2 "
	#	)
	#else()
	#	#Linux, Cygwin, etc.
	#endif()
	############
endif()

#Highest warning level and warnings as errors
if( MSVC )
	set( WARNING_LEVEL_STRICTEST "/W4 /WX " )
else()
	set( WARNING_LEVEL_STRICTEST "-Wall -Werror " )
endif()

#Shared directories
#Include paths for external dependencies. Should always come after all project paths.
set( SHARED_EXTERNAL_INCLUDE_PATHS
)

set( SHARED_INCLUDE_PATHS
	common
	public
	public/math
	engine
	pm_shared
	game_shared
	game/shared
	game/shared/entities/player
	game/server
	shared/engine
	source_sdk/public/tier1
	source_sdk/public
	source_sdk/common
	${CMAKE_SOURCE_DIR}  #Should ideally get rid of this.
)

add_subdirectory( common )
add_subdirectory( engine )
add_subdirectory( game/shared )
add_subdirectory( pm_shared )
add_subdirectory( public )
add_subdirectory( shared/engine )

#Save the shares sources into SHARED_SRCS
get_sources( SHARED_SRCS )

clear_sources()

#Find shared dependencies

add_library( spdlog INTERFACE IMPORTED )

set_property( TARGET spdlog PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EXTERNAL_DIR}/spdlog/include )

if( WIN32 )
	#Only do this for Windows builds, for Linux we need to rely on a symlink so the compiler can find it without embedding an absolute path
	set( tier0_DIR ${GAME_BIN_PATH} CACHE PATH "tier0 library location" )
	find_package( tier0 MODULE REQUIRED )

	set( vstdlib_DIR ${GAME_BIN_PATH} CACHE PATH "vstdlib library location" )
	find_package( vstdlib MODULE REQUIRED )
endif()

set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )

if( UNIX AND NOT APPLE )
	set( UNIX_FS_LIB stdc++fs )
endif()

set( SHARED_DEPENDENCIES
	tier0
	vstdlib
	tier1
	spdlog
)

#Set up source_sdk projects first
add_subdirectory( source_sdk )

#
#Client library
#

#Add in the shared sources
add_sources( "${SHARED_SRCS}" )

#Add in the client directories
add_subdirectory( game/client )
add_subdirectory( vgui_utils )
add_subdirectory( shared/engine/client )
add_subdirectory( source_sdk/common )
add_subdirectory( source_sdk/public )

preprocess_sources()

add_library( client SHARED ${PREP_SRCS} )

#Needs static CRT to avoid alloc issues.
configure_msvc_runtime( client STATIC )

check_winxp_support( client )

target_include_directories( client PRIVATE
	shared/engine/client
	vgui_utils
	game/client
	game/client/ui
	game/client/ui/hud
	game/client/ui/vgui
	${CMAKE_SOURCE_DIR}/utils/vgui/include
	${SHARED_INCLUDE_PATHS}
	${SHARED_EXTERNAL_INCLUDE_PATHS}
)

target_compile_definitions( client PRIVATE
	${SHARED_DEFS}
	${SHARED_GAME_DEFS}
	CLIENT_DLL
	USE_VGUI2=${USE_VGUI2_DEFINE}
)

#Find client dependencies
if( WIN32 )
	#Only do this for Windows builds, for Linux we need to rely on a symlink so the compiler can find it without embedding an absolute path
	set( vgui_DIR ${GAME_BIN_PATH} CACHE PATH "VGUI1 shared library location" )
	find_package( vgui MODULE REQUIRED )
endif()

#Create a symlink so vgui can be found using -lvgui
if( CMAKE_HOST_UNIX )
	if( NOT EXISTS ${GAME_BIN_PATH}/libvgui.so )
		EXECUTE_PROCESS(COMMAND ln -s vgui.so libvgui.so
			WORKING_DIRECTORY ${GAME_BIN_PATH}
		)
	endif()
endif()

if( NOT CLANG_DB_BUILD )
	set( SDL2_DIR ${GAME_BIN_PATH} CACHE PATH "SDL2 shared library location" )
	find_package( SDL2 MODULE REQUIRED )
endif()

if( USE_VGUI2 )
	set( VGUI2_LIBS
		vgui2_controls
	)
else()
	set( VGUI2_LIBS "" )
endif()

#Set the list of platform specific libraries
if( WIN32 )
	set( CLIENT_PLAT_LIBS "wsock32" )
else()
	set( CLIENT_PLAT_LIBS "dl" )
endif()

target_link_libraries( client 
	${DELAYIMP}
	vgui
	${VGUI2_LIBS}
	${CLIENT_PLAT_LIBS}
	SDL2
	${SHARED_DEPENDENCIES}
	Threads::Threads
	${UNIX_FS_LIB}
)

#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
set_target_properties( client PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${GAME_BASE_PATH}/cl_dlls"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GAME_BASE_PATH}/cl_dlls"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GAME_BASE_PATH}/cl_dlls"
)

set( CLIENT_LINK_FLAGS )

if( MSVC )
	set( CLIENT_LINK_FLAGS "${CLIENT_LINK_FLAGS} " )
elseif( NOT APPLE )
	#Generate a map file that provides information about the linking stage.
	set( CLIENT_LINK_FLAGS
		${CLIENT_LINK_FLAGS} "-Wl,-Map,client_map.txt "
	)
endif()

set_target_properties( client PROPERTIES
	COMPILE_FLAGS "${WARNING_LEVEL_STRICTEST}"
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${CLIENT_LINK_FLAGS}"
)

#No lib prefix
set_target_properties( client PROPERTIES PREFIX "" )

#Create filters
create_source_groups( "${CMAKE_SOURCE_DIR}" )

#Set the startup project to the client
#TODO: does this even work?
set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT client )

clear_sources()

#
#End client library
#

#
#Server library
#

#Add in the shared sources
add_sources( "${SHARED_SRCS}" )

#Add in the server directories
add_subdirectory( game/server )

preprocess_sources()

add_library( hl SHARED ${PREP_SRCS} )

#Needs static CRT to avoid alloc issues.
configure_msvc_runtime( hl STATIC )

check_winxp_support( hl )

target_include_directories( hl PRIVATE
	${SHARED_INCLUDE_PATHS}
	${SHARED_EXTERNAL_INCLUDE_PATHS}
)

target_compile_definitions( hl PRIVATE
	${SHARED_DEFS}
	${SHARED_GAME_DEFS}
	SERVER_DLL
)

#Copy the libraries first so they can be found by the code below.
set( LIBS_TO_COPY )

foreach( lib IN LISTS LIBS_TO_COPY )
	message( STATUS "Copying library \"${lib}\"" )
	file( COPY ${lib} DESTINATION ${GAME_BASE_PATH} )
endforeach()

#Set the list of platform specific libraries
if( WIN32 )
	set( SERVER_PLAT_LIBS Ws2_32 Shlwapi )
else()
	set( SERVER_PLAT_LIBS dl )
endif()

target_link_libraries( hl 
	${DELAYIMP}
	Threads::Threads
	${SERVER_PLAT_LIBS}
	${SHARED_DEPENDENCIES}
	${UNIX_FS_LIB}
)

#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
set_target_properties( hl PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${GAME_BASE_PATH}/dlls"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GAME_BASE_PATH}/dlls"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GAME_BASE_PATH}/dlls"
)

set( SERVER_LINK_FLAGS )

if( MSVC )
	#Copy the def file so it's local to the binary directory
	file( COPY ${CMAKE_SOURCE_DIR}/hl.def DESTINATION ${CMAKE_BINARY_DIR} )

	#Add the module definition file. Needed for GiveFnptrsToDll function export.
	#Add delay loaded libraries.
	set( SERVER_LINK_FLAGS "/DEF:hl.def ${SQL_DLL_DELAYLOAD}" )
else()
	if( NOT APPLE )
		#Generate a map file that provides information about the linking stage.
		set( SERVER_LINK_FLAGS
			${SERVER_LINK_FLAGS} "-Wl,-Map,hl_map.txt "
		)
	endif()
endif()

#SQL libraries are delay loaded to keep them in the game directory.
set_target_properties( hl PROPERTIES
	COMPILE_FLAGS "${WARNING_LEVEL_STRICTEST}" 
	LINK_FLAGS "${SHARED_GAME_LINKER_FLAGS} ${SERVER_LINK_FLAGS}"
)

#No lib prefix
set_target_properties( hl PROPERTIES PREFIX "" )

#Create filters
create_source_groups( "${CMAKE_SOURCE_DIR}" )

clear_sources()

#
#End server library
#
