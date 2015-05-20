
/** Find a number that can be represented by 0 and 1 digits
 * in base 2, base 3, ..., base {MaxBase}.
 *
 * 82000 works for base 2 thru 5.
 * Can you find anything higher?
 * How about for base 2 thru 6?
 * (Or perhaps a nonexistence proof is needed!)
 *
 * Compile with:
 *   gcc basedsearch.c -lgmp -o basedsearch
 *
 * Modify parameters by changing {MaxBase} in main().
 *
 * Code adapted from /u/threenplusone's comment:
 *   http://www.reddit.com/r/math/comments/36jq0k/a_curious_property_of_82000/crf0pkn
 */

#include <assert.h>
#include <stdio.h>
#include <gmp.h>
/* #include <mpir.h> */

typedef unsigned int uint;

int main()
{
  FILE* out = stdout;
  const uint MinBase = 3;
  const uint MaxBase = 5;
  uint base = MaxBase;
  mpz_t guess;
  mpz_t high, r1, r2;
  int passing = 1;
  size_t progress_ndigits = 0;

  /* Initialized with a guess of 2.*/
  mpz_init_set_ui (guess, 2);
  //mpz_init_set_ui (guess, 82001);

  mpz_init (high);
  mpz_init (r1);
  mpz_init (r2);

  assert(1 < MinBase);
  assert(MinBase <= MaxBase);

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
    int passed = 1;
    ndigits = mpz_sizeinbase(guess, base);
    mpz_ui_pow_ui(high, base, ndigits);
    mpz_set(r1, guess);

    {
      size_t base10digits = mpz_sizeinbase(guess, 10);
      if (base10digits > progress_ndigits) {
        fprintf (out, "digits: %u\n", (uint)base10digits);
        progress_ndigits = base10digits;
      }
    }

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
    }
    else {
      base -= 1;
    }
  }

  fputs ("SUCCESS: ", out);
  mpz_out_str (out, 10, guess);
  fputs ("\n", out);

  mpz_clear (guess);
  mpz_clear (high);
  mpz_clear (r1);
  mpz_clear (r2);

  return 0;
}

