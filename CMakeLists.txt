cmake_minimum_required(VERSION 3.7)
project(minecraft VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)

set(INCLUDES_FOLDER includes)
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${INCLUDES_FOLDER}/*.cpp")

set(EXTERNAL_FOLDER external)
file(GLOB_RECURSE EXTERNAL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${EXTERNAL_FOLDER}/*.cpp")

set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(minecraft main.cpp ${SOURCE_FILES} ${EXTERNAL_FILES})
find_package(OpenGL REQUIRED)
find_package(GTest REQUIRED)
find_package(GLEW REQUIRED)
if (WIN32)
    find_package(Freetype REQUIRED)
else()
    target_include_directories(minecraft PUBLIC /opt/homebrew/include/freetype2)
    target_link_libraries(minecraft /opt/homebrew/lib/libfreetype.dylib)
endif()
find_package(freeglut REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(glm REQUIRED)


message(STATUS ${PROJECT_SOURCE_DIR})

target_include_directories(minecraft PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(minecraft ${FREETYPE_LIBRARIES})
target_link_libraries(minecraft SDL2::SDL2)
target_link_libraries(minecraft SDL2_image::SDL2_image)
if (WIN32)
    target_link_libraries(minecraft glew)
    target_link_libraries(minecraft glm)
    target_link_libraries(minecraft glu32)
else()
    target_link_libraries(minecraft GLEW::glew)
    target_link_libraries(minecraft glm::glm)
endif ()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
