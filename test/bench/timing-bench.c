//
//  timing-bench.c
//  mulle-container
//
//  Wall-clock benchmark: prints ns/op for the core operations of each
//  container. There is deliberately no .stdout file, so the test harness
//  runs it (smoke test) but does not compare the output. Run it directly
//  (./timing-bench.exe) or via `mulle-sde test run test/bench/timing-bench.c`
//  to see the numbers.
//
//  Results are noisy; treat them as ballpark figures.
//
#include "bench.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


//
// N defaults small on purpose: the debug test build asserts inside
// mulle_assoc_add (linear membership scan) and _mulle__rangeset_insert
// (O(n) internal assert per insert), which makes large N quadratic in
// debug builds. For meaningful numbers, run a release build:
//
//   mulle-sde test --release run test/bench/timing-bench.c
//
// For a larger N, craft and run the executable directly (N is argv[1]):
//   mulle-sde test --release craft
//   ./test/bench/timing-bench.exe 100000
//
// Results are noisy; treat them as ballpark figures.
//
#define DEFAULT_N   10000
#define MAX_N       1000000


static size_t  g_n = DEFAULT_N;


static uint64_t  now( void)
{
   return( bench_now());
}


static void  report( char *label, uint64_t elapsed, size_t count)
{
   fprintf( stderr, "%-28s %8.1f ns/op  (%zu ops)\n",
           label,
           (double) elapsed / (double) count,
           count);
}


static void  bench_pointerarray( void)
{
   struct mulle_pointerarray   array;
   uint64_t                    t0;
   uint64_t                    t1;
   size_t                      i;
   size_t                      n;

   mulle_pointerarray_init( &array, 0, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_pointerarray_add( &array, (void *) (intptr_t) i);
   t1 = now();
   report( "pointerarray add", t1 - t0, g_n);

   t0 = now();
   n = 0;
   for( i = 0; i < g_n; i++)
      n += (mulle_pointerarray_get( &array, i) != NULL);
   t1 = now();
   report( "pointerarray get", t1 - t0, g_n);
   // keep the accumulator observable, or the compiler deletes the loop
   // (dead-code elimination).  No noinline guard is needed here: there
   // are no raw baselines to protect - the container internals live in
   // the library as opaque function calls, which keeps the reads honest
   // on their own.  See head-to-head-bench.c for the full story.
   fprintf( stderr, "  (checksum %zu)\n", n);

   mulle_pointerarray_done( &array);
}


static void  bench_array( void)
{
   struct mulle_array   *array;
   uint64_t              t0;
   uint64_t              t1;
   size_t                i;

   array = mulle_array_create( 0, &mulle_container_keycallback_intptr, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_array_add( array, (void *) (intptr_t) i);
   t1 = now();
   report( "array add", t1 - t0, g_n);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_array_get( array, i);
   t1 = now();
   report( "array get", t1 - t0, g_n);

   mulle_array_destroy( array);
}


static void  bench_map( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;
   uint64_t                                  t0;
   uint64_t                                  t1;
   size_t                                    i;

   callback.keycallback   = mulle_container_keycallback_intptr;
   callback.valuecallback = mulle_container_valuecallback_intptr;

   map = mulle_map_create( 0, &callback, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_map_set( map, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "map set", t1 - t0, g_n);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_map_get( map, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "map get", t1 - t0, g_n);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_map_remove( map, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "map remove", t1 - t0, g_n);

   mulle_map_destroy( map);
}


static void  bench_set( void)
{
   struct mulle_set   *set;
   uint64_t           t0;
   uint64_t           t1;
   size_t             i;

   set = mulle_set_create( 0, &mulle_container_keycallback_intptr, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_set_set( set, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "set set", t1 - t0, g_n);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_set_member( set, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "set member", t1 - t0, g_n);

   mulle_set_destroy( set);
}


static void  bench_assoc( void)
{
   struct mulle_assoc                         *assoc;
   struct mulle_container_keyvaluecallback    callback;
   uint64_t                                   t0;
   uint64_t                                   t1;
   size_t                                     i;

   callback.keycallback   = mulle_container_keycallback_intptr;
   callback.valuecallback = mulle_container_valuecallback_intptr;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_intptr_key, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_assoc_add( assoc, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "assoc add (sorted)", t1 - t0, g_n);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_assoc_get( assoc, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "assoc get", t1 - t0, g_n);

   mulle_assoc_destroy( assoc);
}


static void  bench_pointerqueue( void)
{
   struct mulle_pointerqueue   queue;
   uint64_t                    t0;
   uint64_t                    t1;
   size_t                      i;

   mulle_pointerqueue_init( &queue, 64, 0, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_pointerqueue_add( &queue, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "pointerqueue add", t1 - t0, g_n);

   t0 = now();
   for( i = 0; i < g_n; i++)
      mulle_pointerqueue_pop( &queue);
   t1 = now();
   report( "pointerqueue pop", t1 - t0, g_n);

   mulle_pointerqueue_done( &queue);
}


static void  bench_rangeset( void)
{
   struct mulle__rangeset   set;
   uint64_t                 t0;
   uint64_t                 t1;
   size_t                   i;

   _mulle__rangeset_init( &set, 0, NULL);

   t0 = now();
   for( i = 0; i < g_n; i++)
      _mulle__rangeset_insert( &set, mulle_range_make( i * 2, 1), NULL);
   t1 = now();
   report( "rangeset insert", t1 - t0, g_n);

   _mulle__rangeset_done( &set, NULL);
}


int  main( int argc, char *argv[])
{
   if( argc > 1)
   {
      g_n = (size_t) strtoul( argv[ 1], NULL, 10);
      if( ! g_n || g_n > MAX_N)          // 0, garbage or overflow
         g_n = DEFAULT_N;
   }

   bench_warn_if_debug();
   fprintf( stderr, "mulle-container wall-clock benchmark (N=%zu)\n", g_n);
   bench_pointerarray();
   bench_array();
   bench_map();
   bench_set();
   bench_assoc();
   bench_pointerqueue();
   bench_rangeset();
   return( 0);
}
