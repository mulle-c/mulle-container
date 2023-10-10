/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1992, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * (adapted to mulle-c by Nat!)
 */
#ifndef mulle_qsort__h__
#define mulle_qsort__h__

#include "include.h"

#include <stddef.h>



typedef int   mulle_qsort_cmp_t( void *a, void *b, void *thunk);

#define MULLE_QSORT_MIN( a, b)   ((a) < (b) ? a : b)

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */

static inline void   mulle_qsort_swapfunc(char *a, char *b, size_t es)
{
   char t;

   do {
      t = *a;
      *a++ = *b;
      *b++ = t;
   } while (--es > 0);
}

static inline void   mulle_qsort_vecswap(char *a, char *b, size_t es)
{
   if( es > 0)
      mulle_qsort_swapfunc( a, b, es);
}


static inline char *
_mulle_qsort_med3( char *a, char *b, char *c, mulle_qsort_cmp_t *cmp, void *thunk)
{
   return (*cmp)( a, b, thunk) < 0
          ? ((*cmp)( b, c, thunk) < 0 ? b : ((*cmp)( a, c, thunk) < 0 ? c : a ))
          : ((*cmp)( b, c, thunk) > 0 ? b : ((*cmp)( a, c, thunk) < 0 ? a : c ));
}


static inline void   _mulle_qsort_r_inline( void *a,
                                            size_t n,
                                            size_t es,
                                            mulle_qsort_cmp_t *cmp,
                                            void *thunk)
{
   char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
   size_t d1, d2;
   int cmp_result;
   int swap_cnt;

   /* if there are less than 2 elements, then sorting is not needed */
   if( n < 2)
      return;
loop:
   swap_cnt = 0;
   if (n < 7)
   {
      for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
         for (pl = pm;
              pl > (char *)a && (*cmp)( pl - es, pl, thunk) > 0;
              pl -= es)
            mulle_qsort_swapfunc(pl, pl - es, es);
      return;
   }

   pm = (char *)a + (n / 2) * es;
   if (n > 7)
   {
      pl = a;
      pn = (char *)a + (n - 1) * es;
      if (n > 40)
      {
         size_t d = (n / 8) * es;

         pl = _mulle_qsort_med3( pl, pl + d, pl + 2 * d, cmp, thunk);
         pm = _mulle_qsort_med3( pm - d, pm, pm + d, cmp, thunk);
         pn = _mulle_qsort_med3( pn - 2 * d, pn - d, pn, cmp, thunk);
      }
      pm = _mulle_qsort_med3( pl, pm, pn, cmp, thunk);
   }
   mulle_qsort_swapfunc(a, pm, es);
   pa = pb = (char *)a + es;

   pc = pd = (char *)a + (n - 1) * es;
   for (;;)
   {
      while (pb <= pc && (cmp_result = (*cmp)( pb, a, thunk)) <= 0)
      {
         if (cmp_result == 0)
         {
            swap_cnt = 1;
            mulle_qsort_swapfunc(pa, pb, es);
            pa += es;
         }
         pb += es;
      }

      while (pb <= pc && (cmp_result = (*cmp)( pc, a, thunk)) >= 0)
      {
         if (cmp_result == 0)
         {
            swap_cnt = 1;
            mulle_qsort_swapfunc(pc, pd, es);
            pd -= es;
         }
         pc -= es;
      }
      if (pb > pc)
         break;

      mulle_qsort_swapfunc(pb, pc, es);
      swap_cnt = 1;
      pb += es;
      pc -= es;
   }

   if( swap_cnt == 0)
   {  /* Switch to insertion sort */
      for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
         for (pl = pm;
              pl > (char *)a && (*cmp)( pl - es, pl, thunk) > 0;
              pl -= es)
            mulle_qsort_swapfunc(pl, pl - es, es);
      return;
   }

   pn = (char *)a + n * es;
   d1 = MULLE_QSORT_MIN(pa - (char *)a, pb - pa);
   mulle_qsort_vecswap(a, pb - d1, d1);

   d1 = MULLE_QSORT_MIN(pd - pc, pn - &pd[ es]);
   mulle_qsort_vecswap(pb, pn - d1, d1);

   d1 = pb - pa;
   d2 = pd - pc;
   if (d1 <= d2)
   {
      /* Recurse on left partition, then iterate on right partition */
      if (d1 > es)
      {
         _mulle_qsort_r_inline(a, d1 / es, es, cmp, thunk);
      }
      if (d2 > es)
      {
         /* Iterate rather than recurse to save stack space */
         /* mulle_qsort(pn - d2, d2 / es, es, cmp); */
         a = pn - d2;
         n = d2 / es;
         goto loop;
      }
   } else
   {
      /* Recurse on right partition, then iterate on left partition */
      if (d2 > es)
      {
         _mulle_qsort_r_inline(pn - d2, d2 / es, es, cmp, thunk);
      }
      if (d1 > es)
      {
         /* Iterate rather than recurse to save stack space */
         /* mulle_qsort(a, d1 / es, es, cmp); */
         n = d1 / es;
         goto loop;
      }
   }
}


MULLE_C_NONNULL_FOURTH
void   mulle_qsort_r( void *a, size_t n, size_t es, mulle_qsort_cmp_t *cmp, void *thunk);


MULLE_C_NONNULL_FOURTH
static inline void   mulle_qsort( void *a, size_t n, size_t es, mulle_qsort_cmp_t *cmp)
{
   mulle_qsort_r( a, n, es, cmp, NULL);
}

#endif
