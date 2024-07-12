cmake_minimum_required(VERSION 3.20)

project(
    SECURITY_SYSTEM
    VERSION 1.0.0
    LANGUAGES C CXX
)

set(SOURCES
    main.cpp
    src/alert.cpp
    src/camera.cpp
    src/detect_person.cpp
    src/select_area.cpp
    src/main_window.cpp
)

include_directories(include)

# include OpenCV library
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV Version: " ${OpenCV_VERSION})
include_directories(${OpenCV_INCLUDE_DIRS})

# irrKlang Library
include_directories(lib/irrKlang/include)
set(IRRKLANG_DLL_PATH ${CMAKE_SOURCE_DIR})

# adding ncnn header files
include_directories(
    C:/ncnn/src 
    C:/ncnn/build/src
)

# linking options for ncnn library
add_link_options(-fopenmp -s -lpthread)

# adding glfw library
set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")
add_subdirectory(lib/glfw)
include_directories(${CMAKE_SOURCE_DIR}/lib/glfw/include/)

# adding OpenGL Library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# adding glew library
include_directories(${CMAKE_SOURCE_DIR}/lib/glew/include/)
find_library(GLEWLIB libglew32 ${CMAKE_SOURCE_DIR}/lib/glew/lib/)

# adding ImGui Library
add_subdirectory(lib/imgui)
include_directories(${CMAKE_SOURCE_DIR}/lib/imgui)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    ${IRRKLANG_DLL_PATH}/lib/irrKlang/bin/win32-gcc/irrKlang.dll
    Wininet
    ${OpenCV_LIBS}
    C:/ncnn/build/src/libncnn.a
    ${OPENGL_LIBRARIES}
    ${GLEWLIB}
    glfw
    imgui
)