project(gear2d)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" "${CMAKE_SOURCE_DIR}")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif (NOT CMAKE_BUILD_TYPE)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  add_definitions(-DLOGTRACE)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")

include(GetGitRevisionDescription)

git_describe(Gear2D_COMPONENT_VERSION --tags)

if(Gear2D_COMPONENT_VERSION AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in" "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" @ONLY)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
  file(STRINGS VERSION Gear2D_COMPONENT_VERSION)
else()
  set(Gear2D_COMPONENT_VERSION "unknown")
endif()


add_definitions(-DGEAR2D_COMPONENT_VERSION="${Gear2D_COMPONENT_VERSION}")
message(STATUS "Building Gear2D components version ${Gear2D_COMPONENT_VERSION} as ${CMAKE_BUILD_TYPE} mode.")

add_definitions(-std=c++11)
      
# We do not want SDLMain
set(SDL2_BUILDING_LIBRARY 1)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Gear2D REQUIRED)

add_library(space2d MODULE space2d.cc)
add_library(kinematic2d MODULE kinematic2d.cc)
add_library(keyboard MODULE keyboard.cc)
add_library(mouse MODULE mouse.cc)
add_library(mouseover MODULE mouseover.cc)
add_library(charactercontroller MODULE charactercontroller.cc)
add_library(collider2d MODULE collider2d.cc)
add_library(rigidbody2d MODULE rigidbody2d.cc)
add_library(audiosource MODULE audiosource.cc)
add_library(singleselect MODULE singleselect.cc)
add_library(spritesheet MODULE spritesheet.cc)

set(COMPONENT_INSTALL_DIR lib/gear2d/component)

include_directories(${Gear2D_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})
link_directories(${Gear2D_LINK_DIR})

install(TARGETS space2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/spatial
)

install(TARGETS kinematic2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/kinematics
)

install(TARGETS keyboard
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/keyboard
)

install(TARGETS mouse
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/mouse
)

install(TARGETS mouseover
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/mouseover
)

install(TARGETS charactercontroller
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/controller
)

install(TARGETS collider2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/collider
)

install(TARGETS rigidbody2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/dynamics
)

install(TARGETS audiosource
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/audiosource
)

install(TARGETS singleselect
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/menu
)

install(TARGETS spritesheet
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/animation
)

if(WIN32)
	get_filename_component(SDL2_MIXER_LIBRARY_DIR ${SDL2_MIXER_LIBRARY} PATH)
	set(SDL2_MIXER_DLLS
		"${SDL2_MIXER_LIBRARY_DIR}/SDL2_mixer.dll"
#		"${SDL2_MIXER_LIBRARY_DIR}/smpeg.dll"
		"${SDL2_MIXER_LIBRARY_DIR}/libvorbisfile-3.dll"
		"${SDL2_MIXER_LIBRARY_DIR}/libvorbis-0.dll"
		"${SDL2_MIXER_LIBRARY_DIR}/libogg-0.dll"
		"${SDL2_MIXER_LIBRARY_DIR}/libmikmod-2.dll"
		"${SDL2_MIXER_LIBRARY_DIR}/libFLAC-8.dll"
	)
	message(STATUS "Packaging SDL2_mixer DLLs: ${SDL2_MIXER_DLLS}")
	install(FILES ${SDL2_MIXER_DLLS}
		DESTINATION bin
	)
	if(MINGW)
		message(STATUS "Compiling with MINGW!")
		set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread")
		set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -static -lpthread")
		set(CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc -static -lpthread")
	endif(MINGW)
endif (WIN32)

target_link_libraries(mouseover ${Gear2D_LIBRARY})
target_link_libraries(keyboard ${SDL2_LIBRARY} ${Gear2D_LIBRARY})
target_link_libraries(mouse ${SDL2_LIBRARY} ${Gear2D_LIBRARY})
target_link_libraries(audiosource ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY} ${Gear2D_LIBRARY})
target_link_libraries(space2d ${Gear2D_LIBRARY})
target_link_libraries(kinematic2d ${Gear2D_LIBRARY})
target_link_libraries(charactercontroller ${Gear2D_LIBRARY})
target_link_libraries(collider2d ${Gear2D_LIBRARY})
target_link_libraries(rigidbody2d ${Gear2D_LIBRARY})
target_link_libraries(singleselect ${Gear2D_LIBRARY})
target_link_libraries(spritesheet ${Gear2D_LIBRARY})
#add_subdirectory(renderer)
add_subdirectory(renderer2)

add_subdirectory(doc)
if (WIN32)
	set(CPACK_SOURCE_GENERATOR ZIP)
	set(CPACK_GENERATOR NSIS;ZIP)
	set(CPACK_PACKAGE_NAME "Gear2D-Components")
elseif(UNIX)
	set(CPACK_PACKAGE_NAME "gear2d-components")
	set(CPACK_SOURCE_GENERATOR TGZ)
	set(CPACK_GENERATOR STGZ)
endif()

set(CPACK_PACKAGE_VENDOR "Gear2D Labs")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gear2d")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gear2D - Component-based game engine")
set(CPACK_PACKAGE_VERSION "${Gear2D_COMPONENT_VERSION}")

if(WIN32)
	set(CPACK_SOURCE_IGNORE_FILES "VERSION.in;\\\\.git.*;${CMAKE_BINARY_DIR}/.*;.*~;\\\\.kdev4/.*;.*\\\\.kdev4;.*\\\\.swp")
else()
	set(CPACK_SOURCE_IGNORE_FILES "VERSION.in;\\\\.git.*;${CMAKE_BINARY_DIR}/.*;.*~;\\\\.kdev4/.*;.*\\\\.kdev4;.*\\\\.swp;.*thirdparty/win32/.*")
endif(WIN32)

set(PACK_PACKAGE_EXECUTABLES gear2d;Gear2D Game Engine)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)


if (MSVC)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-MSVC-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
else()
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

if(NOT WIN32)
	set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
endif(NOT WIN32)

include(CPack)
