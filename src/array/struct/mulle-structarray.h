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


#define MULLE_STRUCTARRAY_DATA( storage, type, count, xallocator)     \
   ((struct mulle_structarray)                                        \
   {                                                                  \
      ._storage            = (storage),                               \
      ._curr               = (storage),                               \
      ._sentinel           = &((char *) (storage))[ (count) * MULLE__STRUCTARRAY_ALIGNED_SIZE( type)], \
      ._initial_storage    = (storage),                               \
      ._sizeof_struct      = MULLE__STRUCTARRAY_ALIGNED_SIZE( type),  \
      ._copy_sizeof_struct = sizeof( type),                           \
      .allocator           = (xallocator)                             \
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
                                              size_t alignof_struct,
                                              size_t capacity,
                                              struct mulle_allocator *allocator)
{
   _mulle__structarray_init( (struct mulle__structarray *) array,
                             _sizeof_struct,
                             alignof_struct,
                             capacity,
                             allocator);

   array->allocator = allocator;
}

/* TODO: simplify to..
 * #define mulle_structarray_init( array, type, capacity, allocator) \
 * do                                                                \
 * {                                                                 \
 *    if( array)                                                     \
 *       _mulle_structarray_init( array,                             \
 *                                sizeof( type),                     \
 *                                alignof( type),                    \
 *                                capacity,                          \
 *                                allocator);                        \
 * }                                                                 \
 * while( 0)
 */
static inline void   mulle_structarray_init( struct mulle_structarray *array,
                                             size_t sizeof_struct,
                                             size_t alignof_struct,
                                             size_t capacity,
                                             struct mulle_allocator *allocator)
{
   if( array)
      _mulle_structarray_init( array,
                               sizeof_struct,
                               alignof_struct,
                               capacity,
                               allocator);
}

#define mulle_structarray_init_default( type)   \
   mulle_structarray_init( array, sizeof( type), alignof( type), 8, NULL)


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_init_with_static_storage( struct mulle_structarray *array,
                                                size_t sizeof_struct,
                                                size_t alignof_struct,
                                                size_t count,
                                                void  *storage,
                                                struct mulle_allocator *allocator)
{
   _mulle__structarray_init_with_static_storage( (struct mulle__structarray *) array,
                                                  sizeof_struct,
                                                  alignof_struct,
                                                  count,
                                                  storage);

   array->allocator = allocator;
}


static inline void
   mulle_structarray_init_with_static_storage( struct mulle_structarray *array,
                                               size_t sizeof_struct,
                                               size_t alignof_struct,
                                               size_t count,
                                               void  *storage,
                                               struct mulle_allocator *allocator)
{
   if(  array)
   {
      _mulle__structarray_init_with_static_storage( (struct mulle__structarray *) array,
                                                    sizeof_struct,
                                                    alignof_struct,
                                                    count,
                                                    storage);
      array->allocator = allocator;
   }
}



static inline struct mulle_structarray *
   mulle_structarray_create( size_t sizeof_struct,
                             size_t alignof_struct,
                             size_t capacity,
                             struct mulle_allocator *allocator)
{
   struct mulle_structarray  *array;

   array = mulle_structarray_alloc( allocator);
   _mulle_structarray_init( array, sizeof_struct, alignof_struct, capacity, allocator);
   return( array);
}


#define mulle_structarray_create_default( type)   \
   mulle_structarray_create( sizeof( type), alignof( type), 8, NULL)



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


MULLE_C_NONNULL_FIRST
MULLE__CONTAINER_GLOBAL
void  _mulle_structarray_destroy( struct mulle_structarray *array);


static inline void  mulle_structarray_destroy( struct mulle_structarray *array)
{
   if( array)
      _mulle_structarray_destroy( array);
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
static inline void **
   _mulle_structarray_get_storage( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_storage( (struct mulle__structarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_structarray_get_count( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_count( (struct mulle__structarray *) array));
}


static inline size_t
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
static inline size_t
   _mulle_structarray_get_size( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_size( (struct mulle__structarray *) array));
}


static inline size_t
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



MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle_structarray_assert_pointer( struct mulle_structarray *array, void *p)
{
   _mulle__structarray_assert_pointer( (struct mulle__structarray *) array, p);
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


// MEMO: alternative name, maybe it will become the standard
MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_structarray_get_struct_size( struct mulle_structarray *array)
{
   return( _mulle__structarray_get_element_size( (struct mulle__structarray *) array));
}


static inline size_t
   mulle_structarray_get_struct_size( struct mulle_structarray *array)
{
   if( ! array)
      return( 0);

   return( _mulle__structarray_get_element_size( (struct mulle__structarray *) array));
}



# pragma mark - array operations

MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle_structarray_add( struct mulle_structarray *array, void *item)
{
   _mulle__structarray_add( (struct mulle__structarray *) array,
                             item,
                             array->allocator);
}


static inline int
   mulle_structarray_pop( struct mulle_structarray *array, void *item)
{
   if( ! array)
      return( 0);

   return( _mulle__structarray_pop( (struct mulle__structarray *) array,
                                    item,
                                    array->allocator));
}


static inline void
   mulle_structarray_add( struct mulle_structarray *array, void *item)
{
   if( array && item)
      _mulle_structarray_add( array, item);
}



MULLE_C_NONNULL_FIRST_THIRD
static inline void
   _mulle_structarray_set( struct mulle_structarray *array, size_t i, void *item)
{
   _mulle__structarray_set( (struct mulle__structarray *) array, i, item);
}


static inline void
   mulle_structarray_set( struct mulle_structarray *array, size_t i, void *item)
{
   if( array && item)
      _mulle_structarray_set( array, i, item);
}




// guarantee ensures that you can add up to length items without a realloc
MULLE_C_NONNULL_FIRST
static inline void   *_mulle_structarray_guarantee( struct mulle_structarray *array,
                                                    size_t count)
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
                               size_t count)
{
   return( _mulle__structarray_advance( (struct mulle__structarray *) array,
                                        count,
                                        array->allocator));
}


static inline void
   mulle_structarray_add_structarray( struct mulle_structarray *array,
                                      struct mulle_structarray *other,
                                      struct mulle_range range)
{
   if( array)
      mulle__structarray_add_structarray( (struct mulle__structarray *) array,
                                          (struct mulle__structarray *) other,
                                          range,
                                          array->allocator);
}


static inline struct mulle_structarray *
   mulle_structarray_copy( struct mulle_structarray *other,
                           struct mulle_allocator *allocator)
{
   struct mulle_structarray  *array;

   if( ! other)
      return( NULL);

   array = mulle_structarray_alloc( allocator);
   memset( array, 0, sizeof( *array));
   array->_sizeof_struct      = other->_sizeof_struct;
   array->_copy_sizeof_struct = other->_copy_sizeof_struct;
   array->allocator           = allocator;
   mulle_structarray_add_structarray( array, other, mulle_range_make_all());
   return( array);
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
static inline void   _mulle_structarray_set_count( struct mulle_structarray *array,
                                                   size_t count)
{
   _mulle__structarray_set_count( (struct mulle__structarray *) array,
                                  count,
                                  array->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_structarray_zero_to_count( struct mulle_structarray *array,
                                                       size_t count)
{
   _mulle__structarray_zero_to_count( (struct mulle__structarray *) array,
                                      count,
                                      array->allocator);
}


#pragma mark - sorting


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_qsort_r_inline( struct mulle_structarray *array,
                                      mulle_structarray_cmp_t *compare,
                                      void *userinfo)
{
   _mulle__structarray_qsort_r_inline( (struct mulle__structarray *) array,
                                       compare,
                                       userinfo);
}


static inline void
   mulle_structarray_qsort_r_inline( struct mulle_structarray *array,
                                     mulle_structarray_cmp_t *compare,
                                     void *userinfo)
{
   if( array)
      _mulle_structarray_qsort_r_inline( array, compare, userinfo);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_structarray_qsort_r( struct mulle_structarray *array,
                               mulle_structarray_cmp_t *compare,
                               void *userinfo)
{
   _mulle__structarray_qsort_r( (struct mulle__structarray *) array, compare, userinfo);
}


static inline void
   mulle_structarray_qsort_r( struct mulle_structarray *array,
                              mulle_structarray_cmp_t *compare,
                              void *userinfo)
{
   if( array)
      _mulle_structarray_qsort_r( array, compare, userinfo);
}


#pragma mark - petty accessors

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
   _mulle_structarray_get( struct mulle_structarray *array, size_t i)
{
   return( _mulle__structarray_get( (struct mulle__structarray *) array, i));
}


static inline void *
   mulle_structarray_get( struct mulle_structarray *array, size_t i)
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
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle_structarrayenumerator_done( struct mulle_structarrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
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
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle_structarrayreverseenumerator_done( struct mulle_structarrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


//
// we have to keep storage out of the for loop
//
#define mulle_structarray_do( name, type)                                   \
   for( struct mulle_structarray                                            \
           name ## __array =  MULLE_STRUCTARRAY_DATA( NULL, type, 0, NULL), \
           *name = &name ## __array,                                        \
           *name ## __i = NULL;                                             \
        ! name ## __i;                                                      \
        name ## __i =                                                       \
        (                                                                   \
           _mulle_structarray_done( &name ## __array),                      \
           (void *) 0x1                                                     \
        )                                                                   \
      )                                                                     \
                                                                            \
      for( int  name ## __j = 0;    /* break protection */                  \
           name ## __j < 1;                                                 \
           name ## __j++)


#define mulle_structarray_do_flexible( name, type, stackcount)                 \
   type   name ## __storage[ stackcount];                                      \
   for( struct mulle_structarray                                               \
           name ## __array =                                                   \
              MULLE_STRUCTARRAY_DATA( name ## __storage, type, stackcount, 0), \
           *name = &name ## __array,                                           \
           *name ## __i = NULL;                                                \
        ! name ## __i;                                                         \
        name ## __i =                                                          \
        (                                                                      \
           _mulle_structarray_done( &name ## __array),                         \
           (void *) 0x1                                                        \
        )                                                                      \
      )                                                                        \
                                                                               \
      for( int  name ## __j = 0;    /* break protection */                     \
           name ## __j < 1;                                                    \
           name ## __j++)


// created by make-container-for.sh src/array/struct/mulle-structarray.c

#define mulle_structarray_for( name, item)                                              \
   assert( sizeof( item) == sizeof( void *));                                           \
   for( struct mulle_structarrayenumerator                                              \
           rover__ ## item = mulle_structarray_enumerate( name),                        \
           *rover__  ## item ## __i = (void *) 0;                                       \
        ! rover__  ## item ## __i;                                                      \
        rover__ ## item ## __i = (_mulle_structarrayenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                         \
      while( _mulle_structarrayenumerator_next( &rover__ ## item, (void **) &item))


// created by make-container-for.sh --reverse src/array/struct/mulle-structarray.c

#define mulle_structarray_for_reverse( name, item)                                             \
   assert( sizeof( item) == sizeof( void *));                                                  \
   for( struct mulle_structarrayreverseenumerator                                              \
           rover__ ## item = mulle_structarray_reverseenumerate( name),                        \
           *rover__  ## item ## __i = (void *) 0;                                              \
        ! rover__  ## item ## __i;                                                             \
        rover__ ## item ## __i = (_mulle_structarrayreverseenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                                \
      while( _mulle_structarrayreverseenumerator_next( &rover__ ## item, (void **) &item))

#endif

