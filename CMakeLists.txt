project(gear2d)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/" "${CMAKE_SOURCE_DIR}")
#set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif (NOT CMAKE_BUILD_TYPE)

message(STATUS ${CMAKE_PREFIX_PATH} ---- ${CMAKE_MODULE_PATH})

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

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  add_definitions(-DLOGTRACE)
  message(STATUS "Tracing will be enabled fo gear2d-components")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")


add_definitions(-std=c++11)
      
# We do not want SDLMain
set(SDL2_BUILDING_LIBRARY 1)

find_package(Gear2D REQUIRED)

set(COMPONENT_INSTALL_DIR lib/gear2d/component)
include_directories(${Gear2D_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})
link_directories(${Gear2D_LINK_DIR})

if(WIN32)
  get_filename_component(SDL2_MIXER_LIBRARY_DIR ${SDL2_MIXER_LIBRARY} PATH)
  set(SDL2_MIXER_DLLS
    "${SDL2_MIXER_LIBRARY_DIR}/SDL2_mixer.dll"
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
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread -dynamic")
    set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -static -lpthread -dynamic")
    set(CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc -static -lpthread -dynamic")
  endif(MINGW)
endif (WIN32)

# All component subdirs
add_subdirectory(audiosource)
add_subdirectory(charactercontroller)
add_subdirectory(collider2d)
add_subdirectory(keyboard)
add_subdirectory(kinematic2d)
add_subdirectory(mouse)
add_subdirectory(mouseover)
#add_subdirectory(renderer)
add_subdirectory(renderer2)
add_subdirectory(rigidbody2d)
add_subdirectory(singleselect)
add_subdirectory(space2d)
add_subdirectory(spritesheet)
add_subdirectory(touch)

add_subdirectory(doc)

# cpack stuff
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
