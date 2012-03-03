find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Gear2D REQUIRED)

add_library(renderer MODULE renderer.cc SDL_rotozoom.c)

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
)

target_link_libraries(renderer SDL SDL_image SDL_ttf ${Gear2D_LIBRARY})