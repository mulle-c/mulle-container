//
//   mulle-uniquepointerarray.h
//  mulle-container
//
//  Created by Nat! on 03.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle_uniquepointerarray_h__
#define mulle_uniquepointerarray_h__

#include "mulle--uniquepointerarray.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


//
// Apart from the embedded allocator, mulle_uniquepointerarray adds no features
// to struct mulle__uniquepointerarray
//
struct mulle_uniquepointerarray
{
   MULLE__UNIQUEPOINTERARRAY_BASE;
   struct mulle_allocator   *allocator;
};


#define MULLE_UNIQUEPOINTERARRAY_DATA( storage, count, xallocator) \
   ((struct mulle_uniquepointerarray)                              \
   {                                                               \
      ._storage         = (storage),                               \
      ._curr            = (storage),                               \
      ._sentinel        = &(storage)[ (count)],                    \
      ._initial_storage = (storage),                               \
      .allocator        = (xallocator)                             \
   })


MULLE_C_NONNULL_FIRST
static inline void    _mulle_uniquepointerarray_init( struct mulle_uniquepointerarray *array,
                                                      struct mulle_allocator *allocator)
{
   _mulle__uniquepointerarray_init( (struct mulle__uniquepointerarray *) array);
   array->allocator = allocator;
}


static inline void    mulle_uniquepointerarray_init( struct mulle_uniquepointerarray *array,
                                                     struct mulle_allocator *allocator)
{
   if( array)
      _mulle_uniquepointerarray_init( array, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle_uniquepointerarray_done( struct mulle_uniquepointerarray *array)
{
   _mulle__uniquepointerarray_done( (struct mulle__uniquepointerarray *) array, array->allocator);
}


static inline void    mulle_uniquepointerarray_done( struct mulle_uniquepointerarray *array)
{
   if( array)
      _mulle_uniquepointerarray_done( array);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_uniquepointerarray_get_size( struct mulle_uniquepointerarray *array)
{
   return( mulle__uniquepointerarray_get_size( (struct mulle__uniquepointerarray *) array));
}


static inline size_t
   mulle_uniquepointerarray_get_size( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_get_size( (struct mulle__uniquepointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_uniquepointerarray_get_count( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_get_count( (struct mulle__uniquepointerarray *) array));
}


static inline size_t
   mulle_uniquepointerarray_get_count( struct mulle_uniquepointerarray *array)
{
   return( mulle__uniquepointerarray_get_count( (struct mulle__uniquepointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_uniquepointerarray_get_guaranteed_size( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_get_guaranteed_size( (struct mulle__uniquepointerarray *) array));
}


static inline size_t
	mulle_uniquepointerarray_get_guaranteed_size( struct mulle_uniquepointerarray *array)
{
   return( mulle__uniquepointerarray_get_guaranteed_size( (struct mulle__uniquepointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_uniquepointerarray_is_full( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_is_full( (struct mulle__uniquepointerarray *) array));
}


static inline int
   mulle_uniquepointerarray_is_full( struct mulle_uniquepointerarray *array)
{
   return( mulle__uniquepointerarray_is_full( (struct mulle__uniquepointerarray *) array));
}


# pragma mark - operations

MULLE_C_NONNULL_FIRST_SECOND
static inline void    _mulle_uniquepointerarray_insert( struct mulle_uniquepointerarray *array, void *p)
{
   _mulle__uniquepointerarray_insert( (struct mulle__uniquepointerarray *) array, p, array->allocator);
}


static inline void    mulle_uniquepointerarray_insert( struct mulle_uniquepointerarray *array, void *p)
{
   if( array && p)
      _mulle_uniquepointerarray_insert( array, p);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle_uniquepointerarray_get( struct mulle_uniquepointerarray *array, void *p)
{
   return( _mulle__uniquepointerarray_get( (struct mulle__uniquepointerarray *) array, p));
}


MULLE_C_NONNULL_SECOND
static inline void   *mulle_uniquepointerarray_get( struct mulle_uniquepointerarray *array, void *p)
{
   return( array ? _mulle_uniquepointerarray_get( array, p) : NULL);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int   _mulle_uniquepointerarray_member( struct mulle_uniquepointerarray *array,
                                                      void *p)
{
   return( _mulle__uniquepointerarray_member( (struct mulle__uniquepointerarray *) array, p));
}


MULLE_C_NONNULL_SECOND
static inline int   mulle_uniquepointerarray_member( struct mulle_uniquepointerarray *array,
                                                     void *p)
{
   if( array)
      return( _mulle_uniquepointerarray_member( array, p));
}



#define mulle_uniquepointerarray_do( name)                        \
   for( struct mulle_uniquepointerarray                           \
           name ## __container = { 0 },                           \
           *name = &name ## __container,                          \
           *name ## __i = NULL;                                   \
        ! name ## __i;                                            \
        name ## __i =                                             \
        (                                                         \
           _mulle_uniquepointerarray_done( &name ## __container), \
           (void *) 0x1                                           \
        )                                                         \
      )                                                           \
      for( int  name ## __j = 0;    /* break protection */        \
           name ## __j < 1;                                       \
           name ## __j++)

#define mulle_uniquepointerarray_do_flexible( name, stackcount)                 \
   void   *name ## __storage[ stackcount];                                      \
   for( struct mulle_uniquepointerarray                                         \
           name ## __container =                                                \
              MULLE_UNIQUEPOINTERARRAY_DATA( name ## __storage, stackcount, 0), \
           *name = &name ## __container,                                        \
           *name ## __i = NULL;                                                 \
        ! name ## __i;                                                          \
        name ## __i =                                                           \
        (                                                                       \
           _mulle_uniquepointerarray_done( &name ## __container),               \
           (void *) 0x1                                                         \
        )                                                                       \
      )                                                                         \
      for( int  name ## __j = 0;    /* break protection */                      \
           name ## __j < 1;                                                     \
           name ## __j++)


#endif
