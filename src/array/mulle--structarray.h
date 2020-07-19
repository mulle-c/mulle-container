#ifndef mulle__structarray_h__
#define mulle__structarray_h__

#include <mulle-allocator/mulle-allocator.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


#pragma mark - item



#pragma mark - item array

//
// This is a growing array of struct sized structs.
// It has been coded for a fast "reserve" operation.
//
#define MULLE__STRUCTARRAY_BASE              \
   void                     *storage;       \
   void                     *curr;          \
   void                     *sentinel;      \
   int                      sizeof_struct

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


static inline void   _mulle__structarray_init( struct mulle__structarray *array,
                                               size_t sizeof_struct,
                                               unsigned int alignof_struct,
                                               size_t capacity,
                                               struct mulle_allocator *allocator)
{
   void   _mulle__structarray_sizeto( struct mulle__structarray *array, size_t new_size, struct mulle_allocator *allocator);
   intptr_t   misalignment;

   array->storage       = NULL;
   array->curr          = NULL;
   array->sentinel      = NULL;
   array->sizeof_struct = (int) (sizeof_struct + (sizeof_struct % alignof_struct));

   assert( array->sizeof_struct);

   if( capacity)
      _mulle__structarray_sizeto( array, capacity * array->sizeof_struct, allocator);
}


static inline struct mulle__structarray *
   mulle__structarray_create( size_t sizeof_struct,
                              unsigned int alignof_struct,
                              size_t capacity,
                              struct mulle_allocator *allocator)
{
   struct mulle__structarray  *array;

   array = mulle__structarray_alloc( allocator);
   _mulle__structarray_init( array, sizeof_struct, alignof_struct, capacity, allocator);
   return( array);
}


static inline void  _mulle__structarray_done( struct mulle__structarray *array,
                                              struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, array->storage);
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


static inline void   _mulle__structarray_reset( struct mulle__structarray *array)
{
   array->curr = array->storage;
}


# pragma mark - petty accessors

static inline size_t
   _mulle__structarray_get_count( struct mulle__structarray *array)
{
   return( (size_t) (((char *) array->curr - (char *) array->storage) / array->sizeof_struct));
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
static inline size_t
   _mulle__structarray_get_usedsize( struct mulle__structarray *array)
{
   return( (size_t) ((char *) array->curr - (char *) array->storage));
}

# pragma mark - array operations

static inline void
   _mulle__structarray_add( struct mulle__structarray *array,
                            void *item,
                            struct mulle_allocator *allocator)
{
   void   _mulle__structarray_grow( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator);

   if( array->curr == array->sentinel)
      _mulle__structarray_grow( array, allocator);

   memcpy( array->curr, item, array->sizeof_struct);
   array->curr = &((char *) array->curr)[ array->sizeof_struct];
}


static inline void *
   _mulle__structarray_reserve( struct mulle__structarray *array,
                                struct mulle_allocator *allocator)
{
   void   _mulle__structarray_grow( struct mulle__structarray *array,
                                    struct mulle_allocator *allocator);
   void   *reserved;

   if( array->curr == array->sentinel)
      _mulle__structarray_grow( array, allocator);

   reserved     = array->curr;
   array->curr = &((char *) array->curr)[ array->sizeof_struct];

   return( reserved);
}


static inline void *
   _mulle__structarray_get_first( struct mulle__structarray *array)
{
   return( array->curr > array->storage ? array->storage : NULL);
}



static inline void *
   _mulle__structarray_get( struct mulle__structarray *array, size_t i)
{
   char   *address;

   assert( array);

   address = &((char *) array->storage)[ array->sizeof_struct * i];
   assert( address >= (char *) array->storage);
   assert( address < (char *) array->sentinel);

   return( address);
}


static inline void *
   _mulle__structarray_get_last( struct mulle__structarray *array)
{
   assert( &((char *) array->curr)[ - array->sizeof_struct] >= (char *) array->storage);

   return(  &((char *) array->curr)[ - array->sizeof_struct]);
}


static inline void
   _mulle__structarray_remove_last( struct mulle__structarray *array)
{
   if( array->curr > array->storage)
      array->curr = _mulle__structarray_get_last( array);
}


static inline void *
   mulle__structarray_get_last( struct mulle__structarray *array)
{
   if( ! array || array->curr == array->storage)
      return( NULL);
   return(  &((char *) array->curr)[ - array->sizeof_struct]);
}


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
   void   *curr;                          \
   void   *sentinel;                      \
   int    sizeof_struct


struct mulle__structarrayenumerator
{
   MULLE__STRUCTARRAYENUMERATOR_BASE;
};


static inline struct mulle__structarrayenumerator
   mulle__structarray_enumerate( struct mulle__structarray *array)
{
   struct mulle__structarrayenumerator   rover;

   if( array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   rover.curr          = array->storage;
   rover.sentinel      = array->curr;
   rover.sizeof_struct = array->sizeof_struct;

   assert( (char *) rover.sentinel >= (char *) rover.curr);

   return( rover);
}


static inline void *
   _mulle__structarrayenumerator_next( struct mulle__structarrayenumerator *rover)
{
   void   *address;

   if( rover->curr < rover->sentinel)
   {
      address      = rover->curr;
      rover->curr = &((char *) rover->curr)[ rover->sizeof_struct];

      return( address);
   }
   return( NULL);
}


static inline void
   mulle__structarrayenumerator_done( struct mulle__structarrayenumerator *rover)
{
}



#pragma mark - structarray enumerator

#define MULLE__STRUCTARRAYREVERSEENUMERATOR_BASE MULLE__STRUCTARRAYENUMERATOR_BASE

struct mulle__structarrayreverseenumerator
{
   MULLE__STRUCTARRAYREVERSEENUMERATOR_BASE;
};


static inline struct mulle__structarrayreverseenumerator
   mulle__structarray_reverseenumerate( struct mulle__structarray *array)
{
   struct mulle__structarrayreverseenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   rover.curr          = array->curr;
   rover.sentinel      = array->storage;
   rover.sizeof_struct = array->sizeof_struct;

   assert( (char *) rover.curr >= (char *) rover.sentinel);

   return( rover);
}


static inline void *
   _mulle__structarrayreverseenumerator_next( struct mulle__structarrayreverseenumerator *rover)
{
   void   *address;

   if( rover->curr > rover->sentinel)
   {
      rover->curr = &((char *) rover->curr)[ - rover->sizeof_struct];
      return( rover->curr);
   }
   return( NULL);
}


static inline void
   mulle__structarrayreverseenumerator_done( struct mulle__structarrayreverseenumerator *rover)
{
}

#endif

