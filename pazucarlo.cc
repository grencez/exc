
extern "C" {
#include "cx/syscx.h"
#include "cx/alphatab.h"
}
#include "cx/ofile.hh"
#include "cx/table.hh"
#include "cx/urandom.hh"

bool has_match(const Cx::Table<uint>& cells, uint ncols)
{
  const uint nrows = cells.sz() / ncols;
  for (uint r0 = 0; r0 < nrows; ++r0) {
    uint r1 = r0+1;
    uint r2 = r0+2;
    for (uint c0 = 0; c0 < ncols; ++c0) {
      uint c1 = c0+1;
      uint c2 = c0+2;

      bool hmatch = false;
      bool vmatch = false;
      if (c2 < ncols) {
        hmatch =
          (cells[r0*ncols+c0] == cells[r0*ncols+c1] && 
           cells[r0*ncols+c0] == cells[r0*ncols+c2]);
      }
      if (r2 < nrows) {
        vmatch =
          (cells[r0*ncols+c0] == cells[r1*ncols+c0] && 
           cells[r0*ncols+c0] == cells[r2*ncols+c0]);
      }

      if (hmatch || vmatch)
        return true;
    }
  }
  return false;
}

void testme()
{
  const int ncols = 6;
#define r 0
#define g 1
#define b 2
#define l 3
#define d 4
#define h 5
  const int myboard[30] = {
    r, l, d, d, h, l,
    g, b, b, d, g, r,
    h, h, d, r, l ,h,
    l, g, l, r, h, h,
    h, r, l, b, d, l
  };
#undef r
#undef g
#undef b
#undef l
#undef d
#undef h
  Cx::Table<uint> cells;
  cells.assign(&myboard[0], &myboard[30]);
  Claim(!has_match(cells, ncols));
}

int main(int argc, char** argv)
{
  int argi =
    (init_sysCx (&argc, &argv),
     1);
  Cx::OFile olog( stdout_OFile() );
  bool matches_allowed = false;
  uint ntotalorbs = 0;
  uint ntotalcolors = 0;
  uint ncolors = 0;
  uint nmatches = 0;
  uint npermatch = 0;
  uint ntrials = 0;

  //testme();

  if (argv[argi] && xget_uint_cstr (&nmatches, argv[argi]))
    argi += 1;
  else
    nmatches = 4;

  if (argv[argi] && xget_uint_cstr (&ncolors, argv[argi]))
    argi += 1;
  else
    ncolors = 4;

  if (argv[argi] && xget_uint_cstr (&npermatch, argv[argi]))
    argi += 1;
  else
    npermatch = 3;

  if (argv[argi] && xget_uint_cstr (&ntotalcolors, argv[argi]))
    argi += 1;
  else
    ntotalcolors = 6;

  if (argv[argi] && xget_uint_cstr (&ntotalorbs, argv[argi])) {
    matches_allowed = true;
    argi += 1;
  }
  else
    ntotalorbs = 30;

  if (argv[argi] && xget_uint_cstr (&ntrials, argv[argi]))
    argi += 1;
  else
    ntrials = 1000000;

  olog
    << "\nMatches: " << nmatches
    << "\nColors: " << ncolors
    << "\nOrbs per match: " << npermatch
    << "\nTotal colors: " << ntotalcolors
    << "\nTotal orbs: " << ntotalorbs
    << "\nTrials: " << ntrials
    << "\nMatches allowed on board? " << (matches_allowed ? "yes" : "no")
    << olog.endl();

  if (ntotalcolors < ncolors) {
    failout_sysCx("Not enough total colors.");
    assert(0);
  }
  if (ncolors < nmatches) {
    failout_sysCx("Not enough colors to match.");
    assert(0);
  }
  if (ntrials == 0) {
    failout_sysCx("Not enough trials.");
    assert(0);
  }

  Cx::URandom urandom;
  urandom.use_system_urandom(true);

  Cx::Table<uint> counts( ntotalcolors );
  uint npasses = 0;
  uint nvalids = 0;

  Cx::Table<uint> cells( ntotalorbs );
  for (uint trial_idx = 0; trial_idx < ntrials; ++trial_idx) {
    for (uint i = 0; i < cells.sz(); ++i) {
      cells[i] = urandom.pick(ntotalcolors);
    }

    for (uint i = 0; i < counts.sz(); ++i) {
      counts[i] = 0;
    }
    for (uint i = 0; i < cells.sz(); ++i) {
      counts[cells[i]] += 1;
    }

    if (!matches_allowed) {
      if (has_match (cells, 6))
        continue;
    }
    nvalids += 1;

    uint nmatched = 0;
    for (uint i = 0; i < ncolors; ++i) {
      if (counts[i] >= npermatch)
        nmatched += 1;
    }
    if (nmatched >= nmatches) {
      npasses += 1;
    }
  }

  if (nvalids > 0)
  {
    double p = 100 * ((double) npasses / nvalids);
    fprintf(stdout, "\nPercentage: %f\n", p);
    fprintf(stdout, "\nValid boards: %f\n", (double)nvalids / ntrials);
  }
  else {
    fprintf(stdout, "\nPercentage: N/A\n");
  }

  lose_sysCx ();
  return 0;
}

