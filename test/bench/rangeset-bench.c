//
//  rangeset-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle__rangeset when
//  inserting disjoint and overlapping ranges, plus coalescing behavior.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_disjoint( void)
{
   struct mulle__rangeset        set;
   struct bench_allocator        bench;
   size_t                        i;

   bench_allocator_init( &bench);

   _mulle__rangeset_init( &set, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      _mulle__rangeset_insert( &set, mulle_range_make( i * 2, 1), &bench.allocator);

   printf( "insert %d disjoint:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  rangecount: %lu\n", (unsigned long) _mulle__rangeset_get_rangecount( &set));

   _mulle__rangeset_done( &set, &bench.allocator);
}


static void  bench_overlapping( void)
{
   struct mulle__rangeset        set;
   struct bench_allocator        bench;
   size_t                        i;

   bench_allocator_init( &bench);

   _mulle__rangeset_init( &set, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      _mulle__rangeset_insert( &set, mulle_range_make( i, 1), &bench.allocator);

   printf( "insert %d overlapping:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  rangecount: %lu\n", (unsigned long) _mulle__rangeset_get_rangecount( &set));

   _mulle__rangeset_done( &set, &bench.allocator);
}


static void  bench_remove( void)
{
   struct mulle__rangeset        set;
   struct bench_allocator        bench;
   size_t                        i;

   bench_allocator_init( &bench);

   _mulle__rangeset_init( &set, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      _mulle__rangeset_insert( &set, mulle_range_make( i * 2, 1), &bench.allocator);

   for( i = 0; i < N; i += 2)
      _mulle__rangeset_remove( &set, mulle_range_make( i, 1), &bench.allocator);

   printf( "insert %d disjoint, remove every other:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  rangecount: %lu\n", (unsigned long) _mulle__rangeset_get_rangecount( &set));

   _mulle__rangeset_done( &set, &bench.allocator);
}


int  main( void)
{
   bench_warn_if_debug();
   bench_disjoint();
   bench_overlapping();
   bench_remove();
   return( 0);
}
