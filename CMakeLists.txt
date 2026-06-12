cmake_minimum_required(VERSION 3.16)
project(Arkanoid CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

set(SOURCES
    src/main.cpp
    src/Ball.cpp
    src/Paddle.cpp
    src/Block.cpp
    src/Bonus.cpp
    src/Game.cpp
)

add_executable(arkanoid ${SOURCES})

target_include_directories(arkanoid PRIVATE include)

target_link_libraries(arkanoid
    sfml-graphics
    sfml-window
    sfml-system
)
