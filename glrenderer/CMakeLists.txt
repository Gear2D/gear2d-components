find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)

add_library(glrenderer MODULE glrenderer.cc resize++.cpp)

install(TARGETS glrenderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
)

target_link_libraries(glrenderer SDL SDL_image SDL_ttf GLEW GLU GL gear2d)