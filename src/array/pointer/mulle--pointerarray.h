//
//  mulle-pointerarray.h
//  mulle-container
//
//  Created by Nat! on 10.03.15.
//  Copyright (c) 2015 Mulle kybernetiK. All rights reserved.
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
#ifndef mulle__pointerarray_h__
#define mulle__pointerarray_h__

#include "include.h"
#include "mulle-container-operation.h"
#include "mulle-range.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


struct mulle_pointers
{
   void           **pointers;
   unsigned int   count;
};




//
// mulle__pointerarray is a growing array of pointers, it just reallocs.
//
#define MULLE__POINTERARRAY_BASE    \
   void      **_storage;            \
   void      **_curr;               \
   void      **_sentinel;           \
   void      **_initial_storage


struct mulle__pointerarray
{
   MULLE__POINTERARRAY_BASE;
};


static inline struct mulle__pointerarray  *
	mulle__pointerarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle__pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle__pointerarray));
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerarray_init_with_static_pointers( struct mulle__pointerarray *array,
                                                                     void **storage,
                                                                     unsigned int capacity)
{
   assert( storage);
   assert( capacity);

   array->_storage         =
   array->_initial_storage = storage;
   array->_curr            = array->_storage;
   array->_sentinel        = &array->_curr[ capacity];
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerarray_init( struct mulle__pointerarray *array,
                                                unsigned int capacity,
                                                struct mulle_allocator *allocator)
{
   if( ! capacity)
   {
      memset( array, 0, sizeof( *array));
      return;
   }

   array->_storage         = mulle_allocator_malloc( allocator,
                                                     capacity * sizeof( void *));
   array->_curr            = array->_storage;
   array->_sentinel        = &array->_curr[ capacity];
   array->_initial_storage = NULL;
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle__pointerarray_done( struct mulle__pointerarray *array,
                                               struct mulle_allocator *allocator)
{
   if( array->_storage != array->_initial_storage)
      mulle_allocator_free( allocator, array->_storage);
}


static inline void  mulle__pointerarray_done( struct mulle__pointerarray *array,
                                              struct mulle_allocator *allocator)
{
   if( array)
      _mulle__pointerarray_done( array, allocator);
}

static inline struct mulle__pointerarray *
   mulle__pointerarray_create( struct mulle_allocator *allocator)
{
   struct mulle__pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( *array));
   _mulle__pointerarray_init( array, 0, allocator);
   return( array);
}


static inline void  mulle__pointerarray_destroy( struct mulle__pointerarray *array,
                                                 struct mulle_allocator *allocator)
{
   if( array)
   {
      _mulle__pointerarray_done( array, allocator);
      mulle_allocator_free( allocator, array);
   }
}


# pragma mark - petty accessors


// there can be no useful "lenient" variant (used by MulleJS)
MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__pointerarray_get_pointers( struct mulle__pointerarray *array)
{
   return( array->_storage);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__pointerarray_get_size( struct mulle__pointerarray *array)
{
   return( (unsigned int) (array->_sentinel - array->_storage));
}


static inline unsigned int
   mulle__pointerarray_get_size( struct mulle__pointerarray *array)
{
   return(  array ? _mulle__pointerarray_get_size( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( (unsigned int) (array->_curr - array->_storage));
}


static inline unsigned int
   mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( array ? _mulle__pointerarray_get_count( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
	_mulle__pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
{
   return( (unsigned int) (array->_sentinel - array->_curr));
}


static inline unsigned int
	mulle__pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
{
   return( array ? (unsigned int) (array->_sentinel - array->_curr) : 0);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointerarray_is_full( struct mulle__pointerarray *array)
{
   return( array->_curr == array->_sentinel);
}


static inline int
   mulle__pointerarray_is_full( struct mulle__pointerarray *array)
{
   return( array ? _mulle__pointerarray_is_full( array) : 1);
}


# pragma mark - memory operations

MULLE_C_NONNULL_FIRST
void **  _mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                         unsigned int length,
                                         struct mulle_allocator *allocator);

static inline
void **   mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                         unsigned int length,
                                         struct mulle_allocator *allocator)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_guarantee( array, length, allocator));
}


MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                  struct mulle_allocator *allocator);

static inline
void   mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                 struct mulle_allocator *allocator)
{
   if( array)
      _mulle__pointerarray_grow( array, allocator);
}


MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                     void *notakey);


static inline
void   mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                    void *notakey)
{
   if( array)
      _mulle__pointerarray_compact( array, notakey);
}


// there can be no useful "lenient" variant (used by MulleJS)
MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__pointerarray_advance( struct mulle__pointerarray *array,
                                 unsigned int length,
                                 struct mulle_allocator *allocator)
{
   void   **reserved;

   _mulle__pointerarray_guarantee( array, length, allocator);

   reserved      = array->_curr;
   array->_curr += length;

   return( reserved);
}


# pragma mark - operations

// if you guarantee that there is enough space for the add beforehand
// you can use this faster version
MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerarray_add_guaranteed( struct mulle__pointerarray *array,
                                            void *pointer)
{
   *array->_curr++ = pointer;
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerarray_add( struct mulle__pointerarray *array,
                                 void *pointer,
                                 struct mulle_allocator *allocator)
{
   if( array->_curr == array->_sentinel)
      _mulle__pointerarray_grow( array, allocator);

   *array->_curr++ = pointer;
}


static inline
void   mulle__pointerarray_add( struct mulle__pointerarray *array,
                                void  *pointer,
                                struct mulle_allocator *allocator)
{
   if( array)
      return( _mulle__pointerarray_add( array, pointer, allocator));
}


MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__pointerarray_remove_last( struct mulle__pointerarray *array)
{
   if( array->_curr > array->_storage)
      return( *--array->_curr);
   return( NULL);
}


static inline
void   *mulle__pointerarray_remove_last( struct mulle__pointerarray *array)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_remove_last( array));
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerarray_reset( struct mulle__pointerarray *array)
{
   array->_curr = array->_storage;
}


static inline
void   mulle__pointerarray_reset( struct mulle__pointerarray *array)
{
   if( array)
      _mulle__pointerarray_reset( array);
}


MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                             struct mulle_range range);


static inline
void
   mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                        struct mulle_range range)
{
   if( array)
      _mulle__pointerarray_remove_in_range( array, range);
}


MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__pointerarray_get( struct mulle__pointerarray *array, unsigned int i)
{
   assert( array);
   assert( &array->_storage[ i] >= array->_storage);
   assert( &array->_storage[ i] < array->_curr);

   return( array->_storage[ i]);
}


static inline
void   *mulle__pointerarray_get( struct mulle__pointerarray *array,
                                 unsigned int i)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_get( array, i));
}


MULLE_C_NONNULL_FIRST_THIRD
unsigned int
   _mulle__pointerarray_get_in_range( struct mulle__pointerarray *array,
                                      struct mulle_range range,
                                      void *buf);

static inline
unsigned int
   mulle__pointerarray_get_in_range( struct mulle__pointerarray *array,
                                     struct mulle_range range,
                                     void *buf)
{
   if( ! array)
      return( 0);
   return( _mulle__pointerarray_get_in_range( array, range, buf));
}

//
// return value is the index of a compacted array, but it doesn't compact
// yet
//
MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                                void *p,
                                                struct mulle_range range);

static inline uintptr_t
   mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                      void *p,
                                      struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerarray_find_in_range( array, p, range));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__pointerarray_find( struct mulle__pointerarray *array,
                              void *p)
{
   return( _mulle__pointerarray_find_in_range( array, p, mulle_range_make_all()));
}


static inline
uintptr_t   mulle__pointerarray_find( struct mulle__pointerarray *array,
                                      void *p)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerarray_find( array, p));
}


MULLE_C_NONNULL_FIRST
static inline
void  *_mulle__pointerarray_get_last( struct mulle__pointerarray *array)
{
   if( array->_curr == array->_storage)
      return( NULL);
   return( array->_curr[ -1]);
}

static inline
void  *mulle__pointerarray_get_last( struct mulle__pointerarray *array)
{
   if( array)
      return( NULL);
   return( _mulle__pointerarray_get_last( array));
}



MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__pointerarray_set( struct mulle__pointerarray *array,
                                  unsigned int i,
                                  void *p)
{
   void   *old;

   assert( array);
   assert( &array->_storage[ i] < array->_curr);

   old = array->_storage[ i];
   array->_storage[ i] = p;
   return( old);
}


static inline
void   *mulle__pointerarray_set( struct mulle__pointerarray *array,
                                 unsigned int i,
                                 void *p)
{
   if( ! array || &array->_storage[ i] >= array->_curr)
      return( NULL);
   return( _mulle__pointerarray_set( array, i, p));
}



MULLE_C_NONNULL_FIRST
struct mulle_pointers
   _mulle__pointerarray_extract_pointers( struct mulle__pointerarray *buffer,
                                          struct mulle_allocator *allocator);


#pragma mark - enumeration

#define MULLE__POINTERARRAYENUMERATOR_BASE   \
   void   **_curr;                           \
   void   **_sentinel


struct mulle__pointerarrayenumerator
{
   MULLE__POINTERARRAYENUMERATOR_BASE;
};

extern struct mulle__pointerarrayenumerator   mulle__pointerarrayenumerator_empty;


MULLE_C_NONNULL_FIRST
static inline struct mulle__pointerarrayenumerator
   _mulle__pointerarray_enumerate( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   assert( array);

   rover._curr     = array->_storage;
   rover._sentinel = array->_curr;
   assert( rover._sentinel >= rover._curr);

   return( rover);
}

static inline struct mulle__pointerarrayenumerator
   mulle__pointerarray_enumerate( struct mulle__pointerarray *array)
{
   if( ! array)
      return( mulle__pointerarrayenumerator_empty);

   return( _mulle__pointerarray_enumerate( array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__pointerarrayenumerator_next( struct mulle__pointerarrayenumerator *rover,
                                        void **item)
{
   if( rover->_curr < rover->_sentinel)
   {
      *item = *rover->_curr++;
      return( 1);
   }
   *item = NULL;
   return( 0);
}


static inline int
   mulle__pointerarrayenumerator_next( struct mulle__pointerarrayenumerator *rover,
                                       void **item)
{
   if( rover)
      if( rover->_curr < rover->_sentinel)
      {
         if( item)
            *item = *rover->_curr;
         rover->_curr++;
         return( 1);
      }

   if( item)
      *item = NULL;

   return( 0);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarrayenumerator_done( struct mulle__pointerarrayenumerator *rover)
{
}


static inline void
   mulle__pointerarrayenumerator_done( struct mulle__pointerarrayenumerator *rover)
{
}

#pragma mark - reverse enumeration


#define MULLE__POINTERARRAYREVERSEENUMERATOR_BASE   MULLE__POINTERARRAYENUMERATOR_BASE


struct mulle__pointerarrayreverseenumerator
{
   MULLE__POINTERARRAYREVERSEENUMERATOR_BASE;
};

extern struct mulle__pointerarrayreverseenumerator   mulle__pointerarrayreverseenumerator_empty;


MULLE_C_NONNULL_FIRST
static inline
struct  mulle__pointerarrayreverseenumerator
   _mulle__pointerarray_reverseenumerate( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayreverseenumerator   rover;

   assert( array);

   rover._curr     = array->_curr;
   rover._sentinel = array->_storage;
   assert( rover._sentinel <= rover._curr);

   return( rover);
}


static inline
struct mulle__pointerarrayreverseenumerator
   mulle__pointerarray_reverseenumerate( struct mulle__pointerarray *array)
{
   if( ! array)
      return( mulle__pointerarrayreverseenumerator_empty);

   return( _mulle__pointerarray_reverseenumerate( array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointerarrayreverseenumerator_next( struct mulle__pointerarrayreverseenumerator *rover,
                                               void **item)
{
   if( rover->_curr == rover->_sentinel)
   {
      *item = NULL;
      return( 0);
   }

   *item = *--rover->_curr;
   return( 1);
}


static inline int
   mulle__pointerarrayreverseenumerator_next( struct mulle__pointerarrayreverseenumerator *rover,
                                              void **item)
{
   if( ! rover || rover->_curr == rover->_sentinel)
   {
      if( item)
         *item = NULL;
      return( 0);
   }

   --rover->_curr;
   if( item)
      *item = *rover->_curr;
   return( 1);
}


MULLE_C_NONNULL_FIRST
static inline void
	_mulle__pointerarrayreverseenumerator_done( struct mulle__pointerarrayreverseenumerator *rover)
{
}


static inline void
   mulle__pointerarrayreverseenumerator_done( struct mulle__pointerarrayreverseenumerator *rover)
{
}



#pragma mark - enumerator convenience

static inline int   mulle__pointerarray_member( struct mulle__pointerarray *array,
                                                void *p)
{
   struct  mulle__pointerarrayenumerator   rover;
   void                                    *q;

   rover = _mulle__pointerarray_enumerate( array);
   {
      while( _mulle__pointerarrayenumerator_next( &rover, &q))
         if( q == p)
            break;
   }
   mulle__pointerarrayenumerator_done( &rover);

   return( q == p);
}

#endif
