//
//  mulle-map.h
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#ifndef mulle_map__h__
#define mulle_map__h__

#include "_mulle-map.h"

#include <stdarg.h>


//
// just like struct _mulle_map but has the callback embedded in
// its structure
//
#define MULLE_MAP_BASE                                   \
   _MULLE_MAP_BASE;                                      \
   struct mulle_container_keyvaluecallback   *_callback; \
   struct mulle_allocator                    *_allocator


struct mulle_map
{
   MULLE_MAP_BASE;
};

#define MULLE_MAPENUMERATOR_BASE    _MULLE_MAPENUMERATOR_BASE


struct mulle_mapenumerator
{
   MULLE_MAPENUMERATOR_BASE;
};


extern struct mulle_mapenumerator   mulle_mapenumerator_empty;


#define MULLE_MAPTINYENUMERATOR_BASE    _MULLE_MAPTINYENUMERATOR_BASE


struct mulle_maptinyenumerator
{
   MULLE_MAPTINYENUMERATOR_BASE;
};


extern struct mulle_maptinyenumerator   mulle_maptinyenumerator_empty;


#pragma mark -
#pragma mark setup and teardown

struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
                                          mulle_nonnull_second;

void   mulle_map_init( struct mulle_map *map,
                       size_t capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
                           mulle_nonnull_first_third;


static inline void    mulle_map_done( struct mulle_map *map)
{
   if( map)
      _mulle_map_done( (struct _mulle_map *) map, map->_callback, map->_allocator);
}


static inline void   mulle_map_destroy( struct mulle_map *map)
{
   if( map)
      _mulle_map_destroy( (struct _mulle_map *) map, map->_callback, map->_allocator);
}


static inline void    mulle_map_reset( struct mulle_map *map)
{
   mulle_map_done( map);
   mulle_map_init( map, 0, map->_callback, map->_allocator);
}

#pragma mark -
#pragma mark petty accessors


static inline struct mulle_container_keyvaluecallback   *mulle_map_get_callback( struct mulle_map *map)
{
   return( map ? map->_callback : NULL);
}

static inline struct mulle_allocator   *mulle_map_get_allocator( struct mulle_map *map)
{
   return( map ? map->_allocator : NULL);
}


static inline size_t   mulle_map_get_count( struct mulle_map *map)
{
   return( map ? _mulle_map_get_count( (struct _mulle_map *) map) : 0);
}


static inline void   *mulle_map_get_notakey( struct mulle_map *map)
{
   return( map ? mulle_container_keycallback_get_notakey( &map->_callback->keycallback) : NULL);
}


#pragma mark -
#pragma mark operations


static inline void   *mulle_map_get( struct mulle_map *map, void *key)
{
   if( ! map)
      return( NULL);
   return( _mulle_map_get( (struct _mulle_map *) map, key, map->_callback));
}


static inline void   mulle_map_remove( struct mulle_map *map, void *key)
{
   if( map)
      _mulle_map_remove( (struct _mulle_map *) map, key, map->_callback, map->_allocator);
}


static inline void   mulle_map_shrink_if_needed( struct mulle_map *map)
{
   if( map)
      _mulle_map_shrink_if_needed( (struct _mulle_map *) map, map->_callback, map->_allocator);
}


static inline void   mulle_map_insert_values_for_keysv( struct mulle_map *map, void *firstvalue, void *firstkey, va_list args)
{
   if( map)
      _mulle_map_insert_values_for_keysv( (struct _mulle_map *) map, firstvalue, firstkey, args, map->_callback, map->_allocator);
}


static inline void   mulle_map_insert_values_for_keys( struct mulle_map *map, void *firstvalue, void *firstkey, ...)
{
   va_list   args;

   va_start( args, firstkey);
   if( map)
      _mulle_map_insert_values_for_keysv( (struct _mulle_map *) map, firstvalue, firstkey, args, map->_callback, map->_allocator);
   va_end( args);
}


static inline void   mulle_map_set( struct mulle_map *map, void *key, void *value)
{
   if( map)
   {
      struct mulle_pointerpair   pair;

      pair._key   = key;
      pair._value = value;
      _mulle_map_set( (struct _mulle_map *) map, &pair, map->_callback, map->_allocator);
   }
}


static inline void   *mulle_map_insert( struct mulle_map *map, void *key, void *value)
{
   if( map)
   {
      struct mulle_pointerpair   pair;

      pair._key   = key;
      pair._value = value;
      return( _mulle_map_insert( (struct _mulle_map *) map, &pair, map->_callback, map->_allocator));
   }
   return( NULL);
}

#pragma mark -
#pragma mark copy

static inline int   mulle_map_copy_items( struct mulle_map *dst, struct mulle_map *src)
{
   if( dst)
      return( _mulle_map_copy_items( (struct _mulle_map *) dst,
                                     (struct _mulle_map *) src,
                                     dst->_callback,
                                     dst->_allocator));
   return( -1);
}

static inline struct mulle_map   *mulle_map_copy( struct mulle_map *map)
{
   struct mulle_map   *other;

   // can't allow creation to be done by struct _mulle_map
   other = mulle_map_create( mulle_map_get_count( map), map->_callback, map->_allocator);
   if( map)
      _mulle_map_copy_items( (struct _mulle_map *) other, (struct _mulle_map *) map, map->_callback, map->_allocator);
   return( other);
}


#pragma mark -
#pragma mark debugging

// in C, expect a strdup()ed string, in ObjC an autorelease NSString *
static inline void   *mulle_map_describe( struct mulle_map *map,
                                          struct mulle_container_keyvaluecallback *callback,
                                          struct mulle_allocator *allocator)
{
   if( ! map)
      return( NULL);
   return( _mulle_map_describe( (struct _mulle_map *) map, map->_callback, map->_allocator));
}

#pragma mark -
#pragma mark enumeration

static inline struct mulle_mapenumerator   mulle_map_enumerate( struct mulle_map *map)
{
   struct _mulle_mapenumerator   rover;

   if( ! map)
   {
      memset( &rover, 0, sizeof( rover));
      return( *(struct mulle_mapenumerator *) &rover);
   }
   rover = _mulle_map_enumerate( (struct _mulle_map *) map, map->_callback);
   return( *(struct mulle_mapenumerator *) &rover);
}


static inline int   mulle_mapenumerator_next( struct mulle_mapenumerator *rover, void **key, void **value)
{
   struct mulle_pointerpair  *pair;

   if( ! rover)
      return( 0);

   pair = _mulle_mapenumerator_next( (struct _mulle_mapenumerator *) rover);
   if( ! pair)
      return( 0);

   if( key)
      *key = pair->_key;
   if( value)
      *value = pair->_value;
   return( 1);
}


static inline void   mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
{
   if( rover)
      _mulle_mapenumerator_done( (struct _mulle_mapenumerator *) rover);
}


#pragma mark -
#pragma mark tiny enumeration

static inline struct mulle_maptinyenumerator   mulle_map_tinyenumerate_nil( struct mulle_map *map)
{
   struct _mulle_maptinyenumerator   rover;

   if( ! map)
   {
      memset( &rover, 0, sizeof( rover));
      return( *(struct mulle_maptinyenumerator *) &rover);
   }

   assert( ! map->_callback->keycallback.notakey);
   rover = _mulle_map_tinyenumerate_nil( (struct _mulle_map *) map);
   return( *(struct mulle_maptinyenumerator *) &rover);
}


static inline int   mulle_maptinyenumerator_next( struct mulle_maptinyenumerator *rover,
                                                  void **key,
                                                  void **value)
{
   if( ! rover)
      return( 0);
   return( _mulle_maptinyenumerator_next( (struct _mulle_maptinyenumerator *) rover, key, value));
}


static inline void   mulle_maptinyenumerator_done( struct mulle_maptinyenumerator *rover)
{
   if( rover)
      _mulle_maptinyenumerator_done( (struct _mulle_maptinyenumerator *) rover);
}


#endif
