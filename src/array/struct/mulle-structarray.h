#ifndef mulle_structarray_h__
#define mulle_structarray_h__

#include "mulle--structarray.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>



#pragma mark - item array

//
// This is a growing array of struct sized structs.
// It has been coded for a fast "reserve" operation.
//
struct mulle_structarray
{
   MULLE__STRUCTARRAY_BASE;
   struct mulle_allocator   *allocator;
};


static inline struct mulle_structarray  *
   mulle_structarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_structarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle_structarray));
   return( array);
}

MULLE_C_NONNULL_FIRST
static inline void   _mulle_structarray_init( struct mulle_structarray *array,
                                              unsigned int sizeof_struct,
                                              unsigned int alignof_struct,
                                              unsigned int capacity,
                                              struct mulle_allocator *allocator)
{
   _mulle__structarray_init( (struct mulle__structarray *) array,
                             sizeof_struct,
                             alignof_struct,
                             capacity,
                             allocator);

   array->allocator      = allocator;
}


static inline struct mulle_structarray *
   mulle_structarray_create( unsigned int sizeof_struct,
                             unsigned int alignof_struct,
                             unsigned int capacity,
                             struct mulle_allocator *allocator)
{
   struct mulle_structarray  *array;

   array = mulle_structarray_alloc( allocator);
   _mulle_structarray_init( array, sizeof_struct, alignof_struct, capacity, allocator);
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle_structarray_done( struct mulle_structarray *array)
{
   _mulle__structarray_done( (struct mulle__structarray *) array, array->allocator);
}



static inline void  mulle_structarray_destroy( struct mulle_structarray *array)
{
   struct mulle_allocator   *allocator;

   if( array)
   {
      allocator = array->allocator;
      _mulle_structarray_done( array);
      mulle_allocator_free( allocator, array);
   }
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_structarray_reset( struct mulle_structarray *array)
{
   _mulle__structarray_reset( (struct mulle__structarray *) array);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_structarray_get_count( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_count( (struct mulle__structarray *) array));
}


static inline unsigned int
   mulle_structarray_get_count( struct mulle_structarray *array)
{
   if( ! array)
      return( 0);
   return( _mulle_structarray_get_count( array));
}


// cheaper as we don't divide here
MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_structarray_get_usedsize( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_usedsize( (struct mulle__structarray *) array));
}


# pragma mark - array operations

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_add( struct mulle_structarray *array,
                           void *item)
{
   return( _mulle__structarray_add( (struct mulle__structarray *) array,
                                    item,
                                    array->allocator));
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_reserve( struct mulle_structarray *array)
{
   return( _mulle__structarray_reserve( (struct mulle__structarray *) array,
                                        array->allocator));
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator  *
   _mulle_structarray_get_allocator( struct mulle_structarray *array)
{
   return( array->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get_first( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_first( (struct mulle__structarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get( struct mulle_structarray *array, unsigned int i)
{
   return( _mulle__structarray_get( (struct mulle__structarray *) array, i));
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get_last( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_last( (struct mulle__structarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_remove_last( struct mulle_structarray *array)
{
   return( _mulle__structarray_remove_last( (struct mulle__structarray *) array));
}


static inline void *
   mulle_structarray_get_last( struct mulle_structarray *array)
{
   return( mulle__structarray_get_last( (struct mulle__structarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_size_to_fit( struct mulle_structarray *array)
{
   return( _mulle__structarray_size_to_fit( (struct mulle__structarray *) array,
                                            array->allocator));
}


#pragma mark - structarray enumerator

#define MULLE_STRUCTARRAYENUMERATOR_BASE   MULLE__STRUCTARRAYENUMERATOR_BASE


struct mulle_structarrayenumerator
{
   MULLE_STRUCTARRAYENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct mulle_structarrayenumerator
   _mulle_structarray_enumerate( struct mulle_structarray *array)
{
   struct mulle__structarrayenumerator   rover;

   rover = _mulle__structarray_enumerate( (struct mulle__structarray *) array);
   return( *(struct mulle_structarrayenumerator *) &rover);
}


static inline struct mulle_structarrayenumerator
   mulle_structarray_enumerate( struct mulle_structarray *array)
{
   struct mulle__structarrayenumerator   rover;

   rover = mulle__structarray_enumerate( (struct mulle__structarray *) array);
   return( *(struct mulle_structarrayenumerator *) &rover);
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarrayenumerator_next( struct mulle_structarrayenumerator *rover)
{
   return( _mulle__structarrayenumerator_next( (struct mulle__structarrayenumerator *) rover));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarrayenumerator_done( struct mulle_structarrayenumerator *rover)
{
}


static inline void
   mulle_structarrayenumerator_done( struct mulle_structarrayenumerator *rover)
{
}


#pragma mark - structarray enumerator

#define MULLE_STRUCTARRAYREVERSEENUMERATOR_BASE   MULLE__STRUCTARRAYREVERSEENUMERATOR_BASE

struct mulle_structarrayreverseenumerator
{
   MULLE_STRUCTARRAYENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct mulle_structarrayreverseenumerator
   _mulle_structarray_reverseenumerate( struct mulle_structarray *array)
{
   struct mulle__structarrayreverseenumerator   rover;

   rover = _mulle__structarray_reverseenumerate( (struct mulle__structarray *) array);
   return( *(struct mulle_structarrayreverseenumerator *) &rover);
}


static inline struct mulle_structarrayreverseenumerator
   mulle_structarray_reverseenumerate( struct mulle_structarray *array)
{
   struct mulle__structarrayreverseenumerator   rover;

   rover = mulle__structarray_reverseenumerate( (struct mulle__structarray *) array);
   return( *(struct mulle_structarrayreverseenumerator *) &rover);
}

MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarrayreverseenumerator_next( struct mulle_structarrayreverseenumerator *rover)
{
   return( _mulle__structarrayreverseenumerator_next( (struct mulle__structarrayreverseenumerator *) rover));
}

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarrayreverseenumerator_done( struct mulle_structarrayreverseenumerator *rover)
{
}


static inline void
   mulle_structarrayreverseenumerator_done( struct mulle_structarrayreverseenumerator *rover)
{
}

#endif

