
cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (exc)

find_package(OpenMP)
find_package(MPI)

set (CxPath ${CMAKE_CURRENT_SOURCE_DIR}/../dep/cx/src)
list (APPEND CMAKE_MODULE_PATH ${CxPath}/cmake)
set (BinPath ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set (BldPath exc)

list (APPEND CFiles
  count-search.c
  polystep.c
  urandom.c
  bddtest.c
  fliptil.c
  basedsearch.c
  )
list (APPEND CCFiles
  aperiodic.cc
  leadercard.cc
  pazucarlo.cc
  basedsearch-mpi.cc
  )

include (${CxPath}/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (aperiodic aperiodic.cc)
addbinexe (leadercard leadercard.cc)
addbinexe (count-search count-search.c)
addbinexe (urandom urandom.c)
addbinexe (fliptil fliptil.c)
addbinexe (pazucarlo pazucarlo.cc)

addbinexe (polystep polystep.c)
target_link_libraries (polystep gmp)

addbinexe (basedsearch basedsearch.c)
#target_link_libraries (basedsearch mpir)
target_link_libraries (basedsearch gmp)

if (OPENMP_FOUND)
  foreach (target_name basedsearch)
    set_property (TARGET ${target_name}
      APPEND_STRING PROPERTY COMPILE_FLAGS " ${OpenMP_C_FLAGS}")
    set_property (TARGET ${target_name}
      APPEND_STRING PROPERTY LINK_FLAGS " ${OpenMP_C_FLAGS}")
  endforeach ()
endif ()

if (MPI_FOUND)
  addbinexe (basedsearch-mpi
    basedsearch-mpi.cc
    ${CxBldPath}/mpidissem.cc
    ${CxBldPath}/kautz.cc
    ${CxBldPath}/mpiloop.cc
    )
  set_property (TARGET basedsearch-mpi
    APPEND PROPERTY INCLUDE_DIRECTORIES ${MPI_INCLUDE_PATH})
  target_link_libraries (basedsearch-mpi ${MPI_LIBRARIES})
  target_link_libraries (basedsearch-mpi gmp)

  if(MPI_COMPILE_FLAGS)
    set (MPI_COMPILE_FLAGS "${MPI_COMPILE_FLAGS} ${DEFAULT_COMPILE_FLAGS}")
  else()
    set (MPI_COMPILE_FLAGS ${DEFAULT_COMPILE_FLAGS})
  endif()

  string (REGEX REPLACE "(^| )[/-]ansi($| )" " "
    MPI_COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  string (REGEX REPLACE "(^| )[/-]pedantic($| )" " "
    MPI_COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")

  set_target_properties (basedsearch-mpi PROPERTIES
    COMPILE_FLAGS ${MPI_COMPILE_FLAGS})

  if(MPI_LINK_FLAGS)
    set_target_properties (basedsearch-mpi PROPERTIES
      LINK_FLAGS "${MPI_LINK_FLAGS}")
  endif()
endif ()

find_package (Buddy)
if (BUDDY_FOUND)
  addbinexe (bddtest bddtest.c)
  target_include_directories (bddtest PUBLIC ${BUDDY_INCLUDE_DIR})
  target_link_libraries (bddtest ${BUDDY_LIBRARIES})
endif ()


