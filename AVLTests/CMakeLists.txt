cmake_minimum_required(VERSION 3.15)
project(avl_test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../extern/googletest"
                 "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
                 EXCLUDE_FROM_ALL)

add_executable(avl_test
                main_test.cpp
                test_basic.cpp
                test_balance.cpp
                test_traverse.cpp
                test_serialize.cpp
                test_iterator.cpp
                test_subtree.cpp)

target_link_libraries(avl_test 
                      gtest_main
                      array_sequence)

target_include_directories(avl_test PRIVATE 
                           ${CMAKE_CURRENT_SOURCE_DIR}/..
                           ${CMAKE_CURRENT_SOURCE_DIR}/../AVL
                           ${CMAKE_CURRENT_SOURCE_DIR}/../base_structures)
set_target_properties(avl_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})