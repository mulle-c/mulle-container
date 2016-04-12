//
//  mulle_map.h
//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
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
#ifndef mulle_map__h__
#define mulle_map__h__
 
#include "_mulle_map.h"

//
// just like struct _mulle_map but has the callback embedded in
// its structure
//
#define MULLE_MAP_BASE                                    \
   _MULLE_MAP_BASE;                                       \
   struct mulle_container_keyvaluecallback    *_callback; \
   struct mulle_allocator                     *_allocator


struct mulle_map
{
   MULLE_MAP_BASE;
};
   
#define MULLE_MAPENUMERATOR_BASE    _MULLE_MAPENUMERATOR_BASE 


struct mulle_mapenumerator
{
   MULLE_MAPENUMERATOR_BASE;
};


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


static inline void   mulle_map_free( struct mulle_map *map)
{
   if( map)
      _mulle_map_free( (struct _mulle_map *) map, map->_callback, map->_allocator);
}


#pragma mark -
#pragma mark operations


static inline struct mulle_allocator   *mulle_map_get_allocator( struct mulle_map *map)
{
   return( map->_allocator);
}


static inline void   *mulle_map_get( struct mulle_map *map, void *key)
{
   return( _mulle_map_get( (struct _mulle_map *) map, key, map->_callback));
}


static inline void   mulle_map_remove( struct mulle_map *map, void *key)
{
   _mulle_map_remove( (struct _mulle_map *) map, key, map->_callback, map->_allocator);
}


static inline size_t   mulle_map_get_count( struct mulle_map *map)
{
   return( _mulle_map_get_count( (struct _mulle_map *) map));
}


static inline void   mulle_map_insert_keys_and_values_v( struct mulle_map *map, void *firstkey, void *firstvalue, va_list args)
{
   _mulle_map_insert_keys_and_valuesv( (struct _mulle_map *) map, firstkey,  firstvalue, args, map->_callback, map->_allocator);
}


static inline void   mulle_map_insert_keys_and_values( struct mulle_map *map, void *firstkey, void *firstvalue, ...)
{
   va_list   args;
   
   va_start( args, firstvalue);
   _mulle_map_insert_keys_and_valuesv( (struct _mulle_map *) map, firstkey, firstvalue, args, map->_callback, map->_allocator);
   va_end( args);
}


static inline void   mulle_map_put( struct mulle_map *map, void *key, void *value)
{
   _mulle_map_put( (struct _mulle_map *) map, key, value, map->_callback, map->_allocator);
}


static inline void   *mulle_map_insert( struct mulle_map *map, void *key, void *value)
{
   return( _mulle_map_insert( (struct _mulle_map *) map, key, value, map->_callback, map->_allocator));
}


static inline struct mulle_map   *mulle_map_copy( struct mulle_map *map)
{
   struct mulle_map   *other;
   extern void   __mulle_map_copy( struct _mulle_map *, struct _mulle_map *, struct mulle_container_keyvaluecallback *, struct mulle_allocator *);
   
   // can't allow creation to be done by struct _mulle_map
   other = mulle_map_create( mulle_map_get_count( map), map->_callback, map->_allocator);
   __mulle_map_copy( (struct _mulle_map *) other, (struct _mulle_map *) map, map->_callback, map->_allocator);
   return( other);
}



static inline struct mulle_mapenumerator   mulle_map_enumerate( struct mulle_map *map)
{  
   struct _mulle_mapenumerator   rover;
   
   rover = _mulle_map_enumerate( (struct _mulle_map *) map, map->_callback);
   return( *(struct mulle_mapenumerator *) &rover);
}


static inline int    mulle_mapenumerator_next( struct mulle_mapenumerator *rover, void **key, void **value)
{
   return( _mulle_mapenumerator_next( (struct _mulle_mapenumerator *) rover, key, value));
}


static inline void    mulle_mapenumerator_done( struct mulle_mapenumerator *rover)
{
   _mulle_mapenumerator_done( (struct _mulle_mapenumerator *) rover);
}


static inline void    mulle_map_reset( struct mulle_map *map)
{
   mulle_map_done( map);
   mulle_map_init( map, 0, map->_callback, map->_allocator);
}


#endif
