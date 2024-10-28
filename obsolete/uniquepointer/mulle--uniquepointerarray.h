//
//  mulle--uniquepointerarray.h
//  mulle-container
//
//  Created by Nat! on 13.09.20.
//  Copyright © 2020 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle__uniquepointerarray_h__
#define mulle__uniquepointerarray_h__

#include "mulle--pointerarray.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>


//
// A really simple kind of array, that just stores pointers with pointer
// equality and uses bsearch for lookup. Useful for small sets ( < 100 entries)
// You can't store NULL.  It grows and never shrinks.
//
#define MULLE__UNIQUEPOINTERARRAY_BASE   \
   MULLE__POINTERARRAY_BASE;             \
   int   _sorted


struct mulle__uniquepointerarray
{
   MULLE__UNIQUEPOINTERARRAY_BASE;
};

#define MULLE__UNIQUEPOINTERARRAY_DATA( storage, count) \
   MULLE__POINTERARRAY_DATA( storage, count)


MULLE_C_NONNULL_FIRST
static inline void    _mulle__uniquepointerarray_init( struct mulle__uniquepointerarray *array)
{
   memset( array, 0, sizeof( *array));
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle__uniquepointerarray_done( struct mulle__uniquepointerarray *array,
                                                       struct mulle_allocator *allocator)
{
   _mulle__pointerarray_done( (struct mulle__pointerarray *) array, allocator);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__uniquepointerarray_get_size( struct mulle__uniquepointerarray *array)
{
   return( mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


static inline unsigned int
   mulle__uniquepointerarray_get_size( struct mulle__uniquepointerarray *array)
{
   return( _mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__uniquepointerarray_get_count( struct mulle__uniquepointerarray *array)
{
   return( _mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


static inline unsigned int
   mulle__uniquepointerarray_get_count( struct mulle__uniquepointerarray *array)
{
   return( mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__uniquepointerarray_get_guaranteed_size( struct mulle__uniquepointerarray *array)
{
   return( _mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}


static inline unsigned int
	mulle__uniquepointerarray_get_guaranteed_size( struct mulle__uniquepointerarray *array)
{
   return( mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__uniquepointerarray_is_full( struct mulle__uniquepointerarray *array)
{
   return( _mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


static inline int
   mulle__uniquepointerarray_is_full( struct mulle__uniquepointerarray *array)
{
   return( mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


# pragma mark - operations


MULLE_C_NONNULL_FIRST_SECOND
static inline void    _mulle__uniquepointerarray_insert( struct mulle__uniquepointerarray *array,
                                                         void *p,
                                                         struct mulle_allocator *allocator)
{
   _mulle__pointerarray_add( (struct mulle__pointerarray *) array, p, allocator);
   array->_sorted = 0;
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle__uniquepointerarray_get( struct mulle__uniquepointerarray *array, void *p)
{
   MULLE_C_NONNULL_FIRST_SECOND
   extern void   *_mulle__uniquepointerarray_member2( struct mulle__uniquepointerarray *array, void *p);
   void          **q;
   void          **sentinel;

   if( _mulle__uniquepointerarray_get_count( array) >= 12)
      return( _mulle__uniquepointerarray_member2( array, p));

   q        = array->_storage;
   sentinel = array->_curr;

   while( q < sentinel)
      if( *q++ == p)
         return( p);

   return( NULL);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int   _mulle__uniquepointerarray_member( struct mulle__uniquepointerarray *array,
                                                       void *p)
{
   return( _mulle__uniquepointerarray_get( array, p) != NULL);
}


// created by make-container-do.sh --flexible mulle--uniquepointerarray.c

#define mulle__uniquepointerarray_do( name)                              \
   for( struct mulle__uniquepointerarray                                 \
           name ## __container = { 0 },                                  \
           *name = &name ## __container,                                 \
           *name ## __i = NULL;                                          \
        ! name ## __i;                                                   \
        name ## __i =                                                    \
        (                                                                \
           _mulle__uniquepointerarray_done( &name ## __container, NULL), \
           (void *) 0x1                                                  \
        )                                                                \
      )                                                                  \
      for( int  name ## __j = 0;    /* break protection */               \
           name ## __j < 1;                                              \
           name ## __j++)

#define mulle__uniquepointerarray_do_flexible( name, stackcount)              \
   void   *name ## __storage[ stackcount];                                    \
   for( struct mulle__uniquepointerarray                                      \
           name ## __container =                                              \
              MULLE__UNIQUEPOINTERARRAY_DATA( name ## __storage, stackcount), \
           *name = &name ## __container,                                      \
           *name ## __i = NULL;                                               \
        ! name ## __i;                                                        \
        name ## __i =                                                         \
        (                                                                     \
           _mulle__uniquepointerarray_done( &name ## __container, NULL),      \
           (void *) 0x1                                                       \
        )                                                                     \
      )                                                                       \
      for( int  name ## __j = 0;    /* break protection */                    \
           name ## __j < 1;                                                   \
           name ## __j++)



#endif
