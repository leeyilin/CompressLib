cmake_minimum_required(VERSION 2.8)

project(multicommon)

if( NOT MSYS)
    add_definitions( -D_LINUX_SYS )
endif()

file( GLOB MultiCommonSources src/*.h src/*.cpp src/*.c )

add_library( ${PROJECT_NAME} ${MultiCommonSources} )
