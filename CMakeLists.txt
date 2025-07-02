cmake_minimum_required(VERSION 3.8)
project(MusicVisualizer)


find_package(SFML 3.0.0 REQUIRED COMPONENTS Graphics System Window Audio)
set(SFML_LIBS sfml-graphics sfml-system sfml-window sfml-audio) 

file(GLOB SOURCES src/*.cpp)


add_executable(${PROJECT_NAME} ${SOURCES})
add_subdirectory(ImGUI)
add_subdirectory(ImPlot)


# --- INICIO DEL BLOQUE PARA COPIAR ASSETS ---

# Define la ruta de origen y de destino
set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/assets")

# Añade un comando personalizado que se ejecuta después de construir el ejecutable
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${ASSETS_SRC_DIR}"
            "${ASSETS_DEST_DIR}"
    COMMENT "Copiando assets al directorio de compilación..."
)

# --- FIN DEL BLOQUE ---


target_link_libraries(
  ${PROJECT_NAME}
  SFML::Graphics
  SFML::System
  SFML::Window
  SFML::Audio
  fftw3 GL ImGUI ImPlot
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/ImGUI/include ${CMAKE_CURRENT_LIST_DIR}/ImPlot/include)