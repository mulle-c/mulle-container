//
//  bench.h
//  mulle-container benchmarks
//
//  Shared helpers for the benchmark suite in test/bench/.
//
//  Two kinds of benchmarks live here:
//
//  * Deterministic "allocation/cost" benchmarks (golden .stdout files).
//    They print allocation and comparison counts, which are stable across
//    runs and across 32/64-bit, so the test harness can compare them.
//
//  * A wall-clock timing benchmark (no .stdout file). It prints ns/op.
//
//  The counting allocator wraps mulle_allocator_default and uses the
//  allocator's `aba` slot as a back-pointer to its counter struct.
//

#ifndef bench_h__
#define bench_h__

// for clock_gettime( CLOCK_MONOTONIC)
#ifndef _POSIX_C_SOURCE
# define _POSIX_C_SOURCE 200809L
#endif

#include <mulle-container/mulle-container.h>

#include <stdio.h>
#include <stdint.h>
#include <time.h>


struct bench_allocator
{
   struct mulle_allocator   allocator;   // the one handed to containers
   struct mulle_allocator   *delegate;   // mulle_allocator_default
   size_t                   n_calloc;
   size_t                   n_realloc;
   size_t                   n_free;
   size_t                   calloc_bytes;
   size_t                   realloc_bytes;
   size_t                   live;        // outstanding allocations
};


static void  *bench_calloc( size_t n, size_t size, struct mulle_allocator *allocator)
{
   struct bench_allocator  *bench;

   bench = (struct bench_allocator *) allocator->aba;

   bench->n_calloc++;
   bench->calloc_bytes += n * size;
   bench->live++;

   return( bench->delegate->calloc( n, size, bench->delegate));
}


static void  *bench_realloc( void *block, size_t size, struct mulle_allocator *allocator)
{
   struct bench_allocator  *bench;

   bench = (struct bench_allocator *) allocator->aba;

   bench->n_realloc++;
   bench->realloc_bytes += size;
   if( ! block)             // realloc( NULL, size) allocates
      bench->live++;

   return( bench->delegate->realloc( block, size, bench->delegate));
}


static void  bench_free( void *block, struct mulle_allocator *allocator)
{
   struct bench_allocator  *bench;

   bench = (struct bench_allocator *) allocator->aba;

   bench->n_free++;
   if( block)
      bench->live--;

   bench->delegate->free( block, bench->delegate);
}


static void  bench_allocator_init( struct bench_allocator *bench)
{
   bench->delegate  = &mulle_allocator_default;
   bench->allocator = *bench->delegate;   // copy fail/abafree/aba from default

   bench->allocator.calloc  = bench_calloc;
   bench->allocator.realloc = bench_realloc;
   bench->allocator.free    = bench_free;
   bench->allocator.aba     = bench;      // back-pointer

   bench->n_calloc = 0;
   bench->n_realloc = 0;
   bench->n_free = 0;
   bench->calloc_bytes = 0;
   bench->realloc_bytes = 0;
   bench->live = 0;
}


static void  bench_allocator_print( struct bench_allocator *bench, char *label)
{
   printf( "%scalloc=%zu realloc=%zu free=%zu live=%zu\n",
           label,
           bench->n_calloc,
           bench->n_realloc,
           bench->n_free,
           bench->live);
}


//
// monotonic clock in nanoseconds
//
#if defined(_WIN32)

# include <windows.h>

static uint64_t  bench_now( void)
{
   LARGE_INTEGER   counter;
   LARGE_INTEGER   frequency;

   QueryPerformanceCounter( &counter);
   QueryPerformanceFrequency( &frequency);
   return( (uint64_t) ((counter.QuadPart * 1000000000LL) / frequency.QuadPart));
}

#else

static uint64_t  bench_now( void)
{
   struct timespec  ts;

   clock_gettime( CLOCK_MONOTONIC, &ts);
   return( (uint64_t) ts.tv_sec * 1000000000ULL + (uint64_t) ts.tv_nsec);
}

#endif


//
// comparison counter for mulle_assoc and mulle_pointerpairarray
//
static size_t  g_comparisons;


static int  bench_compare_intptr( struct mulle_pointerpair *a,
                                  struct mulle_pointerpair *b,
                                  void *userinfo)
{
   g_comparisons++;
   return( _mulle_pointerpair_compare_intptr_key( a, b, userinfo));
}

#endif
