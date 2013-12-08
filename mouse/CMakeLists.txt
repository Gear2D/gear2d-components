find_package(Gear2D REQUIRED)
gear2d_component(mouse mouse mouse.cc)
target_link_libraries(mouse ${SDL2_LIBRARY} ${Gear2D_LIBRARY})