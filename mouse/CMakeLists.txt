find_package(Gear2D REQUIRED)
g2dcomponent(TYPE mouse SOURCES mouse.cc)
target_link_libraries(mouse ${SDL2_LIBRARY} ${Gear2D_LIBRARY})