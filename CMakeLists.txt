cmake_minimum_required(VERSION 3.8)
project(MusicVisualizer)


find_package(SFML 2.5 REQUIRED COMPONENTS graphics system window audio)

set(SFML_LIBS sfml-graphics sfml-system sfml-window sfml-audio) 

file(GLOB SOURCES src/*.cpp)


add_executable(${PROJECT_NAME} ${SOURCES})
add_subdirectory(ImGUI)
add_subdirectory(ImPlot)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-system sfml-window sfml-audio fftw3 GL ImGUI ImPlot)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/ImGUI/include ${CMAKE_CURRENT_LIST_DIR}/ImPlot/include)

