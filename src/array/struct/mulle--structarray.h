#ifndef mulle__structarray_h__
#define mulle__structarray_h__

#include "include.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


//
// This is a growing array of struct sized structs.
// It has been coded for a fast "reserve" operation.
//
#define MULLE__STRUCTARRAY_BASE \
   void     *_storage;          \
   void     *_curr;             \
   void     *_sentinel;         \
   void     *_initial_storage;  \
   size_t   _sizeof_struct


struct mulle__structarray
{
   MULLE__STRUCTARRAY_BASE;
   struct mulle_allocator   *allocator;
};


#define MULLE__STRUCTARRAY_INIT( storage, type, count)              \
   ((struct mulle__structarray)                                     \
   {                                                                \
      storage,                                                      \
      storage,                                                      \
      &((char *) storage)[ count],                                  \
      storage,                                                      \
      (size_t) (sizeof( type) + (sizeof( type) % alignof( type)))   \
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
   MULLE_CONTAINER_GLOBAL
   void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                      unsigned int new_size,
                                      struct mulle_allocator *allocator);

   array->_storage         = NULL;
   array->_curr            = NULL;
   array->_sentinel        = NULL;
   array->_initial_storage = NULL;
   array->_sizeof_struct   = (size_t) (sizeof_struct + (sizeof_struct % alignof_struct));

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
   array->_storage         = storage;
   array->_curr            = storage;
   array->_sentinel        = &((char *) array->_storage)[ count];
   array->_initial_storage = storage;
   array->_sizeof_struct   = (size_t) (sizeof_struct + (sizeof_struct % alignof_struct));

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
   return(  &((char *) array->_curr)[ - array->_sizeof_struct]);
}



# pragma mark - array operations

MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__structarray_add( struct mulle__structarray *array,
                            void *item,
                            struct mulle_allocator *allocator)
{
   MULLE_CONTAINER_GLOBAL
   void   _mulle__structarray_grow( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator);

   assert( array->_sizeof_struct);

   if( array->_curr == array->_sentinel)
      _mulle__structarray_grow( array, allocator);

   memcpy( array->_curr, item, array->_sizeof_struct);
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];
}


MULLE_CONTAINER_GLOBAL
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
   MULLE_CONTAINER_GLOBAL
   void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                      unsigned int new_size,
                                      struct mulle_allocator *allocator);

   _mulle__structarray_sizeto( array, _mulle__structarray_get_used( array), allocator);
}



MULLE_C_NONNULL_FIRST
static inline struct mulle_data
   _mulle__structarray_extract_data( struct mulle__structarray *buffer,
                                     struct mulle_allocator *allocator)
{
   struct mulle_data   data;

   data.bytes  = buffer->_storage;
   data.length = _mulle__structarray_get_used_as_length( buffer);

   buffer->_storage          =
   buffer->_curr             =
   buffer->_sentinel         = NULL;

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


#define mulle__structarray_for( array, item)                                                       \
   for( struct mulle__structarrayenumerator rover__ ## item = mulle__structarray_enumerate( array); \
        _mulle__structarrayenumerator_next( &rover__ ## item, (void **) &item);)

#define mulle__structarray_for_reverse( array, item)                                                              \
   for( struct mulle_structarrayreverseenumerator rover__ ## item = mulle__structarray_reverseenumerate( array); \
        _mulle__structarrayreverseenumerator_next( &rover__ ## item, (void **) &item);)

#endif

