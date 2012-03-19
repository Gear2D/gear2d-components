find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Freetype REQUIRED)

add_library(glrenderer MODULE glrenderer.cc gltext.cc resize++.cpp)


install(TARGETS glrenderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
)

include_directories(${FREETYPE_INCLUDE_DIRS})

set(GLEW "GLEW")
set(GLU "GLU")
set(GLUT "glut")
set(FTGL_LIBRARY "ftgl")
if (WIN32)
	set(GLEW "glew32.lib")
	set(GLU "glu32.lib")
	set(GLUT "freeglut")
	if("${OpenGL_LIBRARY}" STREQUAL "")
		set(OpenGL_LIBRARY "opengl32")
	endif("${OpenGL_LIBRARY}" STREQUAL "")
endif(WIN32)

target_link_libraries(glrenderer SDL SDL_image SDL_ttf ${Gear2D_LIBRARY} ${GLEW} ${GLU} ${GLUT} ${OpenGL_LIBRARY} ${FTGL_LIBRARY})