find_package(Gear2D REQUIRED)
find_package(SDL2 REQUIRED)

g2dcomponent(TYPE keyboard SOURCES keyboard.cc)

target_link_libraries(keyboard ${SDL2_LIBRARY} ${Gear2D_LIBRARY})
include_directories(${SDL2_INCLUDE_DIR})