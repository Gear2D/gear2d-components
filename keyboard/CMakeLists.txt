find_package(Gear2D REQUIRED)
gear2d_component(keyboard keyboard keyboard.cc)
target_link_libraries(keyboard ${SDL2_LIBRARY} ${Gear2D_LIBRARY})