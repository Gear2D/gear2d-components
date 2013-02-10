find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Gear2D REQUIRED)

add_library(renderer MODULE renderer.cc SDL_rotozoom.c)

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
)

target_link_libraries(renderer ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARY} ${SDL_TTF_LIBRARY} ${Gear2D_LIBRARY})

if (WIN32)
	get_filename_component(SDL_IMAGE_LIBRARY_DIR ${SDL_IMAGE_LIBRARY} PATH)
	set(SDL_IMAGE_DLLS 
		"${SDL_IMAGE_LIBRARY_DIR}/SDL_image.dll"
		"${SDL_IMAGE_LIBRARY_DIR}/zlib1.dll"
		"${SDL_IMAGE_LIBRARY_DIR}/libwebp-2.dll"
		"${SDL_IMAGE_LIBRARY_DIR}/libtiff-5.dll"
		"${SDL_IMAGE_LIBRARY_DIR}/libpng15-15.dll"
		"${SDL_IMAGE_LIBRARY_DIR}/libjpeg-8.dll"
	)
	message(STATUS "Packaging SDL_image DLLs: ${SDL_IMAGE_DLLS}")
	install(FILES ${SDL_IMAGE_DLLS}
		DESTINATION bin
	)
	
	get_filename_component(SDL_TTF_LIBRARY_DIR ${SDL_TTF_LIBRARY} PATH)
	set(SDL_TTF_DLLS 
		"${SDL_TTF_LIBRARY_DIR}/SDL_ttf.dll"
		"${SDL_TTF_LIBRARY_DIR}/zlib1.dll"
		"${SDL_TTF_LIBRARY_DIR}/libfreetype-6.dll"
	)
	message(STATUS "Packaging SDL_ttf DLLs: ${SDL_TTF_DLLS}")
	install(FILES ${SDL_TTF_DLLS}
		DESTINATION bin
	)
endif (WIN32)