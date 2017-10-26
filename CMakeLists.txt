cmake_minimum_required(VERSION 2.8)

project(mcm)

file( GLOB MCMSources src/*.h src/*.cpp)

INCLUDE_DIRECTORIES( 
                     ${multicommon_SOURCE_DIR}/src
                     ${multicachedata_SOURCE_DIR}/src
                   )

add_library( ${PROJECT_NAME} ${MCMSources} )

