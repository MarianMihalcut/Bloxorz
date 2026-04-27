cmake_minimum_required(VERSION 4.2)
project(Bloxorz_SPG)

set(CMAKE_CXX_STANDARD 20)

set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glew_freeglut/freeglut/freeglut-3.8.0)

add_subdirectory(glm)

include_directories(glew_freeglut/include)
include_directories(glew_freeglut/freeglut/freeglut-3.8.0/include)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h") # Comandă utilă să nu mai scrii manual fiecare fișier
add_executable(${PROJECT_NAME} ${SOURCES}
        src/Levels/Level.h
        src/Levels/Level1.h
        src/Levels/Level2.h
        src/Objects/Tiles/FinishTile.h
        src/Game/Game.cpp
        src/Game/Game.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE FREEGLUT_STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glm::glm
        freeglut_static
        "${CMAKE_CURRENT_SOURCE_DIR}/glew_freeglut/lib/glew32.lib"
        opengl32
        winmm
        gdi32
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/glew_freeglut/bin/glew32.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
