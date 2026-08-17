//
//  set-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_set with intptr
//  keys, plus membership hit/miss counts.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_insert_member( void)
{
   struct mulle_set             *set;
   struct bench_allocator       bench;
   size_t                       i;
   size_t                       hits;

   bench_allocator_init( &bench);

   set = mulle_set_create( 0, &mulle_container_keycallback_intptr, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_set_set( set, (void *) (intptr_t) (i + 1));

   printf( "insert %d:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_set_get_count( set),
           mulle_set_get_size( set));

   hits = 0;
   for( i = 0; i < N; i++)
      hits += mulle_set_member( set, (void *) (intptr_t) (i + 1));
   for( i = 0; i < N; i++)
      hits += mulle_set_member( set, (void *) (intptr_t) (i + N + 1));

   printf( "member: hits=%zu\n", hits);
   mulle_set_destroy( set);
}


static void  bench_insert_remove( void)
{
   struct mulle_set             *set;
   struct bench_allocator       bench;
   size_t                       i;

   bench_allocator_init( &bench);

   set = mulle_set_create( 0, &mulle_container_keycallback_intptr, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_set_set( set, (void *) (intptr_t) (i + 1));

   for( i = 0; i < N / 2; i++)
      mulle_set_remove( set, (void *) (intptr_t) (i * 2 + 1));

   printf( "insert %d, remove half:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_set_get_count( set),
           mulle_set_get_size( set));

   mulle_set_destroy( set);
}


int  main( void)
{
   bench_warn_if_debug();
   bench_insert_member();
   bench_insert_remove();
   return( 0);
}
