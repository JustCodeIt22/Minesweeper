cmake_minimum_required(VERSION 3.10)
project(Minesweeper VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)


include_directories("C:/SFML/include")
include_directories(${CMAKE_SOURCE_DIR}/include)
# include_directories(${CMAKE_SOURCE_DIR}/imgui)

link_libraries("C:/SFML/lib")

set(SFML_DIR "C:/SFML/lib/cmake/SFML")

set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5 COMPONENTS system window graphics audio network REQUIRED)
find_package(Freetype REQUIRED)

add_definitions(-DSFML_STATIC)
file(GLOB_RECURSE SRC_FILES src/*.cpp) # imgui/*.cpp)

add_executable(Minesweeper ${SRC_FILES})

target_link_libraries(Minesweeper 
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
    opengl32
    winmm
    user32
    freetype
)
