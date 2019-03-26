cmake_minimum_required (VERSION 3.10 FATAL_ERROR)    #requirements
project(LucyProject CXX)                    #project name
set(SLEEPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../sleepy-discord"
CACHE PATH "The path to the SleepyDiscord Library source.")

set(TOKEN )  #global variables
set(OWNERID )
set(s1 )
set(s2 )
set(s3 )
                                        #options


configure_file (                        #pass CMake settings to Project headers
    "${PROJECT_SOURCE_DIR}/Headers/Ref_upload.hpp" #template
    "${PROJECT_SOURCE_DIR}/Headers/Ref.hpp"    #target
)                                        

configure_file( 
    "${PROJECT_SOURCE_DIR}/Headers/ServerList_upload.hpp" #template
    "${PROJECT_SOURCE_DIR}/Headers/ServerList.hpp"    #target
 )

add_subdirectory(${SLEEPY_SOURCE_DIR} ${SLEEPY_SOURCE_DIR}/build)

add_executable(LucyExec LucyBot.cpp)   #executable target

target_include_directories(LucyExec PUBLIC ${PROJECT_SOURCE_DIR}/Headers)

target_link_libraries (LucyExec sleepy-discord)# "${SLEEPY_SOURCE_DIR}/build/lib/libcurl.a" "${SLEEPY_SOURCE_DIR}/build/lib/libcpr.a" )  #link against Library
