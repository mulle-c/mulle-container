//
//  mulle-map.h
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
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
#define MULLE_MAP_BASE                                   \
   MULLE__MAP_BASE;                                      \
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
   _mulle__map_done( (struct mulle__map *) map, map->callback, map->allocator);
}static inline void   mulle_map_done( struct mulle_map *map)
{
   if( ! map)
      return;
   _mulle_map_done( map);
}


static inline int   mulle_map_is_empty( struct mulle_map *map)
{
   return( map ? ! _mulle__map_get_count( (struct mulle__map *) map) : 1);
}


static inline int   mulle_map_contains( struct mulle_map *map, void *key)
{
   if( ! map)
      return( 0);
   return( _mulle__map_get( (struct mulle__map *) map, key, map->callback) != NULL);
}


static inline void   mulle_map_remove_all( struct mulle_map *map)
{
   if( map)
      _mulle__map_reset( (struct mulle__map *) map, map->callback, map->allocator);
}




MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_destroy( struct mulle_map *map)
{
   _mulle__map_destroy( (struct mulle__map *) map, map->callback, map->allocator);
}


static inline void   mulle_map_destroy( struct mulle_map *map)
{
   if( map)
      _mulle__map_destroy( (struct mulle__map *) map, map->callback, map->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_reset( struct mulle_map *map)
{
   _mulle__map_reset( (struct mulle__map *) map, map->callback, map->allocator);
}


static inline void    mulle_map_reset( struct mulle_map *map)
{
   if( map)
      _mulle__map_reset( (struct mulle__map *) map, map->callback, map->allocator);
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
   return( _mulle__map_get_count( (struct mulle__map *) map));
}


static inline size_t   mulle_map_get_count( struct mulle_map *map)
{
   return( mulle__map_get_count( (struct mulle__map *) map));
}


// _size for key really
MULLE_C_NONNULL_FIRST
static inline size_t   _mulle_map_get_size( struct mulle_map *map)
{
   return( _mulle__map_get_size( (struct mulle__map *) map));
}

static inline size_t   mulle_map_get_size( struct mulle_map *map)
{
   return( mulle__map_get_size( (struct mulle__map *) map));
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
static inline void   *_mulle_map_get( struct mulle_map *map, void *key)
{
   return( _mulle__map_get( (struct mulle__map *) map, key, map->callback));
}


static inline void   *mulle_map_get( struct mulle_map *map, void *key)
{
   if( ! map)
      return( NULL);
   return( _mulle__map_get( (struct mulle__map *) map, key, map->callback));
}



MULLE_C_NONNULL_FIRST_THIRD
static inline struct mulle_pointerpair   *
   _mulle_map_get_pair( struct mulle_map *map,
                        void *key,
                        struct mulle_pointerpair *space)
{
   return( _mulle__map_get_pair( (struct mulle__map *) map,
                                 key,
                                 map->callback,
                                 space));
}


MULLE_C_NONNULL_THIRD
static inline struct mulle_pointerpair   *
   mulle_map_get_pair( struct mulle_map *map,
                       void *key,
                       struct mulle_pointerpair *space)
{
   if( ! map)
      return( NULL);
   return( _mulle_map_get_pair( map, key, space));
}


/// REMOVE


MULLE_C_NONNULL_FIRST
static inline int   _mulle_map_remove( struct mulle_map *map, void *key)
{
   return( _mulle__map_remove( (struct mulle__map *) map,
                               key,
                               map->callback,
                               map->allocator));
}


static inline int   mulle_map_remove( struct mulle_map *map, void *key)
{
   if( ! map)
      return( 0);

   return( _mulle__map_remove( (struct mulle__map *) map,
                               key,
                               map->callback,
                               map->allocator));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_map_shrink_if_needed( struct mulle_map *map)
{
   _mulle__map_shrink_if_needed( (struct mulle__map *) map,
                                  map->callback,
                                  map->allocator);
}


static inline void   mulle_map_shrink_if_needed( struct mulle_map *map)
{
   if( map)
      _mulle__map_shrink_if_needed( (struct mulle__map *) map,
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
   _mulle__map_insert_key_valuesv( (struct mulle__map *) map,
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
   return( _mulle__map_insert_pair( (struct mulle__map *) map,
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
   registered = _mulle__map_register_pair( (struct mulle__map *) map,
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
   return( _mulle__map_update_pair( (struct mulle__map *) map,
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
   _mulle__map_set_pair( (struct mulle__map *) map,
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

   _mulle__map_copy_items( (struct mulle__map *) dst,
                           (struct mulle__map *) src,
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
   _mulle__map_copy_items( (struct mulle__map *) other,
                           (struct mulle__map *) map,
                           map->callback,
                           map->allocator);
   return( other);
}


void   mulle_map_add_map( struct mulle_map *map, struct mulle_map *other);



#pragma mark - debugging

static inline char *
   _mulle_map_describe( struct mulle_map *map)
{
   return( _mulle__map_describe( (struct mulle__map *) map,
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
#define MULLE_MAPENUMERATOR_BASE    MULLE__MAPENUMERATOR_BASE


struct mulle_mapenumerator
{
   MULLE_MAPENUMERATOR_BASE;
};


static inline struct mulle_mapenumerator
   mulle_map_enumerate( struct mulle_map *map)
{
   struct mulle_mapenumerator    rover;
   struct mulle__mapenumerator   tmp;

   tmp = map
         ? mulle__map_enumerate( (struct mulle__map *) map, map->callback)
         : (struct mulle__mapenumerator) { 0 };
   memcpy( &rover, &tmp, sizeof( struct mulle_mapenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair *
   _mulle_mapenumerator_next_pair( struct mulle_mapenumerator *rover)
{
   return( _mulle__mapenumerator_next_pair( (struct mulle__mapenumerator *) rover));
}


static inline struct mulle_pointerpair *
   mulle_mapenumerator_next_pair( struct mulle_mapenumerator *rover)
{
   return( mulle__mapenumerator_next_pair( (struct mulle__mapenumerator *) rover));
}



MULLE_C_NONNULL_FIRST
static inline int
   _mulle_mapenumerator_next( struct mulle_mapenumerator *rover,
                              void **key,
                              void **value)
{
   return( _mulle__mapenumerator_next( (struct mulle__mapenumerator *) rover,
                                       key,
                                       value));
}



static inline int
   mulle_mapenumerator_next( struct mulle_mapenumerator *rover,
                             void **key,
                             void **value)
{
   return( mulle__mapenumerator_next( (struct mulle__mapenumerator *) rover,
                                      key,
                                      value));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
{
   mulle__mapenumerator_done( (struct mulle__mapenumerator *) rover);
}


static inline void   mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
{
   mulle__mapenumerator_done( (struct mulle__mapenumerator *) rover);
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
