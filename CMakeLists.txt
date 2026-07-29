cmake_minimum_required(VERSION 3.20)

project(
    AAVideoPlayerMF
    LANGUAGES CXX
)

set(
    CMAKE_CXX_STANDARD
    20
)

set(
    CMAKE_CXX_STANDARD_REQUIRED
    ON
)

add_executable(
    AAVideoPlayerMF
    WIN32

    src/main.cpp
    src/MFVideoPlayer.cpp
   src/MediaEngineNotify.h "src/AppMenu.h" "src/AppMenu.cpp")

target_include_directories(
    AAVideoPlayerMF
    PRIVATE

    src
)

target_link_libraries(
    AAVideoPlayerMF
    PRIVATE

    mfplat
    mf
    mfreadwrite
    mfuuid
    evr
    mfplay
)