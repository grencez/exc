
cmake_minimum_required (VERSION 2.8)
#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include(ExternalProject)

set (GluPfx glu-2.4)
set (PegPfx peg-0.1.15)

set (TopPath ${CMAKE_CURRENT_SOURCE_DIR})
set (DepPath ${TopPath}/dep)

ExternalProject_Add(cx-pp
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cx-pp
  SOURCE_DIR ${DepPath}/cx-pp
  #GIT_REPOSITORY git@github.com:grencez/cx-pp.git
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND echo "No configure step."
  BUILD_COMMAND echo "No build step."
  INSTALL_COMMAND echo "No install step."
  )

ExternalProject_Add(cx
  DEPENDS cx-pp
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/cx
  SOURCE_DIR ${DepPath}/cx
  #BINARY_DIR ${DepPath}/cx/bld
  #GIT_REPOSITORY git@github.com:grencez/cx.git
  INSTALL_COMMAND echo "No install step."
  )

ExternalProject_Add(exc
  DEPENDS cx
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/protocon
  SOURCE_DIR ${TopPath}/src
  BINARY_DIR ${TopPath}/bld/
  CONFIGURE_COMMAND ${CMAKE_COMMAND}
  ../src
  INSTALL_COMMAND echo "No install step."
  )

