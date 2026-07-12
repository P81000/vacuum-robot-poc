cmake_minimum_required(VERSION 3.14)
project(vacuum_sim VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(PkgConfig REQUIRED)

pkg_check_modules(RAYLIB REQUIRED raylib)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

add_executable(mock_brain
    src/mock_brain.cpp
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${RAYLIB_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${RAYLIB_LIBRARIES}
)
