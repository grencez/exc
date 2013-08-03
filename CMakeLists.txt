
cmake_minimum_required (VERSION 2.8)

project (exc)

set (CxPath ../cx)
set (BinPath ../bin)
set (BldPath exc)

list (APPEND CFiles
  count-search.c
  polystep.c
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

addbinexe (polystep polystep.c)
target_link_libraries (polystep gmp)


