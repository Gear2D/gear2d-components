project(gear2d)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
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



include_directories(${Gear2D_INCLUDE_DIR} ${SDL_INCLUDE_DIR})
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

target_link_libraries(mouseover ${Gear2D_LIBRARY})
target_link_libraries(keyboard ${SDL_LIBRARY} ${Gear2D_LIBRARY})
target_link_libraries(mouse ${SDL_LIBRARY} ${Gear2D_LIBRARY})
target_link_libraries(audiosource ${SDL_LIBRARY} SDL_mixer ${Gear2D_LIBRARY})
target_link_libraries(space2d ${Gear2D_LIBRARY})
target_link_libraries(kinematic2d ${Gear2D_LIBRARY})
target_link_libraries(charactercontroller ${Gear2D_LIBRARY})
target_link_libraries(collider2d ${Gear2D_LIBRARY})
target_link_libraries(rigidbody2d ${Gear2D_LIBRARY})
target_link_libraries(singleselect ${Gear2D_LIBRARY})
target_link_libraries(spritesheet ${Gear2D_LIBRARY})


add_subdirectory(renderer)

if (WIN32)
	add_subdirectory(thirdparty/win32)
endif (WIN32)

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
set(CPACK_PACKAGE_VERSION ${Gear2D_VERSION})

set(CPACK_SOURCE_IGNORE_FILES "\\\\.git.*;${CMAKE_BINARY_DIR}/.*;.*~;\\\\.kdev4/.*;.*\\\\.kdev4;.*\\\\.swp")
set(PACK_PACKAGE_EXECUTABLES gear2d;Gear2D Game Engine)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)


set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
if(NOT WIN32)
	set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
endif(NOT WIN32)

include(CPack)
