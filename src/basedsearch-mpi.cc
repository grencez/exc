
/** See basedsearch.c
 *
 * This is the MPI version.
 */

extern "C" {
#include "cx/syscx.h"
}
#define NO_MAIN_FUNC
#include "basedsearch.c"
#undef NO_MAIN_FUNC

#include "cx/mpiloop.hh"
using Cx::MpiLoop;

#define MpiTag_MpiLoop 1

#if 1
static const uint min_guess = 2;
#else
static const uint min_guess = 82001;
#endif

/** The main function.
 *
 * Here we have logic for partitioning the search across threads.
 * Also some progress reporting.
 */
int main(int argc, char** argv)
{
  MPI_Init (&argc, &argv);
  int argi = (init_sysCx (&argc, &argv), 1);
  (void) argi;
  push_losefn_sysCx ((void (*) ()) MPI_Finalize);

  uint PcIdx = 0;
  uint NPcs = 1;
  MPI_Comm_rank (MPI_COMM_WORLD, (int*) &PcIdx);
  MPI_Comm_size (MPI_COMM_WORLD, (int*) &NPcs);

  FILE* out = stdout;
  /* For parallel code, report progress every time
   * the master thread finishes a call to try_until().
   * Each call grows the guesses by 100 binary digits.
   */
  static const uint nbits_inc = 128;

  bool solution_found = 0;
  uint nbits_lim = 0;
  size_t progress = 0;

  mpz_t guess;
  mpz_t high, r1, r2;

  /* Initialized with a guess of 2.*/
  mpz_init_set_ui (guess, 2);

  /* Alternative initializations.*/
  if (0) {
    size_t base10digits = 432000;
    mpz_set_ui (guess, 10);
    mpz_pow_ui (guess, guess, base10digits-1);
  }

  nbits_lim = mpz_sizeinbase (guess, 2);

  mpz_init (high);
  mpz_init (r1);
  mpz_init (r2);

  assert(1 < MinBase);
  assert(MinBase <= MaxBase);

  MpiLoop* mpi_loop = new MpiLoop(MpiTag_MpiLoop, MPI_COMM_WORLD);

  for (uint work_idx = mpi_loop->begin_from((uint)(nbits_lim / nbits_inc));
       !mpi_loop->done_ck();
       work_idx = mpi_loop->next())
  {
    mp_bitcnt_t nbits = work_idx;
    nbits *= nbits_inc;

    mpz_set_ui (guess, 0);
    mpz_setbit (guess, nbits);
    nbits += nbits_inc;
    if (mpz_cmp_ui(guess, min_guess) < 0) {
      mpz_set_ui (guess, min_guess);
    }

    if (PcIdx == 0) {
      size_t base10digits = mpz_sizeinbase(guess, 10);
      if (base10digits > progress) {
        progress = base10digits;
        fprintf (out, "digits: %u\n", (uint)base10digits);
      }
      continue;
    }

    solution_found =
      try_until (guess, nbits, high, r1, r2);

    if (solution_found)
      mpi_loop->done_fo();
  }

  MPI_Barrier(MPI_COMM_WORLD);
  delete mpi_loop;
  //MPI_Allreduce(&send_nlayers, &max_nlayers, 1,
  //              MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  if (solution_found) {
    fputs ("SUCCESS: ", out);
    mpz_out_str (out, 10, guess);
    fputs ("\n", out);
  }

  mpz_clear (guess);
  mpz_clear (high);
  mpz_clear (r1);
  mpz_clear (r2);


  lose_sysCx ();
  return 0;
}


