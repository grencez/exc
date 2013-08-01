
#include "cx/syscx.h"

#include "cx/bittable.h"
#include "cx/fileb.h"

#include <gmp.h>

DeclTableT( mpq_t, mpq_t );

struct XmQ
{
    TableT(mpq_t) t;
    uint sz[2];
};
typedef struct XmQ XmQ[1];


    void
size_fo_XmQ (XmQ A, uint m, uint n)
{
    uint sz = m * n;
    A->sz[0] = m;
    A->sz[1] = n;

    if (A->t.sz < sz)
    {
        uint oldsz = A->t.sz;

        SizeTable( A->t, m*m );

        for (uint i = oldsz; i < A->t.sz; ++i)
            mpq_init (A->t.s[i]);
    }
}

    void
size1_fo_XmQ (XmQ A, uint m)
{
    size_fo_XmQ (A, m, m);
}

    mpq_t*
row_of_XmQ (XmQ A, uint i)
{
    return &A->t.s[i * A->sz[1]];
}

    mpq_t*
cell_of_XmQ (const XmQ A, uint i, uint j)
{
    return &A->t.s[i * A->sz[1] + j];
}

    void
cellui2_fo_XmQ (XmQ A, uint i, uint j, uint p, uint q)
{
    mpq_set_ui (*cell_of_XmQ (A, i, j), p, q);
}

    void
cellui1_fo_XmQ (XmQ A, uint i, uint j, uint p)
{
    cellui2_fo_XmQ (A, i, j, p, 1);
}

    void
celli2_fo_XmQ (XmQ A, uint i, uint j, int p, int q)
{
    bool neg = false;
    mpq_t* c = cell_of_XmQ (A, i, j);
    if (p < 0)
    {
        neg = !neg;
        p = - p;
    }
    if (q < 0)
    {
        neg = !neg;
        q = - p;
    }
    mpq_set_ui (*c, p, q);
    if (neg)  mpq_neg (*c, *c);
}

    void
celli1_fo_XmQ (XmQ A, uint i, uint j, int p)
{
    celli2_fo_XmQ (A, i, j, p, 1);
}

    void
init_XmQ (XmQ A)
{
    InitTable( A->t );
}

    void
init1_XmQ (XmQ A, uint m)
{
    init_XmQ (A);
    size_fo_XmQ (A, m, m);
    for (uint i = 0; i < m; ++i)
        cellui1_fo_XmQ (A, i, i, 1);
}

    void
init2_XmQ (XmQ A, uint m, uint n)
{
    init_XmQ (A);
    size_fo_XmQ (A, m, n);
}

    void
lose_XmQ (XmQ A)
{
    for (uint i = 0; i < A->t.sz; ++i)
        mpq_clear (A->t.s[i]);
    LoseTable( A->t );
}

  void
copy_XmQ (XmQ dst, const XmQ src)
{
  size_fo_XmQ (dst, src->sz[0], src->sz[1]);
  {:for (i ; dst->sz[0])
    {:for (j ; dst->sz[1])
      mpq_set (*cell_of_XmQ (dst, i, j),
               *cell_of_XmQ (src, i, j));
    }
  }
}

    void
mul_XmQ (XmQ C, const XmQ A, const XmQ B)
{
    mpq_t prod;
    mpq_init (prod);

    size_fo_XmQ (C, A->sz[0], B->sz[1]);
    for (uint i = 0; i < A->sz[0]; ++i)
    {
        for (uint j = 0; j < B->sz[1]; ++j)
        {
            mpq_t* c = cell_of_XmQ (C, i, j);
            mpq_set_ui (*c, 0, 1);

            for (uint k = 0; k < B->sz[0]; ++k)
            {
                mpq_mul (prod, *cell_of_XmQ (A, i, k), *cell_of_XmQ (B, k, j));
                mpq_add (*c, *c, prod);
            }
        }
    }
    mpq_clear (prod);
}

    uint
oputsz_mpq_t (const mpq_t x)
{
    char buf[1];
    return gmp_snprintf (buf, 0, "%Qd", x);
}

    uint
oputsz_col_XmQ (const XmQ A, uint j)
{
    uint max = 0;
    for (uint i = 0; i < A->sz[0]; ++i)
    {
        uint sz = oputsz_mpq_t (*cell_of_XmQ (A, i, j));
        if (sz > max)  max = sz;
    }
    return max;
}

    void
oputw_mpq_t (OFile* of, const mpq_t x, uint w)
{
    //int n = oputsz_mpq_t (x);

    EnsizeTable( of->buf, of->off + w + 1 );
    gmp_snprintf ((char*) &of->buf.s[of->off], w + 1, "%*Qd", w, x);
    of->off += w;
    mayflush_OFile (of);
}

    void
oput_XmQ (OFile* of, const XmQ A)
{
    DeclTable( uint, cols );
    SizeTable( cols, A->sz[1] );

    for (uint j = 0; j < A->sz[1]; ++j)
        cols.s[j] = oputsz_col_XmQ (A, j);

    for (uint i = 0; i < A->sz[0]; ++i)
    {
        for (uint j = 0; j < A->sz[1]; ++j)
        {
            oputw_mpq_t (of, *cell_of_XmQ (A, i, j), cols.s[j]);
            if (j + 1 < A->sz[1])
                oput_char_OFile (of, ' ');
            else
                oput_char_OFile (of, '\n');
        }
    }
    LoseTable( cols );
}

    void
polystep_XmQ (XmQ G, uint n)
{
    size1_fo_XmQ (G, n+1);

    cellui1_fo_XmQ (G, 0, 0, 1);
    for (uint i = 1; i < G->sz[1]; ++i)
    {
        cellui1_fo_XmQ (G, i, 0, 0);
        cellui1_fo_XmQ (G, 0, i, 0);
    }

    for (uint i = 1; i < G->sz[0]; ++i)
    {
        for (uint j = 1; j < G->sz[1]; ++j)
        {
            mpq_t* c = cell_of_XmQ (G, i, j);
            mpq_set (*c, *cell_of_XmQ (G, i, j-1));

            mpz_mul_ui (mpq_numref (*c),
                        mpq_numref (*c),
                        i);
            mpq_canonicalize (*c);

            mpq_sub (*c, *cell_of_XmQ (G, i-1, j-1), *c);
        }
    }
}

    void
polystep_inv_XmQ (XmQ G, uint n)
{
    size1_fo_XmQ (G, n+1);

    cellui1_fo_XmQ (G, 0, 0, 1);
    for (uint i = 1; i < G->sz[1]; ++i)
    {
        cellui1_fo_XmQ (G, i, 0, 0);
        cellui1_fo_XmQ (G, 0, i, 0);
    }

    for (uint i = 1; i < G->sz[0]; ++i)
    {
        for (uint j = 1; j < G->sz[1]; ++j)
        {
            mpq_t* c = cell_of_XmQ (G, i, j);
            mpq_set (*c, *cell_of_XmQ (G, i, j-1));

            mpz_mul_ui (mpq_numref (*c),
                        mpq_numref (*c),
                        j-1);
            mpq_canonicalize (*c);

            mpq_add (*c, *cell_of_XmQ (G, i-1, j-1), *c);
        }
    }
}

    void
polystep_step_XmQ (XmQ S, uint n)
{
    size1_fo_XmQ (S, n+1);
    for (uint i = 0; i < S->sz[0]; ++i)
        for (uint j = 0; j < S->sz[1]; ++j)
            cellui1_fo_XmQ (S, i, j, (i <= j) ? 1 : 0);
}

    void
polystep_fac_XmQ (XmQ A, uint n)
{
    size1_fo_XmQ (A, n+1);
    for (uint i = 0; i < A->sz[0]; ++i)
        for (uint j = 0; j < A->sz[1]; ++j)
            cellui1_fo_XmQ (A, i, j, 0);

    cellui1_fo_XmQ (A, 0, 0, 1);
    for (uint i = 1; i < A->sz[0]; ++i)
    {
        mpq_t* c = cell_of_XmQ (A, i, i);
        mpq_set (*c, *cell_of_XmQ (A, i-1, i-1));
        mpz_mul_ui (mpq_numref (*c),
                    mpq_numref (*c),
                    i);
        mpq_canonicalize (*c);
    }
}

    void
polystep_scale_XmQ (XmQ A, uint n, mpq_t a)
{
    size1_fo_XmQ (A, n+1);
    for (uint i = 0; i < A->sz[0]; ++i)
        for (uint j = 0; j < A->sz[1]; ++j)
            cellui1_fo_XmQ (A, i, j, 0);

    cellui1_fo_XmQ (A, 0, 0, 1);
    for (uint i = 1; i < A->sz[0]; ++i)
        mpq_mul (*cell_of_XmQ (A, i, i),
                 *cell_of_XmQ (A, i-1, i-1),
                 a);
}

    void
posystep_fac_G_scale_XmQ (XmQ C, uint n, mpq_t a)
{
    XmQ A, B;

    init1_XmQ (A, n);
    init1_XmQ (B, n);

    polystep_fac_XmQ (C, n);
    polystep_XmQ (B, n);
    mul_XmQ (A, C, B);
    polystep_scale_XmQ (B, n, a);
    mul_XmQ (C, A, B);

    lose_XmQ (A);
    lose_XmQ (B);
}

    void
do_something ()
{
    OFile* of = stdout_OFile ();
    XmQ A, B, C;
    XmQ G;
    init1_XmQ (A, 3);
    init1_XmQ (B, 3);
    init1_XmQ (C, 3);
    init_XmQ (G);

    cellui1_fo_XmQ (A, 0, 0, 3);
    cellui1_fo_XmQ (A, 0, 1, 2);
    cellui1_fo_XmQ (A, 0, 2, 1);

    cellui1_fo_XmQ (B, 0, 0, 7);
    cellui1_fo_XmQ (B, 0, 1, 5);
    cellui1_fo_XmQ (B, 0, 2, 3);

    mul_XmQ (C, A, B);

    oput_XmQ (of, A);
    oput_char_OFile (of, '\n');
    oput_XmQ (of, B);
    oput_char_OFile (of, '\n');
    oput_XmQ (of, C);
    oput_char_OFile (of, '\n');

    polystep_XmQ (G, 10);
    polystep_inv_XmQ (A, 10);
    mul_XmQ (B, G, A);

    polystep_fac_XmQ (B, 20);

    {
        mpq_t a;
        XmQ f, g;

        mpq_init (a);
        mpq_set_ui (a, 1, 1);
        init_XmQ (g);

        init2_XmQ (f, 4, 1);
        celli1_fo_XmQ (f, 0, 0,  7);
        celli1_fo_XmQ (f, 1, 0, -2);
        celli1_fo_XmQ (f, 2, 0,  1);
        celli1_fo_XmQ (f, 3, 0,  1);

        posystep_fac_G_scale_XmQ (B, f->sz[0]-1, a);

        mul_XmQ (g, B, f);
        oput_XmQ (of, g);
        lose_XmQ (f);
        lose_XmQ (g);
        mpq_clear (a);
    }

    oput_XmQ (of, A);
    //oput_XmQ (of, B);
    oput_char_OFile (of, '\n');

    lose_XmQ (A);
    lose_XmQ (B);
    lose_XmQ (C);
    lose_XmQ (G);
}

int main(int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);

    if (argi < argc)  return 1;

    do_something ();

    {
      OFile* of = stdout_OFile ();
      uint m = 10;
      XmQ A, B, F;

      init1_XmQ (A, m);
      init1_XmQ (B, m);

      polystep_XmQ (A, m-1);

      init1_XmQ (F, m);

      {:for (uint i = 2; i < m; ++i)
        mpq_t* c = cell_of_XmQ (F, i, i);

        mpz_mul_ui (mpq_numref (*c),
                    mpq_numref (*cell_of_XmQ (F, i-1, i-1)),
                    i);
        mpq_canonicalize (*c);
      }

      oput_XmQ (of, F);
      oput_char_OFile (of, '\n');
      oput_XmQ (of, A);
      oput_char_OFile (of, '\n');


      mul_XmQ (B, F, A);

      oput_XmQ (of, B);
      oput_char_OFile (of, '\n');

      lose_XmQ (A);
      lose_XmQ (B);
      lose_XmQ (F);
    }

    lose_sysCx ();
    return 0;
}

