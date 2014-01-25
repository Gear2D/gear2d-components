find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Gear2D REQUIRED)

add_library(renderer MODULE renderer.cc SDL_rotozoom.c)

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
)

target_link_libraries(renderer ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${GEAR2D_LIBRARY})

if (WIN32)
	get_filename_component(SDL2_IMAGE_LIBRARY_DIR ${SDL2_IMAGE_LIBRARY} PATH)
	set(SDL2_IMAGE_DLLS 
		"${SDL2_IMAGE_LIBRARY_DIR}/SDL_image.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/zlib1.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libwebp-2.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libtiff-5.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libpng15-15.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libjpeg-8.dll"
	)
	message(STATUS "Packaging SDL2_image DLLs: ${SDL2_IMAGE_DLLS}")
	install(FILES ${SDL_IMAGE_DLLS}
		DESTINATION bin
	)
	
	get_filename_component(SDL2_TTF_LIBRARY_DIR ${SDL2_TTF_LIBRARY} PATH)
	set(SDL2_TTF_DLLS 
		"${SDL2_TTF_LIBRARY_DIR}/SDL2_ttf.dll"
		"${SDL2_TTF_LIBRARY_DIR}/zlib1.dll"
		"${SDL2_TTF_LIBRARY_DIR}/libfreetype-6.dll"
	)
	message(STATUS "Packaging SDL2_ttf DLLs: ${SDL2_TTF_DLLS}")
	install(FILES ${SDL2_TTF_DLLS}
		DESTINATION bin
	)
endif (WIN32)