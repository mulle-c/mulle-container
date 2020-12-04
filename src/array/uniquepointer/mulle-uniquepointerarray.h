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


MULLE_C_NONNULL_FIRST
static inline void    _mulle_uniquepointerarray_init( struct mulle_uniquepointerarray *set,
                                              struct mulle_allocator *allocator)
{
   _mulle__uniquepointerarray_init( (struct mulle__uniquepointerarray *) set);
   set->allocator = allocator;
}


static inline void    mulle_uniquepointerarray_init( struct mulle_uniquepointerarray *set,
                                             struct mulle_allocator *allocator)
{
   if( set)
      _mulle_uniquepointerarray_init( set, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle_uniquepointerarray_done( struct mulle_uniquepointerarray *set)
{
   _mulle__uniquepointerarray_done( (struct mulle__uniquepointerarray *) set, set->allocator);
}


static inline void    mulle_uniquepointerarray_done( struct mulle_uniquepointerarray *set)
{
   if( set)
      _mulle_uniquepointerarray_done( set);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle_uniquepointerarray_get_size( struct mulle_uniquepointerarray *array)
{
   return( mulle__uniquepointerarray_get_size( (struct mulle__uniquepointerarray *) array));
}


static inline unsigned int
   mulle_uniquepointerarray_get_size( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_get_size( (struct mulle__uniquepointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle_uniquepointerarray_get_count( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_get_count( (struct mulle__uniquepointerarray *) array));
}


static inline unsigned int
   mulle_uniquepointerarray_get_count( struct mulle_uniquepointerarray *array)
{
   return( mulle__uniquepointerarray_get_count( (struct mulle__uniquepointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle_uniquepointerarray_get_guaranteed_size( struct mulle_uniquepointerarray *array)
{
   return( _mulle__uniquepointerarray_get_guaranteed_size( (struct mulle__uniquepointerarray *) array));
}


static inline unsigned int
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
static inline void    _mulle_uniquepointerarray_insert( struct mulle_uniquepointerarray *set, void *p)
{
   _mulle__uniquepointerarray_insert( (struct mulle__uniquepointerarray *) set, p, set->allocator);
}


static inline void    mulle_uniquepointerarray_insert( struct mulle_uniquepointerarray *set, void *p)
{
   if( set && p)
      _mulle_uniquepointerarray_insert( set, p);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   *_mulle_uniquepointerarray_get( struct mulle_uniquepointerarray *set, void *p)
{
   return( _mulle__uniquepointerarray_get( (struct mulle__uniquepointerarray *) set, p));
}


MULLE_C_NONNULL_SECOND
static inline void   *mulle_uniquepointerarray_get( struct mulle_uniquepointerarray *set, void *p)
{
   return( set ? _mulle_uniquepointerarray_get( set, p) : NULL);
}

MULLE_C_NONNULL_FIRST_SECOND
static inline int   _mulle_uniquepointerarray_member( struct mulle_uniquepointerarray *set,
                                                        void *p)
{
   return( _mulle__uniquepointerarray_member( (struct mulle__uniquepointerarray *) set, p));
}

#endif
