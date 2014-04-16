find_package(Gear2D REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

g2dcomponent(TYPE audiosource
             FAMILY audiosource
             SOURCES audiosource.cc)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})
target_link_libraries(audiosource ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})

# On Win32, package SDL2 mixer dlls
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
endif()