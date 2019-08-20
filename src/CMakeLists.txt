
cmake_minimum_required (VERSION 3.1)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (exc)

find_package(OpenMP)
find_package(MPI)

set (TopPath ${CMAKE_CURRENT_SOURCE_DIR}/..)
set (BinPath ${TopPath}/bin)
set (DepPath ${TopPath}/dep)
set (CxTopPath ${DepPath}/cx)
list (APPEND CMAKE_MODULE_PATH ${CxTopPath}/cmake)
set (BldPath exc)

list (APPEND CFiles
  count-search.c
  polystep.c
  urandom.c
  bddtest.c
  fliptil.c
  search82000.c
  )
list (APPEND CCFiles
  leadercard.cc
  pazucarlo.cc
  search82000-mpi.cc
  )

include (${CxTopPath}/src/include.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinPath})

addbinexe (leadercard leadercard.cc)
addbinexe (count-search count-search.c)
addbinexe (urandom urandom.c)
addbinexe (fliptil fliptil.c)
addbinexe (pazucarlo pazucarlo.cc)

addbinexe (polystep polystep.c)
target_link_libraries (polystep gmp)

addbinexe (search82000 search82000.c)
#target_link_libraries (search82000 mpir)
target_link_libraries (search82000 gmp)

if (OPENMP_FOUND)
  foreach (target_name search82000)
    set_property (TARGET ${target_name}
      APPEND_STRING PROPERTY COMPILE_FLAGS " ${OpenMP_C_FLAGS}")
    set_property (TARGET ${target_name}
      APPEND_STRING PROPERTY LINK_FLAGS " ${OpenMP_C_FLAGS}")
  endforeach ()
endif ()

if (MPI_FOUND)
  addbinexe (search82000-mpi
    search82000-mpi.cc
    ${CxBldPath}/mpidissem.cc
    ${CxBldPath}/kautz.cc
    ${CxBldPath}/mpiloop.cc
    )
  set_property (TARGET search82000-mpi
    APPEND PROPERTY INCLUDE_DIRECTORIES ${MPI_INCLUDE_PATH})
  target_link_libraries (search82000-mpi ${MPI_LIBRARIES})
  target_link_libraries (search82000-mpi gmp)

  if(MPI_COMPILE_FLAGS)
    set (MPI_COMPILE_FLAGS "${MPI_COMPILE_FLAGS} ${DEFAULT_COMPILE_FLAGS}")
  else()
    set (MPI_COMPILE_FLAGS ${DEFAULT_COMPILE_FLAGS})
  endif()

  string (REGEX REPLACE "(^| )[/-]ansi($| )" " "
    MPI_COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  string (REGEX REPLACE "(^| )[/-]pedantic($| )" " "
    MPI_COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")

  set_target_properties (search82000-mpi PROPERTIES
    COMPILE_FLAGS ${MPI_COMPILE_FLAGS})

  if(MPI_LINK_FLAGS)
    set_target_properties (search82000-mpi PROPERTIES
      LINK_FLAGS "${MPI_LINK_FLAGS}")
  endif()
endif ()

find_package (Buddy)
if (BUDDY_FOUND)
  addbinexe (bddtest bddtest.c)

  set (BUDDY_COMPILE_FLAGS ${DEFAULT_COMPILE_FLAGS})
  string (REGEX REPLACE "(^| )[/-]ansi($| )" " "
    BUDDY_COMPILE_FLAGS "${BUDDY_COMPILE_FLAGS}")
  string (REGEX REPLACE "(^| )[/-]pedantic($| )" " "
    BUDDY_COMPILE_FLAGS "${BUDDY_COMPILE_FLAGS}")

  set_target_properties (bddtest PROPERTIES
    COMPILE_FLAGS ${BUDDY_COMPILE_FLAGS})

  target_include_directories (bddtest PUBLIC ${BUDDY_INCLUDE_DIR})
  target_link_libraries (bddtest ${BUDDY_LIBRARIES})
endif ()


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wextra -Wall")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3")

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

add_executable (soniagen soniagen.cc)

