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

#include "mulle--pointermap.h"

#include "mulle-container-callback.h"
#include "mulle-pointerpair.h"
#include <stdarg.h>
#include <string.h>


// NSMapTable/NSDictionary/NSMutableDictionary
//
// this is your traditional key value table
// for CPU access, it oughta be better to have separate key and value storages,
// so that the keys are all bunched together.
// That didn't really show up in benchmarks though, actually it seemed
// a smidgen slower (for very small dictionaries)
//
#define MULLE__MAP_BASE    MULLE__POINTERMAP_BASE


struct mulle__map
{
   MULLE__MAP_BASE;
};


#define mulle__mapenumerator_empty  \
   ((struct mulle__mapenumerator) { 0 })


#pragma mark - setup and takedown


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_THIRD
struct mulle__map   *_mulle__map_create( unsigned int capacity,
                                         size_t extra,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__map_destroy( struct mulle__map *map,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__map_init( struct mulle__map *map,
                         unsigned int capacity,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST //_SECOND
void   _mulle__map_done( struct mulle__map *map,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST // _SECOND
void   _mulle__map_reset( struct mulle__map *map,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator);

#pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline int   _mulle__map_is_full( struct mulle__map *map)
{
   return( _mulle__pointermap_is_full( (struct mulle__pointermap *) map));
}

static inline int   mulle__map_is_full( struct mulle__map *map)
{
   return( mulle__pointermap_is_full( (struct mulle__pointermap *) map));
}


MULLE_C_NONNULL_FIRST
static inline int   _mulle__map_is_sparse( struct mulle__map *map)
{
   return( _mulle__pointermap_is_sparse( (struct mulle__pointermap *) map));
}

static inline int   mulle__map_is_sparse( struct mulle__map *map)
{
   return( mulle__pointermap_is_sparse( (struct mulle__pointermap *) map));
}



MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__map_get_count( struct mulle__map *map)
{
   return( _mulle__pointermap_get_count( (struct mulle__pointermap *) map));
}

static inline unsigned int   mulle__map_get_count( struct mulle__map *map)
{
   return( mulle__pointermap_get_count( (struct mulle__pointermap *) map));
}


// _size for key really
MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__map_get_size( struct mulle__map *map)
{
   return( _mulle__pointermap_get_size( (struct mulle__pointermap *) map));
}

static inline unsigned int   mulle__map_get_size( struct mulle__map *map)
{
   return( mulle__pointermap_get_size( (struct mulle__pointermap *) map));
}


#pragma mark - operations

//
// Returns the value for the key, will return NULL if no value is found.
// This function will *not* do a quick pointer-equality check, which is often a
// major speedup, but can also be a slowdown, if the keys are not prone
// to pointer equality.
//
MULLE_C_NONNULL_FIRST_THIRD
static inline
void   *_mulle__map__get( struct mulle__map *map,
                          void *key,
                          struct mulle_container_keyvaluecallback *callback)
{
   return( _mulle__pointermap__get_generic( (struct mulle__pointermap *) map,
                                            key,
                                            callback));
}

//
// This function will *not* do a quick pointer-equality check, which is often a
// major speedup, but can also be a slowdown, if the keys are not prone
// to pointer equality.
//
MULLE_C_NONNULL_FIRST_THIRD_FOURTH
static inline struct mulle_pointerpair   *
   _mulle__map__get_pair( struct mulle__map *map,
                          void *key,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_pointerpair *space)
{
   return( _mulle__pointermap__get_pair_generic( (struct mulle__pointermap *) map,
                                                  key,
                                                  callback,
                                                  space));
}


//
// returns the value for the key, will return NULL if no value is found.
//
MULLE_C_NONNULL_FIRST_FOURTH
static inline void   *
   _mulle__map__get_knownhash( struct mulle__map *map,
                               void *key,
                               uintptr_t hash,
                               struct mulle_container_keyvaluecallback *callback)
{
   return( _mulle__pointermap__get_generic_knownhash( (struct mulle__pointermap *) map,
                                                      key,
                                                      hash,
                                                      callback));
}


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline struct mulle_pointerpair   *
   _mulle__map_get_any_pair( struct mulle__map *map,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_pointerpair *space)
{
   return( _mulle__pointermap_get_any_pair_generic( (struct mulle__pointermap *) map, callback, space));
}


// this will do a little cursor pointer equality check before actually
// doing the hash lookup
MULLE_C_NONNULL_FIRST_THIRD
static inline void   *
   _mulle__map_get( struct mulle__map *map,
                    void *key,
                    struct mulle_container_keyvaluecallback *callback)
{
   return( _mulle__pointermap_get_generic( (struct mulle__pointermap *) map, key, callback));
}


MULLE_C_NONNULL_FIRST_THIRD_FOURTH
static inline struct mulle_pointerpair   *
   _mulle__map_get_pair( struct mulle__map *map,
                         void *key,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_pointerpair *space)
{
   return( _mulle__pointermap_get_pair_generic( (struct mulle__pointermap *) map,
                                                 key,
                                                 callback,
                                                 space));
}


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline void
   _mulle__map_set_pair( struct mulle__map *map,
                         struct mulle_pointerpair *pair,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
{
   _mulle__pointermap_set_pair_generic( (struct mulle__pointermap *) map,
                                         pair,
                                         callback,
                                         allocator);
}


static inline void
   mulle__map_set_pair( struct mulle__map *map,
                        struct mulle_pointerpair *pair,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
   if( map)
      _mulle__map_set_pair( map, pair, callback, allocator);
}


MULLE_C_NONNULL_FIRST_FOURTH
static inline void
   _mulle__map_set( struct mulle__map *map,
                    void *key,
                    void *value,
                    struct mulle_container_keyvaluecallback *callback,
                    struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   _mulle__map_set_pair( map, &pair, callback, allocator);
}


static inline void
   mulle__map_set( struct mulle__map *map,
                   void *key,
                   void *value,
                   struct mulle_container_keyvaluecallback *callback,
                   struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   mulle__map_set_pair( map, &pair, callback, allocator);
}



MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline void    *
   _mulle__map_insert_pair( struct mulle__map *map,
                            struct mulle_pointerpair *pair,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_insert_pair_generic( (struct mulle__pointermap *) map,
                                                   pair,
                                                   callback,
                                                   allocator));
}


static inline void    *
   mulle__map_insert_pair( struct mulle__map *map,
                           struct mulle_pointerpair *pair,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   if( ! map)
      return( NULL);
   return( _mulle__map_insert_pair( map, pair, callback, allocator));
}


MULLE_C_NONNULL_FIRST_FOURTH
static inline void
   _mulle__map_insert( struct mulle__map *map,
                       void *key,
                       void *value,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   _mulle__map_insert_pair( map, &pair, callback, allocator);
}


static inline void
   mulle__map_insert( struct mulle__map *map,
                      void *key,
                      void *value,
                      struct mulle_container_keyvaluecallback *callback,
                      struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   mulle__map_insert_pair( map, &pair, callback, allocator);
}


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline int
   _mulle__map_remove( struct mulle__map *map,
                       void *key,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_remove_generic( (struct mulle__pointermap *) map,
                                              key,
                                              callback,
                                              allocator));
}


static inline int
   mulle__map_remove( struct mulle__map *map,
                      void *key,
                      struct mulle_container_keyvaluecallback *callback,
                      struct mulle_allocator *allocator)
{
   if( ! map)
      return( 0);
   return( _mulle__map_remove( map, key, callback, allocator));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__map_shrink( struct mulle__map *map,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
{
   _mulle__pointermap_shrink_generic( (struct mulle__pointermap *)  map,
                                      callback,
                                      allocator);
}


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline int
   _mulle__map_copy_items( struct mulle__map *dst,
                           struct mulle__map *src,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_copy_items_generic( (struct mulle__pointermap *) dst,
                                                  (struct mulle__pointermap *) src,
                                                  callback,
                                                  allocator));
}


// Note: this function is not possible, as we have no equality checker for
//       the values. Blowing up the API to add yet another callback feels
//       awkward.
//
// MULLE_C_NONNULL_FIRST_SECOND_THIRD
// int    _mulle__map_is_equal( struct mulle__map *map,
//                              struct mulle__map *other,
//                              struct mulle_container_keyvaluecallback *callback)
//


//
// call this before enumerations operations to reduce the number of holes
//
MULLE_C_NONNULL_FIRST_SECOND
static inline void
   _mulle__map_shrink_if_needed( struct mulle__map *map,
                                 struct mulle_container_keyvaluecallback *callback,
                                 struct mulle_allocator *allocator)
{
   if( _mulle__map_is_sparse( map))
      _mulle__map_shrink( map, callback, allocator);
}


//
static inline void
   mulle__map_shrink_if_needed( struct mulle__map *map,
                                struct mulle_container_keyvaluecallback *callback,
                                struct mulle_allocator *allocator)
{
   if( map)
      _mulle__map_shrink_if_needed( map, callback, allocator);
}

//
// Experimental!
// You need to provide zeroed space on the initial call for the return value.
// Afterwards you keep it empty.
// The intended use is for iterating over and deleting contents. This could be
// fairly efficient, but its untested.
//

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_FIFTH
void   _mulle__map_insert_values_for_keysv( struct mulle__map *map,
                                            void *firstvalue,
                                            void *firstkey,
                                            va_list args,
                                            struct mulle_container_keyvaluecallback *callback,
                                            struct mulle_allocator *allocator);


# pragma mark - copy

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
struct mulle__map   *_mulle__map_copy( struct mulle__map *map,
                                       struct mulle_container_keyvaluecallback *callback,
                                       struct mulle_allocator *allocator);


# pragma mark - debugging

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_SECOND
char   *_mulle__map_describe( struct mulle__map *map,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator);


MULLE_C_NONNULL_SECOND
static inline char   *
   mulle__map_describe( struct mulle__map *map,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
   if( ! map)
      return( mulle_allocator_strdup( allocator, "NULL"));
   return( _mulle__map_describe( map, callback, allocator));
}


//
// collisions is the amount of additional compares needed for misplaced keys
//
MULLE_C_NONNULL_FIRST_SECOND
static inline unsigned int
   _mulle__map_count_collisions( struct mulle__map *set,
                                 struct mulle_container_keyvaluecallback *callback,
                                 unsigned int *perfects)
{
   return( _mulle__pointermap_count_collisions_generic( (struct mulle__pointermap *) set,
                                                         callback,
                                                         perfects));
}


# pragma mark - enumeration


#define MULLE__MAPENUMERATOR_BASE  MULLE__GENERICPOINTERMAPENUMERATOR_BASE


struct mulle__mapenumerator
{
   MULLE__MAPENUMERATOR_BASE;
};



MULLE_C_NONNULL_SECOND
static inline struct mulle__mapenumerator
   mulle__map_enumerate( struct mulle__map *map,
                         struct mulle_container_keyvaluecallback *callback)
{
   struct mulle__mapenumerator                 rover;
   struct mulle__genericpointermapenumerator   tmp;

   tmp = mulle__pointermap_enumerate_generic( (struct mulle__pointermap *) map, callback);
   memcpy( &rover, &tmp, sizeof( struct mulle__genericpointermapenumerator));
   return( rover);
}


//
// the map mustn't have too much free space for this to be reasonably
// efficient, therefore it's important to shrink after lots of removes
//
MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair   *
   _mulle__mapenumerator_next_pair( struct mulle__mapenumerator *rover)
{
   return( _mulle__genericpointermapenumerator_next_pair( (struct mulle__genericpointermapenumerator *) rover));
}


static inline struct mulle_pointerpair   *
   mulle__mapenumerator_next_pair( struct mulle__mapenumerator *rover)
{
   return( mulle__genericpointermapenumerator_next_pair( (struct mulle__genericpointermapenumerator *) rover));
}



MULLE_C_NONNULL_FIRST
static inline int
   _mulle__mapenumerator_next( struct mulle__mapenumerator *rover,
                               void **key,
                               void **value)
{
   return( _mulle__genericpointermapenumerator_next( (struct mulle__genericpointermapenumerator *) rover, key, value));
}


static inline int
   mulle__mapenumerator_next( struct mulle__mapenumerator *rover,
                              void **key,
                              void **value)
{
   return( mulle__genericpointermapenumerator_next( (struct mulle__genericpointermapenumerator *) rover, key, value));
}



MULLE_C_NONNULL_FIRST
static inline void
   _mulle__mapenumerator_done( struct mulle__mapenumerator *rover)
{
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

#define MULLE__MAPTINYENUMERATOR_BASE  \
   void           **_curr;             \
   unsigned int   _left;               \
   unsigned int   _offset


struct mulle__maptinyenumerator
{
   MULLE__MAPTINYENUMERATOR_BASE;
};


static inline struct mulle__maptinyenumerator
   _mulle__map_tinyenumerate_nil( struct mulle__map *map)
{
   struct mulle__maptinyenumerator   rover;

   assert( map);
   rover._left   = map->_count;
   rover._curr   = map->_storage;
   rover._offset = _mulle__map_get_size( map);

   return( rover);
}



static inline struct mulle__maptinyenumerator
   mulle__map_tinyenumerate_nil( struct mulle__map *map)
{
   struct mulle__maptinyenumerator   rover;

   if( map)
      return( _mulle__map_tinyenumerate_nil( map));

   rover._left = 0;
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
   {
      if( key)
         *key   = NULL;
      if( value)
         *value = NULL;
      return( 0);
   }

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



MULLE_C_NONNULL_FIRST
static inline void
   _mulle__maptinyenumerator_done( struct mulle__maptinyenumerator *rover)
{
}


static inline void
   mulle__maptinyenumerator_done( struct mulle__maptinyenumerator *rover)
{
}


#endif
