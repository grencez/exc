
#include <vector>
#include <sstream>
#include <stdio.h>
#include <string.h>
using std::vector;
using std::istringstream;
typedef unsigned int uint;

  int
main (int argc, char** argv)
{
  int argi = 1;
  const uint n = (uint) argc-argi;
  FILE* out = stdout;
  vector<uint> t(n);
  vector<uint> cards(n);
  vector<uint> moves(n, 0);
  uint nstartcols = 22;
  uint width = 4;
  char buf[200];

  if (n == 0) {
    fprintf (stderr, "Usage: %s int [int]*\n", argv[0]);
    return 1;
  }
  argv = &argv[argi];

  fprintf(out, "N = %u\n", n);
  for (uint i = 0; i < n; ++i) {
    istringstream in(argv[i]);
    in >> t[i];
    if (in.bad()) {
      fprintf (stderr, "Number at index %u is malformed: %s\n", i, argv[i]);
      return 1;
    }
  }
  for (uint i = 0; i < n; ++i) {
    int diff = (int) t[i] - (int) t[(i+1) % n];
    while (diff > (int) n) {
      diff -= (int) n;
    }
    while (diff <= 0) {
      diff += (int) n;
    }
    cards[i] = (uint) diff;
  }

  vector<uint> playcards( cards );
  for (bool done = false; !done;) {
    done = true;
    for (uint i = 0; i < n; ++i) {
      uint me = n-i-1;
      uint pd = (me+n-1) % n;
      if (playcards[me] < playcards[pd]) {
        moves[me] += 1;
        ++ playcards[me];
        -- playcards[pd];
        done = false;
      }
    }
  }

  fprintf(out, "%-*s", nstartcols, "process:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, me);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "pick:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, t[me]);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "(pick - left) gap N:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, cards[me]);
  }
  fputc('\n', out);

  strcpy (buf, " WORK ");
  fputs (buf, out);
  for (uint i = strlen(buf); i < nstartcols + n*width; ++i) {
    fputc('-', out);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "pick:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, t[me]);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "... minus left pick:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    uint sc = (me+1)%n;
    fprintf(out, "%*u", width, t[sc]);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "... equals:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    uint sc = (me+1)%n;
    int diff = (int) t[me] - (int) t[sc];
    fprintf(out, "%*i", width, diff);
  }
  fputc('\n', out);
  sprintf(buf, "... +%u until positive:", n);
  fprintf(out, "%-*s", nstartcols, buf);
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, cards[me]);
  }
  fputc('\n', out);

  strcpy (buf, " OUTCOME ");
  fputs (buf, out);
  for (uint i = strlen(buf); i < nstartcols + n*width; ++i) {
    fputc('-', out);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "cards:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, playcards[me]);
  }
  fputc('\n', out);

  fprintf(out, "%-*s", nstartcols, "moves:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    fprintf(out, "%*u", width, moves[me]);
  }
  fputc('\n', out);
  fprintf(out, "%-*s", nstartcols, "(pick + moves) gap N:");
  for (uint i = 0; i < n; ++i) {
    uint me = n-i-1;
    uint a = moves[me] + t[me];
    while (a > n) {
      a -= n;
    }
    fprintf(out, "%*u", width, a);
  }
  fputc('\n', out);
  return 0;
}

