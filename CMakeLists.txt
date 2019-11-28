# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

project ("VoxVR")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

add_subdirectory(${PROJECT_SOURCE_DIR}/glfw-3.3)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/glfw-3.3/include)

add_library(BitMap STATIC ${PROJECT_SOURCE_DIR}/src/BitMap.cpp)

add_library(VoxelWorld STATIC ${PROJECT_SOURCE_DIR}/src/VoxelWorld.cpp)

add_library(vrutil STATIC ${PROJECT_SOURCE_DIR}/src/vrutil.cpp)

add_library(GLSLShader STATIC ${PROJECT_SOURCE_DIR}/src/GLSLShader.cpp)

add_library(Grid STATIC ${PROJECT_SOURCE_DIR}/src/Grid.cpp ${PROJECT_SOURCE_DIR}/src/RenderableObject.cpp)

add_library(tinyfiledialogs STATIC ${PROJECT_SOURCE_DIR}/src/tinyfiledialogs.c)

add_executable (VoxVR ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/src/Application.cpp)

set(OPENVR_ROOT_DIR ${PROJECT_SOURCE_DIR}/openvr-1.7.15)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenVR REQUIRED)

target_include_directories(VoxVR PUBLIC ${OPENGL_INCLUDE_DIRS} ${OPENVR_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_include_directories(vrutil PUBLIC ${OPENGL_INCLUDE_DIRS} ${OPENVR_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_include_directories(VoxelWorld PUBLIC ${OPENGL_INCLUDE_DIRS} ${OPENVR_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_include_directories(GLSLShader PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_include_directories(Grid PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_include_directories(BitMap PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(VoxVR VoxelWorld BitMap GLSLShader Grid vrutil tinyfiledialogs glfw ${OPENGL_LIBRARIES} ${OPENVR_LIBRARY} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES})

add_custom_command(TARGET VoxVR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${OPENVR_ROOT_DIR}/binaries/${OPENVR_PLATFORM}"
        ${CMAKE_BINARY_DIR})

add_custom_command(TARGET VoxVR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        ${CMAKE_BINARY_DIR}/shaders)