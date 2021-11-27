cmake_minimum_required(VERSION 3.0)

project(SimpleVideoPlayerForUnity)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message("on Mac OS !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	message("on Linux !")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message("on Windows !")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

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

include_directories(${ffmpeg_include})
link_directories(${ffmpeg_lib})

include_directories(Unity)
include_directories(videoplayer/include)
include_directories(gl3w)

add_subdirectory(renderplugin)
add_subdirectory(videoplayer)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(${PROJECT_NAME} renderplugin)
target_link_libraries(${PROJECT_NAME} nativevideoplayer)
target_link_libraries(${PROJECT_NAME} avcodec avdevice avformat swscale avutil)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# copy ffmpeglib dll to Run Path

if(MSVC)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../ffmpeglib-win/bin $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    # copy nativevideoplayer dll to exe folder
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/nativevideoplayer/$<CONFIGURATION> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

    # copy renderplugin dll to exe folder
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/renderplugin/$<CONFIGURATION> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()