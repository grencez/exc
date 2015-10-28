
/** See search82000.c
 *
 * This is the MPI version.
 */

extern "C" {
#include "cx/syscx.h"
}
#define NO_MAIN_FUNC
#include "search82000.c"
#undef NO_MAIN_FUNC

#include "cx/mpiloop.hh"
using Cx::MpiLoop;

#define MpiTag_MpiLoop 1

/** The main function.
 *
 * Here we have logic for partitioning the search across threads.
 * Also some progress reporting.
 */
int main(int argc, char** argv)
{
  MPI_Init (&argc, &argv);
  int argi = init_sysCx (&argc, &argv);
  (void) argi;
  push_losefn_sysCx ((void (*) ()) MPI_Finalize);

  uint PcIdx = 0;
  uint NPcs = 1;
  MPI_Comm_rank (MPI_COMM_WORLD, (int*) &PcIdx);
  MPI_Comm_size (MPI_COMM_WORLD, (int*) &NPcs);

  FILE* out = stdout;
  /* Each iteration grows the guesses by 128 binary digits.*/
  static const uint nbits_inc = 128;

  bool solution_found = 0;
  size_t progress = 0;

  mpz_t guess;
  mpz_t high, r1, r2;

  init_search_vbls (guess, high, r1, r2);

  MpiLoop* mpi_loop = new MpiLoop(MpiTag_MpiLoop, MPI_COMM_WORLD);

  const uint nbits_lowlim = mpz_sizeinbase (guess, 2);
  for (uint work_idx = mpi_loop->begin_from((uint)(nbits_lowlim / nbits_inc));
       !mpi_loop->done_ck();
       work_idx = mpi_loop->next())
  {
    mp_bitcnt_t nbits_max = 0;
    assign_partition_range (work_idx, nbits_inc, guess, &nbits_max);

    if (PcIdx == 0) {
      print_progress (out, guess, &progress, 0);
      continue;
    }

    solution_found =
      try_until (guess, nbits_max, high, r1, r2);

    if (solution_found)
      mpi_loop->done_fo();
  }

  MPI_Barrier(MPI_COMM_WORLD);
  delete mpi_loop;

  if (solution_found) {
    fputs ("SUCCESS: ", out);
    mpz_out_str (out, 10, guess);
    fputs ("\n", out);
  }

  lose_search_vbls (guess, high, r1, r2);
  lose_sysCx ();
  return 0;
}


