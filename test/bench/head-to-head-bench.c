//
//  head-to-head-bench.c
//  mulle-container
//
//  Wall-clock head-to-head benchmark: mulle-container containers vs.
//  plain-C baselines, so the numbers mean something:
//
//    mulle_pointerarray  vs. a raw C array (no checks, no callbacks)
//    mulle_map           vs. a khash-style open-addressing hash table
//    mulle_set           vs. the same khash-style table (membership)
//
//  The baselines use the same hash function as mulle-container
//  (mulle_pointer_hash) so the comparison isolates the container's
//  mechanics (probing, growth, indirection, NULL-safety), not the hash
//  quality. Reporting ns/op against these baselines is the only honest
//  way to say "fast" or "slow" - absolute ns/op alone says nothing.
//
//  Like timing-bench.c there is deliberately no .stdout file, so the
//  test harness runs it as a smoke test without comparing output. Use a
//  release build for meaningful numbers:
//
//    mulle-sde test --release craft
//    ./test/bench/head-to-head-bench.exe
//
//  Results are noisy; treat them as ballpark figures.
//
#include "bench.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define N   100000


//
// ---- raw C array baseline: no bounds checks, no callbacks ---------------
//
struct raw_array
{
   void   **storage;
   size_t  count;
   size_t  capacity;
};


//
// noinline: the guard against the optimizer.  Keep the baseline buffer
// contents opaque, or the compiler folds the timed loops away.
//
// Without the guard, the compiler can inline raw_array_add, see exactly
// what was written into the buffer, prove the contents never change
// during the timed phase, and replace every load with a constant - the
// "loop" is then computed at compile time and measures ~0.0 ns.  This is
// not theoretical: it was observed with gcc -O3, where the assembly
// showed clock_gettime calls with no loop between them at all.
//
// Two guards work together here:
//
//   1. checksum accumulation - every timed loop adds its hits to a
//      checksum that is printed, so no loop is "dead code" the compiler
//      may delete (dead-code elimination).
//   2. noinline on the baseline helpers - the buffer contents stay
//      opaque, so the loads inside the timed loops are real memory
//      reads, not folded constants.
//
// Contrast with the library's NULL guard in mulle_pointerarray_get: that
// `if( ! array)` is *supposed* to fold away.  When init and get happen in
// the same function, the compiler knows &parray is a non-NULL stack
// address and the get compiles to a single load, on par with an
// unchecked index (measured ~1.0x, see "pointerarray get" below).  Here
// we protect the opposite side: the *baseline* must not be optimized
// away, or the ratio is garbage.  (A bogus ~5-6x ratio was once reported
// for pointerarray get because an unoptimized harness build emitted
// out-of-line copies of the inline getters - a build artifact, not a
// property of the library.)
//
__attribute__((noinline))
static void  raw_array_add( struct raw_array *array, void *p)
{
   if( array->count == array->capacity)
   {
      array->capacity = array->capacity ? array->capacity * 2 : 8;
      array->storage  = realloc( array->storage, array->capacity * sizeof( void *));
   }
   array->storage[ array->count++] = p;
}


static void  raw_array_done( struct raw_array *array)
{
   free( array->storage);
}


//
// ---- khash-style open-addressing hash table ------------------------------
// Classic design (like khash/uthash): one array of key/value pairs, linear
// probing, power-of-two buckets, 75% load factor growth. Keys are never
// NULL in the benchmark, so NULL marks an empty slot (the khash idiom of
// a sentinel key). Same hash as mulle-container (mulle_pointer_hash).
//
struct khash_table
{
   void      **keys;
   void      **values;
   size_t    n_buckets;
   size_t    n_items;
};


static void  khash_grow( struct khash_table *table)
{
   size_t        new_buckets;
   size_t        i;
   size_t        j;
   uintptr_t     index;
   void          **new_keys;
   void          **new_values;

   new_buckets = table->n_buckets ? table->n_buckets * 2 : 8;
   new_keys    = calloc( new_buckets, sizeof( void *));
   new_values  = calloc( new_buckets, sizeof( void *));

   for( i = 0; i < table->n_buckets; i++)
      if( table->keys[ i])
      {
         index = mulle_pointer_hash( table->keys[ i]) & (new_buckets - 1);
         while( new_keys[ index])
            index = (index + 1) & (new_buckets - 1);
         new_keys[ index]   = table->keys[ i];
         new_values[ index] = table->values[ i];
      }

   free( table->keys);
   free( table->values);
   table->keys      = new_keys;
   table->values    = new_values;
   table->n_buckets = new_buckets;
}


// same noinline guard as raw_array_add (see explanation above)
__attribute__((noinline))
static void  khash_set( struct khash_table *table, void *key, void *value)
{
   uintptr_t  index;

   if( (table->n_items + 1) * 4 >= table->n_buckets * 3)   // > 75 %
      khash_grow( table);

   index = mulle_pointer_hash( key) & (table->n_buckets - 1);
   while( table->keys[ index])
   {
      if( table->keys[ index] == key)
         break;
      index = (index + 1) & (table->n_buckets - 1);
   }
   if( ! table->keys[ index])
      table->n_items++;
   table->keys[ index]   = key;
   table->values[ index] = value;
}


// same noinline guard as raw_array_add (see explanation above)
__attribute__((noinline))
static void  *khash_get( struct khash_table *table, void *key)
{
   uintptr_t  index;

   if( ! table->n_buckets)
      return( NULL);

   index = mulle_pointer_hash( key) & (table->n_buckets - 1);
   while( table->keys[ index])
   {
      if( table->keys[ index] == key)
         return( table->values[ index]);
      index = (index + 1) & (table->n_buckets - 1);
   }
   return( NULL);
}


static void  khash_done( struct khash_table *table)
{
   free( table->keys);
   free( table->values);
}


//
// ---- benchmark driver ----------------------------------------------------
//
static uint64_t  now( void)
{
   return( bench_now());
}


static void  report( char *label, uint64_t elapsed, size_t count, char *base, double base_ns)
{
   printf( "%-24s %8.1f ns/op   vs %-13s %6.1f ns/op   (%.2fx)\n",
           label,
           (double) elapsed / (double) count,
           base,
           base_ns,
           (double) elapsed / (double) count / base_ns);
}


int  main( int argc, char *argv[])
{
   struct raw_array              raw;
   struct mulle_pointerarray     parray;
   struct khash_table            khash;
   struct mulle_map              *map;
   struct mulle_set              *set;
   struct mulle_container_keyvaluecallback   callback;
   size_t                        checksum;
   uint64_t                      t0;
   uint64_t                      t1;
   size_t                        i;
   size_t                        hits;
   size_t                        n;
   double                        raw_add_ns;
   double                        raw_get_ns;
   double                        khash_set_ns;
   double                        khash_get_ns;

   checksum = 0;
   n = N;
   if( argc > 1)
   {
      n = (size_t) strtoul( argv[ 1], NULL, 10);
      if( ! n || n > 1000000)
         n = N;
   }

   callback.keycallback   = mulle_container_keycallback_intptr;
   callback.valuecallback = mulle_container_valuecallback_intptr;

   printf( "mulle-container vs. plain C baselines (n=%zu)\n", n);

   //
   // raw C array
   //
   memset( &raw, 0, sizeof( raw));
   t0 = now();
   for( i = 0; i < n; i++)
      raw_array_add( &raw, (void *) (intptr_t) (i + 1));
   t1 = now();
   raw_add_ns = (double) (t1 - t0) / (double) n;

   t0 = now();
   hits = 0;
   for( i = 0; i < n; i++)
      hits += (raw.storage[ i] != NULL);
   t1 = now();
   raw_get_ns = (double) (t1 - t0) / (double) n;
   checksum += hits;

   //
   // mulle_pointerarray
   //
   printf( "%-24s %8.1f ns/op   (the baseline)\n", "raw array add", raw_add_ns);
   printf( "%-24s %8.1f ns/op   (the baseline)\n", "raw array get", raw_get_ns);

   mulle_pointerarray_init( &parray, 0, NULL);
   t0 = now();
   for( i = 0; i < n; i++)
      mulle_pointerarray_add( &parray, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "pointerarray add", t1 - t0, n, "raw array add", raw_add_ns);

   t0 = now();
   hits = 0;
   for( i = 0; i < n; i++)
      hits += (mulle_pointerarray_get( &parray, i) != NULL);
   t1 = now();
   report( "pointerarray get", t1 - t0, n, "raw array get", raw_get_ns);
   checksum += hits;
   mulle_pointerarray_done( &parray);

   printf( "\n");

   //
   // khash-style open addressing
   //
   memset( &khash, 0, sizeof( khash));
   t0 = now();
   for( i = 0; i < n; i++)
      khash_set( &khash, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));
   t1 = now();
   khash_set_ns = (double) (t1 - t0) / (double) n;

   t0 = now();
   hits = 0;
   for( i = 0; i < n; i++)
      hits += (khash_get( &khash, (void *) (intptr_t) (i + 1)) != NULL);
   t1 = now();
   khash_get_ns = (double) (t1 - t0) / (double) n;
   checksum += hits;

   printf( "%-24s %8.1f ns/op   (the baseline)\n", "khash set", khash_set_ns);
   printf( "%-24s %8.1f ns/op   (the baseline)\n", "khash get", khash_get_ns);
   printf( "\n");

   //
   // mulle_map
   //
   map = mulle_map_create( 0, &callback, NULL);
   t0 = now();
   for( i = 0; i < n; i++)
      mulle_map_set( map, (void *) (intptr_t) (i + 1), (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "map set", t1 - t0, n, "khash set", khash_set_ns);

   t0 = now();
   hits = 0;
   for( i = 0; i < n; i++)
      hits += (mulle_map_get( map, (void *) (intptr_t) (i + 1)) != NULL);
   t1 = now();
   report( "map get", t1 - t0, n, "khash get", khash_get_ns);
   checksum += hits;
   mulle_map_destroy( map);

   //
   // mulle_set (membership, no values)
   //
   set = mulle_set_create( 0, &mulle_container_keycallback_intptr, NULL);
   t0 = now();
   for( i = 0; i < n; i++)
      mulle_set_set( set, (void *) (intptr_t) (i + 1));
   t1 = now();
   report( "set set", t1 - t0, n, "khash set", khash_set_ns);

   t0 = now();
   hits = 0;
   for( i = 0; i < n; i++)
      hits += (mulle_set_member( set, (void *) (intptr_t) (i + 1)) != 0);
   t1 = now();
   report( "set member", t1 - t0, n, "khash get", khash_get_ns);
   checksum += hits;
   mulle_set_destroy( set);

   khash_done( &khash);
   raw_array_done( &raw);

   // keep the accumulators observable, or the compiler deletes the loops
   printf( "checksum: %zu\n", checksum);

   printf( "\nratios >1.0 mean mulle-container is slower than the baseline;\n");
   printf( "the gap is what you pay for bounds checks, NULL-safety and\n");
   printf( "allocator/callback indirection.\n");
   return( 0);
}
