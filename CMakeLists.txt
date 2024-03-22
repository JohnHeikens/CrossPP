

# Use c++ 23
set(CMAKE_CXX_STANDARD 23)

# set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
# Shoutout to CMake guide: https://eliasdaler.github.io/using-cmake/
cmake_minimum_required(VERSION 3.5)
 


#set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

IF (WIN32)
    set(CMAKE_C_COMPILER C:/msys64/mingw64/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER C:/msys64/mingw64/bin/g++.exe)
    # this makes sure that .dll-files are build into build/ folder
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
ELSEIF (APPLE)
    # run 'where clang' and 'where clang++' to get paths
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
ELSEIF (UNIX AND NOT APPLE)
    # run 'which gcc' and 'which g++' to get paths
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
ENDIF()

set(VCPKG_VERSION edge)
include(vcpkg.cmake)

#find_path(PLOG_INCLUDE_DIRS "gzip/compress.hpp" REQUIRED)

# Get project name from folder name
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId}) 

# Enable additional warnings and debug mode
# https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
set (CMAKE_CXX_FLAGS "-g -W -pedantic")

# add .cpp-files

file(GLOB_RECURSE source_list "src/*.*")

list(FILTER source_list EXCLUDE REGEX "!(${PROJECT_SOURCE_DIR}/src/*.(c|cpp))" )
#list(FILTER source_list EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/src/gzip/*" )

foreach(header_dir ${source_list})
    message(STATUS "[INFO] source file included: ${header_dir}")
endforeach()
set(SOURCES ${source_list}) 

# folder with .h files, e.g. include/
#MACRO(HEADER_DIRECTORIES return_list)
#    FILE(GLOB_RECURSE new_list 
#    src/*.h)
#    SET(dir_list "")
#    FOREACH(file_path ${new_list})
#        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
#        SET(dir_list ${dir_list} ${dir_path})
#    ENDFOREACH()
#    LIST(REMOVE_DUPLICATES dir_list)
#    SET(${return_list} ${dir_list})
#ENDMACRO()
#
#
#HEADER_DIRECTORIES(header_dir_list)
#
#foreach(header_dir ${header_dir_list})
#    message(STATUS "[INFO] Directory included: ${header_dir}")
#endforeach()
#
#list(LENGTH header_dir_list header_dir_list_count)
#message(STATUS "[INFO] Found ${header_dir_list_count} header directories.")


#add directories to add to #include statements for example we don't have to #include "include/math/..." but with this shortcut we can #include "math/..."
include_directories(src) 
include_directories(src/include)

#include_directories(ZLIB src/gzip/zlib-1.2.11/zlib-1.2.11)

#set(SOURCE_DIR )
# 3rd party dependencies
add_subdirectory(lib)

#add_subdirectory(src/gzip/zlib-1.2.11/zlib-1.2.11)
#find_package(ZLIB REQUIRED)

#vcpkg_init()
#vcpkg_add_package(zlib)
#find_package(ZLIB REQUIRED)

# Here the .c/.cpp files get linked
add_executable(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(main PRIVATE ZLIB::ZLIB)

# Here we link 3rd party library ImGui-SFML
# No need to link SFML implicitly as ImGui-SFML does it for you!
# Refer to https://eliasdaler.github.io/using-cmake/
#target_link_libraries(${PROJECT_NAME} PRIVATE ImGui-SFML::ImGui-SFML)
#https://stackoverflow.com/questions/59573574/how-do-i-use-sfml-with-cmake-fetchcontent
target_link_libraries(${PROJECT_NAME} PRIVATE 
sfml-audio
sfml-network
sfml-graphics
sfml-system
sfml-window
)


#set(ZLIB_DIR C:/Program Files (x86)/GnuWin32/bin)
#find_package( ZLIB )
#if ( ZLIB_FOUND )
#    include_directories( ${ZLIB_INCLUDE_DIRS} )
#    target_link_libraries( YourProject ${ZLIB_LIBRARIES} )
#endif( ZLIB_FOUND )

#include(FindZLIB)
#
#FindZLIB()
#set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/zlib)
#find_package(ZLIB)
#set(ZLIB_INCLUDE_DIR src/zlib)
##set(ZLIB_ROOT src/zlib)
#target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

# Additional resources like images and fonts
# We want the to copy from permanent folder "res" to temporary folder "build"
# configure_file(res/OpenSans-Regular.ttf OpenSans-Regular.ttf COPYONLY)

# Set "a" to be the output filename, does not really matter, shorter is quicker to type...
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "a")
