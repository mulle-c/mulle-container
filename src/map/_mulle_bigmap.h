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
#ifndef _mulle_bigmap__h__
#define _mulle_bigmap__h__
 
#include "_mulle_map.h"
#include "mulle_container_compiler_attributes.h"
#include "mulle_container_operation.h"
#include <stdarg.h>


//
// this a hash table, that indexes another hashtable if it outgrows it's
// capacity. Advantage: doesn't need to copy during growth so much
// Disadvantage: can be slower
//
#define _MULLE_BIGMAP_BASE                 \
   struct mulle_pointerpair   *_storage;   \
   size_t                       _count;    \
   short                        _depth

struct _mulle_bigmap
{
   _MULLE_BIGMAP_BASE;
};
   

#define _MULLE_BIGMAPENUMERATOR_BASE               \
   struct _mulle_mapenumerator   _bucketsRover;    \
   size_t              _index;                     \
   size_t              _left;                      \
   struct _mulle_bigmap   *_map;                   \
   struct mulle_container_keyvaluecallback   *_callback
   

struct  _mulle_bigmapenumerator
{  
   _MULLE_BIGMAPENUMERATOR_BASE;
};


void  _mulle_bigmap_init( struct _mulle_bigmap *map,
                          size_t capacity,
                          struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
                          mulle_nonnull_first_third_fourth;
                       

void   _mulle_bigmap_done( struct _mulle_bigmap *map,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
                           mulle_nonnull_first_second_third;


struct _mulle_bigmap   *_mulle_bigmap_create( size_t capacity,
                                        struct mulle_container_keyvaluecallback *callback,
                                        struct mulle_allocator *allocator)
                                             mulle_nonnull_second_third;



void   _mulle_bigmap_destroy( struct _mulle_bigmap *map,
                          struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
                             mulle_nonnull_second_third;

void   *_mulle_bigmap_get( struct _mulle_bigmap *map,
                        void *key,
                        struct mulle_container_keyvaluecallback *callback)
                           mulle_nonnull_first_third;

void   _mulle_bigmap_remove( struct _mulle_bigmap *map,
                          void *key,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
                             mulle_nonnull_first_third;

void _mulle_bigmap_insert_keys_and_valuesv( struct _mulle_bigmap *map,
                                         void *firstkey,
                                         void *firstvalue,
                                         va_list arg,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator)
                                            mulle_nonnull_first_fifth;


static inline void  _mulle_bigmap_insert_keys_and_values( struct _mulle_bigmap *map,
                                                       struct mulle_container_keyvaluecallback *callback,
                                                       struct mulle_allocator *allocator,
                                                       void *firstkey,
                                                       void *firstvalue,
                                                       ...)
{
   va_list   args;
   
   va_start( args, firstvalue);
   _mulle_bigmap_insert_keys_and_valuesv( map, firstkey, firstvalue, args, callback, allocator);
   va_end( args);
}


static inline size_t   _mulle_bigmap_get_count( struct _mulle_bigmap *map)
{
   return( map->_count);
}


void   *_mulle_bigmap_write( struct _mulle_bigmap *,
                             struct mulle_pointerpair *,
                             enum mulle_container_write_mode,
                             struct mulle_container_keyvaluecallback *,
                             struct mulle_allocator *allocator) mulle_nonnull_first_second_fourth_fifth;


static inline void   _mulle_bigmap_set( struct _mulle_bigmap *map,
                                        void *key,
                                        void *value,
                                        struct mulle_container_keyvaluecallback *callback,
                                        struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;
   
   pair._key   = key;
   pair._value = value;

   _mulle_bigmap_write( map, &pair, mulle_container_overwrite_e, callback, allocator);
}


static inline void   *_mulle_bigmap_insert( struct _mulle_bigmap *map,
                                            void *key,
                                            void *value,
                                            struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;
   
   pair._key   = key;
   pair._value = value;

   return( _mulle_bigmap_write( map, &pair, mulle_container_insert_e, callback, allocator));
}



struct _mulle_bigmap   *_mulle_bigmap_clone( struct _mulle_bigmap *map, 
                                             struct mulle_container_keyvaluecallback *callback,
                                              struct mulle_allocator *allocator)
                                         mulle_nonnull_first_second;


static inline struct _mulle_bigmapenumerator   _mulle_bigmap_enumerate( struct _mulle_bigmap *map,
                                                                  struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_bigmapenumerator   rover;
   
   rover._bucketsRover._left = 0;
   
   rover._index    = 0;
   rover._left     = map->_count;
   rover._map      = map;
   rover._callback = callback;
   
   return( rover);
}


int    _mulle_bigmapenumerator_next( struct _mulle_bigmapenumerator *enumerator, void **key, void **value) mulle_nonnull_first;


static inline void    _mulle_bigmapenumerator_done( struct _mulle_bigmapenumerator *enumerator)
{
}

void    _mulle_bigmap_reset( struct _mulle_bigmap *map,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
                             mulle_nonnull_first_second;

#endif
