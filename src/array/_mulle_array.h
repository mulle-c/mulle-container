//  _mulle_array.h
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
#ifndef _mulle_array__h__
#define _mulle_array__h__

#include "mulle_container_compiler_attributes.h"
#include "mulle_container_callback.h"

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
#define _MULLE_ARRAY_BASE   \
   void      **_storage;    \
   void      **_curr;       \
   void      **_sentinel;   \
   unsigned int   _size            


struct _mulle_array
{
   _MULLE_ARRAY_BASE;
};


#define _MULLE_ARRAYENUMERATOR_BASE   \
   void   **_curr;                    \
   void   **_sentinel;                \
   void   *_notakey


struct _mulle_arrayenumerator
{
   _MULLE_ARRAYENUMERATOR_BASE;
};


struct _mulle_array    *_mulle_array_create( struct mulle_allocator *allocator)
                                                mulle_nonnull_first;

void   _mulle_array_destroy( struct _mulle_array *array,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
                              mulle_nonnull_first_second;
void   _mulle_array_done( struct _mulle_array *array,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
                              mulle_nonnull_first_second;
int    _mulle_array_grow( struct _mulle_array *array,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
                              mulle_nonnull_first_second;
int    _mulle_array_size_to_fit( struct _mulle_array *array,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator)
                                    mulle_nonnull_first_second;

void   _mulle_array_remove_in_range( struct _mulle_array *array,
                                     unsigned int location,
                                     unsigned int length,
                                     struct mulle_container_keycallback *callback,
                                     struct mulle_allocator *allocator)
                                        mulle_nonnull_first_fourth;
void   _mulle_array_zero_in_range( struct _mulle_array *array,
                                   unsigned int location,
                                   unsigned int length,
                                   struct mulle_container_keycallback *callback)
                                       mulle_nonnull_first_fourth;
void   _mulle_array_compact_zeroes( struct _mulle_array *array,
                                    struct mulle_container_keycallback *callback)
                                       mulle_nonnull_first_second;

void   _mulle_array_remove_all( struct _mulle_array *array,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator)
                                   mulle_nonnull_first_second;


static inline void    _mulle_array_init( struct _mulle_array *array,
                                         unsigned int length)
{
   assert( array);
   assert( length != (unsigned int) -1);
   
   array->_curr     =
   array->_storage  = 
   array->_sentinel = NULL;
   array->_size     = length >> 1;
}


static inline int   _mulle_array_has_overflown( struct _mulle_array *buffer)
{
   return( buffer->_sentinel == mulle_container_not_a_pointer_key);
}


static inline void   **_mulle_array_get_all( struct _mulle_array *array)
{
   assert( ! _mulle_array_has_overflown( array));
   return( array->_storage);
}


static inline unsigned int   _mulle_array_get_count( struct _mulle_array *array)
{
   assert( ! _mulle_array_has_overflown( array));
   return( (unsigned int) (array->_curr - array->_storage));
}


// other may be NULL
int    _mulle_array_is_equal( struct _mulle_array *array,
                              struct _mulle_array *other,
                              struct mulle_container_keycallback *callback)
                                 mulle_nonnull_first_third;

static inline void   **_mulle_array_advance( struct _mulle_array *array,
                                             unsigned int length,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator)
{
   void   *old;
   
   while( &array->_curr[ length] > array->_sentinel)
      if( _mulle_array_grow( array, callback, allocator))
         return( NULL);

   old           = array->_curr;
   array->_curr = &array->_curr[ length];
   return( old);
}


static inline int   _mulle_array_is_full( struct _mulle_array *array) mulle_nonnull_first;
static inline int   _mulle_array_is_full( struct _mulle_array *array)
{
   assert( ! _mulle_array_has_overflown( array));
   return( array->_curr >= array->_sentinel);
}


static inline unsigned int   _mulle_array_guaranteedsize( struct _mulle_array *array) mulle_nonnull_first;
static inline unsigned int   _mulle_array_guaranteedsize( struct _mulle_array *array)
{
   assert( ! _mulle_array_has_overflown( array));
   return( (unsigned int) (array->_sentinel - array->_curr));
}


static inline void   **_mulle_array_guarantee( struct _mulle_array *array,
                                               unsigned int length,
                                               struct mulle_container_keycallback *callback,
                                               struct mulle_allocator *allocator)
{
   while( &array->_curr[ length] > array->_sentinel)
      if( _mulle_array_grow( array, callback, allocator))
         return( NULL);

   return( array->_curr);
}


static inline void    _mulle_array_add( struct _mulle_array *array,
                                        void  *p,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   assert( p != callback->notakey);
   
   if( _mulle_array_is_full( array))
      if( _mulle_array_grow( array, callback, allocator))
         return;
   
   *array->_curr++ = (*callback->retain)( callback, p, allocator);
   assert( array->_curr[ -1]  != callback->notakey);
}


static inline void   *_mulle_array_get( struct _mulle_array *array, unsigned int index)
{
   assert( ! _mulle_array_has_overflown( array));
   assert( &array->_storage[ index] < array->_sentinel);
   return( array->_storage[ index]);
}


static inline void   *_mulle_array_get_last(  struct _mulle_array *array)
{
   assert( ! _mulle_array_has_overflown( array));
   if( array->_storage == array->_curr)
      return( NULL);

   return( array->_curr[ -1]);
}


static inline void   _mulle_array_remove_last( struct _mulle_array *array,
                                               struct mulle_container_keycallback *callback,
                                               struct mulle_allocator *allocator)
{
   assert( ! _mulle_array_has_overflown( array));

   if( array->_storage != array->_curr)
      (*callback->release)( callback, *--array->_curr, allocator);
}


int   _mulle_array_add_multiple( struct _mulle_array *array,
                                 void **pointers,
                                 unsigned int length,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator)
                                    mulle_nonnull_first_fourth;


static inline void    _mulle_array_add_array( struct _mulle_array *array,
                                              struct _mulle_array *other,
                                              struct mulle_container_keycallback *callback,
                                              struct mulle_allocator *allocator)
{
   assert( array != other);
   if( other)
      _mulle_array_add_multiple( array,
                                 _mulle_array_get_all( other),
                                 _mulle_array_get_count( other),
                                 callback,
                                 allocator);
}


static inline void   _mulle_array_reset( struct _mulle_array *array,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator)
{
   _mulle_array_done( array, callback, allocator);
   _mulle_array_init( array, array->_size);
}



#pragma mark -
#pragma mark grab contents destrutively
//
// you now own the allocated block now
//
static inline void   **_mulle_array_extract_all( struct _mulle_array *array)
{
   void   **block;
   
   assert( ! _mulle_array_has_overflown( array));

   block = array->_storage;
   
   array->_storage  =
   array->_curr     =
   array->_sentinel = NULL;
   
   return( block);
}


#pragma mark -
#pragma mark enumeration

static inline struct _mulle_arrayenumerator   _mulle_array_enumerate( struct _mulle_array *array,
                                                                      struct mulle_container_keycallback *callback) mulle_nonnull_first_second;
static inline struct _mulle_arrayenumerator   _mulle_array_enumerate( struct _mulle_array *array,
                                                                      struct mulle_container_keycallback *callback)
{
   struct _mulle_arrayenumerator   rover;
   
   assert( ! _mulle_array_has_overflown( array));

   rover._sentinel         = array->_curr;
   rover._curr             = array->_storage;
   rover._notakey = callback->notakey;

   return( rover);
}


static inline void   *_mulle_arrayenumerator_next( struct _mulle_arrayenumerator *rover) mulle_nonnull_first;
static inline void   *_mulle_arrayenumerator_next( struct _mulle_arrayenumerator *rover)
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


static inline void   _mulle_arrayenumerator_done( struct _mulle_arrayenumerator *rover) mulle_nonnull_first;
static inline void   _mulle_arrayenumerator_done( struct _mulle_arrayenumerator *rover)
{
}


unsigned long  _mulle_array_find_in_range_identical( struct _mulle_array *array,
                                                     void *obj,
                                                     unsigned int location,
                                                     unsigned int length) mulle_nonnull_first;

unsigned long  _mulle_array_find_in_range( struct _mulle_array *array,
                                           void *obj,
                                           unsigned int location,
                                           unsigned int length,
                                           struct mulle_container_keycallback *callback) mulle_nonnull_first_fifth;

#endif

