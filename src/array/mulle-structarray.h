#ifndef mulle_structarray_h__
#define mulle_structarray_h__

#include <mulle-allocator/mulle-allocator.h>

#include <stdint.h>
#include <stddef.h>
#include <assert.h>


#pragma mark - item



#pragma mark - item array

//
// This is a growing array of struct sized structs.
// It has been coded for a fast "reserve" operation.
//
struct mulle_structarray
{
   void                     *_structs;
   void                     *_curr;
   void                     *_sentinel;
   int                       _sizeof_struct;
   struct mulle_allocator   *_allocator;
};


static inline struct mulle_structarray  *
   mulle_structarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_structarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle_structarray));
   return( array);
}


static inline void   _mulle_structarray_init( struct mulle_structarray *array,
                                              unsigned int sizeof_struct,
                                              unsigned int alignof_struct,
                                              unsigned int capacity,
                                              struct mulle_allocator *allocator)
{
   void   _mulle_structarray_sizeto( struct mulle_structarray *array, size_t new_size);
   intptr_t   misalignment;

   array->_structs       = NULL;
   array->_curr          = NULL;
   array->_sentinel      = NULL;
   array->_sizeof_struct = (int) (sizeof_struct + (sizeof_struct % alignof_struct));
   array->_allocator     = allocator;

   assert( array->_sizeof_struct);

   if( capacity)
      _mulle_structarray_sizeto( array, capacity * array->_sizeof_struct);
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


static inline void  _mulle_structarray_done( struct mulle_structarray *array)
{
   mulle_allocator_free( array->_allocator, array->_structs);
}



static inline void  mulle_structarray_destroy( struct mulle_structarray *array)
{
   if( array)
   {
      _mulle_structarray_done( array);
      mulle_allocator_free( array->_allocator, array);
   }
}


static inline void   _mulle_structarray_reset( struct mulle_structarray *array)
{
   array->_curr = array->_structs;
}


# pragma mark - petty accessors

static inline unsigned int
   _mulle_structarray_get_count( struct mulle_structarray *array)
{
   return( (unsigned int) (((char *) array->_curr - (char *) array->_structs) / array->_sizeof_struct));
}


static inline unsigned int
   mulle_structarray_get_count( struct mulle_structarray *array)
{
   if( ! array)
      return( 0);
   return( _mulle_structarray_get_count( array));
}


// cheaper as we don't divide here
static inline size_t
   _mulle_structarray_get_usedsize( struct mulle_structarray *array)
{
   return( (size_t) ((char *) array->_curr - (char *) array->_structs));
}


# pragma mark - array operations

static inline void
   _mulle_structarray_add( struct mulle_structarray *array,
                          void *item)
{
   void   _mulle_structarray_grow( struct mulle_structarray *array);

   if( array->_curr == array->_sentinel)
      _mulle_structarray_grow( array);

   memcpy( array->_curr, item, array->_sizeof_struct);
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];
}


static inline void *
   _mulle_structarray_reserve( struct mulle_structarray *array)
{
   void   _mulle_structarray_grow( struct mulle_structarray *array);
   void   *reserved;

   if( array->_curr == array->_sentinel)
      _mulle_structarray_grow( array);

   reserved     = array->_curr;
   array->_curr = &((char *) array->_curr)[ array->_sizeof_struct];

   return( reserved);
}


static inline struct mulle_allocator  *
   _mulle_structarray_get_allocator( struct mulle_structarray *array)
{
   return( array->_allocator);
}


static inline void *
   _mulle_structarray_get( struct mulle_structarray *array, unsigned int i)
{
   char   *address;

   assert( array);

   address = &((char *) array->_structs)[ array->_sizeof_struct * i];
   assert( address >= (char *) array->_structs);
   assert( address < (char *) array->_sentinel);

   return( address);
}


static inline void *
   _mulle_structarray_get_last( struct mulle_structarray *array)
{
   assert( &((char *) array->_curr)[ - array->_sizeof_struct] >= (char *) array->_structs);

   return(  &((char *) array->_curr)[ - array->_sizeof_struct]);
}


static inline void
   _mulle_structarray_remove_last( struct mulle_structarray *array)
{
   if( array->_curr > array->_structs)
      array->_curr = _mulle_structarray_get_last( array);
}


static inline void *
   mulle_structarray_get_last( struct mulle_structarray *array)
{
   if( ! array || array->_curr == array->_structs)
      return( NULL);
   return(  &((char *) array->_curr)[ - array->_sizeof_struct]);
}


static inline void
   _mulle_structarray_size_to_fit( struct mulle_structarray *array)
{
   void   _mulle_structarray_sizeto( struct mulle_structarray *array, size_t new_size);

   _mulle_structarray_sizeto( array, _mulle_structarray_get_usedsize( array));
}



#pragma mark - structarray enumerator

struct mulle_structarrayenumerator
{
   void   *_curr;
   void   *_sentinel;
   int    _sizeof_struct;
};


static inline struct mulle_structarrayenumerator
   mulle_structarray_enumerate( struct mulle_structarray *array)
{
   struct mulle_structarrayenumerator   rover;

   if( array)
   {
      rover._curr          = array->_structs;
      rover._sentinel      = array->_curr;
      rover._sizeof_struct = array->_sizeof_struct;
   }
   else
      memset( &rover, 0, sizeof( rover));

   assert( (char *) rover._sentinel >= (char *) rover._curr);

   return( rover);
}


static inline void *
   _mulle_structarrayenumerator_next( struct mulle_structarrayenumerator *rover)
{
   void   *address;

   if( rover->_curr < rover->_sentinel)
   {
      address      = rover->_curr;
      rover->_curr = &((char *) rover->_curr)[ rover->_sizeof_struct];

      return( address);
   }
   return( NULL);
}


static inline void
   mulle_structarrayenumerator_done( struct mulle_structarrayenumerator *rover)
{
}



#pragma mark - structarray enumerator

struct mulle_structarrayreverseenumerator
{
   void      *_curr;
   void      *_structs;
   int        _sizeof_struct;
};


static inline struct mulle_structarrayreverseenumerator
   mulle_structarray_reverseenumerate( struct mulle_structarray *array)
{
   struct mulle_structarrayreverseenumerator   rover;

   if( array)
   {
      rover._curr          = array->_curr;
      rover._structs       = array->_structs;
      rover._sizeof_struct = array->_sizeof_struct;
   }
   else
      memset( &rover, 0, sizeof( rover));

   assert( (char *) rover._curr >= (char *) rover._structs);

   return( rover);
}


static inline void *
   _mulle_structarrayreverseenumerator_next( struct mulle_structarrayreverseenumerator *rover)
{
   void   *address;

   if( rover->_curr > rover->_structs)
   {
      rover->_curr = &((char *) rover->_curr)[ - rover->_sizeof_struct];
      return( rover->_curr);
   }
   return( NULL);
}


static inline void
   mulle_structarrayreverseenumerator_done( struct mulle_structarrayreverseenumerator *rover)
{
}

#endif

