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
#ifndef mulle__pointermap__h__
#define mulle__pointermap__h__


#include "mulle-pointerpair.h"
#include <stdarg.h>


#define MULLE_POINTERMAP_FILL_SHIFT    2
#define MULLE_POINTERMAP_MIN_SIZE      (1 << MULLE_POINTERMAP_FILL_SHIFT)

//
// This is a stripped down version of _mulle_map without callbacks
// So you just map unique pointers to each other. You can't map
// NULL to anything. It contains no allocator, so is prefixed (somewhat
// inconveniently) with a '_'
//
#define MULLE__POINTERMAP_BASE            \
   void                       **_storage; \
   unsigned int               _count;     \
   unsigned int               _size

struct mulle__pointermap
{
   MULLE__POINTERMAP_BASE;
};


#define MULLE__POINTERMAPENUMERATOR_BASE  \
   struct mulle_pointerpair   space;      \
   void                       **_curr;    \
   unsigned int               _left;      \
   unsigned int               _offset


struct mulle__pointermapenumerator
{
   MULLE__POINTERMAPENUMERATOR_BASE;
};


#pragma mark - setup and takedown


struct mulle__pointermap   *mulle__pointermap_create( unsigned int capacity,
                                                      size_t extra,
                                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_destroy( struct mulle__pointermap *map,
                                   struct mulle_allocator *allocator);
MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_init( struct mulle__pointermap *map,
                                unsigned int capacity,
                                struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_done( struct mulle__pointermap *map,
                                struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_reset( struct mulle__pointermap *map,
                                 struct mulle_allocator *allocator);

#pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline int   _mulle__pointermap_is_full( struct mulle__pointermap *map)
{
   unsigned int    size;

   size = map->_size;
   size = (size - (size >> MULLE_POINTERMAP_FILL_SHIFT));  // full when 75% occupied
   return( map->_count >= size);
}


MULLE_C_NONNULL_FIRST
static inline int   _mulle__pointermap_is_sparse( struct mulle__pointermap *map)
{
   unsigned int    size;

   size = map->_size / 2;
   size = (size - (size >> MULLE_POINTERMAP_FILL_SHIFT));  // sparse if 50% of it wouldn't be full
   return( map->_count < size);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__pointermap_get_count( struct mulle__pointermap *map)
{
   return( map->_count);
}


// size for key really
MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__pointermap_get_size( struct mulle__pointermap *map)
{
   return( map->_size);
}


#pragma mark - operations


MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__pointermap_set_pair( struct mulle__pointermap *map,
                                    struct mulle_pointerpair *pair,
                                    struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline void   _mulle__pointermap_set( struct mulle__pointermap *map,
                                             void *key,
                                             void *value,
                                             struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair._key   = key;
   pair._value = value;
   _mulle__pointermap_set_pair( map, &pair, allocator);
}


MULLE_C_NONNULL_FIRST_SECOND
void    *_mulle__pointermap_insert_pair( struct mulle__pointermap *map,
                                         struct mulle_pointerpair *pair,
                                         struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   *_mulle__pointermap_insert_pair_known_absent( struct mulle__pointermap *map,
                                                     struct mulle_pointerpair *pair,
                                                     struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
int   _mulle__pointermap_remove( struct mulle__pointermap *map,
                                 void *key,
                                 struct mulle_allocator *allocator);

//
// call this after remove operations, to make enumerations quicker and
// conserve memory
//
MULLE_C_NONNULL_FIRST
void  _mulle__pointermap_shrink_if_needed( struct mulle__pointermap *map,
                                           struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   *_mulle__pointermap_get( struct mulle__pointermap *map,
                                void *key);



// Experimental!
// You need to provide zeroed space on the initial call for the return value.
// Afterwards you keep it empty.
// The intended use is for iterating over and deleting contents. This could be
// fairly efficient, but its untested.
//
MULLE_C_NONNULL_FIRST_SECOND
struct mulle_pointerpair   *_mulle__pointermap_get_any_pair( struct mulle__pointermap *map,
                                                             struct mulle_pointerpair *space);

MULLE_C_NONNULL_FIRST
void   _mulle__pointermap_insert_values_for_keysv( struct mulle__pointermap *map,
                                                   void *firstvalue,
                                                   void *firstkey,
                                                   va_list args,
                                                   struct mulle_allocator *allocator);


# pragma mark -
# pragma mark copy

MULLE_C_NONNULL_FIRST_SECOND
int   _mulle__pointermap_copy_items( struct mulle__pointermap *dst,
                                     struct mulle__pointermap *src,
                                     struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
struct mulle__pointermap   *_mulle__pointermap_copy( struct mulle__pointermap *set,
                                                     struct mulle_allocator *allocator);


# pragma mark -
# pragma mark debugging

MULLE_C_NONNULL_FIRST
char   *_mulle__pointermap_describe( struct mulle__pointermap *set,
                                     struct mulle_allocator *allocator);

# pragma mark -
# pragma mark enumeration


static inline struct mulle__pointermapenumerator
   mulle__pointermap_enumerate( struct mulle__pointermap *map)
{
   struct mulle__pointermapenumerator   rover;

   if( map)
   {
      rover._left   = map->_count;
      rover._curr   = map->_storage;
      rover._offset = _mulle__pointermap_get_size( map);
   }
   else
      rover._left = 0;

   return( rover);
}


//
// the map mustn't have too much free space for this to be reasonably
// efficient, therefore it's important to shrink after lots of removes
//
MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair   *
   _mulle__pointermapenumerator_next( struct mulle__pointermapenumerator *rover)
{
   void   **p;
   void   *key;

   if( ! rover->_left)
      return( 0);

   rover->_left--;
   for(;;)
   {
      p = rover->_curr++;
      if( *p)
      {
         rover->space._key   = *p;
         rover->space._value = p[ rover->_offset];
         return( &rover->space);
      }
   }
}


static inline void
   mulle__pointermapenumerator_done( struct mulle__pointermapenumerator *rover)
{
}


#endif
