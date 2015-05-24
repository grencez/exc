
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

uint count_combos(const Cx::Table<uint>& color_counts)
{
  const uint ncolors = color_counts.sz();
  uint combos = 0;
  for (uint i = 0; i < ncolors; ++i) {
    if (color_counts[i] <= 15) {
      combos += color_counts[i] / 3;
    }
    if (color_counts[i] > 15) {
      combos += 6 - (color_counts[i] - 13) / 3;
    }
  }
  return combos;
}

uint count_matches(const Cx::Table<uint>& color_counts, uint n_match_colors)
{
  const uint n_per_match = 3;
  uint matches = 0;
  for (uint i = 0; i < n_match_colors; ++i) {
    if (color_counts[i] >= n_per_match)
      matches += 1;
  }
  return matches;
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
  uint n_required_matches = 4;
  uint n_match_colors = 6;
  uint n_total_colors = 6;
  uint n_per_match = 3;
  uint n_total_orbs = 30;
  bool skill_board = false;
  bool combo_leader = false;
  bool matches_allowed = false;
  uint ntrials = 1000000;
  bool use_system_urandom = false;

  //testme();

  while (argi < argc) {
    const char* arg = argv[argi++];
    if (eq_cstr ("-sysrand", arg)) {
      use_system_urandom = true;
    }
    else if (eq_cstr ("-allow-matches", arg)) {
      matches_allowed = true;
    }
    else if (eq_cstr ("-ntrials", arg)) {
      arg = argv[argi++];
      if (!xget_uint_cstr (&ntrials, arg))
        failout_sysCx("Need a number after -trials.");
    }
    else if (eq_cstr ("-skill-board", arg)) {
      arg = argv[argi++];
      if (!xget_uint_cstr (&n_total_colors, arg) || n_total_colors == 0)
        failout_sysCx("Need a positive number of colors after -skill-board.");
      skill_board = true;
      matches_allowed = true;
    }
    else if (eq_cstr ("-ncolors", arg)) {
      arg = argv[argi++];
      if (!xget_uint_cstr (&n_total_colors, arg) || n_total_colors == 0)
        failout_sysCx("Need a positive number of colors after -ncolors.");
      skill_board = false;
      matches_allowed = false;
    }
    else if (eq_cstr ("-combo", arg)) {
      combo_leader = true;
      arg = argv[argi++];
      if (!xget_uint_cstr (&n_required_matches, arg))
        failout_sysCx("Need a number after -combo.");
    }
    else if (eq_cstr ("-match", arg)) {
      combo_leader = false;
      arg = argv[argi++];
      if (!xget_uint_cstr (&n_required_matches, arg) || n_required_matches == 0)
        failout_sysCx("Need two positive numbers after -match.");
      arg = argv[argi++];
      if (!xget_uint_cstr (&n_match_colors, arg) || n_match_colors == 0)
        failout_sysCx("Need two positive numbers after -match.");
    }
    else if (eq_cstr ("-tricolor", arg)) {
      n_total_colors = 4;
    }
    else if (eq_cstr ("-no-rcv", arg)) {
      n_total_colors = 5;
    }
    else {
      fprintf (stderr, "Unknown argument: %s\n", arg);
      failout_sysCx(0);
    }
  }

  if (combo_leader) {
    n_match_colors = n_total_colors;
  }

  static const char HorizLine[] = "--------------------------------";
  olog << HorizLine
    << "\nLeader: " << n_required_matches;
  if (combo_leader) {
    olog << " combos";
  }
  else {
    olog << " matches"
      << "\nMatch colors: " << n_match_colors;
  }
  olog
    << "\nTotal colors: " << n_total_colors
    << "\nTrials: " << ntrials
    << "\nOrbs per match (fixed): " << n_per_match
    << "\nTotal orbs (fixed): " << n_total_orbs
    << "\nMatches allowed on board? " << (matches_allowed ? "yes" : "no")
    << "\nRandom source: " << (use_system_urandom ? "/dev/urandom" : "Marsaglia")
    << "\n" << HorizLine
    << olog.endl();

  if (!combo_leader) {
    if (n_required_matches > n_match_colors) {
      failout_sysCx("Not enough colors to match.");
    }
    if (n_match_colors > n_total_colors) {
      failout_sysCx("Too many match colors.");
    }
  }
  if (ntrials == 0) {
    failout_sysCx("Not enough trials.");
  }

  Cx::URandom urandom;
  if (use_system_urandom) {
    urandom.use_system_urandom(true);
  }

  Cx::Table<uint> color_counts( n_total_colors );
  uint npasses = 0;
  uint nmistrials = 0;

  Cx::Table<uint> cells( n_total_orbs );
  for (uint trial_idx = 0; trial_idx < ntrials;) {
    for (uint i = 0; i < cells.sz(); ++i) {
      cells[i] = urandom.pick(n_total_colors);
    }

    if (!matches_allowed && has_match (cells, 6)) {
      nmistrials += 1;
      continue;
    }

    for (uint i = 0; i < color_counts.sz(); ++i) {
      color_counts[i] = 0;
    }
    for (uint i = 0; i < cells.sz(); ++i) {
      color_counts[cells[i]] += 1;
    }

    if (false)
    {
      bool good = true;
      for (uint i = 0; i < n_total_colors; ++i) {
        if (color_counts[i] > 15) {
          good = false;
        }
      }
      if (!good) {
        nmistrials += 1;
        continue;
      }
    }

    if (skill_board) {
      if (n_total_colors != count_matches (color_counts, n_total_colors)) {
        nmistrials += 1;
        continue;
      }
    }

    uint nmatches = 0;
    if (combo_leader) {
      nmatches = count_combos (color_counts);
    }
    else {
      nmatches = count_matches (color_counts, n_match_colors);
    }
    if (nmatches >= n_required_matches) {
      npasses += 1;
    }
    else if (false) {
      for (uint i = 0; i < n_total_colors; ++i) {
        olog << color_counts[i] << " ";
      }
      olog << olog.endl();
    }

    const ujint percent_complete = (100 * (ujint) (trial_idx+1)) / ntrials;
    if (trial_idx==0 || percent_complete > (100 * (ujint) trial_idx) / ntrials) {
      fprintf(stderr, "\r%3lu%% complete...", percent_complete);
    }

    trial_idx += 1;
  }
  fprintf(stderr, "\n");

  if (ntrials > 0)
  {
    double p;
    p = 100 * ((double)nmistrials / (ntrials + nmistrials));
    fprintf(stdout, "Mistrial%%: %f\n", p);
    fprintf(stdout, "Total trials: %u\n", (ntrials + nmistrials));
    p = 100 * ((double) npasses / ntrials);
    fprintf(stdout, "\nActivation%%: %f\n\n", p);
  }
  else {
    fprintf(stdout, "\nPercentage: N/A\n");
  }

  lose_sysCx ();
  return 0;
}

