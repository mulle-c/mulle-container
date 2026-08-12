//
//  pointermap-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle__pointermap
//  with synthetic pointer keys (no address-dependent hashing).
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_insert_lookup( void)
{
   struct mulle__pointermap     map;
   struct bench_allocator       bench;
   size_t                       i;
   size_t                       hits;

   bench_allocator_init( &bench);

   _mulle__pointermap_init( &map, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle__pointermap_set( &map, (void *) (intptr_t) (i + 1), (void *) (intptr_t) ((i + 1) * 2), &bench.allocator);

   printf( "insert %d:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle__pointermap_get_count( &map),
           mulle__pointermap_get_size( &map));

   hits = 0;
   for( i = 0; i < N; i++)
      hits += (mulle__pointermap_get( &map, (void *) (intptr_t) (i + 1)) != NULL);
   for( i = 0; i < N; i++)
      hits += (mulle__pointermap_get( &map, (void *) (intptr_t) (i + N + 1)) != NULL);

   printf( "lookup: hits=%zu\n", hits);
   _mulle__pointermap_done( &map, &bench.allocator);
}


int  main( void)
{
   bench_insert_lookup();
   return( 0);
}
