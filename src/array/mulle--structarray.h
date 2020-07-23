#ifndef mulle__structarray_h__
#define mulle__structarray_h__

#include <mulle-allocator/mulle-allocator.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


//
// This is a growing array of struct sized structs.
// It has been coded for a fast "reserve" operation.
//
#define MULLE__STRUCTARRAY_BASE              \
   void                     *_storage;       \
   void                     *_curr;          \
   void                     *_sentinel;      \
   int                      _sizeof_struct

struct mulle__structarray
{
   MULLE__STRUCTARRAY_BASE;
   struct mulle_allocator   *allocator;
};


static inline struct mulle__structarray  *
   mulle__structarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle__structarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle__structarray));
   return( array);
}

MULLE_C_NONNULL_FIRST
static inline void   _mulle__structarray_init( struct mulle__structarray *array,
                                               size_t _sizeof_struct,
                                               unsigned int alignof_struct,
                                               size_t capacity,
                                               struct mulle_allocator *allocator)
{
   void   _mulle__structarray_sizeto( struct mulle__structarray *array, size_t new_size, struct mulle_allocator *allocator);
   intptr_t   misalignment;

   array->_storage       = NULL;
   array->_curr          = NULL;
   array->_sentinel      = NULL;
   array->_sizeof_struct = (int) (_sizeof_struct + (_sizeof_struct % alignof_struct));

   assert( array->_sizeof_struct);

   if( capacity)
      _mulle__structarray_sizeto( array, capacity * array->_sizeof_struct, allocator);
}


static inline struct mulle__structarray *
   mulle__structarray_create( size_t _sizeof_struct,
                              unsigned int alignof_struct,
                              size_t capacity,
                              struct mulle_allocator *allocator)
{
   struct mulle__structarray  *array;

   array = mulle__structarray_alloc( allocator);
   _mulle__structarray_init( array, _sizeof_struct, alignof_struct, capacity, allocator);
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle__structarray_done( struct mulle__structarray *array,
                                              struct mulle_allocator *allocator)
{
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

MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structarray_get_count( struct mulle__structarray *array)
{
   return( (size_t) (((char *) array->_curr - (char *) array->_storage) / array->_sizeof_struct));
}


static inline size_t
   mulle__structarray_get_count( struct mulle__structarray *array)
{
   if( ! array)
      return( 0);
   return( _mulle__structarray_get_count( array));
}


static inline size_t
   _mulle__structarray_get_used( struct mulle__structarray *array)
{
   return (_mulle__structarray_get_count( array));
}


// cheaper as we don't divide here
MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__structarray_get_usedsize( struct mulle__structarray *array)
{
   return( (size_t) ((char *) array->_curr - (char *) array->_storage));
}

# pragma mark - array operations

MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__structarray_add( struct mulle__structarray *array,
                            void *item,
                            struct mulle_allocator *allocator)
{
   void   _mulle__structarray_grow( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator);

   if( array->_curr == array->_sentinel)
      _mulle__structarray_grow( array, allocator);

   memcpy( array->_curr, item, array->_sizeof_struct);
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_reserve( struct mulle__structarray *array,
                                struct mulle_allocator *allocator)
{
   void   _mulle__structarray_grow( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator);
   void   *reserved;

   if( array->_curr == array->_sentinel)
      _mulle__structarray_grow( array, allocator);

   reserved     = array->_curr;
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];

   return( reserved);
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_get_first( struct mulle__structarray *array)
{
   return( array->_curr > array->_storage ? array->_storage : NULL);
}



MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__structarray_get( struct mulle__structarray *array, size_t i)
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
   if( ! array || array->_curr == array->_storage)
      return( NULL);
   return(  &((char *) array->_curr)[ - array->_sizeof_struct]);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__structarray_size_to_fit( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator)
{
   void   _mulle__structarray_sizeto( struct mulle__structarray *array,
                                      size_t new_size,
                                      struct mulle_allocator *allocator);

   _mulle__structarray_sizeto( array, _mulle__structarray_get_usedsize( array), allocator);
}


#pragma mark - structarray enumerator

#define MULLE__STRUCTARRAYENUMERATOR_BASE \
   void   *_curr;                          \
   void   *_sentinel;                      \
   int    _sizeof_struct


struct mulle__structarrayenumerator
{
   MULLE__STRUCTARRAYENUMERATOR_BASE;
};

extern struct mulle__structarrayenumerator   mulle__structarrayenumerator_empty;



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
   void   *address;

   if( rover->_curr < rover->_sentinel)
   {
      *item        = rover->_curr;
      rover->_curr = &((char *) rover->_curr)[ rover->_sizeof_struct];
      return( 1);
   }
   return( 0);
}

static inline int
   mulle__structarrayenumerator_next( struct mulle__structarrayenumerator *rover,
                                      void **item)
{
   void   *address;

   if( ! rover)
      return( 0);
   if( rover->_curr >= rover->_sentinel)
      return( 0);
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

extern struct mulle__structarrayreverseenumerator   mulle__structarrayreverseenumerator_empty;

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
   return( 0);
}

static inline int
   mulle__structarrayreverseenumerator_next( struct mulle__structarrayreverseenumerator *rover,
                                              void **item)
{
   if( ! rover)
      return( 0);
   if( rover->_curr > rover->_sentinel)
   {
      rover->_curr = &((char *) rover->_curr)[ - rover->_sizeof_struct];
      if( item)
         *item = rover->_curr;
      return( 1);
   }
   return( 0);
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

#endif

