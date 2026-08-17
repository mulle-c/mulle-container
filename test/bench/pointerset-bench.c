//
//  pointerset-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_pointerset with
//  synthetic pointer values (no address-dependent hashing).
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_insert_member( void)
{
   struct mulle_pointerset       set;
   struct bench_allocator        bench;
   size_t                        i;
   size_t                        hits;

   bench_allocator_init( &bench);

   mulle_pointerset_init( &set, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_pointerset_insert( &set, (void *) (intptr_t) (i + 1));

   printf( "insert %d:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_pointerset_get_count( &set),
           mulle_pointerset_get_size( &set));

   hits = 0;
   for( i = 0; i < N; i++)
      hits += (mulle_pointerset_get( &set, (void *) (intptr_t) (i + 1)) != NULL);
   for( i = 0; i < N; i++)
      hits += (mulle_pointerset_get( &set, (void *) (intptr_t) (i + N + 1)) != NULL);

   printf( "get: hits=%zu\n", hits);
   mulle_pointerset_done( &set);
}


int  main( void)
{
   bench_warn_if_debug();
   bench_insert_member();
   return( 0);
}
