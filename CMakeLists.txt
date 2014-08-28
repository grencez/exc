
cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (exc)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

set (CxPath ${CMAKE_CURRENT_SOURCE_DIR}/../cx)
set (BinPath ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set (BldPath exc)

list (APPEND CFiles
  count-search.c
  polystep.c
  urandom.c
  bddtest.c
  )
list (APPEND CCFiles
  aperiodic.cc
  leadercard.cc
  )

include (${CxPath}/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (aperiodic aperiodic.cc)
addbinexe (leadercard leadercard.cc)
addbinexe (count-search count-search.c)
addbinexe (urandom urandom.c)

addbinexe (polystep polystep.c)
target_link_libraries (polystep gmp)

find_package (Buddy)
if (BUDDY_FOUND)
  addbinexe (bddtest bddtest.c)
  target_include_directories (bddtest PUBLIC ${BUDDY_INCLUDE_DIR})
  target_link_libraries (bddtest ${BUDDY_LIBRARIES})
endif ()


