
#include "cx/def.h"
#include "cx/syscx.h"

#include <bdd.h>
#include <bvec.h>

int main (int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    const int xdomsz = 4;
    int xdoms[3];

    int xfddidcs[3];
    bdd ybdds[3];
    bdd satbdd;
    int off;

    bdd a, b, c, d, e;

    if (argi < argc)  return 1;

    xdoms[0] = xdoms[1] = xdoms[2] = xdomsz;

    bdd_init (1000,100);
    push_losefn_sysCx (bdd_done);

    off = bdd_extvarnum (1);
    satbdd = bdd_ithvar (off);

    xfddidcs[0] = fdd_extdomain (xdoms, 3);
    xfddidcs[1] = xfddidcs[0] + 1;
    xfddidcs[2] = xfddidcs[1] + 1;

    off = bdd_extvarnum (3);
    printf ("y0:%d y1:%d y2:%d\n", off + 0, off + 1, off + 2);
    ybdds[0] = bdd_ithvar (off + 0);
    ybdds[1] = bdd_ithvar (off + 1);
    ybdds[2] = bdd_ithvar (off + 2);

    {
        int cnt = bdd_getnodenum ();
        printf ("Start with %d nodes!\n", cnt);
    }


    d = satbdd;
    {:for (i ; 3)
        a = fdd_equals (xfddidcs[i], xfddidcs[(i+1)%3]);
        a = bdd_addref (a);

        b = bdd_biimp (ybdds[i], ybdds[(i+1)%3]);
        b = bdd_addref (b);

        e = bdd_imp (a, b);
        e = bdd_addref (e);
        bdd_delref (a);
        bdd_delref (b);

        c = d;
        d = bdd_and (c, e);
        d = bdd_addref (d);
        bdd_delref (c);
        bdd_delref (e);
    }

#if 0
    a = bdd_satone (d);
    a = bdd_addref (a);

    fdd_printset (a);
    fputc ('\n', stdout);
    bdd_printset (a);
    fputc ('\n', stdout);
    bdd_delref (a);
#endif

    bdd_printtable (d);
    bdd_delref (d);
        /* fdd_clearall (); */

    {
        int cnt;
        bdd_gbc ();
        cnt = bdd_getnodenum ();
        printf ("Still have %d nodes!\n", cnt);
    }

    lose_sysCx ();
    return 0;
}

