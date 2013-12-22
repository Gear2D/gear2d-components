find_package(Gear2D REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

g2dcomponent(TYPE audiosource
             FAMILY audiosource
             SOURCES audiosource.cc)

include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(audiosource ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})