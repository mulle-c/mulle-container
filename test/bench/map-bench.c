//
//  map-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_map with intptr
//  keys. Bucket growth is count-based, so allocation counts are stable.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static struct mulle_container_keyvaluecallback   g_callback;


static void  bench_insert_lookup( void)
{
   struct mulle_map             *map;
   struct bench_allocator       bench;
   size_t                       i;
   size_t                       hits;

   bench_allocator_init( &bench);

   map = mulle_map_create( 0, &g_callback, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_map_set( map, (void *) (intptr_t) (i + 1), (void *) (intptr_t) ((i + 1) * 2));

   printf( "insert %d:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_map_get_count( map),
           mulle_map_get_size( map));

   hits = 0;
   for( i = 0; i < N; i++)
      hits += (mulle_map_get( map, (void *) (intptr_t) (i + 1)) != NULL);
   // misses: the map is dense, so misses probe longer
   for( i = 0; i < N; i++)
      hits += (mulle_map_get( map, (void *) (intptr_t) (i + N + 1)) != NULL);

   printf( "lookup: hits=%zu\n", hits);
   mulle_map_destroy( map);
}


static void  bench_insert_remove( void)
{
   struct mulle_map             *map;
   struct bench_allocator       bench;
   size_t                       i;

   bench_allocator_init( &bench);

   map = mulle_map_create( 0, &g_callback, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_map_set( map, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));

   for( i = 0; i < N / 2; i++)
      mulle_map_remove( map, (void *) (intptr_t) (i * 2 + 1));

   printf( "insert %d, remove half:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_map_get_count( map),
           mulle_map_get_size( map));

   mulle_map_destroy( map);
}


int  main( void)
{
   bench_warn_if_debug();
   g_callback.keycallback   = mulle_container_keycallback_intptr;
   g_callback.valuecallback = mulle_container_valuecallback_intptr;

   bench_insert_lookup();
   bench_insert_remove();
   return( 0);
}
