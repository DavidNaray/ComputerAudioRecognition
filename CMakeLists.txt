cmake_minimum_required(VERSION 3.10)

project(ComputerAudioRecognition LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Your executable
add_executable(computerAudioRecognition 
    src/main.cpp
    src/Recording.cpp
)

# PortAudio setup
set(PORTAUDIO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/portaudio/include")
set(PORTAUDIO_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/portaudio/builddir")  # adjust if your build folder has a different name

target_include_directories(computerAudioRecognition PRIVATE ${PORTAUDIO_INCLUDE_DIR})
target_link_directories(computerAudioRecognition PRIVATE ${PORTAUDIO_LIBRARY_DIR})

#yeah... was following the guide for portAudio and got errors, looked online and portAudio apparently calls winmm and setupapi
target_link_libraries(computerAudioRecognition PRIVATE portaudio winmm setupapi)  # or portaudio_static.lib on Windows