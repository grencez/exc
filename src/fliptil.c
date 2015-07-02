
#include "cx/syscx.h"
#include "cx/ofile.h"

int main(int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);
  int excess = 0;
  ujint ntrials = 0;

  if (argi +1 != argc)  return 1;
  excess = atoi(argv[argi]);

  {
    int x = 0;
    byte b = 0;
    uint i = NBitsInByte;
    while (x < excess && x > -excess) {
      if (i == NBitsInByte) {
        i = 0;
        Randomize( b );
      }
      x += (b % 2 == 1) ? 1 : 0;
      b /= 2;
      ++ i;
      x += (b % 2 == 1) ? -1 : 0;
      b /= 2;
      ++ i;
      ++ ntrials;

      if (ntrials==0) {
        oput_cstr_OFile (stdout_OFile (), "Overflow\n");
        flush_OFile (stdout_OFile ());
      }
    }
  }

  oput_ujint_OFile (stdout_OFile (), ntrials);
  oput_char_OFile (stdout_OFile (), '\n');
  lose_sysCx ();
  return 0;
}

