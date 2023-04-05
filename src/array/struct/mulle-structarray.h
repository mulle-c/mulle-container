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


#define MULLE_STRUCTARRAY_INIT( storage, type, count, allocator)   \
   ((struct mulle_structarray)                                     \
   {                                                               \
      storage,                                                     \
      storage,                                                     \
      &((char *) storage)[ count],                                 \
      storage,                                                     \
      (size_t) (sizeof( type) + (sizeof( type) % alignof( type))), \
      allocator                                                    \
   })


static inline struct mulle_structarray  *
   mulle_structarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_structarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle_structarray));
   return( array);
}


//
// it's assumed that "malloc" always returns memory that is properly aligned
// for any given type
//
MULLE_C_NONNULL_FIRST
static inline void   _mulle_structarray_init( struct mulle_structarray *array,
                                              size_t _sizeof_struct,
                                              unsigned int alignof_struct,
                                              unsigned int capacity,
                                              struct mulle_allocator *allocator)
{
   _mulle__structarray_init( (struct mulle__structarray *) array,
                             _sizeof_struct,
                             alignof_struct,
                             capacity,
                             allocator);

   array->allocator = allocator;
}


static inline void   mulle_structarray_init( struct mulle_structarray *array,
                                             size_t _sizeof_struct,
                                             unsigned int alignof_struct,
                                             unsigned int capacity,
                                             struct mulle_allocator *allocator)
{
   if(  array)
      _mulle_structarray_init( array,
                               _sizeof_struct,
                               alignof_struct,
                               capacity,
                               allocator);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_init_with_static_storage( struct mulle_structarray *array,
                                                 size_t _sizeof_struct,
                                                 unsigned int alignof_struct,
                                                 unsigned int count,
                                                 void  *storage,
                                                 struct mulle_allocator *allocator)
{
   _mulle__structarray_init_with_static_storage( (struct mulle__structarray *) array,
                                                  _sizeof_struct,
                                                  alignof_struct,
                                                  count,
                                                  storage,
                                                  allocator);

   array->allocator = allocator;
}


static inline void
   mulle_structarray_init_with_static_storage( struct mulle_structarray *array,
                                               size_t _sizeof_struct,
                                               unsigned int alignof_struct,
                                               unsigned int count,
                                               void  *storage,
                                               struct mulle_allocator *allocator)
{
   if(  array)
   {
      _mulle__structarray_init_with_static_storage( (struct mulle__structarray *) array,
                                                    _sizeof_struct,
                                                    alignof_struct,
                                                    count,
                                                    storage, //  NULL storage is OK, so useless
                                                    allocator);
      array->allocator = allocator;
   }
}



static inline struct mulle_structarray *
   mulle_structarray_create( size_t _sizeof_struct,
                             unsigned int alignof_struct,
                             unsigned int capacity,
                             struct mulle_allocator *allocator)
{
   struct mulle_structarray  *array;

   array = mulle_structarray_alloc( allocator);
   _mulle_structarray_init( array, _sizeof_struct, alignof_struct, capacity, allocator);
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle_structarray_done( struct mulle_structarray *array)
{
   _mulle__structarray_done( (struct mulle__structarray *) array, array->allocator);
}


static inline void  mulle_structarray_done( struct mulle_structarray *array)
{
   if( array)
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


static inline void   mulle_structarray_reset( struct mulle_structarray *array)
{
   if( array)
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
   _mulle_structarray_get_used_as_length( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_used_as_length( (struct mulle__structarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_structarray_get_size( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_size( (struct mulle__structarray *) array));
}


static inline unsigned int
   mulle_structarray_get_size( struct mulle_structarray *array)
{
   if( ! array)
      return( 0);

   return( _mulle__structarray_get_size( (struct mulle__structarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_structarray_get_size_as_length( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_size_as_length( (struct mulle__structarray *) array));
}


static inline size_t
   mulle_structarray_get_size_as_length( struct mulle_structarray *array)
{
   if( ! array)
      return( 0);

   return( _mulle__structarray_get_size_as_length( (struct mulle__structarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_structarray_get_element_size( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_element_size( (struct mulle__structarray *) array));
}


static inline size_t
   mulle_structarray_get_element_size( struct mulle_structarray *array)
{
   if( ! array)
      return( 0);

   return( _mulle__structarray_get_element_size( (struct mulle__structarray *) array));
}



# pragma mark - array operations

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_add( struct mulle_structarray *array, void *item)
{
   _mulle__structarray_add( (struct mulle__structarray *) array,
                             item,
                             array->allocator);
}


static inline void
   mulle_structarray_add( struct mulle_structarray *array, void *item)
{
   if( array && item)
      _mulle_structarray_add( array, item);
}



// guarantee ensures that you can add up to length items without a realloc
MULLE_C_NONNULL_FIRST
static inline void   *_mulle_structarray_guarantee( struct mulle_structarray *array,
                                                    unsigned int count)
{
   return( _mulle__structarray_guarantee( (struct mulle__structarray *) array,
                                          count,
                                          array->allocator));
}


// reserve, returns storage for multiple items, but it will be already counted
// as used. copy data in and your good
MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_advance( struct mulle_structarray *array,
                               unsigned int count)
{
   return( _mulle__structarray_advance( (struct mulle__structarray *) array,
                                        count,
                                        array->allocator));
}


// _mulle_structarray_reserve is like _mulle_structarray_advance but just for
// one item
MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_reserve( struct mulle_structarray *array)
{
   return( _mulle__structarray_reserve( (struct mulle__structarray *) array,
                                         array->allocator));
}


static inline void *
   mulle_structarray_reserve( struct mulle_structarray *array)
{
   if( ! array)
      return( NULL);
   return( _mulle__structarray_reserve( (struct mulle__structarray *) array,
                                         array->allocator));
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator  *
   _mulle_structarray_get_allocator( struct mulle_structarray *array)
{
   return( array->allocator);
}


static inline struct mulle_allocator  *
   mulle_structarray_get_allocator( struct mulle_structarray *array)
{
   return( array ? array->allocator : NULL);
}


// use in conjunction with guarantee only
MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get_current( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_current( (struct mulle__structarray *) array));
}


static inline void *
   mulle_structarray_get_current( struct mulle_structarray *array)
{
   return( array ? _mulle__structarray_get_current( (struct mulle__structarray *) array) : NULL);
}



MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get_first( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_first( (struct mulle__structarray *) array));
}


static inline void *
   mulle_structarray_get_first( struct mulle_structarray *array)
{
   return( array  ? _mulle__structarray_get_first( (struct mulle__structarray *) array) : NULL);
}



// use get to set as well
MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get( struct mulle_structarray *array, unsigned int i)
{
   return( _mulle__structarray_get( (struct mulle__structarray *) array, i));
}


static inline void *
   mulle_structarray_get( struct mulle_structarray *array, unsigned int i)
{
   if( array)
      return( _mulle_structarray_get( array, i));
   return( NULL);
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle_structarray_get_last( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_last( (struct mulle__structarray *) array));
}


static inline void *
   mulle_structarray_get_last( struct mulle_structarray *array)
{
   if( array)
      return( _mulle_structarray_get_last( array));
    return( NULL);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_remove_last( struct mulle_structarray *array)
{
   _mulle__structarray_remove_last( (struct mulle__structarray *) array);
}


static inline void
   mulle_structarray_remove_last( struct mulle_structarray *array)
{
   if( array)
      _mulle_structarray_remove_last( array);
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
   struct mulle_structarrayenumerator    rover;
   struct mulle__structarrayenumerator   tmp;

   tmp = _mulle__structarray_enumerate( (struct mulle__structarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__structarrayenumerator));
   return( rover);
}


static inline struct mulle_structarrayenumerator
   mulle_structarray_enumerate( struct mulle_structarray *array)
{
   struct mulle_structarrayenumerator    rover;
   struct mulle__structarrayenumerator   tmp;

   tmp = mulle__structarray_enumerate( (struct mulle__structarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__structarrayenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_structarrayenumerator_next( struct mulle_structarrayenumerator *rover,
                                      void **item)
{
   return( _mulle__structarrayenumerator_next( (struct mulle__structarrayenumerator *) rover,
                                               item));
}


static inline int
   mulle_structarrayenumerator_next( struct mulle_structarrayenumerator *rover,
                                     void **item)
{
   return( mulle__structarrayenumerator_next( (struct mulle__structarrayenumerator *) rover,
                                               item));
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
   struct mulle_structarrayreverseenumerator    rover;
   struct mulle__structarrayreverseenumerator   tmp;

   tmp = _mulle__structarray_reverseenumerate( (struct mulle__structarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__structarrayreverseenumerator));
   return( rover);
}


static inline struct mulle_structarrayreverseenumerator
   mulle_structarray_reverseenumerate( struct mulle_structarray *array)
{
   struct mulle_structarrayreverseenumerator    rover;
   struct mulle__structarrayreverseenumerator   tmp;

   tmp = mulle__structarray_reverseenumerate( (struct mulle__structarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__structarrayreverseenumerator));
   return( rover);
}

MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle_structarrayreverseenumerator_next( struct mulle_structarrayreverseenumerator *rover,
                                             void **item)
{
   return( _mulle__structarrayreverseenumerator_next( (struct mulle__structarrayreverseenumerator *) rover,
                                                      item));
}


static inline int
   mulle_structarrayreverseenumerator_next( struct mulle_structarrayreverseenumerator *rover,
                                             void **item)
{
   return( mulle__structarrayreverseenumerator_next( (struct mulle__structarrayreverseenumerator *) rover,
                                                     item));
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


#define mulle_structarray_for( array, item)                                                       \
   for( struct mulle_structarrayenumerator rover__ ## item = mulle_structarray_enumerate( array); \
        _mulle_structarrayenumerator_next( &rover__ ## item, (void **) &item);)

#define mulle_structarray_for_reverse( array, item)                                                              \
   for( struct mulle_structarrayreverseenumerator rover__ ## item = mulle_structarray_reverseenumerate( array); \
        _mulle_structarrayreverseenumerator_next( &rover__ ## item, (void **) &item);)


#endif

