cmake_minimum_required(VERSION 3.0.0)
project(minecraft VERSION 0.1.0)

include(CTest)
enable_testing()

set(INCLUDES_FOLDER includes)
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${INCLUDES_FOLDER}/*.cpp")

set(EXTERNAL_FOLDER external)
file(GLOB_RECURSE EXTERNAL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "${EXTERNAL_FOLDER}/*.cpp")

# SET(SOURCE_FILES
#     includes/opengl/Renderer.cpp
#     includes/opengl/IndexBuffer.cpp
#     includes/opengl/VertexBuffer.cpp
#     includes/textures/texture.cpp
#     includes/textures/stb_image.cpp
#     includes/camera/camera.cpp
#     includes/utils/a.cpp
# )


add_executable(minecraft main.cpp ${SOURCE_FILES} ${EXTERNAL_FILES})
find_package(GTest)
find_package(GLEW)
find_package(freeglut)
find_package(SDL2)
find_package(Freetype)
find_package(msgpack)

include_directories("C:/msys64/mingw64/include/freetype2")
message(STATUS ${INCLUDE_DIRECTORIES})

target_link_libraries(minecraft freetype)
target_link_libraries(minecraft gtest)
target_link_libraries(minecraft sdl2)
target_link_libraries(minecraft sdl2_image)
target_link_libraries(minecraft opengl32)
target_link_libraries(minecraft glu32)
target_link_libraries(minecraft glew32)
target_link_libraries(minecraft graphite2)

# target_link_libraries(minecraft freeglut)
# target_link_libraries(minecraft sdl2main)
# target_link_libraries(minecraft mingw32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
