cmake_minimum_required(VERSION 4.2)
project(Bloxorz_SPG)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(glew_freeglut/freeglut/freeglut-3.8.0)

include_directories(glew_freeglut/include)

include_directories(glew_freeglut/freeglut/freeglut-3.8.0/include)

add_executable(${PROJECT_NAME} src/main.cpp
        src/Interfaces/ObjectModelInterface.h
        src/Objects/Cuboid.h
        src/Interfaces/Tile.h
        src/Objects/ConcreteTile.h
        src/Objects/BridgeTile.h
        src/Objects/ButtonTile.h
)

target_link_libraries(${PROJECT_NAME}
        freeglut_static
        opengl32
        "${CMAKE_CURRENT_SOURCE_DIR}/glew_freeglut/lib/glew32.lib"
)
