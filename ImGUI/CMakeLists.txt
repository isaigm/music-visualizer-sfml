include_directories(${PROJECT_SOURCE_DIR}/ImGUI/include)
add_library(ImGUI SHARED src/imgui.cpp src/imgui_draw.cpp src/imgui-SFML.cpp src/imgui_tables.cpp src/imgui_widgets.cpp)
