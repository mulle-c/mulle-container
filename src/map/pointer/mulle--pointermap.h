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
#ifndef mulle__pointermap_h__
#define mulle__pointermap_h__

#include "mulle--pointermap-struct.h"
#include "mulle--pointermap-generic.h"



MULLE_C_NONNULL_FIRST_SECOND
static inline void   _mulle__pointermap_set_pair( struct mulle__pointermap *map,
                                                  struct mulle_pointerpair *pair,
                                                  struct mulle_allocator *allocator)
{
   _mulle__pointermap_set_pair_generic( map, pair, &mulle__pointermap_keyvaluecallback, allocator);
}


static inline void   mulle__pointermap_set_pair( struct mulle__pointermap *map,
                                                 struct mulle_pointerpair *pair,
                                                 struct mulle_allocator *allocator)
{
   if( ! map || ! pair)
      return;
   _mulle__pointermap_set_pair_generic( map, pair, &mulle__pointermap_keyvaluecallback, allocator);
}


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline void   _mulle__pointermap_set( struct mulle__pointermap *map,
                                             void *key,
                                             void *value,
                                             struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.key   = key;
   pair.value = value;
   _mulle__pointermap_set_pair_generic( map, &pair, &mulle__pointermap_keyvaluecallback, allocator);
}


static inline void   mulle__pointermap_set( struct mulle__pointermap *map,
                                            void *key,
                                            void *value,
                                            struct mulle_allocator *allocator)
{
   if( ! map)
      return;
   _mulle__pointermap_set( map, key, value, allocator);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
void   *_mulle__pointermap_insert_pair( struct mulle__pointermap *map,
                                        struct mulle_pointerpair *pair,
                                        struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_insert_pair_generic( map,
                                                   pair,
                                                   &mulle__pointermap_keyvaluecallback,
                                                   allocator));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline
int   _mulle__pointermap_remove( struct mulle__pointermap *map,
                                 void *key,
                                 struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_remove_generic( map,
                                              key,
                                              &mulle__pointermap_keyvaluecallback,
                                              allocator));
}

MULLE_C_NONNULL_SECOND
static inline
int   mulle__pointermap_remove( struct mulle__pointermap *map,
                                void *key,
                                struct mulle_allocator *allocator)
{
   if( map)
      return( _mulle__pointermap_remove( map, key, allocator));
   return( 0);
}


// created by make-container-do.sh mulle--pointermap.c

#define mulle__pointermap_do( name)                              \
   for( struct mulle__pointermap                                 \
           name ## __container = { 0 },                          \
           *name = &name ## __container,                         \
           *name ## __i = NULL;                                  \
        ! name ## __i;                                           \
        name ## __i =                                            \
        (                                                        \
           _mulle__pointermap_done( &name ## __container, NULL), \
           (void *) 0x1                                          \
        )                                                        \
      )                                                          \
      for( int  name ## __j = 0;    /* break protection */       \
           name ## __j < 1;                                      \
           name ## __j++)


#define mulle__pointermap_for( map, key, value)                                                               \
   assert( sizeof( key) == sizeof( void *));                                                                  \
   assert( sizeof( value) == sizeof( void *));                                                                \
   for( struct mulle__pointermapenumerator rover__ ## key ## __ ## value = mulle__pointermap_enumerate( map); \
        _mulle__pointermapenumerator_next( &rover__ ## key ## __ ## value,                                    \
                                           (void **) &(key),                                                  \
                                           (void **) &(value));                                               \
        _mulle__pointermapenumerator_done( &rover__ ## key ## __ ## value))

#endif
