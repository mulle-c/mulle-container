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

#include <stddef.h>
#include <string.h>
#include <assert.h>

// mulle--array acceps notakey and some callback functions in a
// mulle_container_keycallback function. Elements in the array are retained
// on addition and released on removal. Furthermore equality is no longer
// determined by pointer equality but also by a callback.
//
// mulle-array does not autocompact
//
#define MULLE__ARRAY_BASE   MULLE__POINTERARRAY_BASE


struct mulle__array
{
   MULLE__ARRAY_BASE;
};


struct mulle__array    *mulle__array_create( struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
static inline void    _mulle__array_init( struct mulle__array *array,
                                          size_t capacity,
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
static inline size_t
   _mulle__array_get_count( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   mulle__array_get_count( struct mulle__array *array)
{
   return( mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__array_get_used( struct mulle__array *array)
{
   return( _mulle__pointerarray_get_used( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   mulle__array_get_used( struct mulle__array *array)
{
   return( mulle__pointerarray_get_used( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__array_needs_compaction( struct mulle__array *array)
{
   return( _mulle__pointerarray_needs_compaction( (struct mulle__pointerarray *) array));
}


# pragma mark - operations


//
// ranges may contain notakey values...
//
MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__array_remove_in_range( struct mulle__array *array,
                                      struct mulle_range location,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
static inline void   _mulle__array_compact( struct mulle__array *array,
                                            struct mulle_container_keycallback *callback)
{
   _mulle__pointerarray_compact( (struct mulle__pointerarray *) array,
                                 callback->notakey);
}

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
static inline void    _mulle__array_add( struct mulle__array *array,
                                         void  *p,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator)
{

   p = (*callback->retain)( callback, p, allocator);
   _mulle__pointerarray_add( (struct mulle__pointerarray *)  array,
                             p,
                             callback->notakey,
                             allocator);
}

MULLE_C_NONNULL_FIRST_THIRD
void    _mulle__array_set( struct mulle__array *array,
                           size_t i,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
static inline void   *_mulle__array_get( struct mulle__array *array,
                                         size_t i)
{
   return( _mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__array_find_last(  struct mulle__array *array, void *notakey)
{
   return( _mulle__pointerarray_find_last( (struct mulle__pointerarray *) array, notakey));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void  *_mulle__array_remove_last( struct mulle__array *array,
                                                struct mulle_container_keycallback *callback)
{
   return( _mulle__pointerarray_remove_last( (struct mulle__pointerarray *) array,
                                             callback->notakey));
}


MULLE_C_NONNULL_FIRST_FOURTH
int   _mulle__array_add_multiple( struct mulle__array *array,
                                  void **pointers,
                                  size_t length,
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
                                                range,
                                                obj));
}

MULLE_C_NONNULL_FIRST
uintptr_t  _mulle__array_find_in_range( struct mulle__array *array,
                                        void *obj,
                                        struct mulle_range range,
                                        struct mulle_container_keycallback *callback);


#pragma mark - enumeration

#define MULLE__ARRAYENUMERATOR_BASE   MULLE__POINTERARRAYENUMERATOR_BASE


struct mulle__arrayenumerator
{
   MULLE__ARRAYENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct mulle__arrayenumerator
   __mulle__array_enumerate( struct mulle__array *array,
                             void *notakey)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = _mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array,
                                           notakey);
   return( *(struct mulle__arrayenumerator *) &rover);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline struct mulle__arrayenumerator
   _mulle__array_enumerate( struct mulle__array *array,
                            struct mulle_container_keycallback *callback)
{
   return( __mulle__array_enumerate( array, callback->notakey));
}


static inline struct  mulle__arrayenumerator
   mulle__array_enumerate_nil( struct mulle__array *array)
{
   struct mulle__arrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   return( __mulle__array_enumerate( array, NULL));
}


MULLE_C_NONNULL_FIRST
static inline struct  mulle__arrayenumerator
   __mulle__array_reverseenumerate( struct mulle__array *array,
                                    void *notakey)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = _mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array,
                                                  notakey);
   return( *(struct mulle__arrayenumerator *) &rover);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline struct  mulle__arrayenumerator
   _mulle__array_reverseenumerate( struct mulle__array *array,
                                   struct mulle_container_keycallback *callback)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = _mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array,
                                                  callback->notakey);
   return( *(struct mulle__arrayenumerator *) &rover);
}


// use this if array->not_a_key is known to be null
static inline struct  mulle__arrayenumerator
   mulle__array_reverseenumerate_nil( struct mulle__array *array)
{
   struct mulle__arrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   return( __mulle__array_reverseenumerate( array, NULL));
}



MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle__arrayenumerator_next( struct mulle__arrayenumerator *rover)
{
   return( _mulle__pointerarrayenumerator_next( (struct mulle__pointerarrayenumerator *) rover));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}


static inline void   mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}


#endif

