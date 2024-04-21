cmake_minimum_required(VERSION 3.10)
project(ProyectoIOperativos C)

set(CMAKE_C_STANDARD 11)


if ( UNIX )
set( CMAKE_CXX_FLAGS_DEBUG_INIT "40 4" )
elseif( WIN32 )
set( CMAKE_CXX_FLAGS_DEBUG_INIT "" )
endif()
# Find pthread library
find_package(Threads REQUIRED)

# Find GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

# Disable building tests and examples in libfort project
set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(Libs/libFort/libfort-0.4.2)

# add gtk lib
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

# Build Creator
add_executable(Creator Creator.c)
target_link_libraries(Creator PRIVATE fort)
target_link_libraries(Creator ${CMAKE_THREAD_LIBS_INIT})

# Build Client
add_executable(Client Client.c)
target_link_libraries(Client ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Client PRIVATE ${GTK_LIBRARIES})
# Build ReConstructor

add_executable(Reconstructor Reconstructor.c)
target_link_libraries(Reconstructor ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Reconstructor PRIVATE ${GTK_LIBRARIES})
