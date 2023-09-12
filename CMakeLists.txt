cmake_minimum_required(VERSION 2.8)
project(patriot)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
cmake_policy(SET CMP0015 NEW)

set(CMAKE_CXX_STANDARD 17)

if(USE_CXX11_ABI AND USE_CXX11_ABI STREQUAL "yes")
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
else()
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()

set(CXX_FLAG_LIST
    "-fPIC"
    "-fno-short-enums"
    "-Wall"
    "-Wno-deprecated-declarations"
    "-Wformat"
    "-Wno-unknown-pragmas"
    "-Wno-unused-variable"
    "-Wno-unused-parameter"
    "-Wno-unused-function"
    "-Wno-strict-aliasing"
    "-Wno-pmf-conversions"
    "-Wno-attributes"
    )
string(REPLACE ";" " " CXX_FLAGS "${CXX_FLAG_LIST}")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${CXX_FLAGS}) 

if(CMAKE_BUILD_TYPE MATCHES "Release")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -ggdb -D_DEBUG ")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pg -g -ggdb -D_DEBUG $ENV{DEBUG_FLAG}  -lrt ${MY_OPTION} ")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
endif()


include_directories(
  .
  src
  dep/lua-5.3.6/src
  src/TraceTree
)

add_subdirectory(src)

message("")
message("====================================")
message(" Compile Options:")
message(" BUILD_TYPE        =" ${CMAKE_BUILD_TYPE})
message(" Bin output dir    =" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(" Lib output dir    =" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(" CMAKE_CXX_FLAGS   =" ${CMAKE_CXX_FLAGS})
