include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ImGUI/include")

include_directories(${PROJECT_SOURCE_DIR}/ImPlot/include)
add_library(ImPlot SHARED src/implot.cpp src/implot_items.cpp)