#ifndef mulle__structarray_h__
#define mulle__structarray_h__

#include "include.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "mulle-qsort.h"

//
// This is a growing array of struct sized structs.
// It has been coded for a fast "reserve" operation.
// the "internal" sizeof struct needs to be properly aligned, therefore this
// maybe larger than copy. But on "ingress" we can only copy the original
// sizeof, which could be misaligned ? Not 100% sure if this isn't just
// paranoia though, but the point of MULLE__STRUCTARRAY_ALIGNED_SIZE is to
// widen the internal size, if that is superflous then _copy_sizeof_struct
// should be too.
//
#define MULLE__STRUCTARRAY_BASE  \
   void     *_storage;           \
   void     *_curr;              \
   void     *_sentinel;          \
   void     *_initial_storage;   \
   size_t   _sizeof_struct;      \
   size_t   _copy_sizeof_struct


struct mulle__structarray
{
   MULLE__STRUCTARRAY_BASE;
};


#define MULLE__STRUCTARRAY_ALIGNED_SIZE( type)  \
   (size_t) (sizeof( type) + (sizeof( type) % alignof( type)))


#define MULLE__STRUCTARRAY_INIT( storage, type, count)                        \
   ((struct mulle__structarray)                                               \
   {                                                                          \
      ._storage                 = (storage),                                  \
      ._curr                    = (storage),                                  \
      ._sentinel                = &((char *) (storage))[ (count) * MULLE__STRUCTARRAY_ALIGNED_SIZE( type)], \
      ._initial_storage         = (storage),                                  \
      ._sizeof_struct           = MULLE__STRUCTARRAY_ALIGNED_SIZE( type),     \
      ._copy_sizeof_struct      = sizeof( type)                               \
   })


static inline struct mulle__structarray  *
   mulle__structarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle__structarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle__structarray));
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__structarray_init( struct mulle__structarray *array,
                                               size_t sizeof_struct,
                                               unsigned int alignof_struct,
                                               unsigned int capacity,
                                               struct mulle_allocator *allocator)
{
   MULLE__CONTAINER_GLOBAL
   void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                      unsigned int new_size,
                                      struct mulle_allocator *allocator);

   array->_storage            = NULL;
   array->_curr               = NULL;
   array->_sentinel           = NULL;
   array->_initial_storage    = NULL;
   array->_sizeof_struct      = (size_t) (sizeof_struct + (sizeof_struct % alignof_struct));
   array->_copy_sizeof_struct = sizeof_struct;
   assert( array->_sizeof_struct);

   if( capacity)
      _mulle__structarray_sizeto( array, capacity, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarray_init_with_static_storage( struct mulle__structarray *array,
                                                 size_t sizeof_struct,
                                                 unsigned int alignof_struct,
                                                 unsigned int count,
                                                 void  *storage,
                                                 struct mulle_allocator *allocator)
{
   array->_storage            = storage;
   array->_curr               = storage;
   array->_sentinel           = &((char *) array->_storage)[ count];
   array->_initial_storage    = storage;
   array->_sizeof_struct      = (size_t) (sizeof_struct + (sizeof_struct % alignof_struct));
   array->_copy_sizeof_struct = sizeof_struct;

   assert( array->_sizeof_struct);
}



static inline struct mulle__structarray *
   mulle__structarray_create( size_t sizeof_struct,
                              unsigned int alignof_struct,
                              unsigned int capacity,
                              struct mulle_allocator *allocator)
{
   struct mulle__structarray  *array;

   array = mulle__structarray_alloc( allocator);
   _mulle__structarray_init( array, sizeof_struct, alignof_struct, capacity, allocator);
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle__structarray_done( struct mulle__structarray *array,
                                              struct mulle_allocator *allocator)
{
   if( ! array->_initial_storage)
      mulle_allocator_free( allocator, array->_storage);
#ifdef DEBUG
   memset( array, 0xFD, sizeof( struct mulle__structarray));
#endif
}



static inline void  mulle__structarray_destroy( struct mulle__structarray *array,
                                                struct mulle_allocator *allocator)
{
   if( array)
   {
      _mulle__structarray_done( array, allocator);
      mulle_allocator_free( allocator, array);
   }
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__structarray_reset( struct mulle__structarray *array)
{
   array->_curr = array->_storage;
}


# pragma mark - petty accessors


MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__structarray_get_storage( struct mulle__structarray *array)
{
   return( array->_storage);
}


// if you crash here, you forgot to initialize the array
MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__structarray_get_count( struct mulle__structarray *array)
{
   return( (unsigned int) (((char *) array->_curr - (char *) array->_storage) / array->_sizeof_struct));
}


static inline unsigned int
   mulle__structarray_get_count( struct mulle__structarray *array)
{
   if( ! array)
      return( 0);
   return( _mulle__structarray_get_count( array));
}


static inline unsigned int
   _mulle__structarray_get_used( struct mulle__structarray *array)
{
   return (_mulle__structarray_get_count( array));
}


// cheaper as we don't divide here
MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structarray_get_used_as_length( struct mulle__structarray *array)
{
   return( (size_t) ((char *) array->_curr - (char *) array->_storage));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structarray_get_element_size( struct mulle__structarray *array)
{
   return( (unsigned int) array->_sizeof_struct);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__structarray_get_size( struct mulle__structarray *array)
{
   return( (unsigned int) (((char *) array->_sentinel - (char *) array->_storage) / array->_sizeof_struct));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structarray_get_size_as_length( struct mulle__structarray *array)
{
   return( (size_t) ((char *) array->_sentinel - (char *) array->_storage));
}



MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_get_first( struct mulle__structarray *array)
{
   return( array->_curr > array->_storage ? array->_storage : NULL);
}


// use in conjunction with guarantee only
MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_get_current( struct mulle__structarray *array)
{
   return( array->_curr);
}



MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_get( struct mulle__structarray *array, unsigned int i)
{
   char   *address;

   assert( array);

   address = &((char *) array->_storage)[ array->_sizeof_struct * i];

   assert( address >= (char *) array->_storage);
   assert( address < (char *) array->_sentinel);

   return( address);
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_get_last( struct mulle__structarray *array)
{
   if( array->_curr == array->_storage)
      return( NULL);

   assert( &((char *) array->_curr)[ - array->_sizeof_struct] >= (char *) array->_storage);
   return(  &((char *) array->_curr)[ - array->_sizeof_struct]);
}


static inline void
   _mulle__structarray_remove_last( struct mulle__structarray *array)
{
   if( array->_curr > array->_storage)
      array->_curr = _mulle__structarray_get_last( array);
}


static inline void *
   mulle__structarray_get_last( struct mulle__structarray *array)
{
   if( ! array)
      return( NULL);
   return( _mulle__structarray_get_last( array));
}



# pragma mark - array operations

MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__structarray_add( struct mulle__structarray *array,
                            void *item,
                            struct mulle_allocator *allocator)
{
   MULLE__CONTAINER_GLOBAL
   void   _mulle__structarray_grow( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator);

   assert( array->_sizeof_struct);

   if( array->_curr == array->_sentinel)
      _mulle__structarray_grow( array, allocator);

   memcpy( array->_curr, item, array->_copy_sizeof_struct);
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];
}


static inline void
   _mulle__structarray_set( struct mulle__structarray *array,
                            unsigned int i,
                            void *item)
{
   void  *address;

   address = _mulle__structarray_get( array, i);
   memcpy( address, item, array->_copy_sizeof_struct);
}



MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__structarray_add_guaranteed( struct mulle__structarray *array,
                                       void *item)
{
   assert( array->_sizeof_struct);
   assert( array->_curr < array->_sentinel);

   memcpy( array->_curr, item, array->_copy_sizeof_struct);
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   *_mulle__structarray_guarantee( struct mulle__structarray *array,
                                       unsigned int count,
                                       struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_advance( struct mulle__structarray *array,
                                unsigned int count,
                                struct mulle_allocator *allocator)
{
   void   *reserved;

   reserved = _mulle__structarray_guarantee( array, count, allocator);
   if( reserved)
      array->_curr = &((char *) array->_curr)[ array->_sizeof_struct * count];
   return( reserved);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__structarray_assert_pointer( struct mulle__structarray *array,
                                       void *pointer)
{
   assert( (char *) pointer >= (char *) array->_storage &&
           (char *) pointer < (char *) array->_curr);
   assert( ((char *) pointer - (char *) array->_storage) % array->_sizeof_struct == 0);
}



void
   mulle__structarray_add_array( struct mulle__structarray *array,
                                 struct mulle__structarray *other,
                                 struct mulle_range range,
                                 struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_reserve( struct mulle__structarray *array,
                                struct mulle_allocator *allocator)
{
   void   *reserved;

   reserved = _mulle__structarray_guarantee( array, 1, allocator);
   if( reserved)
      array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];
   return( reserved);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarray_size_to_fit( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator)
{
   MULLE__CONTAINER_GLOBAL
   void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                      unsigned int new_size,
                                      struct mulle_allocator *allocator);

   _mulle__structarray_sizeto( array, _mulle__structarray_get_used( array), allocator);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
unsigned int   _mulle__structarray_set_count( struct mulle__structarray *array,
                                              unsigned int count,
                                              struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__structarray_zero_to_count( struct mulle__structarray *array,
                                          unsigned int count,
                                          struct mulle_allocator *allocator);


typedef int   mulle_structarray_cmp_t( void **, void **, void *);

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarray_qsort_r_inline( struct mulle__structarray *array,
                                       mulle_structarray_cmp_t *compare,
                                       void *userinfo)
{
   _mulle_qsort_r_inline( array->_storage,
                          _mulle__structarray_get_count( array),
                          array->_sizeof_struct,
                          (mulle_qsort_cmp_t *) compare,
                          userinfo);
}


static inline void
   mulle__structarray_qsort_r_inline( struct mulle__structarray *array,
                                      mulle_structarray_cmp_t *compare,
                                      void *userinfo)
{
   if( array)
      _mulle__structarray_qsort_r_inline( array, compare, userinfo);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarray_qsort_r( struct mulle__structarray *array,
                                 mulle_structarray_cmp_t *compare,
                                 void *userinfo)
{
   mulle_qsort_r( array->_storage,
                  _mulle__structarray_get_count( array),
                  array->_sizeof_struct,
                  (mulle_qsort_cmp_t *) compare,
                  userinfo);
}


static inline void
   mulle__structarray_qsort_r( struct mulle__structarray *array,
                                       mulle_structarray_cmp_t *compare,
                                       void *userinfo)
{
   if( array)
      _mulle__structarray_qsort_r( array, compare, userinfo);
}


#pragma mark - extract


MULLE_C_NONNULL_FIRST
static inline struct mulle_data
   _mulle__structarray_extract_data( struct mulle__structarray *buffer,
                                     struct mulle_allocator *allocator)
{
   struct mulle_data   data;

   data = mulle_data_make( buffer->_storage,
                           _mulle__structarray_get_used_as_length( buffer));

   buffer->_storage  =
   buffer->_curr     =
   buffer->_sentinel = NULL;

   return( data);
}



#pragma mark - structarray enumerator

#define MULLE__STRUCTARRAYENUMERATOR_BASE    \
   void     *_curr;                          \
   void     *_sentinel;                      \
   size_t   _sizeof_struct


struct mulle__structarrayenumerator
{
   MULLE__STRUCTARRAYENUMERATOR_BASE;
};

#define mulle__structarrayenumerator_empty  \
   ((struct mulle__structarrayenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct mulle__structarrayenumerator
   _mulle__structarray_enumerate( struct mulle__structarray *array)
{
   struct mulle__structarrayenumerator   rover;

   rover._curr          = array->_storage;
   rover._sentinel      = array->_curr;
   rover._sizeof_struct = array->_sizeof_struct;

   assert( (char *) rover._sentinel >= (char *) rover._curr);

   return( rover);
}


static inline struct mulle__structarrayenumerator
   mulle__structarray_enumerate( struct mulle__structarray *array)
{
   if( ! array)
      return( mulle__structarrayenumerator_empty);
   return( _mulle__structarray_enumerate( array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__structarrayenumerator_next( struct mulle__structarrayenumerator *rover,
                                       void **item)
{
   if( rover->_curr < rover->_sentinel)
   {
      *item        = rover->_curr;
      rover->_curr = &((char *) rover->_curr)[ rover->_sizeof_struct];
      return( 1);
   }
   *item = NULL;
   return( 0);
}

static inline int
   mulle__structarrayenumerator_next( struct mulle__structarrayenumerator *rover,
                                      void **item)
{
   if( ! rover || rover->_curr >= rover->_sentinel)
   {
      if( item)
         *item = NULL;
      return( 0);
   }

   if( item)
      *item = rover->_curr;
   rover->_curr = &((char *) rover->_curr)[ rover->_sizeof_struct];
   return( 1);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarrayenumerator_done( struct mulle__structarrayenumerator *rover)
{
}


static inline void
   mulle__structarrayenumerator_done( struct mulle__structarrayenumerator *rover)
{
}


#pragma mark - reverse enumerator

#define MULLE__STRUCTARRAYREVERSEENUMERATOR_BASE MULLE__STRUCTARRAYENUMERATOR_BASE

struct mulle__structarrayreverseenumerator
{
   MULLE__STRUCTARRAYREVERSEENUMERATOR_BASE;
};


#define mulle__structarrayreverseenumerator_empty  \
   ((struct mulle__structarrayreverseenumerator) { 0 })


static inline struct mulle__structarrayreverseenumerator
   _mulle__structarray_reverseenumerate( struct mulle__structarray *array)
{
   struct mulle__structarrayreverseenumerator   rover;

   rover._curr          = array->_curr;
   rover._sentinel      = array->_storage;
   rover._sizeof_struct = array->_sizeof_struct;

   assert( (char *) rover._curr >= (char *) rover._sentinel);

   return( rover);
}


static inline struct mulle__structarrayreverseenumerator
   mulle__structarray_reverseenumerate( struct mulle__structarray *array)
{
   if( ! array)
      return( mulle__structarrayreverseenumerator_empty);
   return( _mulle__structarray_reverseenumerate( array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__structarrayreverseenumerator_next( struct mulle__structarrayreverseenumerator *rover,
                                              void **item)
{
   if( rover->_curr > rover->_sentinel)
   {
      rover->_curr = &((char *) rover->_curr)[ - rover->_sizeof_struct];
      *item = rover->_curr;
      return( 1);
   }

   *item = NULL;
   return( 0);
}


static inline int
   mulle__structarrayreverseenumerator_next( struct mulle__structarrayreverseenumerator *rover,
                                             void **item)
{
   if( ! rover || rover->_curr <= rover->_sentinel)
   {
      if( item)
         *item = NULL;      
      return( 0);
   }

   rover->_curr = &((char *) rover->_curr)[ - rover->_sizeof_struct];
   if( item)
      *item = rover->_curr;
   return( 1);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarrayreverseenumerator_done( struct mulle__structarrayreverseenumerator *rover)
{
}


static inline void
   mulle__structarrayreverseenumerator_done( struct mulle__structarrayreverseenumerator *rover)
{
}



//
// we have to keep storage out of the for loop
//
#define mulle__structarray_do( name, type)                                    \
   for( struct mulle__structarray                                             \
           name ## __array = MULLE__STRUCTARRAY_INIT( NULL, type, 0),         \
           *name = &name ## __array,                                          \
           *name ## __i = NULL;                                               \
        ! name ## __i;                                                        \
        name ## __i =                                                         \
        (                                                                     \
           _mulle__structarray_done( &name ## __array, NULL),                 \
           (void *) 0x1                                                       \
        )                                                                     \
      )                                                                       \
                                                                              \
      for( int  name ## __j = 0;    /* break protection */                    \
           name ## __j < 1;                                                   \
           name ## __j++)


#define mulle__structarray_do_flexible( name, type, stackcount)               \
   type   name ## __storage[ stackcount];                                     \
   for( struct mulle__structarray                                             \
           name ## __array =                                                  \
              MULLE__STRUCTARRAY_INIT( name ## __storage, type, stackcount),  \
           *name = &name ## __array,                                          \
           *name ## __i = NULL;                                               \
        ! name ## __i;                                                        \
        name ## __i =                                                         \
        (                                                                     \
           _mulle__structarray_done( &name ## __array, NULL),                 \
           (void *) 0x1                                                       \
        )                                                                     \
      )                                                                       \
                                                                              \
      for( int  name ## __j = 0;    /* break protection */                    \
           name ## __j < 1;                                                   \
           name ## __j++)



// created by make-container-for.sh src/array/struct/mulle--structarray.c

#define mulle__structarray_for( name, item)                                               \
   assert( sizeof( item) == sizeof( void *));                                             \
   for( struct mulle__structarrayenumerator                                               \
           rover__ ## item = mulle__structarray_enumerate( name),                         \
           *rover___  ## item ## __i = (void *) 0;                                        \
        ! rover___  ## item ## __i;                                                       \
        rover___ ## item ## __i = (_mulle__structarrayenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                           \
      while( _mulle__structarrayenumerator_next( &rover__ ## item, (void **) &item))


// created by make-container-for.sh --reverse src/array/struct/mulle--structarray.c

#define mulle__structarray_for_reverse( name, item)                                              \
   assert( sizeof( item) == sizeof( void *));                                                    \
   for( struct mulle__structarrayreverseenumerator                                               \
           rover__ ## item = mulle__structarray_reverseenumerate( name),                         \
           *rover___  ## item ## __i = (void *) 0;                                               \
        ! rover___  ## item ## __i;                                                              \
        rover___ ## item ## __i = (_mulle__structarrayreverseenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                                  \
      while( _mulle__structarrayreverseenumerator_next( &rover__ ## item, (void **) &item))


#endif

