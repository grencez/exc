
// Still debugging. :(

#include <iostream>
//#include <bitset>
#include <vector>
#include <assert.h>
#include <stdio.h>

typedef uint64_t board_t;
typedef uint8_t column_t;

#define NRows 5
#define NCols 6

template <typename T>
uint popcount(T x)
{
#if 0
  return __builtin_popcount(x);
#else
  uint n = 0;
  while (x)
  {
    x &= (x-1);
    ++ n;
  }
  return n;
#endif
}

/** Return the score.**/
uint simulate(board_t board)
{
  //uint nrows = 0;
  //uint ntpas = 0;
  uint nmatches = 0;

  column_t a_cols[NCols];
  column_t b_cols[NCols];
  for (uint i = 0; i < NCols; ++i) {
    a_cols[i] =  board & ((1 << NRows) - 1);
    b_cols[i] = ~board & ((1 << NRows) - 1);
    board >>= NRows;
  }


  uint prev_nmatches;
  do {
    prev_nmatches = nmatches;

    column_t a_matches[NCols];
    column_t b_matches[NCols];

    a_matches[0] = 0;
    b_matches[0] = 0;

    a_matches[1] = 0;
    b_matches[1] = 0;

    for (uint i = 2; i < NCols; ++i) {
      a_matches[i] = a_cols[i-2] & a_cols[i-1] & a_cols[i];
      b_matches[i] = b_cols[i-2] & b_cols[i-1] & b_cols[i];

      a_matches[i-1] |= a_matches[i];
      a_matches[i-2] |= a_matches[i];

      b_matches[i-1] |= b_matches[i];
      b_matches[i-2] |= b_matches[i];
    }

    for (uint i = 0; i < NCols; ++i) {
      auto a_match = a_cols[i] & (a_cols[i] >> 1) & (a_cols[i] >> 2);
      auto b_match = b_cols[i] & (b_cols[i] >> 1) & (b_cols[i] >> 2);

      a_matches[i] |= a_match | (a_match << 1) | (a_match << 2);
      b_matches[i] |= b_match | (b_match << 1) | (b_match << 2);
    }

    for (uint i = 0; i < NCols; ++i) {
      for (uint j = 0; j < NCols; ++j) {
        if (0 != ((a_matches[i] | b_matches[i]) & (1 << j))) {
          column_t lo_mask = (1 << j) - 1;
          column_t hi_mask = ~lo_mask << 1;

          a_cols[i] = ((a_cols[i] & hi_mask) >> 1) | (a_cols[i] & lo_mask);
          b_cols[i] = ((b_cols[i] & hi_mask) >> 1) | (b_cols[i] & lo_mask);
        }
      }
      printf("a:[%u] %02x\n", i, a_cols[i]);
      printf("b:[%u] %02x\n", i, b_cols[i]);
    }
    printf("\n");

    column_t a_prev = 0;
    column_t b_prev = 0;

    for (uint i = 0; i < NCols; ++i) {
      auto a_match = a_matches[i];
      auto b_match = b_matches[i];

      a_matches[i] &= ~a_prev & ~(a_matches[i] >> 1);
      b_matches[i] &= ~b_prev & ~(b_matches[i] >> 1);

      a_prev = a_match;
      b_prev = b_match;
    }

    for (uint i = 0; i < NCols; ++i) {
      //nmatches += popcount(a_matches[i]) + popcount(b_matches[i]);
      if (a_matches[i] != 0 || b_matches[i] != 0) {
        nmatches += 1;
        break;
      }
    }

  } while (nmatches > prev_nmatches);
  return nmatches;
}

int main()
{
  std::vector<uint> maximums(NRows * NCols);
  std::vector<board_t> boards(NRows * NCols);
  for (uint i = 0; i < NRows * NCols; ++i) {
    maximums[i] = 0;
    boards[i] = 0;
  }

  //assert( 1 == simulate(((board_t)1 << (NRows * NCols))-1) );

  ////const board_t lim = ((board_t) 1 << (NRows * NCols));
  //const board_t lim = 1000000;
  //for (board_t board = 0; board < lim; ++board) {
  //  uint nmatches = simulate(board);
  //  uint a_count = popcount(board);
  //  if (maximums[a_count] < nmatches) {
  //    maximums[a_count] = nmatches;
  //    boards[a_count] = board;
  //  }
  //}

  //for (uint i = 0; i < NRows * NCols / 2; ++i) {
  //  std::cout << i << ": " << maximums[i] << std::endl;
  //}

  std::cout << "30: " << simulate(((board_t)1 << (NRows * NCols))-1) << std::endl;
  return 0;
}

