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
#ifndef _mulle_map__h__
#define _mulle_map__h__
 
#include "_mulle_indexedkeyvaluebucket.h"
#include "mulle_container_attributes.h"
#include "mulle_container_operation.h"
#include <stdarg.h>


#define _MULLE_MAP_BASE                        \
   struct _mulle_keyvaluepair   *_storage;     \
   size_t                       _count;        \
   short                        _depth

struct _mulle_map
{
   _MULLE_MAP_BASE;
};
   

#define _MULLE_MAPENUMERATOR_BASE                                  \
   struct _mulle_indexedkeyvaluebucketenumerator   _bucketsRover;  \
   size_t              _index;                                     \
   size_t              _left;                                      \
   struct _mulle_map   *_map;                                      \
   struct mulle_container_keyvaluecallback   *_callback
   

struct  _mulle_mapenumerator
{  
   _MULLE_MAPENUMERATOR_BASE;
};


void   _mulle_map_init( struct _mulle_map *map,
                        size_t capacity,
                        struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_third;
                       

void   _mulle_map_done( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;


struct _mulle_map   *_mulle_map_create( size_t capacity,
                                        struct mulle_container_keyvaluecallback *callback) mulle_nonnull_second;



void   _mulle_map_free( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;

void   *_mulle_map_get( struct _mulle_map *map, void *key, struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_third;
void   _mulle_map_remove( struct _mulle_map *map, void *key, struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_third;

void _mulle_map_insert_keys_and_valuesv( struct _mulle_map *map,
                                         void *firstkey,
                                         void *firstvalue,
                                         va_list arg,
                                         struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_fifth;


static inline void  _mulle_map_insert_keys_and_values( struct _mulle_map *map,
                                                       struct mulle_container_keyvaluecallback *callback,
                                                       void *firstkey,
                                                       void *firstvalue,
                                                       ...) mulle_nonnull_first_second;
static inline void  _mulle_map_insert_keys_and_values( struct _mulle_map *map,
                                                       struct mulle_container_keyvaluecallback *callback,
                                                       void *firstkey,
                                                       void *firstvalue,
                                                       ...)
{
   va_list   args;
   
   va_start( args, firstvalue);
   _mulle_map_insert_keys_and_valuesv( map, firstkey, firstvalue, args, callback);
   va_end( args);
}


static inline size_t   _mulle_map_count( struct _mulle_map *map) mulle_nonnull_first;
static inline size_t   _mulle_map_count( struct _mulle_map *map)
{
   return( map->_count);
}


void   *__mulle_map_put( struct _mulle_map *,
                         struct _mulle_keyvaluepair *,
                         enum mulle_container_set_mode,
                         struct mulle_container_keyvaluecallback *) mulle_nonnull_first_second_fourth;


static inline void   _mulle_map_put( struct _mulle_map *map,
                                     void *key,
                                     void *value,
                                     struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_fourth;
static inline void   _mulle_map_put( struct _mulle_map *map,
                                     void *key,
                                     void *value,
                                     struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_keyvaluepair   pair;
   
   pair._key   = key;
   pair._value = value;

   __mulle_map_put( map, &pair, mulle_container_put_e, callback);
}


static inline void   *_mulle_map_insert( struct _mulle_map *map,
                                         void *key,
                                         void *value,
                                         struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_fourth;
static inline void   *_mulle_map_insert( struct _mulle_map *map,
                                         void *key,
                                         void *value,
                                         struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_keyvaluepair   pair;
   
   pair._key   = key;
   pair._value = value;

   return( __mulle_map_put( map, &pair, mulle_container_insert_e, callback));
}



struct _mulle_map   *_mulle_map_copy( struct _mulle_map *map, 
                                      struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;


static inline struct _mulle_mapenumerator   _mulle_map_enumerate( struct _mulle_map *map,
                                                                  struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline struct _mulle_mapenumerator   _mulle_map_enumerate( struct _mulle_map *map,
                                                                  struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_mapenumerator   rover;
   
   rover._bucketsRover._left = 0;
   
   rover._index    = 0;
   rover._left     = map->_count;
   rover._map      = map;
   rover._callback = callback;
   
   return( rover);
}


int    _mulle_mapenumerator_next( struct _mulle_mapenumerator *enumerator, void **key, void **value) mulle_nonnull_first;


static inline void    _mulle_mapenumerator_done( struct _mulle_mapenumerator *enumerator) mulle_nonnull_first;
static inline void    _mulle_mapenumerator_done( struct _mulle_mapenumerator *enumerator)
{
}

void    _mulle_map_reset( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;


#endif
