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
#ifndef _MULLE_MAP_H___
#define _MULLE_MAP_H___


#include "mulle-container-callback.h"
#include "mulle-pointerpair.h"
#include <stdarg.h>

// counts are unsigned int, the result multiplied by sizeof is size_t */

// NSMapTable/NSDictionary/NSMutableDictionary
//
// this is your traditional key value table
// for CPU access, it oughta be better to have separate key and value storages,
// so that the keys are all bunched together.
// That didn't really show up in benchmarks though, actually it seemed
// a smidgen slower (for very small dictionaries)
//
#define _MULLE_MAP_BASE                   \
   void                       **_storage; \
   unsigned int               _count;     \
   unsigned int               _size

struct _mulle_map
{
   _MULLE_MAP_BASE;
};


#define _MULLE_MAPENUMERATOR_BASE        \
   struct mulle_pointerpair   space;     \
   void                       **_curr;   \
   unsigned int               _left;     \
   unsigned int               _offset;   \
   void                       *_notakey


struct _mulle_mapenumerator
{
   _MULLE_MAPENUMERATOR_BASE;
};



#define _MULLE_MAPTINYENUMERATOR_BASE    \
   void                       **_curr;   \
   unsigned int               _left;     \
   unsigned int               _offset


struct _mulle_maptinyenumerator
{
   _MULLE_MAPTINYENUMERATOR_BASE;
};


#pragma mark -
#pragma mark setup and takedown


struct _mulle_map   *_mulle_map_create( unsigned int capacity,
                                        size_t extra,
                                        struct mulle_container_keyvaluecallback *callback,
                                        struct mulle_allocator *allocator);

void   _mulle_map_destroy( struct _mulle_map *map,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator);
void   _mulle_map_init( struct _mulle_map *map,
                        unsigned int capacity,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator);

void   _mulle_map_done( struct _mulle_map *map,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator);


void   _mulle_map_reset( struct _mulle_map *map,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);

#pragma mark -
#pragma mark petty accessors

static inline int   _mulle_map_is_full( struct _mulle_map *map)
{
   unsigned int    size;

   size = map->_size;
   size = (size - (size >> 2));  // full when 75% occupied
   return( map->_count >= size);
}


static inline unsigned int   _mulle_map_get_count( struct _mulle_map *map)
{
   return( map->_count);
}


// size for key really
static inline unsigned int   _mulle_map_get_size( struct _mulle_map *map)
{
   return( map->_size);
}


#pragma mark -
#pragma mark operations


void   _mulle_map_set( struct _mulle_map *map,
                       struct mulle_pointerpair *pair,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator);

void    *_mulle_map_insert( struct _mulle_map *map,
                            struct mulle_pointerpair *pair,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator);

void   *_mulle_map_insert_known_absent( struct _mulle_map *map,
                                        struct mulle_pointerpair *pair,
                                        struct mulle_container_keyvaluecallback *callback,
                                        struct mulle_allocator *allocator);

int   _mulle_map_remove_with_hash( struct _mulle_map *map,
                                   void *key,
                                   unsigned int  hash,
                                   struct mulle_container_keyvaluecallback *callback,
                                   struct mulle_allocator *allocator);

int   _mulle_map_remove( struct _mulle_map *map,
                         void *key,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);

void   *_mulle_map_get_with_hash( struct _mulle_map *map,
                                  void *key,
                                  unsigned int  hash,
                                  struct mulle_container_keyvaluecallback *callback);

void   *_mulle_map_get( struct _mulle_map *map,
                        void *p,
                        struct mulle_container_keyvaluecallback *callback);

void   _mulle_map_insert_values_for_keysv( struct _mulle_map *map,
                                           void *firstvalue,
                                           void *firstkey,
                                           va_list args,
                                           struct mulle_container_keyvaluecallback *callback,
                                           struct mulle_allocator *allocator);


# pragma mark -
# pragma mark copy

int   _mulle_map_copy_items( struct _mulle_map *dst,
                             struct _mulle_map *src,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator);

struct _mulle_map   *_mulle_map_copy( struct _mulle_map *set,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator);


# pragma mark -
# pragma mark debugging

char   *_mulle_map_describe( struct _mulle_map *set,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator);

# pragma mark -
# pragma mark enumeration


static inline struct _mulle_mapenumerator
   _mulle_map_enumerate( struct _mulle_map *map,
                         struct mulle_container_keyvaluecallback *callback) mulle_nonnull_second;

static inline struct _mulle_mapenumerator
   _mulle_map_enumerate( struct _mulle_map *map,
                         struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_mapenumerator   rover;

   if( map)
   {
      rover._left     = map->_count;
      rover._curr     = map->_storage;
      rover._offset   = _mulle_map_get_size( map);
      rover._notakey  = callback->keycallback.notakey;
   }
   else
      rover._left     = 0;

   return( rover);
}


static inline struct mulle_pointerpair   *
   _mulle_mapenumerator_next( struct _mulle_mapenumerator *rover)
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
   _mulle_mapenumerator_done( struct _mulle_mapenumerator *rover)
{
}


/*
 * a different and smaller interface, where you have to pass in space
 * to store the enumeration result each iteration and where notakey
 * must be NULL
 */
static inline struct _mulle_maptinyenumerator
   _mulle_map_tinyenumerate_nil( struct _mulle_map *map)
{
   struct _mulle_maptinyenumerator   rover;

   if( map)
   {
      rover._left   = map->_count;
      rover._curr   = map->_storage;
      rover._offset = _mulle_map_get_size( map);
   }
   else
      rover._left   = 0;

   return( rover);
}


static inline int
   _mulle_maptinyenumerator_next( struct _mulle_maptinyenumerator *rover,
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
   _mulle_maptinyenumerator_done( struct _mulle_maptinyenumerator *rover)
{
}


#endif
