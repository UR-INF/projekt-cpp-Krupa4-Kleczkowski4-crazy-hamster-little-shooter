cmake_minimum_required(VERSION 3.15)
project(little_shooter)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(cmake_modules)
include_directories(images)

add_executable(little_shooter
        cmake_modules/FindSFML.cmake
        images/explosions/type_A.gif
        images/explosions/type_A.png
        images/explosions/type_B.gif
        images/explosions/type_B.png
        images/explosions/type_C.gif
        images/explosions/type_C.png
        images/background.jpg
        images/fire_blue.png
        images/fire_red.png
        images/rock.png
        images/rock_small.png
        images/hamster.png
        CMakeLists.txt
        main.cpp)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(little_shooter ${SFML_LIBRARIES})
endif()