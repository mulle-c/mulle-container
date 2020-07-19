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
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle--pointermap.h"

#include "mulle-container-operation.h"
#include "mulle-container-math.h"
#include "mulle-hash.h"
#include "mulle-prime.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#define MULLE_POINTERMAP_INITIAL_SIZE  4

#pragma clang diagnostic ignored "-Wshift-count-overflow"


// assume we have size 4
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 or 2 (leave odd alone) for better insert perfomance
//
// (?)
static unsigned int   _mulle__pointermap_hash_key_for_size( void *key, unsigned int size)
{
   unsigned int  hash;

   assert( size >= 2);

   hash = (unsigned int) mulle_hash_pointer( key);
   //
   // so if size is pow2, size-1 is the mask
   // now sub one again to kill lowest bit (which will be set)
   // if size is 0, this doesn't work though
   //
   return( hash & (size - 2)); // kill lowest bit (it will be)
}



void   *_mulle__pointermap_write_pair( struct mulle__pointermap *map,
                                      struct mulle_pointerpair *p,
                                      enum mulle_container_write_mode mode,
                                      struct mulle_allocator *allocator);
/*
 *
 */


static inline int   is_full( struct mulle__pointermap *map, unsigned int size)
{
   return( map->count >= (size - (size >> 2)));  // full when only 25% free
}



#pragma mark - setup and teardown


static void   **allocate_storage( unsigned int n,
                                  struct mulle_allocator *allocator)
{
   void   **buf;
   void   **p;
   void   **sentinel;

   if( ! n)
      return( NULL);

   assert( n >= 2);
   assert( mulle_is_pow2( n));
   return( mulle_allocator_calloc( allocator, n, sizeof( void *) * 2));
}


void   _mulle__pointermap_init( struct mulle__pointermap *p,
                               unsigned int capacity,
                               struct mulle_allocator *allocator)
{
   p->count   = 0;

   //
   // our map requires zeroes to find an end so give it ~25% holes
   //
   p->size    = capacity >= MULLE_POINTERMAP_MIN_SIZE
                     ? mulle_pow2round( capacity + (capacity >> MULLE_POINTERMAP_FILL_SHIFT))
                     : 0;
   p->storage = allocate_storage( p->size, allocator);
}


struct mulle__pointermap   *mulle__pointermap_create( unsigned int capacity,
                                                      size_t extra,
                                                      struct mulle_allocator *allocator)
{
   struct mulle__pointermap   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointermap) + extra);
   _mulle__pointermap_init( p, capacity, allocator);
   return( p);
}


static inline void _mulle__pointermap_free_storage( struct mulle__pointermap *map,
                                                   struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, map->storage);
}


void   _mulle__pointermap_done( struct mulle__pointermap *map,
                                struct mulle_allocator *allocator)
{
   _mulle__pointermap_free_storage( map, allocator);
}


void   _mulle__pointermap_destroy( struct mulle__pointermap *map,
                                  struct mulle_allocator *allocator)
{
   _mulle__pointermap_done( map, allocator);
   mulle_allocator_free( allocator, map);
}


void   _mulle__pointermap_reset( struct mulle__pointermap *map,
                                struct mulle_allocator *allocator)
{
   _mulle__pointermap_done( map, allocator);
   _mulle__pointermap_init( map, 0, allocator);
}


#pragma mark - mechanisms

static inline void   store_key_value( void **storage,
                                      unsigned int size,
                                      unsigned int i,
                                      void *key,
                                      void *value)
{
   while( storage[ i])
   {
      if( ++i < size)
         continue;
      i = 0;
   }

   storage[ i]        = key;
   storage[ i + size] = value;
}


//
// this is not a "deep copy", the assumption is that the src will
// disappear after copy
//
static void   copy_storage( void **dst,
                            unsigned int dst_size,
                            void **src,
                            unsigned int src_size)
{
   void           *key;
   void           **sentinel;
   unsigned int   i;
   unsigned int   hash;

   assert( mulle_is_pow2( dst_size));
   assert( mulle_is_pow2( src_size));
   assert( dst_size >= src_size);

   sentinel = &src[ src_size];
   while( src < sentinel)
   {
      key = *src;
      if( key)
      {
         i    = _mulle__pointermap_hash_key_for_size( key, dst_size);
         store_key_value( dst, dst_size, i, key, src[ src_size]);
      }
      ++src;
   }
}


static void   grow( struct mulle__pointermap *map,
                    struct mulle_allocator *allocator)
{
   void           **buf;
   unsigned int   new_size;

   new_size = map->size * 2;
   if( new_size < map->size)
      abort();  // overflow

   if( new_size == 0)
      new_size = MULLE_POINTERMAP_INITIAL_SIZE;

   assert( MULLE_POINTERMAP_INITIAL_SIZE >= 2);

   buf = allocate_storage( new_size, allocator);
   copy_storage( buf, new_size, map->storage, map->size);
   _mulle__pointermap_free_storage( map, allocator);

   map->storage = buf;
   map->size    = new_size;
}


static void   shrink( struct mulle__pointermap *map,
                      struct mulle_allocator *allocator)
{
   void           **buf;
   unsigned int   new_size;

   new_size = map->size / 2;
   if( new_size < MULLE_POINTERMAP_INITIAL_SIZE)
      return;

   buf = allocate_storage( new_size, allocator);
   copy_storage( buf, new_size, map->storage, map->size);
   _mulle__pointermap_free_storage( map, allocator);

   map->storage = buf;
   map->size    = new_size;
}


static unsigned long  _find_index( void **storage,
                                   unsigned int size,
                                   void *key,
                                   void *q,
                                   unsigned int i,
                                   unsigned int *hole_index)
{
   unsigned int   mask;

   mask = size - 1;
   do
   {
      if( q == key)
         return( i);

      i = (i + 1) & mask;
      q = storage[ i];
   }
   while( q);

   *hole_index = i;
   return( mulle_not_found_e);
}


static inline unsigned long  find_index( void **storage,
                                         unsigned int size,
                                         void *key,
                                         unsigned int *hole_index)
{
   void           *q;
   unsigned int   i;

   assert( storage);

   i = _mulle__pointermap_hash_key_for_size( key, size);
   q = storage[ i];
   if( ! q)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }

   return( _find_index( storage, size, key, q, i, hole_index));
}


#pragma mark - operations

void   *_mulle__pointermap_write_pair( struct mulle__pointermap *map,
                                      struct mulle_pointerpair *pair,
                                      enum mulle_container_write_mode mode,
                                      struct mulle_allocator *allocator)
{
   unsigned long              found;
   unsigned int               i;
   void                       **q;
   struct mulle_pointerpair   new_pair;

   assert( pair->_key);

   if( map->count)
   {
      unsigned int   size;
      void           **storage;  // only valid in this no-grow block
      unsigned int   hole_index;

      storage    = map->storage;
      size       = map->size;

      hole_index = 0xfeedface;  // for the analyzer
      found      = find_index( storage, size, pair->_key, &hole_index);

      if( found != mulle_not_found_e)
      {
         i = (unsigned int) found;
         q = &storage[ i];
         switch( mode)
         {
         case mulle_container_overwrite_e :
            q        = &storage[ i];
            *q       = pair->_key;
            q[ size] = pair->_value;
            return( q[ size]);

         case mulle_container_insert_e :
            return( q[ size]);
         }
      }

      i = hole_index;
      if( ! _mulle__pointermap_is_full( map))
      {
         storage[ i]        = pair->_key;
         storage[ i + size] = pair->_value;
         map->count++;
         return( NULL);
      }
   }

   if( _mulle__pointermap_is_full( map))
      grow( map, allocator);

   i = _mulle__pointermap_hash_key_for_size( pair->_key, map->size);
   store_key_value( map->storage, map->size, i, pair->_key, pair->_value);
   map->count++;

   return( NULL);
}


void   *_mulle__pointermap_get( struct mulle__pointermap *map,
                               void *key)
{
   int            (*f)( void *, void *, void *);
   unsigned int   i;
   unsigned int   size;
   unsigned int   mask;
   void           *found;
   void           **storage;

   // important to not hit a NULL storage later
   // size must be > 2 for the hash to work, otherwise we could get
   if( map->count == 0)
      return( NULL);

   storage = map->storage;
   size    = map->size;
   i       = _mulle__pointermap_hash_key_for_size( key, size);
   mask    = size - 1;

   for(;;)
   {
      found = storage[ i];
      if( key == found)
         break;
      if( ! found)
         break;
      i = (i + 1) & mask;
   }

   return( storage[ i + size]);
}


struct mulle_pointerpair   *_mulle__pointermap_get_any_pair( struct mulle__pointermap *map,
                                                            struct mulle_pointerpair *space)
{
   unsigned int   i;
   unsigned int   size;
   unsigned int   mask;
   void           *found;
   void           **storage;

   if( ! map->count)
      return( NULL);

   storage = map->storage;
   size    = map->size;
   mask    = size - 1;

   //
   // we use a random starting point, it doesn't matter what space->_key is
   // if that happens to be the previous key, though this could be
   // fairly optimal ? when you are deleting contents with this in a loop
   // or ?
   //
   i = _mulle__pointermap_hash_key_for_size( space->_key, size);

   for(;;)
   {
      found = storage[ i];
      if( found)
         return( storage[ i + size]);
      i = (i + 1) & mask;
   }
}


void   _mulle__pointermap_set_pair( struct mulle__pointermap *map,
                                   struct mulle_pointerpair *pair,
                                   struct mulle_allocator *allocator)
{
   _mulle__pointermap_write_pair( map, pair, mulle_container_overwrite_e, allocator);
}


void    *_mulle__pointermap_insert_pair( struct mulle__pointermap *map,
                                        struct mulle_pointerpair *pair,
                                        struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_write_pair( map, pair, mulle_container_insert_e, allocator));
}


void   *_mulle__pointermap_insert_pair_known_absent( struct mulle__pointermap *map,
                                               struct mulle_pointerpair *pair,
                                               struct mulle_allocator *allocator)
{
   return( _mulle__pointermap_write_pair( map, pair, mulle_container_insert_e, allocator));
}



int   _mulle__pointermap_remove( struct mulle__pointermap *map,
                                void *key,
                                struct mulle_allocator *allocator)
{
   unsigned int   hole_index;
   unsigned int   i;
   unsigned long  found;
   void           **storage;
   void           **q;
   unsigned int   dst_index;
   unsigned int   search_start;
   unsigned int   size;
   unsigned int   mask;

   // important to not hit a NULL storage later
   if( map->count == 0)
      return( 0);

   storage = map->storage;
   size    = map->size;

   found = find_index( storage, size, key, &hole_index);
   if( found == mulle_not_found_e)
      return( 0);

   i = (unsigned int) found;
   q = &storage[ i];
   map->count--;

   // now we may need to do a whole lot of shifting, if
   // the following object isn't in its proper hash index.
   // Because of the way objects are inserted and deleted
   // we have to possibly move EVERYTHING until we hit the
   // hole.

   dst_index = i;
   mask      = size - 1;
   for(;;)
   {
      i = (i + 1) & mask;

      q = &storage[ i];
      if( ! *q)
      {
         storage[ dst_index]        = *q;
         storage[ dst_index + size] = NULL;
         break;
      }

      // check if its in the slot it's expected
      // otherwise it won't be found because of
      // the "hole" in the front we just made
      // but keep going because "wrong" hashes might be behind
      // be sure not to move items that won't be found
      search_start = _mulle__pointermap_hash_key_for_size( *q, size);

      // object better off where it is ?
      // CASE:
      // [
      //
      //
      //    2 0x5205a0 (2 0xda000000)  *delete*
      //    3 0x531ae0 (2 0xda000000)  consider
      //   ]
      // dst_index=2, search_start=2, index=3
      //
      // CASE:
      // [
      //
      //    1 0x5671b0 (1 0xcad00000)  *delete*
      //    2 0x5205a0 (2 0xda000000)
      //    3 0x531ae0 (3 0xdbfc0000)  consider
      //   ]
      // dst_index=1, search_start=3, index=3
      //
      // CASE:
      // [
      //
      //    1 0x5671b0 (1 0xcad00000)  *delete*
      //    2 0x5205a0 (2 0xda000000)  consider
      //
      //   ]
      // dst_index=1, search_start=2, index=2
      //
      // CASE:
      // [
      //    0 0x537a40 (0 0xdbfc0000)  consider
      //
      //
      //    3 0x531ae0 (3 0xdbfc0000)  *delete*
      //   ]
      // dst_index=3, search_start=0, index=0
      //
      // CASE:
      // [
      //    0 0x537a40 (2 0xda000000)  consider
      //
      //    2 0x5205a0 (2 0xda000000)
      //    3 0x531ae0 (2 0xda000000)  *delete*
      //   ]
      // dst_index=3, search_start=2, index=0
      //
      // CASE:
      // [
      //    0 0x537a40 (0 0xdbfc0000)
      //    1 0x5205a0 (3 0xda000000)  consider
      //
      //    3 0x531ae0 (3 0xdbfc0000)  *delete*
      //   ]
      // dst_index=3, search_start=3, index=1
      //
      // CASE:
      // [
      //    0 0x537a40 (0 0xdbfc0000)  *delete*
      //    1 0x5205a0 (3 0xda000000)  consider
      //
      //    3 0x531ae0 (3 0xdbfc0000)
      //   ]
      // dst_index=0, search_start=3, index=1


      if( search_start > i)      // object from wrapped insert ?
      {                          //
         if( dst_index > i)      // would this move to a non-wrapped position ?
         {
            if( search_start > dst_index)  // would it be found there ?
               continue;                   // no ->
         }
         // wrapped objects love movin ahead ...
      }
      else
      {
         if( dst_index > i)               // hmm search wrapped, object is as
            continue;                     // happy as a pig in a sty
         if( search_start > dst_index)    // would it be found there ?
            continue;                     // no ->
      }
      // move it up
      storage[ dst_index]        = *q;
      storage[ dst_index + size] = q[ size];
      dst_index                  = i;
   }

   return( 1);
}


void   _mulle__pointermap_shrink_if_needed( struct mulle__pointermap *map,
                                           struct mulle_allocator *allocator)
{
   if( _mulle__pointermap_is_sparse( map))
      shrink( map, allocator);
}


void   _mulle__pointermap_insert_values_for_keysv( struct mulle__pointermap *map,
                                                  void *firstvalue,
                                                  void *firstkey,
                                                  va_list args,
                                                  struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair._value = firstvalue;
   pair._key   = firstkey;
   while( pair._key)
   {
      _mulle__pointermap_insert_pair( map, &pair, allocator);

      pair._value = va_arg( args, void *);
      pair._key   = va_arg( args, void *);
   }
}


#pragma mark - copying

int   _mulle__pointermap_copy_items( struct mulle__pointermap *dst,
                                    struct mulle__pointermap *src,
                                    struct mulle_allocator *allocator)
{
   struct mulle__pointermapenumerator  rover;
   struct mulle_pointerpair           *item;
   int                                rval;

   rval  = 0;
   rover = mulle__pointermap_enumerate( src);
   while( item = _mulle__pointermapenumerator_next( &rover))
      if( _mulle__pointermap_insert_pair( dst, item, allocator))
      {
         rval = -1;
         break;
      }
   mulle__pointermapenumerator_done( &rover);
   return( rval);
}


struct mulle__pointermap   *_mulle__pointermap_copy( struct mulle__pointermap *set,
                                                    struct mulle_allocator *allocator)
{
   struct mulle__pointermap   *other;

   other = mulle__pointermap_create( _mulle__pointermap_get_count( set), 0, allocator);
   if( _mulle__pointermap_copy_items( other, set, allocator))
   {
      _mulle__pointermap_destroy( other, allocator);
      other = NULL;
   }
   return( other);
}


// use this only for debugging
char   *_mulle__pointermap_describe( struct mulle__pointermap *set,
                                    struct mulle_allocator *allocator)
{
   char                                *result;
   char                                *key;
   char                                *value;
   int                                 separate;
   size_t                              len;
   size_t                              key_len;
   size_t                              value_len;
   struct mulle__pointermapenumerator  rover;
   struct mulle_pointerpair            *item;
   struct mulle_allocator              *key_allocator;
   struct mulle_allocator              *value_allocator;

   result = NULL;
   len    = 0;
   rover  = mulle__pointermap_enumerate( set);
   while( item = _mulle__pointermapenumerator_next( &rover))
   {
      key_allocator   = allocator ? allocator : &mulle_default_allocator;
      value_allocator = key_allocator;

      key        = item->_key;
      key_len    = strlen( key);
      value      = item->_value;
      value_len  = strlen( value);

      separate = result != NULL;

      result = mulle_allocator_realloc( allocator, result, len + (separate * 2) + key_len + 3 + value_len + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len   += 2;
      }

      memcpy( &result[ len], key, key_len);
      len += key_len;

      memcpy( &result[ len], " = ", 3);
      len += 3;

      memcpy( &result[ len], value, value_len);
      len += value_len;

      if( key_allocator)
         mulle_allocator_free( key_allocator, key);
      if( value_allocator)
         mulle_allocator_free( value_allocator, value);
   }
   mulle__pointermapenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}

