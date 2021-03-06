find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Gear2D REQUIRED)

g2dcomponent(FAMILY renderer
             TYPE renderer2
             SOURCES 
             renderer2.cc renderer2.h renderbase.h renderbase.cc texture.h texture.cc)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(renderer2 ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${GEAR2D_LIBRARY})

if (WIN32)
	get_filename_component(SDL2_IMAGE_LIBRARY_DIR ${SDL2_IMAGE_LIBRARY} PATH)
	set(SDL2_IMAGE_DLLS 
		"${SDL2_IMAGE_LIBRARY_DIR}/SDL2_image.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/zlib1.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libwebp-4.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libtiff-5.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libpng16-16.dll"
		"${SDL2_IMAGE_LIBRARY_DIR}/libjpeg-9.dll"
	)
	message(STATUS "Packaging SDL2_image DLLs: ${SDL2_IMAGE_DLLS}")
	install(FILES ${SDL2_IMAGE_DLLS}
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
