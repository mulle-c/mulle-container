//
//  structarray-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_structarray
//  when growing and when reserving up front.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


struct point
{
   double   x;
   double   y;
   double   z;
};


static void  bench_growth( void)
{
   struct mulle_structarray     array;
   struct bench_allocator       bench;
   struct point                 point;
   size_t                       i;

   bench_allocator_init( &bench);

   mulle_structarray_init( &array, sizeof( struct point), alignof( struct point), 0, &bench.allocator);
   for( i = 0; i < N; i++)
   {
      point.x = (double) i;
      point.y = (double) i;
      point.z = (double) i;
      mulle_structarray_add( &array, &point);
   }

   printf( "add %d from capacity 0:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_structarray_get_count( &array));

   mulle_structarray_done( &array);
}


static void  bench_reserved( void)
{
   struct mulle_structarray     array;
   struct bench_allocator       bench;
   size_t                       i;

   bench_allocator_init( &bench);

   mulle_structarray_init( &array, sizeof( struct point), alignof( struct point), N, &bench.allocator);
   _mulle__structarray_set_count( (struct mulle__structarray *) &array, N, &bench.allocator);
   for( i = 0; i < N; i++)
      ((struct point *) _mulle__structarray_get( (struct mulle__structarray *) &array, i))->x = (double) i;

   printf( "add %d reserved:\n", N);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_structarray_get_count( &array));

   mulle_structarray_done( &array);
}


int  main( void)
{
   bench_growth();
   bench_reserved();
   return( 0);
}
