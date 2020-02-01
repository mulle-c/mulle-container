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
#ifndef _MULLE_POINTERMAP_H___
#define _MULLE_POINTERMAP_H___


#include "mulle-pointerpair.h"
#include <stdarg.h>


#define _MULLE_POINTERMAP_FILL_SHIFT    2
#define _MULLE_POINTERMAP_MIN_SIZE      (1 << _MULLE_POINTERMAP_FILL_SHIFT)

//
// This is a stripped down version of _mulle_map without callbacks
// So you just map unique pointers to each other. You can't map
// NULL to anything. It contains no allocator, so is prefixed (somewhat
// inconveniently) with a '_'
//
#define _MULLE_POINTERMAP_BASE            \
   void                       **_storage; \
   unsigned int               _count;     \
   unsigned int               _size

struct _mulle_pointermap
{
   _MULLE_POINTERMAP_BASE;
};


#define _MULLE_POINTERMAPENUMERATOR_BASE  \
   struct mulle_pointerpair   space;      \
   void                       **_curr;    \
   unsigned int               _left;      \
   unsigned int               _offset


struct _mulle_pointermapenumerator
{
   _MULLE_POINTERMAPENUMERATOR_BASE;
};


#pragma mark -
#pragma mark setup and takedown


struct _mulle_pointermap   *_mulle_pointermap_create( unsigned int capacity,
                                                      size_t extra,
                                                      struct mulle_allocator *allocator);

void   _mulle_pointermap_destroy( struct _mulle_pointermap *map,
                                  struct mulle_allocator *allocator);
void   _mulle_pointermap_init( struct _mulle_pointermap *map,
                               unsigned int capacity,
                               struct mulle_allocator *allocator);

void   _mulle_pointermap_done( struct _mulle_pointermap *map,
                               struct mulle_allocator *allocator);

void   _mulle_pointermap_reset( struct _mulle_pointermap *map,
                                struct mulle_allocator *allocator);

#pragma mark -
#pragma mark petty accessors

static inline int   _mulle_pointermap_is_full( struct _mulle_pointermap *map)
{
   unsigned int    size;

   size = map->_size;
   size = (size - (size >> _MULLE_POINTERMAP_FILL_SHIFT));  // full when 75% occupied
   return( map->_count >= size);
}


static inline int   _mulle_pointermap_is_sparse( struct _mulle_pointermap *map)
{
   unsigned int    size;

   size = map->_size / 2;
   size = (size - (size >> _MULLE_POINTERMAP_FILL_SHIFT));  // sparse if 50% of it wouldn't be full
   return( map->_count < size);
}


static inline unsigned int   _mulle_pointermap_get_count( struct _mulle_pointermap *map)
{
   return( map->_count);
}


// size for key really
static inline unsigned int   _mulle_pointermap_get_size( struct _mulle_pointermap *map)
{
   return( map->_size);
}


#pragma mark -
#pragma mark operations


void   _mulle_pointermap_set_pair( struct _mulle_pointermap *map,
                                   struct mulle_pointerpair *pair,
                                   struct mulle_allocator *allocator);

static inline void   _mulle_pointermap_set( struct _mulle_pointermap *map,
                                            void *key,
                                            void *value,
                                            struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   if( ! map)
      return;

   pair._key   = key;
   pair._value = value;
   _mulle_pointermap_set_pair( map, &pair, allocator);
}


void    *_mulle_pointermap_insert_pair( struct _mulle_pointermap *map,
                                        struct mulle_pointerpair *pair,
                                        struct mulle_allocator *allocator);

void   *_mulle_pointermap_insert_pair_known_absent( struct _mulle_pointermap *map,
                                                    struct mulle_pointerpair *pair,
                                                    struct mulle_allocator *allocator);

int   _mulle_pointermap_remove( struct _mulle_pointermap *map,
                                void *key,
                                struct mulle_allocator *allocator);

//
// call this after remove operations, to make enumerations quicker and
// conserve memory
//
void  _mulle_pointermap_shrink_if_needed( struct _mulle_pointermap *map,
                                          struct mulle_allocator *allocator);

void   *_mulle_pointermap_get( struct _mulle_pointermap *map,
                               void *key);



// Experimental!
// You need to provide zeroed space on the initial call for the return value.
// Afterwards you keep it empty.
// The intended use is for iterating over and deleting contents. This could be
// fairly efficient, but its untested.
//
struct mulle_pointerpair   *_mulle_pointermap_get_any_pair( struct _mulle_pointermap *map,
                                                            struct mulle_pointerpair *space);

void   _mulle_pointermap_insert_values_for_keysv( struct _mulle_pointermap *map,
                                                  void *firstvalue,
                                                  void *firstkey,
                                                  va_list args,
                                                  struct mulle_allocator *allocator);


# pragma mark -
# pragma mark copy

int   _mulle_pointermap_copy_items( struct _mulle_pointermap *dst,
                                    struct _mulle_pointermap *src,
                                    struct mulle_allocator *allocator);

struct _mulle_pointermap   *_mulle_pointermap_copy( struct _mulle_pointermap *set,
                                                   struct mulle_allocator *allocator);


# pragma mark -
# pragma mark debugging

char   *_mulle_pointermap_describe( struct _mulle_pointermap *set,
                                    struct mulle_allocator *allocator);

# pragma mark -
# pragma mark enumeration


static inline struct _mulle_pointermapenumerator
   _mulle_pointermap_enumerate( struct _mulle_pointermap *map)
{
   struct _mulle_pointermapenumerator   rover;

   rover._left   = map->_count;
   rover._curr   = map->_storage;
   rover._offset = _mulle_pointermap_get_size( map);

   return( rover);
}


//
// the map mustn't have too much free space for this to be reasonably
// efficient, therefore it's important to shrink after lots of removes
//
static inline struct mulle_pointerpair   *
   _mulle_pointermapenumerator_next( struct _mulle_pointermapenumerator *rover)
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
   _mulle_pointermapenumerator_done( struct _mulle_pointermapenumerator *rover)
{
}


#endif
