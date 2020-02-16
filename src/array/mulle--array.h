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

#include "mulle-container-callback.h"

#include <stddef.h>
#include <string.h>
#include <assert.h>

// NSArray/NSMutableArray
//
// a _mulle_array is a limited mutable array, you
// can use it as a stack.
// It uses callback, so that you can store arbitrary kinds of data in it.
//
// You can place it on a stack, if you are careful (the storage will be
// allocated from the heap though)
//
#define MULLE__ARRAY_BASE   \
   void      **_storage;    \
   void      **_curr;       \
   void      **_sentinel;   \
   unsigned int   _size


struct mulle__array
{
   MULLE__ARRAY_BASE;
};


#define MULLE__ARRAYENUMERATOR_BASE   \
   void   **_curr;                    \
   void   **_sentinel;                \
   void   *_notakey


struct mulle__arrayenumerator
{
   MULLE__ARRAYENUMERATOR_BASE;
};


struct mulle__array    *mulle__array_create( struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_destroy( struct mulle__array *array,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator);
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_done( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
int    _mulle__array_grow( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
int    _mulle__array_size_to_fit( struct mulle__array *array,
                                  struct mulle_container_keycallback *callback,
                                  struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__array_remove_in_range( struct mulle__array *array,
                                      unsigned int location,
                                      unsigned int length,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__array_zero_in_range( struct mulle__array *array,
                                    unsigned int location,
                                    unsigned int length,
                                    struct mulle_container_keycallback *callback);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_compact_zeroes( struct mulle__array *array,
                                     struct mulle_container_keycallback *callback);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_remove_all( struct mulle__array *array,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST
static inline void    _mulle__array_init( struct mulle__array *array,
                                          unsigned int length)
{
   assert( length != (unsigned int) -1);

   array->_curr     =
   array->_storage  =
   array->_sentinel = NULL;
   array->_size     = length >> 1;
}


MULLE_C_NONNULL_FIRST
static inline int   _mulle__array_has_overflown( struct mulle__array *buffer)
{
   return( buffer->_sentinel == (void *) INTPTR_MIN);
}


MULLE_C_NONNULL_FIRST
static inline void   **_mulle__array_get_all( struct mulle__array *array)
{
   assert( ! _mulle__array_has_overflown( array));
   return( array->_storage);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__array_get_count( struct mulle__array *array)
{
   assert( ! _mulle__array_has_overflown( array));
   return( (unsigned int) (array->_curr - array->_storage));
}


// other may be NULL
MULLE_C_NONNULL_FIRST_SECOND_THIRD
int    _mulle__array_is_equal( struct mulle__array *array,
                               struct mulle__array *other,
                               struct mulle_container_keycallback *callback)
                                 MULLE_C_NONNULL_FIRST_THIRD;

MULLE_C_NONNULL_FIRST_THIRD
static inline void   **_mulle__array_advance( struct mulle__array *array,
                                              unsigned int length,
                                              struct mulle_container_keycallback *callback,
                                              struct mulle_allocator *allocator)
{
   void   *old;

   while( &array->_curr[ length] > array->_sentinel)
      if( _mulle__array_grow( array, callback, allocator))
         return( NULL);

   old           = array->_curr;
   array->_curr = &array->_curr[ length];
   return( old);
}


MULLE_C_NONNULL_FIRST
static inline int   _mulle__array_is_full( struct mulle__array *array)
{
   assert( ! _mulle__array_has_overflown( array));
   return( array->_curr >= array->_sentinel);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__array_guaranteedsize( struct mulle__array *array)
{
   assert( ! _mulle__array_has_overflown( array));
   return( (unsigned int) (array->_sentinel - array->_curr));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline void   **_mulle__array_guarantee( struct mulle__array *array,
                                                unsigned int length,
                                                struct mulle_container_keycallback *callback,
                                                struct mulle_allocator *allocator)
{
   while( &array->_curr[ length] > array->_sentinel)
      if( _mulle__array_grow( array, callback, allocator))
         return( NULL);

   return( array->_curr);
}


MULLE_C_NONNULL_FIRST_THIRD
static inline void    _mulle__array_add( struct mulle__array *array,
                                         void  *p,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator)
{
   assert( p != callback->notakey);

   if( _mulle__array_is_full( array))
      if( _mulle__array_grow( array, callback, allocator))
         return;

   *array->_curr++ = (*callback->retain)( callback, p, allocator);
   assert( array->_curr[ -1]  != callback->notakey);
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__array_get( struct mulle__array *array, unsigned int index)
{
   assert( ! _mulle__array_has_overflown( array));
   assert( &array->_storage[ index] < array->_sentinel);
   return( array->_storage[ index]);
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__array_get_last(  struct mulle__array *array)
{
   assert( ! _mulle__array_has_overflown( array));
   if( array->_storage == array->_curr)
      return( NULL);

   return( array->_curr[ -1]);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   _mulle__array_remove_last( struct mulle__array *array,
                                                struct mulle_container_keycallback *callback,
                                                struct mulle_allocator *allocator)
{
   assert( ! _mulle__array_has_overflown( array));

   if( array->_storage != array->_curr)
      (*callback->release)( callback, *--array->_curr, allocator);
}


MULLE_C_NONNULL_FIRST_FOURTH
int   _mulle__array_add_multiple( struct mulle__array *array,
                                  void **pointers,
                                  unsigned int length,
                                  struct mulle_container_keycallback *callback,
                                  struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline void    _mulle__array_add_array( struct mulle__array *array,
                                               struct mulle__array *other,
                                               struct mulle_container_keycallback *callback,
                                               struct mulle_allocator *allocator)
{
   assert( array != other);
   if( other)
      _mulle__array_add_multiple( array,
                                 _mulle__array_get_all( other),
                                 _mulle__array_get_count( other),
                                 callback,
                                 allocator);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void   _mulle__array_reset( struct mulle__array *array,
                                          struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator)
{
   _mulle__array_done( array, callback, allocator);
   _mulle__array_init( array, array->_size);
}



#pragma mark -
#pragma mark grab contents destrutively
//
// you now own the allocated block now
//
MULLE_C_NONNULL_FIRST
static inline void   **_mulle__array_extract_all( struct mulle__array *array)
{
   void   **block;

   assert( ! _mulle__array_has_overflown( array));

   block = array->_storage;

   array->_storage  =
   array->_curr     =
   array->_sentinel = NULL;

   return( block);
}


MULLE_C_NONNULL_FIRST
unsigned long  _mulle__array_find_in_range_identical( struct mulle__array *array,
                                                      void *obj,
                                                      unsigned int location,
                                                      unsigned int length);

MULLE_C_NONNULL_FIRST_FIFTH
unsigned long  _mulle__array_find_in_range( struct mulle__array *array,
                                            void *obj,
                                            unsigned int location,
                                            unsigned int length,
                                            struct mulle_container_keycallback *callback);


#pragma mark -
#pragma mark enumeration

MULLE_C_NONNULL_FIRST_SECOND
static inline struct mulle__arrayenumerator
   _mulle__array_enumerate( struct mulle__array *array,
                            struct mulle_container_keycallback *callback)
{
   struct mulle__arrayenumerator   rover;

   assert( ! _mulle__array_has_overflown( array));

   rover._sentinel = array->_curr;
   rover._curr     = array->_storage;
   rover._notakey  = callback->notakey;

   return( rover);
}

static inline struct mulle__arrayenumerator
   mulle__array_enumerate_nil( struct mulle__array *array)
{
   struct mulle__arrayenumerator   rover;

   assert( ! _mulle__array_has_overflown( array));

   if( array)
   {
      rover._sentinel = array->_curr;
      rover._curr     = array->_storage;
   }
   else
   {
      rover._curr     = NULL;
      rover._sentinel = NULL;
   }
   rover._notakey  = NULL;
   return( rover);
}



MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle__arrayenumerator_next( struct mulle__arrayenumerator *rover)
{
   void  *p;

   while( rover->_curr < rover->_sentinel)
   {
      p = *rover->_curr++;
      if( p != rover->_notakey)
         return( p);
   }
   return( rover->_notakey);
}


MULLE_C_NONNULL_FIRST
static inline void   mulle__arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}


#endif

