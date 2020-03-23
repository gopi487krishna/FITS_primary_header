cmake_minimum_required(VERSION 2.8)
project(FITS_READER)

#To add source code and setup folder structure
set(src DemoFile.cpp )
set(include . )
include_directories(include)

set(Boost_USE_STATIC_LIBS on)
find_package(Boost COMPONENTS iostreams REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})

#Add Compiler Flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-std=c++1z -stdlib=libc++ -O3 -Wall)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-std=c++17 -O3 -Wall)
endif()

#Create Executable
add_executable(exe ${src} ${include})
target_link_libraries(exe ${Boost_LIBRARIES})

