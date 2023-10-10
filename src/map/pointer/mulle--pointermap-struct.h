//
//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright (C) 2011 Nat!, Mulle kybernetiK. All rights reserved.
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
#ifndef mulle__pointermap_struct_h__
#define mulle__pointermap_struct_h__


#include "include.h"

#include "mulle-pointerpair.h"

#include <stdarg.h>


#define MULLE__POINTERMAP_FILL_SHIFT    2
#define MULLE__POINTERMAP_MIN_SIZE      (1 << MULLE__POINTERMAP_FILL_SHIFT)
#define MULLE__POINTERMAP_INITIAL_SIZE  4

// A mulle__pointermap is like a mulle__pointerset with the corresponding
// values appended. So if the set is [ 0, 1, 2 ] the map for { 0, a }
// { 1, b }, { 2, c} is stored as [ 0, 1, 2 ] [ a, b, c]. The value is
// located at offset +_size.
//
// This is a stripped down version of mulle__map without callbacks
// So you just map unique pointers to each other. You can't map
// mulle_not_a_pointer to anything.
//
#define MULLE__POINTERMAP_BASE   \
   void           **_storage;    \
   unsigned int   _count;        \
   unsigned int   _size


struct mulle__pointermap
{
   MULLE__POINTERMAP_BASE;
};


#define MULLE__POINTERMAP_INIT   { 0, 0, 0 }


MULLE__CONTAINER_GLOBAL
struct mulle_container_keyvaluecallback  mulle__pointermap_keyvaluecallback;


// assume we have _size 4
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 or 2 (leave odd alone) for better insert perfomance
//
// (?)
static inline unsigned int   mulle__pointermap_hash_for_size( uintptr_t  hash,
                                                              unsigned int _size)
{
   assert( _size >= 2);

   //
   // so if _size is pow2, _size-1 is the mask
   // now sub one again to kill lowest bit (which will be set)
   // if _size is 0, this doesn't work though
   //
   return( hash & (_size - 2)); // kill lowest bit (it will be)
}


#pragma mark - setup and takedown


MULLE__CONTAINER_GLOBAL
struct mulle__pointermap   *mulle__pointermap_create( unsigned int capacity,
                                                      size_t extra,
                                                      struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_destroy( struct mulle__pointermap *map,
                                   struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_init( struct mulle__pointermap *map,
                                unsigned int capacity,
                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_done( struct mulle__pointermap *map,
                                struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_reset( struct mulle__pointermap *map,
                                 struct mulle_allocator *allocator);


#pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline int   _mulle__pointermap_is_full( struct mulle__pointermap *map)
{
   unsigned int    _size;

   _size = map->_size;
   _size = (_size - (_size >> MULLE__POINTERMAP_FILL_SHIFT));  // full when 75% occupied
   return( map->_count >= _size);
}

static inline int   mulle__pointermap_is_full( struct mulle__pointermap *map)
{
   return( map ? _mulle__pointermap_is_full( map) : 1);
}


MULLE_C_NONNULL_FIRST
static inline int   _mulle__pointermap_is_sparse( struct mulle__pointermap *map)
{
   unsigned int    _size;

   _size = map->_size / 2;
   _size = (_size - (_size >> MULLE__POINTERMAP_FILL_SHIFT));  // sparse if 50% of it wouldn't be full
   return( map->_count < _size);
}


static inline int   mulle__pointermap_is_sparse( struct mulle__pointermap *map)
{
   return( map ? _mulle__pointermap_is_sparse( map) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__pointermap_get_count( struct mulle__pointermap *map)
{
   return( map->_count);
}

static inline unsigned int   mulle__pointermap_get_count( struct mulle__pointermap *map)
{
   return( map ? _mulle__pointermap_get_count( map) : 0);
}


// _size for key really
MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__pointermap_get_size( struct mulle__pointermap *map)
{
   return( map->_size);
}

static inline unsigned int   mulle__pointermap_get_size( struct mulle__pointermap *map)
{
   return( map ? _mulle__pointermap_get_size( map) : 0);
}


#pragma mark - operations

//
// call this after remove operations, to make enumerations quicker and
// conserve memory
//
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void  _mulle__pointermap_shrink_if_needed( struct mulle__pointermap *map,
                                           struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   *_mulle__pointermap_get( struct mulle__pointermap *map,
                                void *key);


// returns NULL because that's the notfound *value*!
static inline void   *mulle__pointermap_get( struct mulle__pointermap *map,
                                             void *key)
{
   if( ! map)
      return( NULL);
   return( _mulle__pointermap_get( map, key));
}


// check for mulle_pointerpair_is_invalid if found or not
MULLE__CONTAINER_GLOBAL
struct mulle_pointerpair
   mulle__pointermap_find_by_value( struct mulle__pointermap *map, void *value);


// Experimental!
// You need to provide zeroed space on the initial call for the return value.
// Afterwards you keep it empty.
// The intended use is for iterating over and deleting contents. This could be
// fairly efficient, but its untested.
//
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
struct mulle_pointerpair   *_mulle__pointermap_get_any_pair( struct mulle__pointermap *map,
                                                             struct mulle_pointerpair *space);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_insert_values_for_keysv( struct mulle__pointermap *map,
                                                   void *firstvalue,
                                                   void *firstkey,
                                                   va_list args,
                                                   struct mulle_allocator *allocator);


# pragma mark - copy

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
int   _mulle__pointermap_copy_items( struct mulle__pointermap *dst,
                                     struct mulle__pointermap *src,
                                     struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle__pointermap   *_mulle__pointermap_copy( struct mulle__pointermap *set,
                                                     struct mulle_allocator *allocator);


# pragma mark - debugging

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
char   *_mulle__pointermap_describe( struct mulle__pointermap *set,
                                     struct mulle_allocator *allocator);


# pragma mark - enumeration

#define MULLE__POINTERMAPENUMERATOR_BASE  \
   void           **_curr;                \
   unsigned int   _left;                  \
   unsigned int   _offset


struct mulle__pointermapenumerator
{
   MULLE__POINTERMAPENUMERATOR_BASE;
};


static inline struct mulle__pointermapenumerator
   _mulle__pointermap_enumerate( struct mulle__pointermap *map)
{
   struct mulle__pointermapenumerator   rover;

   assert( map);
   rover._left   = map->_count;
   rover._curr   = map->_storage;
   rover._offset = _mulle__pointermap_get_size( map);

   return( rover);
}


static inline struct mulle__pointermapenumerator
   mulle__pointermap_enumerate( struct mulle__pointermap *map)
{
   struct mulle__pointermapenumerator   rover;

   if( map)
      return( _mulle__pointermap_enumerate( map));

   rover._left = 0;
   return( rover);
}


//
// the map mustn't have too much free space for this to be reasonably
// efficient, therefore it's important to shrink after lots of removes
//
MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointermapenumerator_next_pair( struct mulle__pointermapenumerator *rover,
                                           struct mulle_pointerpair *pair)
{
   void   **p;

   if( ! rover->_left)
   {
      if( pair)
         *pair = mulle_pointerpair_make_invalid();
      return( 0);
   }

   rover->_left--;
   for(;;)
   {
      p = rover->_curr++;
      if( *p != mulle_not_a_pointer)
      {
         if( pair)
         {
            pair->key   = *p;
            pair->value = p[ rover->_offset];
         }
         return( 1);
      }
   }
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointermapenumerator_next( struct mulle__pointermapenumerator *rover,
                                      void **key,
                                      void **value)
{
   void   **p;

   if( ! rover->_left)
   {
      if( key)
         *key = NULL;
      if( value)
         *value = 0;
      return( 0);
   }

   rover->_left--;
   for(;;)
   {
      p = rover->_curr++;
      if( *p != mulle_not_a_pointer)
      {
         if( key)
            *key = *p;
         if( value)
            *value = p[ rover->_offset];
         return( 1);
      }
   }
}



static inline void
   mulle__pointermapenumerator_done( struct mulle__pointermapenumerator *rover)
{
}


#endif
