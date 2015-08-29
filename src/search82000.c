
/** Find a number that can be represented by 0 and 1 digits
 * in base 2, base 3, ..., base {MaxBase}.
 *
 * 82000 works for base 2 thru 5.
 * Can you find anything higher?
 * I have checked up to 11 million decimal digits without finding anything.
 * Perhaps a nonexistence proof is needed!
 *
 * Compile with:
 *   gcc basedsearch.c -lgmp -o basedsearch
 *
 * Modify parameters by changing {MaxBase}.
 *
 * Code adapted from /u/threenplusone's comment:
 *   http://www.reddit.com/r/math/comments/36jq0k/a_curious_property_of_82000/crf0pkn
 */

#include <assert.h>
#include <stdio.h>
#include <gmp.h>
/* #include <mpir.h> */

#ifndef NO_MAIN_FUNC
typedef unsigned int uint;
typedef int Bool;
#endif

static const uint MinBase = 3;
static const uint MaxBase = 5;


/** The core algorithm.
 *
 * Assume {nbits} is infinity if this is your first time reading the function.
 * Then this algorithm will terminate only when a solution is found.
 *
 * In general, we try until the number of bits
 * needed to represent {guess} exceeds {nbits}.
 * This allows the search to be partitioned across threads.
 */
static
  Bool
try_until (mpz_t guess, mp_bitcnt_t nbits,
           /* The rest are just temporaries.*/
           mpz_t high, mpz_t r1, mpz_t r2)
{
  uint base = MaxBase;
  Bool passing = 1;

  /* Keep guessing in this order:
   *   base == MaxBase
   *   base == MaxBase-1
   *   ...
   *   base == MinBase
   * And if {passing} is still true, the loop terminates with:
   *   base == MinBase-1 (success!)
   */
  while (base >= MinBase) {
    size_t ndigits;
    Bool passed = 1;
    ndigits = mpz_sizeinbase(guess, base);
    mpz_ui_pow_ui(high, base, ndigits);
    mpz_set(r1, guess);

    while (mpz_cmp_ui(high, 1) > 0) {
      if (mpz_cmp(r1, high) >= 0) {
        mpz_sub(r2, r1, high);
        if (mpz_cmp(r2, high) >= 0) {
          passed = 0;
          break;
        }
        mpz_set(r1, r2);
      }
      mpz_divexact_ui(high, high, base);
    }
    if (mpz_cmp_ui(r1, 1) > 0) {
      passed = 0;
    }

    if (!passed) {
      mpz_sub(guess, guess, r1);
      mpz_mul_ui(high, high, base);
      mpz_add(guess, guess, high);
      passing = 0;
    }

    if (base == MinBase && !passing) {
      base = MaxBase;
      passing = 1;
      if (nbits == 0 || nbits < mpz_sizeinbase(guess, 2)) {
        return 0;
      }
    }
    else {
      base -= 1;
    }
  }
  return 1;
}


#ifndef NO_MAIN_FUNC
/** The main function.
 *
 * Here we have logic for partitioning the search across threads.
 * Also some progress reporting.
 */
int main()
{
  FILE* out = stdout;
#ifdef _OPENMP
  /* For parallel code, report progress every time
   * the master thread finishes a call to try_until().
   * Each call grows the guesses by 100 binary digits.
   */
  static const uint nbits_inc = 100;
  const uint ProgressFrequency = 0;
#else
  /* For sequential code, report progress every 100 guesses.
   * Each guess corresponds to 1 call of try_until().
   */
  static const uint nbits_inc = 0;
  const uint ProgressFrequency = 100;
#endif
  Bool some_found = 0;
  uint nbits_lim = 0;
  size_t progress = ProgressFrequency;
  mpz_t answer;

  mpz_init (answer);

#pragma omp parallel shared(some_found, nbits_lim, answer)
  {
  mpz_t guess;
  mpz_t high, r1, r2;

  /* Initialized with a guess of 2.*/
  mpz_init_set_ui (guess, 2);

  /* Alternative initializations.*/
  if (0) {
    mpz_set_ui (guess, 82001);
  }

  if (0) {
    size_t base10digits = 332000;
    mpz_set_ui (guess, 10);
    mpz_pow_ui (guess, guess, base10digits-1);
  }

#pragma omp master
  {
    nbits_lim = mpz_sizeinbase (guess, 2);
  }
#pragma omp barrier

  mpz_init (high);
  mpz_init (r1);
  mpz_init (r2);

  assert(1 < MinBase);
  assert(MinBase <= MaxBase);

  while (!some_found) {
    mp_bitcnt_t nbits = 0;
    Bool found;

    if (nbits_inc > 0) {
#pragma omp critical (nbits_lim)
      {
        nbits = nbits_lim;
        nbits_lim += nbits_inc;
      }

      mpz_set_ui (guess, 0);
      mpz_setbit (guess, nbits);
      nbits += nbits_inc;
    }

    found = try_until (guess, nbits, high, r1, r2);

    if (found) {
#pragma omp critical (some_found)
      {
        some_found = 1;
        mpz_set (answer, guess);
      }
    }

#pragma omp master
    {
    if (progress >= ProgressFrequency) {
      Bool made_progress = 1;
      size_t base10digits = mpz_sizeinbase(guess, 10);
      /* When ProgressFrequency==0, report progress when #digits changes.*/
      if (ProgressFrequency > 0)
        progress = 0;
      else if (base10digits > progress)
        progress = base10digits;
      else
        made_progress = 0;

      if (made_progress)
        fprintf (out, "digits: %u\n", (uint)base10digits);
    }
    else {
      progress += 1;
    }
    }
  }

  mpz_clear (guess);
  mpz_clear (high);
  mpz_clear (r1);
  mpz_clear (r2);
  }

  fputs ("SUCCESS: ", out);
  mpz_out_str (out, 10, answer);
  fputs ("\n", out);
  mpz_clear (answer);


  return 0;
}
#endif

