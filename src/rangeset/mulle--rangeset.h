#ifndef mulle__rangeset_h__
#define mulle__rangeset_h__

#include "include.h"

#include <stdlib.h>
#include <stdio.h>

#include "mulle-range.h"


//
// the strategy of this container is:
//
// * to not have overlapping ranges
// * to not have adjacent ranges
// * to not have ranges of size 0
// * always be sorted
//
// MEMO: the previous incarnation used lazy coalescing, when deemed
//       necessary. But in the end the amount of memory moves necessary
//       done laty was likely the same as doing it immediately.
//
struct mulle__rangeset
{
   struct mulle_range   *_ranges;
   uintptr_t             _length;
   uintptr_t             _size;
};


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_init( struct mulle__rangeset *p,
                                            uintptr_t capacity,
                                            struct mulle_allocator *allocator)
{
   memset( p, 0, sizeof( *p));
   if( capacity > 0)
   {
      p->_size   = capacity;
      p->_ranges = mulle_allocator_malloc( allocator, sizeof( struct mulle_range) * p->_size);
   }
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_done( struct mulle__rangeset *p,
                                            struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, p->_ranges);
}


MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_reset( struct mulle__rangeset *p,
                               struct mulle_allocator *allocator);

/*
 * primitive access code
 */
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_first( struct mulle__rangeset *p)
{
   return( p->_length ? p->_ranges[ 0].location : mulle_not_found_e);
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_last( struct mulle__rangeset *p)
{
   return( p->_length ? mulle_range_get_end( p->_ranges[ p->_length - 1 ]) - 1 : mulle_not_found_e);
}


/*
 * primitive access code
 */
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_rangecount( struct mulle__rangeset *p)
{
   return( p->_length);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_range  _mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                              uintptr_t i)
{
   if( i >= p->_length)
      return( mulle_range_create( mulle_not_found_e, 0));
   return( p->_ranges[ i]);
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_sum_lengths( struct mulle__rangeset *p)
{
   struct mulle_range   *curr;
   struct mulle_range   *sentinel;
   uintptr_t            length;

   length = 0;
   curr   = p->_ranges;
   sentinel = &p->_ranges[ p->_length];
   for( ; curr < sentinel; curr++)
      length += curr->length;
   return( length);
}


MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__rangeset_sum_lengths_range( struct mulle__rangeset *p,
                                                struct mulle_range range);


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_shrinktofit( struct mulle__rangeset *p,
                                                  struct mulle_allocator *allocator)
{
   if( p->_length > p->_size)
   {
      p->_ranges = mulle_allocator_realloc_strict( allocator,
                                                   p->_ranges,
                                                   p->_length * sizeof( struct mulle_range));
      p->_size   = p->_length;
   }
}


MULLE_C_NONNULL_FIRST
void   __mulle__rangeset_insert( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   __mulle__rangeset_remove( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_grow( struct mulle__rangeset *p,
                             struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_contains( struct mulle__rangeset *p,
                              struct mulle_range range);

MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_intersects( struct mulle__rangeset *p,
                                  struct mulle_range range);

MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_fprint( struct mulle__rangeset *set, FILE *fp);


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_print( struct mulle__rangeset *set)
{
   _mulle__rangeset_fprint( set, stdout);
}


MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert_ranges( struct mulle__rangeset *p,
                                      struct mulle_range *ranges,
                                      uintptr_t n,
                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove_ranges( struct mulle__rangeset *p,
                                      struct mulle_range *ranges,
                                      uintptr_t n,
                                      struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_insert_rangeset( struct mulle__rangeset *p,
                                                      struct mulle__rangeset *other,
                                                      struct mulle_allocator *allocator)
{
   _mulle__rangeset_insert_ranges( p, p->_ranges, p->_length, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_remove_rangeset( struct mulle__rangeset *p,
                                                      struct mulle__rangeset *other,
                                                      struct mulle_allocator *allocator)
{
   _mulle__rangeset_remove_ranges( p, p->_ranges, p->_length, allocator);
}


//
// returns 0 on success, -1 for underflow (no changes) and +1 for overflow
// (no changes)
//
MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_shift( struct mulle__rangeset *p,
                              uintptr_t location,
                              intptr_t delta,
                              struct mulle_allocator *allocator);

enum mulle_rangeset_searchoperation
{
   mulle_rangeset_less_than,
   mulle_rangeset_less_than_or_equal,
   mulle_rangeset_equal,
   mulle_rangeset_greater_than_or_equal,
   mulle_rangeset_greater_than
};


MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__rangeset_search( struct mulle__rangeset *p,
                                     uintptr_t location,
                                     enum mulle_rangeset_searchoperation searchOp);

#endif
