//
//  array-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_array with an
//  intptr callback (no retain/release traffic), growth from 0 vs.
//  preallocated, plus member lookup cost via comparison counting.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_growth( void)
{
   struct mulle_array          *array;
   struct bench_allocator      bench;
   size_t                      i;

   bench_allocator_init( &bench);

   array = mulle_array_create( 0, &mulle_container_keycallback_intptr, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_array_add( array, (void *) (intptr_t) i);

   printf( "add %d from capacity 0:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_array_get_count( array),
           mulle_array_get_size( array));

   mulle_array_destroy( array);
}


static void  bench_preallocated( void)
{
   struct mulle_array          *array;
   struct bench_allocator      bench;
   size_t                      i;

   bench_allocator_init( &bench);

   array = mulle_array_create( N, &mulle_container_keycallback_intptr, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_array_add( array, (void *) (intptr_t) i);

   printf( "add %d preallocated:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_array_get_count( array),
           mulle_array_get_size( array));

   mulle_array_destroy( array);
}


static void  bench_member( void)
{
   struct mulle_array          *array;
   struct bench_allocator      bench;
   size_t                      i;
   int                         hits;

   bench_allocator_init( &bench);

   array = mulle_array_create( N, &mulle_container_keycallback_intptr, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_array_add( array, (void *) (intptr_t) i);

   hits = 0;
   for( i = 0; i < N; i++)
      hits += mulle_array_member( array, (void *) (intptr_t) i);
   // all of these are misses and must walk the whole array
   for( i = 0; i < N; i++)
      hits += mulle_array_member( array, (void *) (intptr_t) (i + N));

   printf( "member: hits=%d (linear scan, %d lookups)\n", hits, 2 * N);
   mulle_array_destroy( array);
}


int  main( void)
{
   bench_growth();
   bench_preallocated();
   bench_member();
   return( 0);
}
