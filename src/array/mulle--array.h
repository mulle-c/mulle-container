//  mulle--array.h
//  mulle-container
//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mulle__array__h__
#define mulle__array__h__

#include "mulle--pointerarray.h"
#include "mulle-container-callback.h"
#include "mulle-range.h"

#include <stddef.h>
#include <string.h>
#include <assert.h>

// mulle--array accepts mulle_container_keycallback. Elements in the array are
// retained on addition and released on removal. Furthermore equality is no
// longer determined by pointer equality but by an is_equal callback.

#define MULLE__ARRAY_BASE   MULLE__POINTERARRAY_BASE


struct mulle__array
{
   MULLE__ARRAY_BASE;
};


struct mulle__array    *mulle__array_create( struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
static inline void    _mulle__array_init( struct mulle__array *array,
                                          unsigned int capacity,
                                          struct mulle_allocator *allocator)
{
   _mulle__pointerarray_init( (struct mulle__pointerarray *) array,
                              capacity,
                              allocator);
}


MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_destroy( struct mulle__array *array,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator);
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_done( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);



# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__array_get_size( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


static inline unsigned int
   mulle__array_get_size( struct mulle__array *array)
{
   return( mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__array_get_guaranteed_size( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}


static inline unsigned int
	mulle__array_get_guaranteed_size( struct mulle__array *array)
{
   return( mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__array_get_count( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


static inline unsigned int
   mulle__array_get_count( struct mulle__array *array)
{
   return( mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__array_is_full( struct mulle__array *array)
{
   return( _mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


static inline int
   mulle__array_is_full( struct mulle__array *array)
{
   return( mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


# pragma mark - memory operations

MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__array_guarantee( struct mulle__array *array,
                            unsigned int length,
                            struct mulle_allocator *allocator)
{
   return( _mulle__pointerarray_guarantee( (struct mulle__pointerarray *) array,
                                           length,
                                           allocator));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__array_grow( struct mulle__array *array,
                       struct mulle_allocator *allocator)
{
   _mulle__pointerarray_grow( (struct mulle__pointerarray *) array,
                              allocator);
}


# pragma mark - operations

MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__array_remove_in_range( struct mulle__array *array,
                                      struct mulle_range location,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_reset( struct mulle__array *array,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);



// other may be NULL
MULLE_C_NONNULL_FIRST_THIRD
int    _mulle__array_is_equal( struct mulle__array *array,
                               struct mulle__array *other,
                               struct mulle_container_keycallback *callback)
                                 MULLE_C_NONNULL_FIRST_THIRD;


MULLE_C_NONNULL_FIRST_THIRD
static inline
void    _mulle__array_add_guaranteed( struct mulle__array *array,
                                      void  *p,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   assert( p != callback->notakey);

   p = (*callback->retain)( callback, p, allocator);
   _mulle__pointerarray_add_guaranteed( (struct mulle__pointerarray *)  array,
                                       p);
}

MULLE_C_NONNULL_FIRST_THIRD
static inline
void    _mulle__array_add( struct mulle__array *array,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{

   p = (*callback->retain)( callback, p, allocator);
   _mulle__pointerarray_add( (struct mulle__pointerarray *)  array,
                             p,
                             allocator);
}

MULLE_C_NONNULL_FIRST_THIRD
void    _mulle__array_set( struct mulle__array *array,
                           unsigned int i,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__array_get( struct mulle__array *array,
                           unsigned int i)
{
   return( _mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


static inline
void   *mulle__array_get( struct mulle__array *array,
                          unsigned int i)
{
   return( mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline unsigned int
   _mulle__array_get_in_range( struct mulle__array *array,
                               struct mulle_range range,
                               void *buf)
{
   return( _mulle__pointerarray_get_in_range( (struct mulle__pointerarray *) array,
                                              range,
                                              buf));
}


// TODO: maybe rename to get_pointers_in_range
static inline unsigned int
   mulle__array_get_in_range( struct mulle__array *array,
                              struct mulle_range range,
                              void *buf)
{
   return( mulle__pointerarray_get_in_range( (struct mulle__pointerarray *) array,
                                              range,
                                              buf));
}


MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__array_get_last( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_last( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
void  _mulle__array_remove_last( struct mulle__array *array,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator)
{
   void   *p;

   p = _mulle__pointerarray_pop( (struct mulle__pointerarray *) array);
   (*callback->release)( callback, p, allocator);
}


void
   mulle__array_add_array( struct mulle__array *array,
                           struct mulle__array *other,
                           struct mulle_range range,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_reset( struct mulle__array *array,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__array_find_in_range_identical( struct mulle__array *array,
                                          void *obj,
                                          struct mulle_range range)
{
   return( _mulle__pointerarray_find_in_range( (struct mulle__pointerarray *) array,
                                                obj,
                                                range));
}

MULLE_C_NONNULL_FIRST
uintptr_t  _mulle__array_find_in_range( struct mulle__array *array,
                                        void *obj,
                                        struct mulle_range range,
                                        struct mulle_container_keycallback *callback);


# pragma mark - debugging

MULLE_C_NONNULL_FIRST_SECOND
char   *_mulle__array_describe( struct mulle__array *array,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator);


MULLE_C_NONNULL_SECOND
static inline
char   *mulle__array_describe( struct mulle__array *array,
                               struct mulle_container_keycallback *callback,
                               struct mulle_allocator *allocator)
{
   if( ! array)
      return( mulle_allocator_strdup( allocator, "NULL"));
   return( _mulle__array_describe( array, callback, allocator));
}


#pragma mark - enumeration

#define MULLE__ARRAYENUMERATOR_BASE   MULLE__POINTERARRAYENUMERATOR_BASE

struct mulle__arrayenumerator
{
   MULLE__ARRAYENUMERATOR_BASE;
};

extern struct mulle__arrayenumerator   mulle__arrayenumerator_empty;


MULLE_C_NONNULL_FIRST
static inline
struct mulle__arrayenumerator   _mulle__array_enumerate( struct mulle__array *array)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = _mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array);
   return( *(struct mulle__arrayenumerator *) &rover);
}


static inline
struct mulle__arrayenumerator   mulle__array_enumerate( struct mulle__array *array)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array);
   return( *(struct mulle__arrayenumerator *) &rover);
}


// item is unchanged if rval is 0 (!)
MULLE_C_NONNULL_FIRST
static inline int
   _mulle__arrayenumerator_next( struct mulle__arrayenumerator *rover, void **item)
{
   return( _mulle__pointerarrayenumerator_next( (struct mulle__pointerarrayenumerator *) rover,
                                               item));
}

// item is unchanged if rval is 0 (!)
MULLE_C_NONNULL_FIRST
static inline int
   mulle__arrayenumerator_next( struct mulle__arrayenumerator *rover, void **item)
{
   return( mulle__pointerarrayenumerator_next( (struct mulle__pointerarrayenumerator *) rover,
                                               item));
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}


static inline
void   mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}


#pragma mark - reverse enumeration

#define MULLE__ARRAYREVERSEENUMERATOR_BASE   MULLE__POINTERARRAYREVERSEENUMERATOR_BASE

struct mulle__arrayreverseenumerator
{
   MULLE__ARRAYREVERSEENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct  mulle__arrayreverseenumerator
   _mulle__array_reverseenumerate( struct mulle__array *array)
{
   struct mulle__pointerarrayreverseenumerator   rover;

   rover = _mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array);
   return( *(struct mulle__arrayreverseenumerator *) &rover);
}


static inline struct  mulle__arrayreverseenumerator
   mulle__array_reverseenumerate( struct mulle__array *array)
{
   struct mulle__pointerarrayreverseenumerator   rover;

   rover = mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array);
   return( *(struct mulle__arrayreverseenumerator *) &rover);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__arrayreverseenumerator_next( struct mulle__arrayreverseenumerator *rover,
                                        void **item)
{
   return( _mulle__pointerarrayreverseenumerator_next( (struct mulle__pointerarrayreverseenumerator *) rover,
                                                       item));
}


static inline int
   mulle__arrayreverseenumerator_next( struct mulle__arrayreverseenumerator *rover,
                                       void **item)
{
   return( mulle__pointerarrayreverseenumerator_next( (struct mulle__pointerarrayreverseenumerator *) rover,
                                                       item));
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle__arrayreverseenumerator_done( struct mulle__arrayreverseenumerator *rover)
{
}


static inline
void   mulle__arrayreverseenumerator_done( struct mulle__arrayreverseenumerator *rover)
{
}

/*
 *
 */
static inline
int   mulle__array_member( struct mulle__array *array,
                           void *p,
                           struct mulle_container_keycallback *callback)
{
   struct mulle__arrayenumerator   rover;
   void                            *q;

   rover = _mulle__array_enumerate( array);
   {
      while( _mulle__arrayenumerator_next( &rover, &q))
         if( callback->is_equal( callback, q, p))
         {
            mulle__arrayenumerator_done( &rover);
            return( 1);
         }
   }
   mulle__arrayenumerator_done( &rover);
   return( 0);
}

#endif

