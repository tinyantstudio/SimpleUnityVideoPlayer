cmake_minimum_required(VERSION 3.0)

project(NativePlayer)

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

include_directories(../ffmpeglib-win/include)
link_directories("../ffmpeglib-win/lib")

include_directories(videoplayer/include)

add_subdirectory(videoplayer)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_link_libraries(${PROJECT_NAME} videoPlayerLib)

target_link_libraries(${PROJECT_NAME} avcodec avdevice avformat swscale avutil)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# copy ffmpeglib dll to Run Path
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../ffmpeglib-win/bin $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/videoPlayerLib/$<CONFIGURATION> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)