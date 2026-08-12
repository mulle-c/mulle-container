//
//  assoc-bench.c
//  mulle-container
//
//  Deterministic benchmark: mulle_assoc sorts on insert. The number of
//  comparisons is measured with a counting comparator, for sorted,
//  reverse and LCG-shuffled input. Also measures allocation profile.
//
#include "bench.h"

#include <stdio.h>


#define N   1000


static struct mulle_container_keyvaluecallback   g_callback;


static uintptr_t  g_seed;


// deterministic LCG, so the comparison counts are reproducible
static uintptr_t  next_random( void)
{
   g_seed = g_seed * 1664525 + 1013904223;
   return( g_seed);
}


// Fisher-Yates shuffle into a permutation of 1..N. Unlike key = random() % N,
// this guarantees N distinct keys, so mulle_assoc_add never hits its
// no-duplicates debug assert, no matter the seed.
static void  make_permutation( uintptr_t *keys, size_t n)
{
   size_t   i;
   size_t   j;
   uintptr_t tmp;

   for( i = 0; i < n; i++)
      keys[ i] = i + 1;

   for( i = n - 1; i > 0; i--)
   {
      j = (size_t) (next_random() % (i + 1));
      tmp          = keys[ i];
      keys[ i]     = keys[ j];
      keys[ j]     = tmp;
   }
}


static void  bench_insert( char *order)
{
   struct mulle_assoc            *assoc;
   struct bench_allocator        bench;
   size_t                        i;
   uintptr_t                     keys[ N];
   uintptr_t                     key;
   size_t                        comparisons;

   bench_allocator_init( &bench);
   g_comparisons = 0;

   assoc = mulle_assoc_create( 0, &g_callback, bench_compare_intptr, &bench.allocator);

   if( order[ 0] != 's' && order[ 0] != 'r')
      make_permutation( keys, N);        // one permutation, N distinct keys

   for( i = 0; i < N; i++)
   {
      switch( order[ 0])
      {
      case 's':   key = i + 1;  break;   // sorted
      case 'r':   key = N - i;  break;   // reverse
      default :   key = keys[ i]; break; // shuffled, all distinct
      }
      mulle_assoc_add( assoc, (void *) (intptr_t) key, (void *) (intptr_t) key);
   }

   // assoc sorts lazily: add() appends, the first get() triggers the sort
   g_comparisons = 0;
   mulle_assoc_get( assoc, (void *) (intptr_t) 1);
   comparisons = g_comparisons;

   printf( "insert %d (%s): sort-on-demand comparisons=%zu\n", N, order, comparisons);
   bench_allocator_print( &bench, "  ");
   printf( "  count: %zu size: %zu sorted: %d\n",
           mulle_assoc_get_count( assoc),
           mulle_assoc_get_size( assoc),
           mulle_assoc_is_sorted( assoc));

   mulle_assoc_destroy( assoc);
}


static void  bench_lookup( char *order)
{
   struct mulle_assoc            *assoc;
   struct bench_allocator        bench;
   size_t                        i;
   uintptr_t                     key;
   size_t                        hits;
   size_t                        comparisons;

   bench_allocator_init( &bench);
   g_comparisons = 0;

   assoc = mulle_assoc_create( 0, &g_callback, bench_compare_intptr, &bench.allocator);
   for( i = 0; i < N; i++)
      mulle_assoc_add( assoc, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));

   g_comparisons = 0;
   hits = 0;
   for( i = 0; i < N; i++)
   {
      key = (order[ 0] == 'r') ? (N - i) : (i + 1);
      hits += (mulle_assoc_get( assoc, (void *) (intptr_t) key) != NULL);
   }
   comparisons = g_comparisons;

   printf( "lookup %d (%s): hits=%zu comparisons=%zu\n",
           N, order, hits, comparisons);

   mulle_assoc_destroy( assoc);
}


int  main( void)
{
   g_callback.keycallback   = mulle_container_keycallback_intptr;
   g_callback.valuecallback = mulle_container_valuecallback_intptr;

   g_seed = 0x12345678;
   bench_insert( "sorted");
   bench_insert( "reverse");
   g_seed = 0x12345678;
   bench_insert( "shuffled");

   g_seed = 0x87654321;   // different seed: permutation must stay valid
   bench_insert( "shuffled");

   bench_lookup( "sorted");
   bench_lookup( "reverse");
   return( 0);
}
