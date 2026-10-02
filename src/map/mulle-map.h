//
//  mulle-map.h
//  mulle-container
//
//  Copyright (c) 2018 Nat! - Mulle kybernetiK.
//  All rights reserved.
//
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
#ifndef mulle_map_h__
#define mulle_map_h__

#include "mulle--map.h"

#include <stdarg.h>
#include <string.h>

//
// just like struct mulle__map but has the callback embedded in
// its structure
//
//
// The public struct holds the internal one as a member (see mulle-array.h for
// why repeating the fields by macro is a strict-aliasing violation).
//
#define MULLE_MAP_BASE                                   \
   struct mulle__map                         base;       \
   struct mulle_container_keyvaluecallback   *callback;  \
   struct mulle_allocator                    *allocator


struct mulle_map
{
   MULLE_MAP_BASE;
};

#define MULLE_MAP_DATA( xcallback, xallocator) \
   ((struct mulle_map)                         \
   {                                           \
      .callback  = (xcallback),                \
      .allocator = (xallocator)                \
   })


#pragma mark - setup and teardown

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_SECOND
struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST_THIRD
void   _mulle_map_init( struct mulle_map *map,
                        size_t capacity,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator);

static inline void   mulle_map_init( struct mulle_map *map,
                                     size_t capacity,
                                     struct mulle_container_keyvaluecallback *callback,
                                     struct mulle_allocator *allocator)
{
   if( ! map)
      return;
   _mulle_map_init( map, capacity, callback, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void    _mulle_map_done( struct mulle_map *map)
{
   _mulle__map_done( &map->base, map->callback, map->allocator);
}static inline void   mulle_map_done( struct mulle_map *map)
{
   if( ! map)
      return;
   _mulle_map_done( map);
}


static inline int   mulle_map_is_empty( struct mulle_map *map)
{
   return( map ? ! _mulle__map_get_count( &map->base) : 1);
}


static inline int   mulle_map_contains( struct mulle_map *map, const void *key)
{
   struct mulle_pointerpair   space;

   if( ! map)
      return( 0);
   return( _mulle__map_get_pair( &map->base,
                                  key,
                                  map->callback,
                                  &space) != NULL);
}


static inline void   mulle_map_remove_all( struct mulle_map *map)
{
   if( map)
      _mulle__map_reset( &map->base, map->callback, map->allocator);
}




MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_destroy( struct mulle_map *map)
{
   _mulle__map_destroy( &map->base, map->callback, map->allocator);
}


static inline void   mulle_map_destroy( struct mulle_map *map)
{
   if( map)
      _mulle__map_destroy( &map->base, map->callback, map->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_reset( struct mulle_map *map)
{
   _mulle__map_reset( &map->base, map->callback, map->allocator);
}


static inline void    mulle_map_reset( struct mulle_map *map)
{
   if( map)
      _mulle__map_reset( &map->base, map->callback, map->allocator);
}


#pragma mark - petty accessors


static inline struct mulle_container_keyvaluecallback *
   mulle_map_get_callback( struct mulle_map *map)
{
   return( map ? map->callback : NULL);
}

static inline struct mulle_allocator *
   mulle_map_get_allocator( struct mulle_map *map)
{
   return( map ? map->allocator : NULL);
}


// TODO: why is this "get" not "lookup" ?

MULLE_C_NONNULL_FIRST
static inline size_t   _mulle_map_get_count( struct mulle_map *map)
{
   return( _mulle__map_get_count( &map->base));
}


static inline size_t   mulle_map_get_count( struct mulle_map *map)
{
   return( mulle__map_get_count( &map->base));
}


// _size for key really
MULLE_C_NONNULL_FIRST
static inline size_t   _mulle_map_get_size( struct mulle_map *map)
{
   return( _mulle__map_get_size( &map->base));
}

static inline size_t   mulle_map_get_size( struct mulle_map *map)
{
   return( mulle__map_get_size( &map->base));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t *
   _mulle_map_get_mutation_pointer( struct mulle_map *map)
{
   return( _mulle__map_get_mutation_pointer( &map->base));
}


static inline uintptr_t *
   mulle_map_get_mutation_pointer( struct mulle_map *map)
{
   return( mulle__map_get_mutation_pointer( map ? &map->base : NULL));
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle_map_get_notakey( struct mulle_map *map)
{
   return( _mulle_container_keycallback_get_notakey( &map->callback->keycallback));
}


static inline void   *mulle_map_get_notakey( struct mulle_map *map)
{
   return( map ? _mulle_map_get_notakey( map) : NULL);
}


#pragma mark - operations

MULLE_C_NONNULL_FIRST
static inline void   *_mulle_map_get( struct mulle_map *map, const void *key)
{
   return( _mulle__map_get( &map->base, key, map->callback));
}


static inline void   *mulle_map_get( struct mulle_map *map, const void *key)
{
   if( ! map)
      return( NULL);
   return( _mulle__map_get( &map->base, key, map->callback));
}



MULLE_C_NONNULL_FIRST_THIRD
static inline struct mulle_pointerpair   *
   _mulle_map_get_pair( struct mulle_map *map,
                        const void *key,
                        struct mulle_pointerpair *space)
{
   return( _mulle__map_get_pair( &map->base,
                                 key,
                                 map->callback,
                                 space));
}


MULLE_C_NONNULL_THIRD
static inline struct mulle_pointerpair   *
   mulle_map_get_pair( struct mulle_map *map,
                       const void *key,
                       struct mulle_pointerpair *space)
{
   if( ! map)
      return( NULL);
   return( _mulle_map_get_pair( map, key, space));
}


/// REMOVE


MULLE_C_NONNULL_FIRST
static inline int   _mulle_map_remove( struct mulle_map *map, const void *key)
{
   return( _mulle__map_remove( &map->base,
                               key,
                               map->callback,
                               map->allocator));
}


static inline int   mulle_map_remove( struct mulle_map *map, const void *key)
{
   if( ! map)
      return( 0);

   return( _mulle__map_remove( &map->base,
                               key,
                               map->callback,
                               map->allocator));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_shrink_if_needed( struct mulle_map *map)
{
   _mulle__map_shrink_if_needed( &map->base,
                                  map->callback,
                                  map->allocator);
}


static inline void   mulle_map_shrink_if_needed( struct mulle_map *map)
{
   if( map)
      _mulle__map_shrink_if_needed( &map->base,
                                     map->callback,
                                     map->allocator);
}


/// INSERT

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_map_insert_key_valuesv( struct mulle_map *map,
                                  void *firstkey,
                                  va_list args)
{
   _mulle__map_insert_key_valuesv( &map->base,
                                   firstkey,
                                   args,
                                   map->callback,
                                   map->allocator);
}


static inline void
   mulle_map_insert_key_valuesv( struct mulle_map *map,
                                 void *firstkey,
                                 va_list args)
{
   if( map)
      _mulle_map_insert_key_valuesv( map, firstkey, args);
}


static inline void
   mulle_map_insert_key_values( struct mulle_map *map,
                                void *firstkey, ...)
{
   va_list   args;

   va_start( args, firstkey);
   mulle_map_insert_key_valuesv( map, firstkey, args);
   va_end( args);
}


static inline int
   mulle_map_insert( struct mulle_map *map, void *key, void *value)
{
   struct mulle_pointerpair   pair;

   if( ! map)
      return( 0);

   pair.key   = key;
   pair.value = value;
   return( _mulle__map_insert_pair( &map->base,
                                     &pair,
                                     map->callback,
                                     map->allocator));
}



/// REGISTER

static inline void   *
   _mulle_map_register( struct mulle_map *map, void *key, void *value)
{
   struct mulle_pointerpair   pair;
   void                       *registered;

   pair.key   = key;
   pair.value = value;
   registered = _mulle__map_register_pair( &map->base,
                                           &pair,
                                           map->callback,
                                           map->allocator);
   return( registered);
}


static inline void   *
   mulle_map_register( struct mulle_map *map, void *key, void *value)
{
   return( map ? _mulle_map_register( map, key, value) : NULL);
}


/// REPLACE

static inline void   *
   _mulle_map_update( struct mulle_map *map, void *key, void *value)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   return( _mulle__map_update_pair( &map->base,
                                     &pair,
                                     map->callback,
                                     map->allocator));
}


static inline void   *
   mulle_map_update( struct mulle_map *map, void *key, void *value)
{
   return( map ? _mulle_map_update( map, key, value) : NULL);
}


/// SET

static inline void
   _mulle_map_set( struct mulle_map *map, void *key, void *value)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   _mulle__map_set_pair( &map->base,
                          &pair,
                          map->callback,
                          map->allocator);
}


static inline
void   mulle_map_set( struct mulle_map *map, void *key, void *value)
{
   if( map)
      _mulle_map_set( map, key, value);
}



#pragma mark - copy

MULLE_C_NONNULL_FIRST_SECOND
static inline  void
   _mulle_map_copy_items( struct mulle_map *dst,
                          struct mulle_map *src,
                          struct mulle_allocator *allocator)
{
   assert( dst->callback == src->callback);

   _mulle__map_copy_items( &dst->base,
                           &src->base,
                           dst->callback,
                           allocator);
}


static inline struct mulle_map   *mulle_map_copy( struct mulle_map *map)
{
   struct mulle_map   *other;

   if( ! map)
      return( map);
   // can't allow creation to be done by struct mulle__map
   other = mulle_map_create( mulle_map_get_count( map), map->callback, map->allocator);
   _mulle__map_copy_items( &other->base,
                           &map->base,
                           map->callback,
                           map->allocator);
   return( other);
}


MULLE__CONTAINER_GLOBAL
void   mulle_map_add_map( struct mulle_map *map, struct mulle_map *other);



#pragma mark - debugging

static inline char *
   _mulle_map_describe( struct mulle_map *map)
{
   return( _mulle__map_describe( &map->base,
                                  map->callback,
                                  map->allocator));
}


static inline char *
   mulle_map_describe( struct mulle_map *map)
{
   if( ! map)
      return( "(null)");
   return( _mulle_map_describe( map));
}


#pragma mark - enumeration

/*
 * struct mulle_mapenumerator    rover;
 * void                          *key;
 * void                          *value;

 * rover = mulle_map_enumerate( map);
 * while( mulle_mapenumerator_next( &rover, &key, &value))
 *    printf( "%s : %s\n", key, value);
 * mulle_mapenumerator_done( &rover);
 */
//
// The public enumerator holds the internal one as a member (see mulle-array.h
// for why repeating the fields by macro is a strict-aliasing violation).
//
#define MULLE_MAPENUMERATOR_BASE   struct mulle__mapenumerator base


struct mulle_mapenumerator
{
   MULLE_MAPENUMERATOR_BASE;
};


static inline struct mulle_mapenumerator
   mulle_map_enumerate( struct mulle_map *map)
{
   struct mulle_mapenumerator   rover;

   rover.base = map
                ? mulle__map_enumerate( &map->base, map->callback)
                : (struct mulle__mapenumerator) { 0 };
   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair *
   _mulle_mapenumerator_next_pair( struct mulle_mapenumerator *rover)
{
   return( _mulle__mapenumerator_next_pair( &rover->base));
}


static inline struct mulle_pointerpair *
   mulle_mapenumerator_next_pair( struct mulle_mapenumerator *rover)
{
   return( mulle__mapenumerator_next_pair( &rover->base));
}



MULLE_C_NONNULL_FIRST
static inline int
   _mulle_mapenumerator_next( struct mulle_mapenumerator *rover,
                              void **key,
                              void **value)
{
   return( _mulle__mapenumerator_next( &rover->base, key, value));
}



static inline int
   mulle_mapenumerator_next( struct mulle_mapenumerator *rover,
                             void **key,
                             void **value)
{
   return( mulle__mapenumerator_next( &rover->base, key, value));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
{
   mulle__mapenumerator_done( &rover->base);
}


static inline void   mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
{
   mulle__mapenumerator_done( &rover->base);
}



// created by make-container-do.sh mulle-map.c

#define mulle_map_do( name, callback)                             \
   for( struct mulle_map                                          \
           name ## __container = MULLE_MAP_DATA( callback, NULL), \
           *name = &name ## __container,                          \
           *name ## __i = NULL;                                   \
        ! name ## __i;                                            \
        name ## __i =                                             \
        (                                                         \
           _mulle_map_done( &name ## __container),                \
           (void *) 0x1                                           \
        )                                                         \
      )                                                           \
      MULLE_C_CONFINED_LOOP                                       \
      for( int  name ## __j = 0;    /* break protection */        \
           name ## __j < 1;                                       \
           name ## __j++)

// created by make-container-for.sh -ls -v src/map/mulle-map.c
// created by make-container-for.sh -ls -v src/map/mulle-map.c

#define mulle_map_for( name, key, value)                                                                    \
   assert( sizeof( key) == sizeof( void *));                                                                \
   assert( sizeof( value) == sizeof( void *));                                                              \
   for( struct mulle_mapenumerator                                                                          \
           rover__ ## key ## __ ## value = mulle_map_enumerate( name),                                      \
           *rover__  ## key ## __ ## value ## __i = (void *) 0;                                             \
        ! rover__  ## key ## __ ## value ## __i;                                                            \
        rover__ ## key ## __ ## value ## __i = (_mulle_mapenumerator_done( &rover__ ## key ## __ ## value), \
                                              (void *) 1))                                                  \
      while( _mulle_mapenumerator_next( &rover__ ## key ## __ ## value,                                     \
                                      (void **) &key,                                                       \
                                      (void **) &value))


/*
 * #define mulle_map_for( map, key, value)                                                        \
 *    assert( sizeof( key) == sizeof( void *));                                                   \
 *    assert( sizeof( value) == sizeof( void *));                                                 \
 *    for( struct mulle_mapenumerator rover__ ## key ## __ ## value = mulle_map_enumerate( map);  \
 *         _mulle_mapenumerator_next( &rover__ ## key ## __ ## value,                             \
 *                                     (void **) &(key),                                          \
 *                                     (void **) &(value));                                       \
 *         _mulle_mapenumerator_done( &rover__ ## key ## __ ## value))
 */

#endif
