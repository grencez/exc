
#include <stdio.h>

int main(int argc, char** argv)
{
  FILE* f;
  long lo;
  long hi;
  unsigned long off;
  unsigned long max_off;
  unsigned long mask;
  if (argc != 3) {
    fputs("Requires two arguments.\n", stderr);
    return 1;
  }
  if (1 != sscanf(argv[1], "%ld", &lo)) {
    fputs("Malformed first argument.\n", stderr);
    return 1;
  }
  if (1 != sscanf(argv[2], "%ld", &hi)) {
    fputs("Malformed second argument.\n", stderr);
    return 1;
  }

  /* Trivial, only one number can occur.*/
  if (lo == hi) {
    fprintf(stdout, "%ld\n", lo);
    return 0;
  }

  if (hi < lo) {
    long c = hi;
    hi = lo;
    lo = c;
  }
  max_off = hi - lo;
  mask = 0;
  while (mask < max_off) {
    mask = (mask << 1) | 1;
  }

  f = fopen("/dev/urandom", "rb");
  if (!f) {
    fputs("Failed to open /dev/urandom!\n", stderr);
    return 1;
  }

  /* Keep choosing until a number falls in the range 0..{max_off}.*/
  do {
    if (fread(&off, sizeof(off), 1, f) != 1) {
      fputs("Failed to read from /dev/urandom!\n", stderr);
      fclose(f);
      return 1;
    }
    /* We can assume each bit is uniformly random,
     * so truncate as much as possible without excluding {max_off}.
     */
    off &= mask;
    /* If {off} is outside of our range, then try again.
     * This has less than a 50% chance of happening, so this loop will terminate eventually.
     */
  } while (off > max_off);
  fclose(f);

  fprintf(stdout, "%ld\n", lo + (long)off);
  return 0;
}

