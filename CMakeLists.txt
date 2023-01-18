cmake_minimum_required(VERSION 3.0.0)
project(minecraft VERSION 0.1.0)

include(CTest)
enable_testing()

set(INCLUDES_FOLDER includes)
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${INCLUDES_FOLDER}/*.cpp")

add_executable(minecraft main.cpp ${SOURCE_FILES})
find_package(GTest)
find_package(GLEW)
find_package(GLUT)
message(STATUS ${SFML_LIBRARIES})

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(minecraft gtest)
target_link_libraries(minecraft glew32)
target_link_libraries(minecraft glu32)
target_link_libraries(minecraft opengl32)
target_link_libraries(minecraft freeglut)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
