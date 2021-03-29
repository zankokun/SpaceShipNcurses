# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (SpaceShipNcurses)

# Add an executable with the above sources
add_executable(spaceship
${PROJECT_SOURCE_DIR}/src/space.cc
${PROJECT_SOURCE_DIR}/src/Point.cc
${PROJECT_SOURCE_DIR}/src/Stone.cc
)

# uncomment to enable draw debugging
#target_compile_definitions(spaceship
#    PRIVATE
#   DRAWDEBUG
#)

# Ncures preparations
find_package(Curses REQUIRED)

# include needed headers
include_directories(${CURSES_INCLUDE_DIR})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(spaceship
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Link Ncurses lib
target_link_libraries(spaceship ${CURSES_LIBRARIES})