//
//  crossover-bench.c
//  mulle-container
//
//  Wall-clock crossover benchmark: measures member/get lookup cost of the
//  four "does it contain key k" containers across a range of sizes, plus
//  a raw C qsort + bsearch baseline. This is the benchmark that tests the
//  README's rule of thumb that hash sets win over sorted arrays above
//  roughly 100 items.
//
//  Like timing-bench.c there is deliberately no .stdout file, so the test
//  harness runs it as a smoke test without comparing output. For
//  meaningful numbers use a release build:
//
//    mulle-sde test --release craft
//    ./test/bench/crossover-bench.exe
//
//  Results are noisy; treat them as ballpark figures.
//
#include "bench.h"

#include <stdio.h>
#include <stdlib.h>


#define N_SIZES   5

static size_t   g_sizes[ N_SIZES] = { 10, 100, 1000, 10000, 100000 };


static struct mulle_container_keyvaluecallback   g_callback;


//
// raw C baseline: sorted array + bsearch
//
static int  compare_uintptr( const void *a, const void *b)
{
   uintptr_t   x;
   uintptr_t   y;

   x = *(uintptr_t *) a;
   y = *(uintptr_t *) b;
   return( (int) (x > y) - (int) (x < y));
}


//
// noinline: same guard as in head-to-head-bench.c - keep the sorted
// contents opaque so the compiler cannot fold the bsearch loads into
// constants and delete the timed loop.  See the full explanation at
// raw_array_add in head-to-head-bench.c.
//
__attribute__((noinline))
static uintptr_t  *make_sorted_array( size_t n)
{
   uintptr_t   *array;
   size_t      i;

   array = malloc( n * sizeof( uintptr_t));
   for( i = 0; i < n; i++)
      array[ i] = i + 1;               // already sorted, matches assoc input

   return( array);
}


static void  free_sorted_array( uintptr_t *array)
{
   free( array);
}


// 1M total lookups per container keeps each measurement in the 10-100 ms
// range regardless of size.
static size_t  lookup_passes( size_t n)
{
   size_t   passes;

   passes = 1000000 / n;
   return( passes ? passes : 1);
}


//
// The array member scan is O(n) per lookup, so probing all n keys for 1M
// lookups would be O(n^2) total work and dominate the benchmark. Probe at
// most MAX_PROBES keys spread evenly across the array instead: the average
// scan depth stays ~n/2, so the reported ns/op is honest, while the total
// work stays bounded.
//
#define MAX_PROBES   1000


//
// Constant-work rule for the array scan: keep ~5e7 element visits per
// measurement so the timing is stable at every size (n=10 gets many cheap
// passes, n=100000 one expensive one). Returns 0 for n == 0 (not reached).
//
#define ARRAY_WORK   50000000


static size_t  array_passes( size_t n, size_t probes)
{
   size_t   passes;
   size_t   depth;

   depth  = n / 2;                     // average scan depth
   passes = ARRAY_WORK / (probes * depth);
   return( passes ? passes : 1);
}


static void  array_probe( size_t i, size_t n, size_t probes, uintptr_t *key)
{
   *key = (uintptr_t) (((i * n) / probes) + 1);
}


static void  bench_size( size_t n)
{
   struct mulle_array              *array;
   struct mulle_assoc              *assoc;
   struct mulle_set                *set;
   struct mulle_map                *map;
   uintptr_t                       *sorted;
   uintptr_t                       *probe;
   uintptr_t                       *hit;
   size_t                          passes;
   size_t                          probes;
   size_t                          a_passes;
   size_t                          checksum;
   size_t                          i;
   size_t                          k;
   size_t                          hits;
   uintptr_t                       key;
   uint64_t                        t0;
   uint64_t                        t1;
   double                          ns[ 5];

   checksum = 0;
   passes   = lookup_passes( n);
   probes   = (n < MAX_PROBES) ? n : MAX_PROBES;
   a_passes = array_passes( n, probes);
   probe    = malloc( n * sizeof( uintptr_t));
   for( i = 0; i < n; i++)
      probe[ i] = i + 1;

   //
   // build the containers
   //
   array = mulle_array_create( 0, &mulle_container_keycallback_intptr, NULL);
   for( i = 0; i < n; i++)
      mulle_array_add( array, (void *) (intptr_t) (i + 1));

   assoc = mulle_assoc_create( n, &g_callback, _mulle_pointerpair_compare_intptr_key, NULL);
   for( i = 0; i < n; i++)
      _mulle__assoc_add_guaranteed( (struct mulle__assoc *) assoc,
                                    (void *) (intptr_t) (i + 1),
                                    (void *) (intptr_t) (i + 1),
                                    &g_callback,
                                    NULL);

   set = mulle_set_create( 0, &mulle_container_keycallback_intptr, NULL);
   for( i = 0; i < n; i++)
      mulle_set_set( set, (void *) (intptr_t) (i + 1));

   map = mulle_map_create( 0, &g_callback, NULL);
   for( i = 0; i < n; i++)
      mulle_map_set( map, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));

   sorted = make_sorted_array( n);

   // warm up the lazy assoc sort outside the timed region
   mulle_assoc_get( assoc, (void *) (intptr_t) 1);

   //
   // raw C bsearch
   //
   hits = 0;
   t0   = bench_now();
   for( k = 0; k < passes; k++)
      for( i = 0; i < n; i++)
      {
         hit = bsearch( &probe[ i], sorted, n, sizeof( uintptr_t), compare_uintptr);
         hits += (hit != NULL);
      }
   t1         = bench_now();
   ns[ 0]     = (double) (t1 - t0) / (double) (passes * n);
   checksum  += hits;

   //
   // mulle_array_member: linear scan (bounded probes, see array_passes)
   //
   hits = 0;
   t0   = bench_now();
   for( k = 0; k < a_passes; k++)
      for( i = 0; i < probes; i++)
      {
         array_probe( i, n, probes, &key);
         hits += (mulle_array_member( array, (void *) (intptr_t) key) != 0);
      }
   t1          = bench_now();
   ns[ 1]      = (double) (t1 - t0) / (double) (a_passes * probes);
   checksum   += hits;

   //
   // mulle_assoc_get: binary search (already sorted by warm-up)
   //
   hits = 0;
   t0   = bench_now();
   for( k = 0; k < passes; k++)
      for( i = 0; i < n; i++)
         hits += (mulle_assoc_get( assoc, (void *) (intptr_t) (i + 1)) != NULL);
   t1     = bench_now();
   ns[ 2] = (double) (t1 - t0) / (double) (passes * n);
   checksum += hits;

   //
   // mulle_set_member: hash
   //
   hits = 0;
   t0   = bench_now();
   for( k = 0; k < passes; k++)
      for( i = 0; i < n; i++)
         hits += (mulle_set_member( set, (void *) (intptr_t) (i + 1)) != 0);
   t1     = bench_now();
   ns[ 3] = (double) (t1 - t0) / (double) (passes * n);
   checksum += hits;

   //
   // mulle_map_get: hash
   //
   hits = 0;
   t0   = bench_now();
   for( k = 0; k < passes; k++)
      for( i = 0; i < n; i++)
         hits += (mulle_map_get( map, (void *) (intptr_t) (i + 1)) != NULL);
   t1     = bench_now();
   ns[ 4] = (double) (t1 - t0) / (double) (passes * n);
   checksum += hits;

   printf( "n=%8zu: bsearch %8.1f | array %8.1f | assoc %8.1f | set %8.1f | map %8.1f ns/op  (checksum %zu)\n",
           n, ns[ 0], ns[ 1], ns[ 2], ns[ 3], ns[ 4], checksum);

   mulle_array_destroy( array);
   mulle_assoc_destroy( assoc);
   mulle_set_destroy( set);
   mulle_map_destroy( map);
   free_sorted_array( sorted);
   free( probe);
}


#define MAX_SIZE   1000000


int  main( int argc, char *argv[])
{
   size_t   i;

   if( argc > 1)
   {
      g_sizes[ 0] = (size_t) strtoul( argv[ 1], NULL, 10);
      if( ! g_sizes[ 0] || g_sizes[ 0] > MAX_SIZE)   // 0, garbage or overflow
         g_sizes[ 0] = 100;
      for( i = 1; i < N_SIZES; i++)
         g_sizes[ i] = g_sizes[ i - 1] * 10;
   }

   g_callback.keycallback   = mulle_container_keycallback_intptr;
   g_callback.valuecallback = mulle_container_valuecallback_intptr;

   printf( "mulle-container lookup crossover (ns/op, hits only)\n");
   for( i = 0; i < N_SIZES; i++)
      bench_size( g_sizes[ i]);

   return( 0);
}
