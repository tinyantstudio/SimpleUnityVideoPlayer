cmake_minimum_required(VERSION 3.1...3.17 FATAL_ERROR)

project(nativevideoplayer VERSION 0.0.1)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_HEADERS include/*.h)

source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Headers" FILES ${PROJECT_HEADERS})


if(MSVC)
    set(ffmpeg_include ../ffmpeglib-win/include)
    set(ffmpeg_lib "../ffmpeglib-win/lib")
else()
    set(ffmpeg_include /usr/local/Cellar/ffmpeg/4.4_2/include)
    set(ffmpeg_lib "/usr/local/Cellar/ffmpeg/4.4_2/lib")
endif()

link_directories(ffmpeg_lib)

message("nativevideoplayer  --> ${PROJECT_NAME}")

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_UNITY} ${PROJECT_GL3W} ${PROJECT_RENDER_PLUGIN})

target_link_libraries(${PROJECT_NAME} avcodec avdevice avformat swscale avutil)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})