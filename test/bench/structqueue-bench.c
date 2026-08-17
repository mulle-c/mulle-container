//
//  structqueue-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_structqueue
//  (bucket-chained FIFO for structs) for a full add/pop cycle.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


struct point
{
   double   x;
   double   y;
};


static void  bench_cycle( unsigned short bucket_size)
{
   struct mulle_structqueue      queue;
   struct bench_allocator        bench;
   struct point                  item;
   struct point                  out;
   size_t                        i;
   size_t                        count;

   bench_allocator_init( &bench);

   mulle_structqueue_init( &queue, sizeof( struct point), alignof( struct point), bucket_size, 0, &bench.allocator);
   for( i = 0; i < N; i++)
   {
      item.x = (double) (i + 1);
      item.y = (double) (i * 2 + 1);
      mulle_structqueue_add( &queue, &item);
   }

   printf( "add %d (bucket_size=%u):\n", N, bucket_size);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_structqueue_get_count( &queue));

   count = 0;
   for( i = 0; i < N; i++)
   {
      if( mulle_structqueue_pop( &queue, &out))
         count++;
   }

   printf( "pop %zu:\n", count);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_structqueue_get_count( &queue));

   mulle_structqueue_done( &queue);
}


int  main( void)
{
   bench_warn_if_debug();
   bench_cycle( 16);
   bench_cycle( 64);
   return( 0);
}
