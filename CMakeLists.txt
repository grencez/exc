
cmake_minimum_required (VERSION 2.8)

project (exc)

set (CxPath ${CMAKE_CURRENT_SOURCE_DIR}/../cx)
set (BinPath ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set (BldPath exc)

list (APPEND CFiles
  count-search.c
  polystep.c
  urandom.c
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


