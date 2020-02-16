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
#ifndef mulle__map__h__
#define mulle__map__h__


#include "mulle-container-callback.h"
#include "mulle-pointerpair.h"
#include <stdarg.h>


#define MULLE_MAP_FILL_SHIFT    2
#define MULLE_MAP_MIN_SIZE      (1 << MULLE_MAP_FILL_SHIFT)

// counts are unsigned int, the result multiplied by sizeof is size_t */

// NSMapTable/NSDictionary/NSMutableDictionary
//
// this is your traditional key value table
// for CPU access, it oughta be better to have separate key and value storages,
// so that the keys are all bunched together.
// That didn't really show up in benchmarks though, actually it seemed
// a smidgen slower (for very small dictionaries)
//
#define MULLE__MAP_BASE                   \
   void                       **_storage; \
   unsigned int               _count;     \
   unsigned int               _size

struct mulle__map
{
   MULLE__MAP_BASE;
};


#define MULLE__MAPENUMERATOR_BASE        \
   struct mulle_pointerpair   space;     \
   void                       **_curr;   \
   unsigned int               _left;     \
   unsigned int               _offset;   \
   void                       *_notakey


struct mulle__mapenumerator
{
   MULLE__MAPENUMERATOR_BASE;
};



#define MULLE__MAPTINYENUMERATOR_BASE    \
   void                       **_curr;   \
   unsigned int               _left;     \
   unsigned int               _offset


struct mulle__maptinyenumerator
{
   MULLE__MAPTINYENUMERATOR_BASE;
};


#pragma mark -
#pragma mark setup and takedown


MULLE_C_NONNULL_THIRD
struct mulle__map   *_mulle__map_create( unsigned int capacity,
                                         size_t extra,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__map_destroy( struct mulle__map *map,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__map_init( struct mulle__map *map,
                         unsigned int capacity,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__map_done( struct mulle__map *map,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__map_reset( struct mulle__map *map,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator);

#pragma mark -
#pragma mark petty accessors

MULLE_C_NONNULL_FIRST
static inline int   _mulle__map_is_full( struct mulle__map *map)
{
   unsigned int    size;

   size = map->_size;
   size = (size - (size >> MULLE_MAP_FILL_SHIFT));  // full when 75% occupied
   return( map->_count >= size);
}


MULLE_C_NONNULL_FIRST
static inline int   _mulle__map_is_sparse( struct mulle__map *map)
{
   unsigned int    size;

   size = map->_size / 2;
   size = (size - (size >> MULLE_MAP_FILL_SHIFT));  // sparse if 50% of it wouldn't be full
   return( map->_count < size);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__map_get_count( struct mulle__map *map)
{
   return( map->_count);
}


// size for key really
MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__map_get_size( struct mulle__map *map)
{
   return( map->_size);
}


#pragma mark -
#pragma mark operations


MULLE_C_NONNULL_FIRST_SECOND_THIRD
void   _mulle__map_set( struct mulle__map *map,
                        struct mulle_pointerpair *pair,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND_THIRD
void    *_mulle__map_insert( struct mulle__map *map,
                             struct mulle_pointerpair *pair,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND_THIRD
void   *_mulle__map_insert_known_absent( struct mulle__map *map,
                                         struct mulle_pointerpair *pair,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_FOURTH
int   _mulle__map_remove_with_hash( struct mulle__map *map,
                                    void *key,
                                    unsigned int  hash,
                                    struct mulle_container_keyvaluecallback *callback,
                                    struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_THIRD
int   _mulle__map_remove( struct mulle__map *map,
                          void *key,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator);


//
// call this before enumerations operations to reduce the number of holes
//
MULLE_C_NONNULL_FIRST_SECOND
static inline void  _mulle__map_shrink_if_needed( struct mulle__map *map,
                                                  struct mulle_container_keyvaluecallback *callback,
                                                  struct mulle_allocator *allocator)
{
   void  _mulle__map_shrink( struct mulle__map *map,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator);

   if( _mulle__map_is_sparse( map))
      _mulle__map_shrink( map, callback, allocator);
}


MULLE_C_NONNULL_FIRST_FOURTH
void   *_mulle__map_get_with_hash( struct mulle__map *map,
                                   void *key,
                                   unsigned int  hash,
                                   struct mulle_container_keyvaluecallback *callback);

MULLE_C_NONNULL_FIRST_THIRD
void   *_mulle__map_get( struct mulle__map *map,
                         void *p,
                         struct mulle_container_keyvaluecallback *callback);

// Experimental!
// You need to provide zeroed space on the initial call for the return value.
// Afterwards you keep it empty.
// The intended use is for iterating over and deleting contents. This could be
// fairly efficient, but its untested.
//
MULLE_C_NONNULL_FIRST_SECOND_THIRD
struct mulle_pointerpair   *_mulle__map_get_any_pair( struct mulle__map *map,
                                                      struct mulle_container_keyvaluecallback *callback,
                                                      struct mulle_pointerpair *space);

MULLE_C_NONNULL_FIRST_FIFTH
void   _mulle__map_insert_values_for_keysv( struct mulle__map *map,
                                            void *firstvalue,
                                            void *firstkey,
                                            va_list args,
                                            struct mulle_container_keyvaluecallback *callback,
                                            struct mulle_allocator *allocator);


# pragma mark -
# pragma mark copy

MULLE_C_NONNULL_FIRST_SECOND_THIRD
int   _mulle__map_copy_items( struct mulle__map *dst,
                              struct mulle__map *src,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
struct mulle__map   *_mulle__map_copy( struct mulle__map *map,
                                       struct mulle_container_keyvaluecallback *callback,
                                       struct mulle_allocator *allocator);


# pragma mark -
# pragma mark debugging

MULLE_C_NONNULL_FIRST_SECOND
char   *_mulle__map_describe( struct mulle__map *map,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator);

# pragma mark -
# pragma mark enumeration


MULLE_C_NONNULL_SECOND
static inline struct mulle__mapenumerator
   mulle__map_enumerate( struct mulle__map *map,
                         struct mulle_container_keyvaluecallback *callback)
{
   struct mulle__mapenumerator   rover;

   if( map)
   {
      rover._left     = map->_count;
      rover._curr     = map->_storage;
      rover._offset   = _mulle__map_get_size( map);
      rover._notakey  = callback->keycallback.notakey;
   }
   else
      rover._left     = 0;

   return( rover);
}


//
// the map mustn't have too much free space for this to be reasonably
// efficient, therefore it's important to shrink after lots of removes
//
MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair   *
   _mulle__mapenumerator_next( struct mulle__mapenumerator *rover)
{
   void   **p;
   void   *key;

   if( ! rover->_left)
      return( 0);

   rover->_left--;
   for(;;)
   {
      p   = rover->_curr++;
      key = *p;
      if( key != rover->_notakey)
      {
         rover->space._key   = key;
         rover->space._value = p[ rover->_offset];
         return( &rover->space);
      }
   }
}



static inline void
   mulle__mapenumerator_done( struct mulle__mapenumerator *rover)
{
}


/*
 * a different and smaller interface, where you have to pass in space
 * to store the enumeration result each iteration and where notakey
 * must be NULL
 */
static inline struct mulle__maptinyenumerator
   mulle__map_tinyenumerate_nil( struct mulle__map *map)
{
   struct mulle__maptinyenumerator   rover;

   if( map)
   {
      rover._left   = map->_count;
      rover._curr   = map->_storage;
      rover._offset = _mulle__map_get_size( map);
   }
   else
      rover._left   = 0;

   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__maptinyenumerator_next( struct mulle__maptinyenumerator *rover,
                                   void **key,
                                   void **value)
{
   void   **p;

   if( ! rover->_left)
      return( 0);

   rover->_left--;
   for(;;)
   {
      p = rover->_curr++;
      if( *p)
      {
         if( key)
            *key   = *p;
         if( value)
            *value = p[ rover->_offset];
         return( 1);
      }
   }
}


static inline void
   mulle__maptinyenumerator_done( struct mulle__maptinyenumerator *rover)
{
}


#endif
