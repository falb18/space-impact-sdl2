cmake_minimum_required(VERSION 3.27.0)
project(space-impact VERSION 0.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Add SDL2 and SDL2_image libraries
find_package(SDL2 REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Main application
add_executable(
    ${PROJECT_NAME}
    "main.c"
    "graphics.c"
    "font.c"
    "enemies.c"
    "saves.c"
    "scenery.c"
    "shotlist.c"
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})