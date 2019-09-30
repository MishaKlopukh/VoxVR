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

add_subdirectory(${PROJECT_SOURCE_DIR}/glfw-3.3)
add_subdirectory(${PROJECT_SOURCE_DIR}/openvr-1.7.15)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/glfw-3.3/include ${PROJECT_SOURCE_DIR}/openvr-1.7.15/headers)

add_executable (VoxVR ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/src/Application.cpp)

find_package(OpenGL REQUIRED)
target_include_directories(VoxVR PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(VoxVR ${LIBNAME} glfw ${OPENGL_LIBRARIES})