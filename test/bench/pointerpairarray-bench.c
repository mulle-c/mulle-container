//
//  pointerpairarray-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_pointerpairarray
//  and the number of comparisons in qsort (measured with a counting
//  comparator) for LCG-shuffled keys.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static uintptr_t  g_seed;


static uintptr_t  next_random( void)
{
   g_seed = g_seed * 1664525 + 1013904223;
   return( g_seed);
}


static void  bench_add_sort( void)
{
   struct mulle_pointerpairarray  *array;
   struct bench_allocator         bench;
   size_t                         i;
   size_t                         comparisons;
   size_t                         sorted;

   bench_allocator_init( &bench);
   g_comparisons = 0;

   array = mulle_pointerpairarray_create( &bench.allocator);
   for( i = 0; i < N; i++)
   {
      struct mulle_pointerpair   pair;

      pair.key   = (void *) (intptr_t) (next_random() % N);
      pair.value = (void *) (intptr_t) i;
      mulle_pointerpairarray_add( array, pair);
   }

   printf( "add %d (shuffled):\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_pointerpairarray_get_count( array));

   g_comparisons = 0;
   mulle_pointerpairarray_qsort_r( array, bench_compare_intptr, NULL);
   comparisons = g_comparisons;

   // verify sortedness
   sorted = 1;
   for( i = 1; i < N; i++)
   {
      if( (intptr_t) mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) array, i - 1).key >
          (intptr_t) mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) array, i).key)
      {
         sorted = 0;
         break;
      }
   }

   printf( "qsort: comparisons=%zu sorted=%d\n", comparisons, sorted);
   mulle_pointerpairarray_destroy( array);
}


int  main( void)
{
   bench_warn_if_debug();
   g_seed = 0x12345678;
   bench_add_sort();
   return( 0);
}
