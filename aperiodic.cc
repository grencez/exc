
extern "C" {
#include "cx/syscx.h"
#include "cx/fileb.h"
}
#include "cx/synhax.hh"

#include "cx/set.hh"
#include "cx/table.hh"

  uint
LookupSymId (map< Cx::Table<uint>, uint >& idmap, const Cx::Table<uint>& key)
{
  uint* p = MapLookup(idmap, key);
  if (p)  return *p;

  uint id = sz_of (idmap);
  idmap[key] = id;
  return id;
}

  void
SelfDisableTiles (Set< Cx::Table<uint> >& acts)
{
  map< Cx::Table<uint>, uint > idmap;
  const uint blank = LookupSymId (idmap, Cx::Table<uint>());
  Set< Cx::Table<uint> > newacts;

  Set< Cx::Table<uint> >::iterator it;
  for (it = acts.begin(); it != acts.end(); ++it) {
    const Cx::Table<uint>& act = *it;
    Cx::Table<uint> sym;

    sym.push(act[0]);
    sym.push(act[1]);
    sym.push(act[2]);
    const uint actsym = LookupSymId (idmap, sym);
    sym.mpop(1);

    sym[0] = act[0];
    sym[1] = 0;
    const uint a_ri = LookupSymId (idmap, sym);
    sym[0] = act[1];
    sym[1] = 1;
    const uint b_up = LookupSymId (idmap, sym);
    sym[0] = act[2];
    sym[1] = 0;
    const uint c_ri = LookupSymId (idmap, sym);
    sym[0] = act[2];
    sym[1] = 1;
    const uint c_up = LookupSymId (idmap, sym);

    Cx::Table<uint> tile;
    tile.push(0);  tile.push(0);  tile.push(0);

    tile[0] = a_ri;
    tile[1] = b_up;
    tile[2] = actsym;
    newacts |= tile;

    tile[0] = actsym;
    tile[1] = blank;
    tile[2] = c_ri;
    newacts |= tile;

    tile[0] = blank;
    tile[1] = actsym;
    tile[2] = c_up;
    newacts |= tile;

    tile[0] = c_up;
    tile[1] = c_ri;
    tile[2] = blank;
    newacts |= tile;
  }
  acts = newacts;
}

int main (int argc, char** argv)
{
  init_sysCx (&argc, &argv);
  const uint wtiles[][4] = {
    { 1, 1, 2, 2 },
    { 1, 5, 1, 4 },
    { 2, 3, 2, 6 },
    { 2, 4, 1, 6 },
    { 2, 5, 1, 3 },
    { 3, 2, 6, 2 },
    { 3, 3, 4, 4 },
    { 3, 4, 5, 4 },
    { 3, 6, 3, 4 },
    { 4, 2, 6, 1 },
    { 4, 3, 4, 5 },
    { 4, 4, 5, 5 },
    { 5, 1, 4, 1 },
    { 5, 2, 3, 1 },
    { 6, 3, 4, 3 },
    { 6, 6, 3, 3 }
  };
  const uint nwtiles = ArraySz( wtiles );
  Set< Cx::Table<uint> > acts;

  for (uint i = 0; i < nwtiles; ++i) {
    Cx::Table<uint> as, bs;
    for (uint j = 0; j < nwtiles; ++j) {
      if (wtiles[i][0] == wtiles[j][2]) {
        as.push(j);
      }
      if (wtiles[i][1] == wtiles[j][3]) {
        bs.push(j);
      }
    }
    for (uint j = 0; j < as.sz(); ++j) {
      for (uint k = 0; k < bs.sz(); ++k) {
        Cx::Table<uint> act;
        act.push(as[j]);
        act.push(bs[k]);
        act.push(i);
        acts |= act;
      }
    }
  }

  SelfDisableTiles (acts);

  Set< Cx::Table<uint> >::iterator it;
#if 0
  for (it = acts.begin(); it != acts.end(); ++it) {
    OFileB* of = stdout_OFileB ();
    printf_OFileB (of, "%3u %3u %3u\n",
                   (*it)[0], (*it)[1], (*it)[2]);
  }
#else
  OFileB* of = stdout_OFileB ();
#define OPut(s)  oput_cstr_OFileB (of, s);  oput_char_OFileB (of, '\n');
  OPut( "#define N 5" );
  OPut( "byte x[N];" );
  OPut( "#define x_lo x[(_pid+N-1)%N]" );
  OPut( "#define x_me x[_pid]" );
  OPut( "#define UniAct(a,b,c)  atomic { (x_lo == a) && (x_me == b) -> x_me = c; }" );
  OPut( "active[N] proctype MainP()" );
  OPut( "{" );
  OPut( "  atomic {" );
  OPut( "    byte tmp;" );
  uint dom_max = 0;
  for (it = acts.begin(); it != acts.end(); ++it) {
    for (uint j = 0; j < 3; ++j) {
      if ((*it)[j] > dom_max)
        dom_max = (*it)[j];
    }
  }

  printf_OFileB (of, "    select(tmp : 0..%u);\n", dom_max);
  OPut( "    x_me = tmp;" );
  OPut( "  }" );
  OPut( "  do" );

  for (it = acts.begin(); it != acts.end(); ++it) {
    printf_OFileB (of, "  :: UniAct( %3u, %3u, %3u )\n",
                   (*it)[0], (*it)[1], (*it)[2]);
  }
  OPut( "  od;" );
  OPut( "}" );
#undef OPut
#endif

  lose_sysCx ();
  return 0;
}

