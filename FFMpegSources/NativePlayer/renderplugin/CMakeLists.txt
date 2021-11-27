cmake_minimum_required(VERSION 3.1...3.17 FATAL_ERROR)

project(renderplugin VERSION 0.0.1)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

file(GLOB PROJECT_UNITY ../Unity/*.h)
file(GLOB PROJECT_GL3W ../gl3w/*.h ../gl3w/*.c)
file(GLOB PROJECT_RENDER_PLUGIN ./*.h ./*.cpp)

# find OpenGL library 
find_package(OpenGL REQUIRED)

source_group("gl3w" FILES ${PROJECT_GL3W})
source_group("Unity" FILES ${PROJECT_UNITY})

message("renderplugin  --> ${PROJECT_NAME}")

add_library(${PROJECT_NAME} SHARED ${PROJECT_UNITY} ${PROJECT_GL3W} ${PROJECT_RENDER_PLUGIN})
# fill with find_package for OPENGL_LIBRARY
message("renderplugin --> ${OPENGL_LIBRARY}")

if(MSVC)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} nativevideoplayer)
else()
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} "-framework Cocoa -framework OpenGL" nativevideoplayer)
endif()

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
endif()
