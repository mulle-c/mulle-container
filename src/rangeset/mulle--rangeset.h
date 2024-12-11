#ifndef mulle__rangeset_h__
#define mulle__rangeset_h__

#include "include.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// A mulle__rangeset is like an array of ns_ranges, where overlapping
// ns_ranges are coalesced and removal of a ns_range may lead to splits.
//
// Remember: ns-range can just carry 0 to INT_MAX -1 !
//
// the strategy of this container is:
//
// * to not have overlapping ranges
// * to not have adjacent ranges
// * to not have ranges of _size 0
// * always be sorted
//
// MEMO: the previous incarnation _used lazy coalescing, when deemed
//       necessary. But in the end the amount of memory moves necessary
//       done laty was likely the same as doing it immediately.
//
struct mulle__rangeset
{
   struct mulle_range   *_ranges;
   struct mulle_range   *_initial_storage; // for use with static storage
   size_t                _length;
   size_t                _size;
};

#define MULLE__RANGESET_DATA( storage, size)  \
   ((struct mulle__rangeset)                  \
   {                                          \
      ._ranges          = (storage),          \
      ._initial_storage = (storage),          \
      ._size            = (size)              \
   })



MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_init( struct mulle__rangeset *p,
                                            size_t capacity,
                                            struct mulle_allocator *allocator)
{
   memset( p, 0, sizeof( *p));
   if( capacity > 0)
   {
      p->_size   = capacity;
      p->_ranges = mulle_allocator_malloc( allocator, sizeof( struct mulle_range) * p->_size);
   }
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   _mulle__rangeset_init_with_static_ranges( struct mulle__rangeset *p,
                                                               struct mulle_range *storage,
                                                               size_t capacity)
{
   memset( p, 0, sizeof( *p));
   p->_size            = capacity;
   p->_ranges          = storage;
   p->_initial_storage = storage;
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_done( struct mulle__rangeset *p,
                                            struct mulle_allocator *allocator)
{
   if( p->_ranges != p->_initial_storage)
      mulle_allocator_free( allocator, p->_ranges);
#ifdef DEBUG   
   memset( p, 0xFD, sizeof( struct mulle__rangeset));
#endif    
}


MULLE__CONTAINER_GLOBAL
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
static inline struct mulle_range   _mulle__rangeset_get( struct mulle__rangeset *p,
                                                         size_t i)
{
   return( i < p->_length ? p->_ranges[ i] : mulle_range_make_invalid());
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_last( struct mulle__rangeset *p)
{
   return( p->_length
           ? mulle_range_get_max( p->_ranges[ p->_length - 1]) - 1
           : mulle_not_found_e);
}


/*
 * primitive access code
 */
MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_get_rangecount( struct mulle__rangeset *p)
{
   return( p->_length);
}


// non checker method for objc
MULLE_C_NONNULL_FIRST
static inline struct mulle_range  __mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                               uintptr_t i)
{
   return( p->_ranges[ i]);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_range  _mulle__rangeset_get_range( struct mulle__rangeset *p,
                                                              uintptr_t i)
{
   if( i >= p->_length)
      return( mulle_range_make( mulle_not_found_e, 0));
   return( p->_ranges[ i]);
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t   _mulle__rangeset_sum_lengths( struct mulle__rangeset *p)
{
   struct mulle_range   *_curr;
   struct mulle_range   *sentinel;
   uintptr_t            length;

   length = 0;
   _curr   = p->_ranges;
   sentinel = &p->_ranges[ p->_length];
   for( ; _curr < sentinel; _curr++)
      length += _curr->length;
   return( length);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__rangeset_sum_lengths_range( struct mulle__rangeset *p,
                                                struct mulle_range range);


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_shrink_to_fit( struct mulle__rangeset *p,
                                                     struct mulle_allocator *allocator)
{
   if( p->_length > p->_size && p->_ranges != p->_initial_storage)
   {
      p->_ranges = mulle_allocator_realloc_strict( allocator,
                                                   p->_ranges,
                                                   p->_length * sizeof( struct mulle_range));
      p->_size   = p->_length;
   }
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   __mulle__rangeset_insert( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   __mulle__rangeset_remove( struct mulle__rangeset *p,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_grow( struct mulle__rangeset *p,
                              struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_contains( struct mulle__rangeset *p,
                                 struct mulle_range range);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int   _mulle__rangeset_intersects( struct mulle__rangeset *p,
                                   struct mulle_range range);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_remove( struct mulle__rangeset *p,
                                struct mulle_range range,
                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_fprint( struct mulle__rangeset *set, FILE *fp);


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_print( struct mulle__rangeset *set)
{
   _mulle__rangeset_fprint( set, stdout);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__rangeset_insert_ranges( struct mulle__rangeset *p,
                                       struct mulle_range *ranges,
                                       uintptr_t n,
                                       struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
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
   _mulle__rangeset_insert_ranges( p, other->_ranges, other->_length, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__rangeset_remove_rangeset( struct mulle__rangeset *p,
                                                       struct mulle__rangeset *other,
                                                       struct mulle_allocator *allocator)
{
   _mulle__rangeset_remove_ranges( p, other->_ranges, other->_length, allocator);
}


//
// shift will destroy indexes that are then outside of the 0-mulle_max_range
//
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void  _mulle__rangeset_shift( struct mulle__rangeset *p,
                              uintptr_t location,
                              intptr_t delta,
                              struct mulle_allocator *allocator);


enum mulle_rangeset_searchoperation
{
   mulle_rangeset_equal                 = 0x1, // equal bit
   mulle_rangeset_less_than             = 0x2, // < bit
   mulle_rangeset_less_than_or_equal    = 0x3, // eq bit + < bit
   mulle_rangeset_greater_than          = 0x4, // > bit
   mulle_rangeset_greater_than_or_equal = 0x5  // eq bit + > bit
};

static inline char  *
   mulle_rangeset_searchoperation_utf8_string( enum mulle_rangeset_searchoperation mode)
{
   switch( mode)
   {
   case mulle_rangeset_less_than             : return( "<");
   case mulle_rangeset_less_than_or_equal    : return( "<=");
   default                                   : return( "==");
   case mulle_rangeset_greater_than_or_equal : return( ">=");
   case mulle_rangeset_greater_than          : return( ">");
   }
}


//
// Find the closest location in the rangeset relative to location
// where the location result has to satisfy the search operation.
//
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__rangeset_search_location( struct mulle__rangeset *p,
                                              uintptr_t location,
                                              enum mulle_rangeset_searchoperation searchOp);


//
// Find the closest range in the rangeset relative to location
// where the range has to satisfy the search operation.
// A range of 2,3,4 is <= location 2, 3, 4, 5, ... and >= location 0, 1, 2 also
//                      < location 5,6... and > location 0,1 also
//                     == 2, 3, 4

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_range
   _mulle__rangeset_search_range( struct mulle__rangeset *p,
                                  uintptr_t location,
                                 enum mulle_rangeset_searchoperation searchOp);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_range
   _mulle__rangeset_search_nearest( struct mulle__rangeset *p,
                                                 uintptr_t location);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_range
   _mulle__rangeset_search_exact( struct mulle__rangeset *p,
                                  uintptr_t location);



// created by make-container-do.sh --no-callback --type struct mulle_range    --flexible mulle-rangeset

// created by make-container-do.sh --no-callback --type struct mulle_range    --flexible mulle--rangeset.c

#define mulle__rangeset_do( name)                              \
   for( struct mulle__rangeset                                 \
           name ## __container = { 0 },                        \
           *name = &name ## __container,                       \
           *name ## __i = NULL;                                \
        ! name ## __i;                                         \
        name ## __i =                                          \
        (                                                      \
           _mulle__rangeset_done( &name ## __container, NULL), \
           (void *) 0x1                                        \
        )                                                      \
      )                                                        \
      for( int  name ## __j = 0;    /* break protection */     \
           name ## __j < 1;                                    \
           name ## __j++)

#define mulle__rangeset_do_flexible( name, stackcount)              \
   struct mulle_range      name ## __storage[ stackcount];          \
   for( struct mulle__rangeset                                      \
           name ## __container =                                    \
              MULLE__RANGESET_DATA( name ## __storage, stackcount), \
           *name = &name ## __container,                            \
           *name ## __i = NULL;                                     \
        ! name ## __i;                                              \
        name ## __i =                                               \
        (                                                           \
           _mulle__rangeset_done( &name ## __container, NULL),      \
           (void *) 0x1                                             \
        )                                                           \
      )                                                             \
      for( int  name ## __j = 0;    /* break protection */          \
           name ## __j < 1;                                         \
           name ## __j++)


#endif
