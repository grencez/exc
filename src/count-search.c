
#include "cx/syscx.h"

#include "cx/bittable.h"
#include "cx/fileb.h"

  luint
count_search (const TableT(zuint) L, BitTable bt)
{
  luint n = 0;
  if (all_BitTable (bt))  return 1;
  for (zuint i = 0; i < bt.sz; ++i)
  {
    if (ck_BitTable (bt, i))  continue;
    for (zuint j = 0; j < L.sz; j += 2)
    {
      if (L.s[j+1] != i)  continue;
      if (!ck_BitTable (bt, L.s[j]))  continue;

      //DBog3( "%u : %u -- %u",
      //       (uint) count_BitTable (bt) -1,
      //       (uint) L.s[j],
      //       (uint) L.s[j+1] );
      set1_BitTable (bt, L.s[j+1]);
      n += count_search (L, bt);
      set0_BitTable (bt, L.s[j+1]);
      break;
    }
  }
  return n;
}

  zuint
count_nodes (const TableT(zuint) L)
{
  zuint max = 0;
  if (L.sz == 0)  return 0;
  for (zuint j = 0; j < L.sz; ++j)
    if (L.s[j] > max)
      max = L.s[j];
  return max+1;
}

  void
add_edge (TableT(zuint)* L, zuint a, zuint b)
{
  for (zuint i = 0; i < L->sz; i += 2)
  {
    if (L->s[i] == a && L->s[i+1] == b)
    {
      DBog2( "Duplicate edge: %zu -- %zu", a, b );
      return;
    }
  }
  PushTable( *L, a );
  PushTable( *L, b );
  PushTable( *L, b );
  PushTable( *L, a );
}

int main(int argc, char** argv)
{
  int argi = init_sysCx (&argc, &argv);
  DeclTable( zuint, L );
  luint n = 0;
  BitTable bt;
  OFile* of = stdout_OFile ();
  if (argi < argc)  return 1;

  add_edge (&L, 0, 1);
  add_edge (&L, 0, 2);
  add_edge (&L, 0, 3);
  //add_edge (&L, 0, 4);
  add_edge (&L, 1, 2);
  //add_edge (&L, 1, 3);
  //add_edge (&L, 1, 4);
  add_edge (&L, 2, 3);
  //add_edge (&L, 2, 4);
  //add_edge (&L, 3, 4);

  Claim2( L.sz % 2 ,==, 0 );

  bt = cons2_BitTable (count_nodes (L), 0);

  for (zuint i = 0; i < bt.sz; ++i)
  {
    luint m = 0;
    //wipe_BitTable (bt, 0);
    set1_BitTable (bt, i);
    m = count_search (L, bt);
    if (m == 0)
      DBog1( "Node %u cannot reach everybody!", i );
    set0_BitTable (bt, i);
    n += m;
  }

  oput_cstr_OFile (of, "Ways: ");
  oput_luint_OFile (of, n);
  oput_char_OFile (of, '\n');

  lose_BitTable (&bt);
  LoseTable( L );
  lose_sysCx ();
  return 0;
}

