cmake_minimum_required (VERSION 3.10 FATAL_ERROR)    #requirements
project(LucyProject CXX)                    #project name
set(SLEEPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../sleepy-discord"
CACHE PATH "The path to the SleepyDiscord Library source.")

set(TOKEN \"NDMyOTYwMzgxOTU1MjExMjY0.Da05lg.CmAtXI73GKeoWmbArEVQrOT4gUM\")  #global variables
set(OWNERID \"251121149981884423\")
                                        #options

configure_file (                        #pass CMake settings to Project headers
    "${PROJECT_SOURCE_DIR}/Ref_upload.h" #template
    "${PROJECT_SOURCE_DIR}/Ref.h"    #target
)                                        

add_subdirectory(${SLEEPY_SOURCE_DIR} ${SLEEPY_SOURCE_DIR}/build)

add_executable(LucyExec LucyBot.cpp)   #executable target

target_link_libraries (LucyExec sleepy-discord)# "${SLEEPY_SOURCE_DIR}/build/lib/libcurl.a" "${SLEEPY_SOURCE_DIR}/build/lib/libcpr.a" )  #link against Library
