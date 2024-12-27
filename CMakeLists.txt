cmake_minimum_required(VERSION 3.7)
project(minecraft VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 23)

set(ASSET_DIR "${CMAKE_SOURCE_DIR}/res" CACHE STRING "Path to the asset directory")
add_definitions("-DASSET_DIR=\"${ASSET_DIR}\"")

set(INCLUDES_FOLDER includes)
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${INCLUDES_FOLDER}/*.cpp")

set(EXTERNAL_FOLDER external)
file(GLOB_RECURSE EXTERNAL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${EXTERNAL_FOLDER}/*.cpp")

set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(minecraft main.cpp ${SOURCE_FILES} ${EXTERNAL_FILES})
find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(frozen REQUIRED)

message(STATUS ${PROJECT_SOURCE_DIR})

target_include_directories(minecraft PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(minecraft Freetype::Freetype)
target_link_libraries(minecraft SDL2::SDL2)
target_link_libraries(minecraft SDL2_image::SDL2_image)
target_link_libraries(minecraft GLEW::glew)
target_link_libraries(minecraft glm::glm)
target_link_libraries(minecraft frozen::frozen)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
