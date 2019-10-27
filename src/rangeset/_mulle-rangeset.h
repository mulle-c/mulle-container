#ifndef _mulle_rangeset_h__
#define _mulle_rangeset_h__

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
struct _mulle_rangeset
{
   struct mulle_range   *_ranges;
   uintptr_t             _length;
   uintptr_t             _size;
};


static inline void   _mulle_rangeset_init( struct _mulle_rangeset *p,
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


static inline void   _mulle_rangeset_done( struct _mulle_rangeset *p,
                                           struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, p->_ranges);
}


/*
 * primitive access code
 */
static inline uintptr_t   _mulle_rangeset_get_rangecount( struct _mulle_rangeset *p)
{
   return( p->_length);
}



static inline struct mulle_range  _mulle_rangeset_get_range( struct _mulle_rangeset *p,
                                                             uintptr_t i)
{
   if( i >= p->_length)
      return( mulle_range_create( mulle_not_found_e, 0));
   return( p->_ranges[ i]);
}



static inline uintptr_t   _mulle_rangeset_sum_lengths( struct _mulle_rangeset *p)
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


uintptr_t   _mulle_rangeset_sum_lengths_range( struct _mulle_rangeset *p,
                                               struct mulle_range range);


static inline void   _mulle_rangeset_shrinktofit( struct _mulle_rangeset *p,
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


static inline void   _mulle_rangeset_insert( struct _mulle_rangeset *p,
                                             struct mulle_range range,
                                             struct mulle_allocator *allocator)
{
   extern void   __mulle_rangeset_insert( struct _mulle_rangeset *p,
                                          struct mulle_range range,
                                          struct mulle_allocator *allocator);
   if( ! range.length)
      return;
   // illegal,
   if( ! mulle_range_is_valid( range))
      return;

   __mulle_rangeset_insert( p, range, allocator);
}


static inline void   _mulle_rangeset_remove( struct _mulle_rangeset *p,
                                             struct mulle_range range,
                                             struct mulle_allocator *allocator)
{
   extern void   __mulle_rangeset_remove( struct _mulle_rangeset *p,
                                          struct mulle_range range,
                                          struct mulle_allocator *allocator);
   if( ! range.length)
      return;
   // illegal,
   if( ! mulle_range_is_valid( range))
      return;

   __mulle_rangeset_remove( p, range, allocator);
}



void   _mulle_rangeset_grow( struct _mulle_rangeset *p,
                             struct mulle_allocator *allocator);

void   _mulle_rangeset_insert( struct _mulle_rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);

int   _mulle_rangeset_member( struct _mulle_rangeset *p,
                              struct mulle_range range);

void   _mulle_rangeset_remove( struct _mulle_rangeset *p,
                               struct mulle_range range,
                               struct mulle_allocator *allocator);

void   _mulle_rangeset_fprint( struct _mulle_rangeset *set, FILE *fp);


static inline void   _mulle_rangeset_print( struct _mulle_rangeset *set)
{
   _mulle_rangeset_fprint( set, stdout);
}

void   _mulle_rangeset_insert_ranges( struct _mulle_rangeset *p,
                                      struct mulle_range *ranges,
                                      uintptr_t n,
                                      struct mulle_allocator *allocator);

void   _mulle_rangeset_remove_ranges( struct _mulle_rangeset *p,
                                      struct mulle_range *ranges,
                                      uintptr_t n,
                                      struct mulle_allocator *allocator);


void   _mulle_rangeset_insert_rangeset( struct _mulle_rangeset *p,
                                        struct _mulle_rangeset *other,
                                        struct mulle_allocator *allocator)
{
   _mulle_rangeset_insert_ranges( p, p->_ranges, p->_length, allocator);
}


void   _mulle_rangeset_remove_rangeset( struct _mulle_rangeset *p,
                                        struct _mulle_rangeset *other,
                                        struct mulle_allocator *allocator)
{
   _mulle_rangeset_remove_ranges( p, p->_ranges, p->_length, allocator);
}


//
// returns 0 on success, -1 for underflow (no changes) and +1 for overflow
// (no changes)
//
int   _mulle_rangeset_shift( struct _mulle_rangeset *p,
                             uintptr_t location,
                             intptr_t delta,
                             struct mulle_allocator *allocator);

enum _mulle_rangeset_searchoperation
{
   _mulle_rangeset_less_than,
   _mulle_rangeset_less_thanOr_equal,
   _mulle_rangeset_equal,
   _mulle_rangeset_greater_than_or_equal,
   _mulle_rangeset_greater_than
};


uintptr_t   _mulle_rangeset_search( struct _mulle_rangeset *p,
                                    uintptr_t location,
                                    enum _mulle_rangeset_searchoperation searchOp);

#endif
