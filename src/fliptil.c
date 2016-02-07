
#include "cx/syscx.h"
#include "cx/ofile.h"

int main(int argc, char** argv)
{
  int argi = init_sysCx (&argc, &argv);
  int excess = 0;
  luint ntrials = 0;

  if (argi +1 != argc)  return 1;
  excess = atoi(argv[argi]);

  {
    int x = 0;
    byte b = 0;
    uint i = BYTE_BIT;
    while (x < excess && x > -excess) {
      if (i == BYTE_BIT) {
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

  oput_luint_OFile (stdout_OFile (), ntrials);
  oput_char_OFile (stdout_OFile (), '\n');
  lose_sysCx ();
  return 0;
}

