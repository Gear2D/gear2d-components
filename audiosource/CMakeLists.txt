find_package(Gear2D REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

gear2d_component(audiosource audiosource audiosource.cc)
target_link_libraries(audiosource ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})