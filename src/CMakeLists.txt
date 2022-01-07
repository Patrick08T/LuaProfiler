set(projectname "luaperf")
project("${projectname}")
file(GLOB_RECURSE SRC_CXX_FILES *.cpp)
file(GLOB_RECURSE SRC_CC_FILES *.cc)
file(GLOB_RECURSE SRC_C_FILES *.c)
file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE HEADER_LUA_FILES ../dep/lua-5.3.6/src/*.h)
list(REMOVE_ITEM SRC_CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(${projectname} SHARED 
  ${SRC_CXX_FILES}
  ${SRC_CC_FILES}
  ${SRC_C_FILES}
  )
