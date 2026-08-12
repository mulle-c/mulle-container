//
//  pointerqueue-bench.c
//  mulle-container
//
//  Deterministic benchmark: allocation profile of mulle_pointerqueue
//  (bucket-chained FIFO) for a full add/pop cycle with various bucket
//  sizes.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static void  bench_cycle( unsigned short bucket_size)
{
   struct mulle_pointerqueue     queue;
   struct bench_allocator        bench;
   size_t                        i;
   size_t                        count;

   bench_allocator_init( &bench);

   mulle_pointerqueue_init( &queue, bucket_size, 0, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_pointerqueue_add( &queue, (void *) (intptr_t) (i + 1));

   printf( "add %d (bucket_size=%u):\n", N, bucket_size);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_pointerqueue_get_count( &queue));

   count = 0;
   for( i = 0; i < N; i++)
   {
      if( mulle_pointerqueue_pop( &queue))
         count++;
   }

   printf( "pop %zu:\n", count);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu\n", mulle_pointerqueue_get_count( &queue));

   mulle_pointerqueue_done( &queue);
}


int  main( void)
{
   bench_cycle( 16);
   bench_cycle( 64);
   return( 0);
}
