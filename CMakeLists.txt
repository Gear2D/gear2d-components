project(gear2d-components)
cmake_minimum_required(VERSION 2.6)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)

find_path(GEAR2D_INCLUDE_DIR gear2d.h
	PATHS
	/usr/include/gear2d
	/usr/local/include/gear2d
	/opt/include/gear2d
)
if (WIN32)
	find_path(
		GEAR2D_LINK_DIR libgear2d.dll
		PATHS
		"C:\\Program Files\\Gear2D\\bin"
		"C:\\Program Files (x86)\\Gear2D\\bin"
		"C:\\Gear2D\\bin"
	)
	
	find_path(GEAR2D_INCLUDE_DIR gear2d.h
	  PATHS
		"C:\\Program Files\\Gear2D\\include"
		"C:\\Program Files (x86)\\Gear2D\\include"
		"C:\\Gear2D\\include"
	)
	# TODO: Add home folders?
elseif (APPLE)
	# TODO: Where does mac stores it?
	find_path(GEAR2D_LINK_DIR libgear2d.dylib)
else ()
	find_path(GEAR2D_LINK_DIR libgear2d.so
		/usr/lib
		/usr/local/lib
		/opt/lib
	)
endif(WIN32)
			
message("Gear2D include dir: ${GEAR2D_INCLUDE_DIR}")
message("Gear2D libdir: ${GEAR2D_LINK_DIR}")

include_directories(${GEAR2D_INCLUDE_DIR})
link_directories(${GEAR2D_LINK_DIR})

add_library(space2d MODULE space2d.cc)
add_library(kinematic2d MODULE kinematic2d.cc)
add_library(renderer MODULE renderer.cc)
add_library(keyboard MODULE keyboard.cc)
add_library(mouse MODULE mouse.cc)
add_library(mouseover MODULE mouseover.cc)
add_library(charactercontroller MODULE charactercontroller.cc)
add_library(collider2d MODULE collider2d.cc)
add_library(rigidbody2d MODULE rigidbody2d.cc)
add_library(audiosource MODULE audiosource.cc)
add_library(singleselect MODULE singleselect.cc)
add_library(spritesheet MODULE spritesheet.cc)

set (COMPONENT_INSTALL_DIR lib/gear2d/component)

include_directories(${SDL_INCLUDE_DIR})

install(TARGETS space2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/spatial
)

install(TARGETS kinematic2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/kinematics
)

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
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

target_link_libraries(mouseover gear2d)
target_link_libraries(keyboard SDL gear2d)
target_link_libraries(mouse SDL gear2d)
target_link_libraries(renderer SDL SDL_image SDL_ttf SDL_gfx gear2d)
target_link_libraries(audiosource SDL SDL_mixer gear2d)
target_link_libraries(space2d gear2d)
target_link_libraries(kinematic2d gear2d)
target_link_libraries(charactercontroller gear2d)
target_link_libraries(collider2d gear2d)
target_link_libraries(rigidbody2d gear2d)
target_link_libraries(singleselect gear2d)
target_link_libraries(spritesheet gear2d)