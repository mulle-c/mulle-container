//
//  rangemap-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle__rangemap for
//  insert, exact search and remove.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_insert_search( void)
{
   struct mulle__rangemap        map;
   struct bench_allocator        bench;
   size_t                        i;
   size_t                        hits;

   bench_allocator_init( &bench);

   _mulle__rangemap_init( &map, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      _mulle__rangemap_insert( &map, mulle_range_make( i * 2, 1), (void *) (intptr_t) (i + 1), &bench.allocator);

   printf( "insert %d:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", _mulle__rangemap_get_count( &map));

   hits = 0;
   for( i = 0; i < N; i++)
      hits += (_mulle__rangemap_search( &map, i * 2) != mulle_not_found_e);
   for( i = 0; i < N; i++)
      hits += (_mulle__rangemap_search( &map, i * 2 + 1) != mulle_not_found_e);

   printf( "search: hits=%zu\n", hits);
   _mulle__rangemap_done( &map, &bench.allocator);
}


static void  bench_insert_remove( void)
{
   struct mulle__rangemap        map;
   struct bench_allocator        bench;
   size_t                        i;

   bench_allocator_init( &bench);

   _mulle__rangemap_init( &map, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      _mulle__rangemap_insert( &map, mulle_range_make( i * 2, 1), (void *) (intptr_t) (i + 1), &bench.allocator);

   for( i = 0; i < N; i += 2)
      _mulle__rangemap_remove( &map, mulle_range_make( i, 1), &bench.allocator);

   printf( "insert %d, remove every other:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", _mulle__rangemap_get_count( &map));

   _mulle__rangemap_done( &map, &bench.allocator);
}


int  main( void)
{
   bench_insert_search();
   bench_insert_remove();
   return( 0);
}
