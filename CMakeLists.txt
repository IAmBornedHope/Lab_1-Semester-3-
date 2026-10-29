cmake_minimum_required(VERSION 3.20)
project(Lab_1_Semester_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(base_structures/dynamic_array)
add_subdirectory(base_structures/array_sequence)
add_subdirectory(base_structures/exceptions)
add_subdirectory(base_structures/sequence)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE
    array_sequence
)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/AVL
    ${CMAKE_SOURCE_DIR}/base_structures
)

add_subdirectory(AVLTests)