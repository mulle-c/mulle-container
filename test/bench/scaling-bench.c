//
//  scaling-bench.c
//  mulle-container
//
//  Scaling benchmark: for every container, measure the wall-clock cost of
//  insert, read and remove across container sizes that are powers of two
//  (2^4 .. 2^20 by default, i.e. up to ~1 MB entries) and print machine
//  readable rows to stderr:
//
//     container,op,n,ns_op
//
//  `op` is the container's real verb, following mulle-c11's SYNTAX.md: "get"
//  (random access by index), "lookup" (hashtable indexing), "member"
//  (presence), "search" (binary), "find" (linear). The verbs are not
//  conflated: `mulle_pointerarray_get` is reported as `get`, a keyed hash read
//  as `lookup`, a binary search as `search`, a linear scan as `find`, and a
//  FIFO queue simply has no read at all.
//
//  There is deliberately no .stdout file, so the test harness runs this as a
//  smoke test without comparing output (timings are noisy). Collect the data
//  and chart it with tools/bench-chart.py:
//
//     mulle-sde test --release craft
//     ./test/bench/scaling-bench.exe 2> pix/bench-scaling.csv
//     tools/bench-chart.py pix/bench-scaling.csv
//
//  When asserts are compiled in (NDEBUG not defined, as in the default test
//  build) the range stops at 2^12, because mulle_assoc_add then runs an O(n)
//  membership check on every insert and removing all elements from a sorted
//  array is inherently O(n^2). Build with -DNDEBUG (release) and raise the
//  limit with argv[1] to reach 2^20.
//
//  Optional argv[1] overrides the largest exponent (default above).
//
#include "bench.h"

#include <stdio.h>
#include <stdlib.h>


#ifdef NDEBUG
# define BENCH_MAX_EXP   20
#else
# define BENCH_MAX_EXP   12
#endif
#define BENCH_MIN_EXP      4

//
// Aim for about this many operations per measurement of a small container, so
// the elapsed time is comfortably above the clock resolution. 10k is plenty;
// a larger target only amplifies a per-op cost that grows with n.
//
#define BENCH_TARGET_OPS   10000UL
#define BENCH_MAX_ROUNDS   100000UL

//
// A linear scan costs O(n) per lookup, so probing all n keys at n = 2^20 would
// be O(n^2). For such containers probe at most this many keys, spread evenly
// across the container, keeping the total work bounded.
//
#define BENCH_MAX_PROBES   1000UL

//
// Above this size a single pass is already far above the clock resolution, so
// extra rounds are pointless - and rounds would amplify a slow operation. Only
// small containers are measured repeatedly.
//
#define BENCH_ROUND_MIN_N  4096UL

//
// Stop growing a container once a single size costs more than this. A slow
// operation must not drag a normal `mulle-sde test` run into its timeout.
//
#define BENCH_SIZE_BUDGET  200000000ULL   // 200 ms


struct bench_item
{
   uintptr_t   x;
   uintptr_t   y;
};


struct bench_adapter
{
   char    *name;
   char    *read_op;         // "get"/"lookup"/"member"/"search"/"find", NULL if none
   char    *remove_op;       // "pop" (from the end) or "remove" (by key/range)
   int      linear_read;     // read is a linear scan: probe a bounded subset
   void   *(*create)( size_t n);
   void    (*warmup)( void *state, size_t n);
   void    (*insert)( void *state, size_t i);
   size_t  (*lookup)( void *state, size_t i);
   void    (*remove)( void *state, size_t i);
   void    (*destroy)( void *state);
};


// -------------------------------------------------------------------------
// pointerarray
// -------------------------------------------------------------------------

static void  *pa_create( size_t n)
{
   struct mulle_pointerarray   *array;

   MULLE_C_UNUSED( n);

   array = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *array));
   mulle_pointerarray_init( array, 0, &mulle_allocator_default);
   return( array);
}


static void  pa_insert( void *state, size_t i)
{
   mulle_pointerarray_add( (struct mulle_pointerarray *) state,
                           (void *) (intptr_t) (i + 1));
}


static size_t  pa_lookup( void *state, size_t i)
{
   return( mulle_pointerarray_get( (struct mulle_pointerarray *) state, i) != NULL);
}


static void  pa_remove( void *state, size_t i)
{
   MULLE_C_UNUSED( i);
   mulle_pointerarray_remove_last( (struct mulle_pointerarray *) state);
}


static void  pa_destroy( void *state)
{
   mulle_pointerarray_done( (struct mulle_pointerarray *) state);
   mulle_allocator_free( &mulle_allocator_default, state);
}


// -------------------------------------------------------------------------
// array (managed)
// -------------------------------------------------------------------------

static void  *array_create( size_t n)
{
   MULLE_C_UNUSED( n);
   return( mulle_array_create( 0, &mulle_container_keycallback_intptr, &mulle_allocator_default));
}


static void  array_insert( void *state, size_t i)
{
   mulle_array_add( (struct mulle_array *) state, (void *) (intptr_t) (i + 1));
}


static size_t  array_lookup( void *state, size_t i)
{
   return( mulle_array_get( (struct mulle_array *) state, i) != NULL);
}


static void  array_remove( void *state, size_t i)
{
   MULLE_C_UNUSED( i);
   mulle_array_remove_last( (struct mulle_array *) state);
}


static void  array_destroy( void *state)
{
   mulle_array_destroy( (struct mulle_array *) state);
}


// -------------------------------------------------------------------------
// structarray
// -------------------------------------------------------------------------

static void  *structarray_create( size_t n)
{
   struct mulle_structarray   *array;

   MULLE_C_UNUSED( n);

   array = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *array));
   mulle_structarray_init( array, sizeof( struct bench_item),
                           alignof( struct bench_item), 0, &mulle_allocator_default);
   return( array);
}


static void  structarray_insert( void *state, size_t i)
{
   struct bench_item   item;

   item.x = i + 1;
   item.y = i + 1;
   mulle_structarray_add( (struct mulle_structarray *) state, &item);
}


static size_t  structarray_lookup( void *state, size_t i)
{
   return( _mulle__structarray_get( (struct mulle__structarray *) state, i) != NULL);
}


static void  structarray_remove( void *state, size_t i)
{
   MULLE_C_UNUSED( i);
   mulle_structarray_remove_last( (struct mulle_structarray *) state);
}


static void  structarray_destroy( void *state)
{
   mulle_structarray_done( (struct mulle_structarray *) state);
   mulle_allocator_free( &mulle_allocator_default, state);
}


// -------------------------------------------------------------------------
// pointerqueue (FIFO, no lookup)
// -------------------------------------------------------------------------

static void  *pointerqueue_create( size_t n)
{
   struct mulle_pointerqueue   *queue;

   MULLE_C_UNUSED( n);

   queue = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *queue));
   mulle_pointerqueue_init( queue, 64, 0, &mulle_allocator_default);
   return( queue);
}


static void  pointerqueue_insert( void *state, size_t i)
{
   mulle_pointerqueue_add( (struct mulle_pointerqueue *) state,
                           (void *) (intptr_t) (i + 1));
}


static void  pointerqueue_remove( void *state, size_t i)
{
   MULLE_C_UNUSED( i);
   mulle_pointerqueue_pop( (struct mulle_pointerqueue *) state);
}


static void  pointerqueue_destroy( void *state)
{
   mulle_pointerqueue_done( (struct mulle_pointerqueue *) state);
   mulle_allocator_free( &mulle_allocator_default, state);
}


// -------------------------------------------------------------------------
// structqueue (FIFO, no lookup)
// -------------------------------------------------------------------------

static void  *structqueue_create( size_t n)
{
   struct mulle_structqueue   *queue;

   MULLE_C_UNUSED( n);

   queue = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *queue));
   mulle_structqueue_init( queue, sizeof( struct bench_item),
                           alignof( struct bench_item), 64, 0, &mulle_allocator_default);
   return( queue);
}


static void  structqueue_insert( void *state, size_t i)
{
   struct bench_item   item;

   item.x = i + 1;
   item.y = i + 1;
   mulle_structqueue_add( (struct mulle_structqueue *) state, &item);
}


static void  structqueue_remove( void *state, size_t i)
{
   struct bench_item   out;

   MULLE_C_UNUSED( i);
   mulle_structqueue_pop( (struct mulle_structqueue *) state, &out);
}


static void  structqueue_destroy( void *state)
{
   mulle_structqueue_done( (struct mulle_structqueue *) state);
   mulle_allocator_free( &mulle_allocator_default, state);
}


// -------------------------------------------------------------------------
// map (hashtable, intptr keys)
// -------------------------------------------------------------------------

static struct mulle_container_keyvaluecallback   g_kvcallback;


static void  *map_create( size_t n)
{
   MULLE_C_UNUSED( n);
   return( mulle_map_create( 0, &g_kvcallback, &mulle_allocator_default));
}


static void  map_insert( void *state, size_t i)
{
   mulle_map_set( (struct mulle_map *) state,
                  (void *) (intptr_t) (i + 1),
                  (void *) (intptr_t) (i + 1));
}


static size_t  map_lookup( void *state, size_t i)
{
   return( mulle_map_get( (struct mulle_map *) state, (void *) (intptr_t) (i + 1)) != NULL);
}


static void  map_remove( void *state, size_t i)
{
   mulle_map_remove( (struct mulle_map *) state, (void *) (intptr_t) (i + 1));
}


static void  map_destroy( void *state)
{
   mulle_map_destroy( (struct mulle_map *) state);
}


// -------------------------------------------------------------------------
// pointermap (hashtable, pointer keys)
// -------------------------------------------------------------------------

static void  *pointermap_create( size_t n)
{
   struct mulle__pointermap   *map;

   MULLE_C_UNUSED( n);

   map = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *map));
   _mulle__pointermap_init( map, 0, &mulle_allocator_default);
   return( map);
}


static void  pointermap_insert( void *state, size_t i)
{
   mulle__pointermap_set( (struct mulle__pointermap *) state,
                          (void *) (intptr_t) (i + 1),
                          (void *) (intptr_t) (i + 1),
                          &mulle_allocator_default);
}


static size_t  pointermap_lookup( void *state, size_t i)
{
   return( mulle__pointermap_get( (struct mulle__pointermap *) state,
                                  (void *) (intptr_t) (i + 1)) != NULL);
}


static void  pointermap_remove( void *state, size_t i)
{
   mulle__pointermap_remove( (struct mulle__pointermap *) state,
                             (void *) (intptr_t) (i + 1),
                             &mulle_allocator_default);
}


static void  pointermap_destroy( void *state)
{
   _mulle__pointermap_done( (struct mulle__pointermap *) state, &mulle_allocator_default);
   mulle_allocator_free( &mulle_allocator_default, state);
}


// -------------------------------------------------------------------------
// set (hashtable membership, intptr keys)
// -------------------------------------------------------------------------

static void  *set_create( size_t n)
{
   MULLE_C_UNUSED( n);
   return( mulle_set_create( 0, &mulle_container_keycallback_intptr, &mulle_allocator_default));
}


static void  set_insert( void *state, size_t i)
{
   mulle_set_set( (struct mulle_set *) state, (void *) (intptr_t) (i + 1));
}


static size_t  set_lookup( void *state, size_t i)
{
   return( mulle_set_member( (struct mulle_set *) state, (void *) (intptr_t) (i + 1)));
}


static void  set_remove( void *state, size_t i)
{
   mulle_set_remove( (struct mulle_set *) state, (void *) (intptr_t) (i + 1));
}


static void  set_destroy( void *state)
{
   mulle_set_destroy( (struct mulle_set *) state);
}


// -------------------------------------------------------------------------
// pointerset (hashtable membership, pointer keys)
// -------------------------------------------------------------------------

static void  *pointerset_create( size_t n)
{
   struct mulle_pointerset   *set;

   MULLE_C_UNUSED( n);

   set = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *set));
   mulle_pointerset_init( set, 0, &mulle_allocator_default);
   return( set);
}


static void  pointerset_insert( void *state, size_t i)
{
   mulle_pointerset_insert( (struct mulle_pointerset *) state, (void *) (intptr_t) (i + 1));
}


static size_t  pointerset_lookup( void *state, size_t i)
{
   return( mulle_pointerset_get( (struct mulle_pointerset *) state,
                                 (void *) (intptr_t) (i + 1)) != NULL);
}


static void  pointerset_remove( void *state, size_t i)
{
   mulle_pointerset_remove( (struct mulle_pointerset *) state, (void *) (intptr_t) (i + 1));
}


static void  pointerset_destroy( void *state)
{
   mulle_pointerset_done( (struct mulle_pointerset *) state);
   mulle_allocator_free( &mulle_allocator_default, state);
}


// -------------------------------------------------------------------------
// assoc (sorted key/value)
// -------------------------------------------------------------------------

static void  *assoc_create( size_t n)
{
   MULLE_C_UNUSED( n);
   return( mulle_assoc_create( 0, &g_kvcallback,
                               _mulle_pointerpair_compare_intptr_key,
                               &mulle_allocator_default));
}


static void  assoc_warmup( void *state, size_t n)
{
   // the first get sorts the backing array; keep that out of the timed loop
   MULLE_C_UNUSED( n);
   mulle_assoc_get( (struct mulle_assoc *) state, (void *) (intptr_t) 1);
}


static void  assoc_insert( void *state, size_t i)
{
   mulle_assoc_add( (struct mulle_assoc *) state,
                    (void *) (intptr_t) (i + 1),
                    (void *) (intptr_t) (i + 1));
}


static size_t  assoc_lookup( void *state, size_t i)
{
   return( mulle_assoc_get( (struct mulle_assoc *) state, (void *) (intptr_t) (i + 1)) != NULL);
}


static void  assoc_remove( void *state, size_t i)
{
   struct mulle_assoc   *assoc;
   size_t                count;

   // remove the largest key still present: it sits at the end of the sorted
   // array, so the removal itself is O(1) and the benchmark stays log-linear
   MULLE_C_UNUSED( i);

   assoc = (struct mulle_assoc *) state;
   count = mulle_assoc_get_count( assoc);
   mulle_assoc_remove( assoc, (void *) (intptr_t) count);
}


static void  assoc_destroy( void *state)
{
   mulle_assoc_destroy( (struct mulle_assoc *) state);
}


// -------------------------------------------------------------------------
// pointerpairarray (sorted pairs, binary search)
// -------------------------------------------------------------------------

static void  *pointerpairarray_create( size_t n)
{
   MULLE_C_UNUSED( n);
   return( mulle_pointerpairarray_create( &mulle_allocator_default));
}


static void  pointerpairarray_insert( void *state, size_t i)
{
   struct mulle_pointerpair   pair;

   // keys ascend, so the array stays sorted and find() stays a binary search
   pair.key   = (void *) (intptr_t) (i + 1);
   pair.value = (void *) (intptr_t) (i + 1);
   mulle_pointerpairarray_add( (struct mulle_pointerpairarray *) state, pair);
}


static size_t  pointerpairarray_lookup( void *state, size_t i)
{
   struct mulle_pointerpair   search;

   search.key   = (void *) (intptr_t) (i + 1);
   search.value = NULL;
   return( mulle_pointerpairarray_find( (struct mulle_pointerpairarray *) state, search)
           != mulle_not_found_e);
}


static void  pointerpairarray_remove( void *state, size_t i)
{
   struct mulle__pointerpairarray   *array;
   size_t                            count;

   // remove the last (largest key) pair: no shift, so removal is O(1) and
   // the benchmark measures the lookup/removal pairing, not an O(n^2) memmove
   MULLE_C_UNUSED( i);

   array = (struct mulle__pointerpairarray *) state;
   count = mulle_pointerpairarray_get_count( (struct mulle_pointerpairarray *) state);
   _mulle__pointerpairarray_remove_in_range( array, mulle_range_make( count - 1, 1));
}


static void  pointerpairarray_destroy( void *state)
{
   mulle_pointerpairarray_destroy( (struct mulle_pointerpairarray *) state);
}


// -------------------------------------------------------------------------
// rangeset (coalescing ranges)
// -------------------------------------------------------------------------

struct rangeset_state
{
   struct mulle__rangeset   set;
   size_t                   n;
};


static void  *rangeset_create( size_t n)
{
   struct rangeset_state   *state;

   state     = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *state));
   state->n  = n;
   _mulle__rangeset_init( &state->set, 0, &mulle_allocator_default);
   return( state);
}


static void  rangeset_insert( void *state, size_t i)
{
   _mulle__rangeset_insert( &((struct rangeset_state *) state)->set,
                            mulle_range_make( i, 1),
                            &mulle_allocator_default);
}


static size_t  rangeset_lookup( void *state, size_t i)
{
   return( _mulle__rangeset_contains( &((struct rangeset_state *) state)->set,
                                      mulle_range_make( i, 1)));
}


static void  rangeset_remove( void *state, size_t i)
{
   struct rangeset_state   *s;

   s = (struct rangeset_state *) state;
   _mulle__rangeset_remove( &s->set,
                            mulle_range_make( s->n - 1 - i, 1),
                            &mulle_allocator_default);
}


static void  rangeset_destroy( void *state)
{
   struct rangeset_state   *s;

   s = (struct rangeset_state *) state;
   _mulle__rangeset_done( &s->set, &mulle_allocator_default);
   mulle_allocator_free( &mulle_allocator_default, s);
}


// -------------------------------------------------------------------------
// rangemap (range -> value)
// -------------------------------------------------------------------------

struct rangemap_state
{
   struct mulle__rangemap   map;
   size_t                   n;
};


static void  *rangemap_create( size_t n)
{
   struct rangemap_state   *state;

   state     = mulle_allocator_calloc( &mulle_allocator_default, 1, sizeof( *state));
   state->n  = n;
   _mulle__rangemap_init( &state->map, 0, &mulle_allocator_default);
   return( state);
}


static void  rangemap_insert( void *state, size_t i)
{
   _mulle__rangemap_insert( &((struct rangemap_state *) state)->map,
                            mulle_range_make( i, 1),
                            (void *) (intptr_t) (i + 1),
                            &mulle_allocator_default);
}


static size_t  rangemap_lookup( void *state, size_t i)
{
   return( _mulle__rangemap_search( &((struct rangemap_state *) state)->map, i)
           != mulle_not_found_e);
}


static void  rangemap_remove( void *state, size_t i)
{
   struct rangemap_state   *s;

   s = (struct rangemap_state *) state;
   _mulle__rangemap_remove( &s->map,
                            mulle_range_make( s->n - 1 - i, 1),
                            &mulle_allocator_default);
}


static void  rangemap_destroy( void *state)
{
   struct rangemap_state   *s;

   s = (struct rangemap_state *) state;
   _mulle__rangemap_done( &s->map, &mulle_allocator_default);
   mulle_allocator_free( &mulle_allocator_default, s);
}


// -------------------------------------------------------------------------
// driver
// -------------------------------------------------------------------------

static struct bench_adapter   g_adapters[] =
{
   { "pointerarray",     "get",    "pop",    0, pa_create,             NULL,          pa_insert,             pa_lookup,             pa_remove,             pa_destroy             },
   { "array",            "get",    "pop",    0, array_create,          NULL,          array_insert,          array_lookup,          array_remove,          array_destroy          },
   { "structarray",      "get",    "pop",    0, structarray_create,    NULL,          structarray_insert,    structarray_lookup,    structarray_remove,    structarray_destroy    },
   { "map",              "lookup", "remove", 0, map_create,            NULL,          map_insert,            map_lookup,            map_remove,            map_destroy            },
   { "pointermap",       "lookup", "remove", 0, pointermap_create,     NULL,          pointermap_insert,     pointermap_lookup,     pointermap_remove,     pointermap_destroy     },
   { "set",              "member", "remove", 0, set_create,            NULL,          set_insert,            set_lookup,            set_remove,            set_destroy            },
   { "pointerset",       "member", "remove", 0, pointerset_create,     NULL,          pointerset_insert,     pointerset_lookup,     pointerset_remove,     pointerset_destroy     },
   { "assoc",            "search", "remove", 0, assoc_create,          assoc_warmup,  assoc_insert,          assoc_lookup,          assoc_remove,          assoc_destroy          },
   { "pointerpairarray", "find",   "remove", 1, pointerpairarray_create, NULL,        pointerpairarray_insert, pointerpairarray_lookup, pointerpairarray_remove, pointerpairarray_destroy },
   { "rangeset",         "member", "remove", 0, rangeset_create,       NULL,          rangeset_insert,       rangeset_lookup,       rangeset_remove,       rangeset_destroy       },
   { "rangemap",         "search", "remove", 0, rangemap_create,       NULL,          rangemap_insert,       rangemap_lookup,       rangemap_remove,       rangemap_destroy       },
   { "pointerqueue",     NULL,     "pop",    0, pointerqueue_create,   NULL,          pointerqueue_insert,   NULL,                  pointerqueue_remove,   pointerqueue_destroy   },
   { "structqueue",      NULL,     "pop",    0, structqueue_create,    NULL,          structqueue_insert,    NULL,                  structqueue_remove,    structqueue_destroy    },
};


static size_t  g_checksum;


static size_t  rounds_for( size_t n)
{
   size_t   rounds;

   if( n >= BENCH_ROUND_MIN_N)
      return( 1);

   rounds = (size_t) (BENCH_TARGET_OPS / n);
   if( ! rounds)
      rounds = 1;
   if( rounds > BENCH_MAX_ROUNDS)
      rounds = BENCH_MAX_ROUNDS;
   return( rounds);
}


static void  report( char *container, char *op, size_t n, uint64_t elapsed, size_t ops)
{
   fprintf( stderr, "%s,%s,%zu,%.3f\n",
            container,
            op,
            n,
            (double) elapsed / (double) ops);
}


static uint64_t  measure_insert( struct bench_adapter *adapter, size_t n)
{
   void        *state;
   size_t       i;
   size_t       r;
   size_t       rounds;
   uint64_t     total;
   uint64_t     t0;
   uint64_t     t1;

   rounds = rounds_for( n);
   total  = 0;

   // time only the insert loop, not create/destroy (many rounds for small n)
   for( r = 0; r < rounds; r++)
   {
      state = (*adapter->create)( n);
      t0    = bench_now();
      for( i = 0; i < n; i++)
         (*adapter->insert)( state, i);
      t1     = bench_now();
      total += t1 - t0;
      (*adapter->destroy)( state);
   }

   report( adapter->name, "insert", n, total, rounds * n);
   return( total);
}


static uint64_t  measure_lookup( struct bench_adapter *adapter, size_t n)
{
   void        *state;
   size_t       i;
   size_t       j;
   size_t       r;
   size_t       rounds;
   uint64_t     elapsed;
   uint64_t     t0;
   uint64_t     t1;

   if( ! adapter->read_op)
      return( 0);

   // build the container once, then hammer it (lookup does not change state)
   state = (*adapter->create)( n);
   for( i = 0; i < n; i++)
      (*adapter->insert)( state, i);
   if( adapter->warmup)
      (*adapter->warmup)( state, n);

   if( adapter->linear_read)
   {
      size_t   depth;
      size_t   probes;
      size_t   passes;

      // bounded probe set: average scan depth stays ~n/2, total work bounded
      probes = (n < BENCH_MAX_PROBES) ? n : BENCH_MAX_PROBES;
      depth  = n / 2 ? n / 2 : 1;
      passes = (size_t) (BENCH_TARGET_OPS / (probes * depth));
      if( ! passes)
         passes = 1;
      if( passes > BENCH_MAX_ROUNDS)
         passes = BENCH_MAX_ROUNDS;

      t0 = bench_now();
      for( r = 0; r < passes; r++)
         for( j = 0; j < probes; j++)
            g_checksum += (*adapter->lookup)( state, (j * n) / probes);
      t1 = bench_now();

      (*adapter->destroy)( state);

      report( adapter->name, adapter->read_op, n, t1 - t0, passes * probes);
      return( t1 - t0);
   }

   rounds = rounds_for( n);
   t0     = bench_now();
   for( r = 0; r < rounds; r++)
      for( i = 0; i < n; i++)
         g_checksum += (*adapter->lookup)( state, i);
   t1      = bench_now();
   elapsed = t1 - t0;

   (*adapter->destroy)( state);

   report( adapter->name, adapter->read_op, n, elapsed, rounds * n);
   return( elapsed);
}


static uint64_t  measure_remove( struct bench_adapter *adapter, size_t n)
{
   void        *state;
   size_t       i;
   size_t       r;
   size_t       rounds;
   uint64_t     total;
   uint64_t     t0;
   uint64_t     t1;

   rounds = rounds_for( n);
   total  = 0;

   // each round rebuilds the container untimed, then times running it empty
   for( r = 0; r < rounds; r++)
   {
      state = (*adapter->create)( n);
      for( i = 0; i < n; i++)
         (*adapter->insert)( state, i);
      if( adapter->warmup)
         (*adapter->warmup)( state, n);
      t0 = bench_now();
      for( i = 0; i < n; i++)
         (*adapter->remove)( state, i);
      t1     = bench_now();
      total += t1 - t0;
      (*adapter->destroy)( state);
   }

   report( adapter->name, adapter->remove_op, n, total, rounds * n);
   return( total);
}


static void  measure_adapter( struct bench_adapter *adapter, int max_exp)
{
   size_t      n;
   int         e;
   uint64_t    total;

   for( e = BENCH_MIN_EXP; e <= max_exp; e++)
   {
      n     = (size_t) 1 << e;
      total = measure_insert( adapter, n);
      total += measure_lookup( adapter, n);
      total += measure_remove( adapter, n);

      if( total > BENCH_SIZE_BUDGET)
      {
         fprintf( stderr, "# %s: stopped after n=%zu (> %llu ms)\n",
                  adapter->name,
                  n,
                  (unsigned long long) (BENCH_SIZE_BUDGET / 1000000ULL));
         break;
      }
   }
}


int  main( int argc, char *argv[])
{
   size_t   i;
   int      max_exp;

   max_exp = BENCH_MAX_EXP;
   if( argc > 1)
   {
      max_exp = atoi( argv[ 1]);
      if( max_exp < BENCH_MIN_EXP || max_exp > 30)
         max_exp = BENCH_MAX_EXP;
   }

   g_kvcallback.keycallback   = mulle_container_keycallback_intptr;
   g_kvcallback.valuecallback = mulle_container_valuecallback_intptr;

   bench_warn_if_debug();
   fprintf( stderr, "# mulle-container scaling benchmark\n");
   fprintf( stderr, "# columns: container,op,n,ns_op   units: ns/op\n");
   fprintf( stderr, "container,op,n,ns_op\n");

   for( i = 0; i < sizeof( g_adapters) / sizeof( g_adapters[ 0]); i++)
      measure_adapter( &g_adapters[ i], max_exp);

   fprintf( stderr, "# checksum %zu\n", g_checksum);
   return( 0);
}
