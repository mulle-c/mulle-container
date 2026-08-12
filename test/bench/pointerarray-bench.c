//
//  pointerarray-bench.c
//  mulle-container
//
//  Deterministic benchmark: measures the allocation profile of
//  mulle_pointerarray (growth, removal, insertion) using a counting
//  allocator. Counts are stable across runs and platforms, so this
//  doubles as a regression guard on the growth policy.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_growth( void)
{
   struct mulle_pointerarray   array;
   struct bench_allocator      bench;
   size_t                      i;

   bench_allocator_init( &bench);

   mulle_pointerarray_init( &array, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_pointerarray_add( &array, (void *) (intptr_t) i);

   printf( "add %d from capacity 0:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_pointerarray_get_count( &array),
           mulle_pointerarray_get_size( &array));

   mulle_pointerarray_done( &array);
}


static void  bench_preallocated( void)
{
   struct mulle_pointerarray   array;
   struct bench_allocator      bench;
   size_t                      i;

   bench_allocator_init( &bench);

   mulle_pointerarray_init( &array, N, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_pointerarray_add( &array, (void *) (intptr_t) i);

   printf( "add %d preallocated:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_pointerarray_get_count( &array),
           mulle_pointerarray_get_size( &array));

   mulle_pointerarray_done( &array);
}


static void  bench_remove_in_range( void)
{
   struct mulle_pointerarray   array;
   struct bench_allocator      bench;
   size_t                      i;

   bench_allocator_init( &bench);

   mulle_pointerarray_init( &array, N, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_pointerarray_add( &array, (void *) (intptr_t) i);

   // remove front half
   mulle_pointerarray_remove_in_range( &array, mulle_range_make( 0, N / 2));

   printf( "remove_in_range front half:\n");
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_pointerarray_get_count( &array),
           mulle_pointerarray_get_size( &array));

   mulle_pointerarray_done( &array);
}


static void  bench_insert_in_range( void)
{
   struct mulle_pointerarray   array;
   struct bench_allocator      bench;
   size_t                      i;
   void                        *buf[ 8];

   bench_allocator_init( &bench);

   mulle_pointerarray_init( &array, N, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_pointerarray_add( &array, (void *) (intptr_t) i);

   for( i = 0; i < 8; i++)
      buf[ i] = (void *) (intptr_t) (i + 1000);

   mulle_pointerarray_insert_in_range( &array, mulle_range_make( 4, 8), buf);

   printf( "insert_in_range 8 at 4:\n");
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu\n",
           mulle_pointerarray_get_count( &array),
           mulle_pointerarray_get_size( &array));

   mulle_pointerarray_done( &array);
}


int  main( void)
{
   bench_growth();
   bench_preallocated();
   bench_remove_in_range();
   bench_insert_in_range();
   return( 0);
}
