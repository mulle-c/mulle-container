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

#include "_mulle_map.h"

#include "mulle_container_operation.h"
#include "mulle_prime.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>


static inline size_t   _mulle_map_mask_for_depth( short depth)
{
   return( (1UL << depth) - 1);
}


// assume we have
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 an 2 for better insert perfomance
//
// 0 2
static uintptr_t   _mulle_map_hash_for_modulo( uintptr_t hash, size_t modulo)
{
   modulo &= ~1;
   hash   &= modulo;
   return( hash);
}



void   *_mulle_map_write( struct _mulle_map *map,
                          struct mulle_pointerpair *p,
                          uintptr_t hash,
                          enum mulle_container_write_mode mode,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator);
/*
 *
 */
#define _MULLE_MAP_INITIAL_DEPTH    1
#define _MULLE_MAP_MAX_DEPTH        30

//
// with 4, large tables will be 1 MB pretty quickly even with little filling
// try 8KB sometime and see the kernel go on a VM rampage, because of some
// throttling mechanism its actually slowing down things...
//
#define _MULLE_MAP_MIN_INITIAL_DEPTH    1
#define _MULLE_MAP_MAX_INITIAL_DEPTH    3


static inline size_t   is_full( struct _mulle_map *map, size_t size)
{
   return( map->_count >= (size - (size >> 1)));  // full when only 25% free
}



static short   depth_for_capacity( size_t capacity)
{
   unsigned int  i;

   capacity >>= 1;
   for( i = _MULLE_MAP_MIN_INITIAL_DEPTH; i <= _MULLE_MAP_MAX_INITIAL_DEPTH; i++)
      if( capacity <= _mulle_map_size_for_depth( i))
         return( (short) i);
   return( _MULLE_MAP_MAX_INITIAL_DEPTH);
}

#pragma mark -
#pragma mark setup and teardown


static struct mulle_pointerpair   *allocate_pairs( size_t n,
                                                     void *notakey,
                                                     struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   *buf;
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   if( ! notakey)
      return( mulle_allocator_calloc( allocator, n, sizeof( struct mulle_pointerpair)));

   buf = mulle_allocator_malloc( allocator, n * sizeof( struct mulle_pointerpair));
   p        = &buf[ 0];
   sentinel = &buf[ n];
   while( p < sentinel)
   {
      p->_key = notakey;
      ++p;
   }
   return( buf);
}



void   _mulle_map_init( struct _mulle_map *p,
                        size_t capacity,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
   size_t   n;
   short    depth;

   assert_mulle_container_keyvaluecallback( callback);

   depth       = depth_for_capacity( capacity);
   n           = _mulle_map_size_for_depth( depth);
   p->_count   = 0;
   p->_depth   = depth;
   p->_storage = allocate_pairs( n, callback->keycallback.notakey, allocator);
}


struct _mulle_map   *_mulle_map_create( size_t capacity,
                                       size_t extra,
                                       struct mulle_container_keyvaluecallback *callback,
                                       struct mulle_allocator *allocator)
{
   struct _mulle_map   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct _mulle_map) + extra);
   _mulle_map_init( p, capacity, callback, allocator);
   return( p);
}


void   _mulle_map_done( struct _mulle_map *map,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
   struct _mulle_mapenumerator   rover;
   struct mulle_pointerpair    *pair;

   rover = _mulle_map_enumerate( map, callback);
   while( pair = _mulle_mapenumerator_next( &rover))
   {
      (callback->keycallback.release)( &callback->keycallback, pair->_key, allocator);
      (callback->valuecallback.release)( &callback->valuecallback, pair->_value, allocator);
   }
   _mulle_mapenumerator_done( &rover);

   mulle_allocator_free(allocator, map->_storage);
}


void   _mulle_map_destroy( struct _mulle_map *map,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   _mulle_map_done( map, callback, allocator);
   mulle_allocator_free( allocator, map);
}




void   _mulle_map_reset( struct _mulle_map *map,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
   _mulle_map_done( map, callback, allocator);
   _mulle_map_init( map,
                   _mulle_map_size_for_depth( map->_depth),
                   callback,
                   allocator);
}


#pragma mark -
#pragma mark mechanisms

static inline void   store_key_value_pair( struct mulle_pointerpair *data,
                                           size_t i,
                                           size_t modulo,
                                           struct mulle_pointerpair *p,
                                           struct mulle_container_keycallback *callback)
{
   size_t     limit;

   limit = modulo + 1;
   while( data[ i]._key != callback->notakey)
   {
      if( ++i < limit)
         continue;
      i = 0;
   }

   data[ i] = *p;
}


//
// this is not a "deep copy", the assumption is that the src will
// disappear after copy
//
static void   copy_maps( struct mulle_pointerpair *dst,
                            size_t modulo,
                            struct mulle_pointerpair *src,
                            size_t n,
                            struct mulle_container_keycallback *callback)
{
   struct mulle_pointerpair *p;
   size_t     i;
   uintptr_t  hash;

   assert( modulo + 1 >= n);

   for( ;n--;)
   {
      p = src++;
      if( p->_key == callback->notakey)
         continue;

      hash = (callback->hash)( callback, p->_key);
      i    = _mulle_map_hash_for_modulo( hash, modulo);
      store_key_value_pair( dst, i, modulo, p, callback);
   }
}

static size_t   grow( struct _mulle_map *map,
                      struct mulle_container_keycallback *callback,
                      struct mulle_allocator *allocator)
{
   size_t                       modulo;
   size_t                       new_modulo;
   struct mulle_pointerpair   *buf;
   short                        depth;

   // for good "not found" performance, there should be a high possibility of
   // a NULL after each slot
   //
   depth  = map->_depth;
   modulo = _mulle_map_mask_for_depth( depth);
   if( depth == _MULLE_MAP_MAX_DEPTH)
   {
      static int  complain;

      if( ! complain)
      {
         fprintf( stderr, "%s: Your hash is so weak, that you reached the end of the rope", __FUNCTION__);
         ++complain;
      }
      return( modulo);
   }

   new_modulo = _mulle_map_mask_for_depth( ++depth);

   buf = allocate_pairs( new_modulo + 1, callback->notakey, allocator);

   copy_maps( buf, new_modulo, map->_storage, modulo + 1, callback);
   mulle_allocator_free( allocator, map->_storage);

   map->_depth   = depth;
   map->_storage = buf;

   return( new_modulo);
}


static size_t  _find_index( struct mulle_pointerpair  *storage,
                            void *key,
                            struct mulle_pointerpair *q,
                            size_t limit,
                            size_t i,
                            size_t *hole_index,
                            struct mulle_container_keycallback *callback)
{
   int   (*f)( void *, void *, void *);
   void   *param1;
   void   *param2;
   void   *notAKey;

   f      = (int (*)()) callback->is_equal;
   param1 = callback;
   param2 = key;
   notAKey = callback->notakey;
   do
   {
      if( (*f)( param1, param2, q->_key))
         return( i);

      if( ++i >= limit)
         i = 0;

      q = &storage[ i];
   }
   while( q->_key != notAKey);

   *hole_index = i;
   return( mulle_not_found_e);
}


static inline size_t  find_index( struct mulle_pointerpair *storage,
                                  void *key,
                                  uintptr_t hash,
                                  size_t modulo,
                                  size_t *hole_index,
                                  struct mulle_container_keycallback *callback)
{
   struct mulle_pointerpair   *q;
   size_t                       i;

   assert( storage);

   i = _mulle_map_hash_for_modulo( hash, modulo);
   q = &storage[ i];
   if( q->_key == callback->notakey)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }

   i = _find_index( storage, key, q, modulo + 1, i, hole_index, callback);
   return( i);
}


#pragma mark -
#pragma mark operations

void   *_mulle_map_write( struct _mulle_map *map,
                          struct mulle_pointerpair *pair,
                          uintptr_t hash,
                          enum mulle_container_write_mode mode,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
{
   size_t                       hole_index;
   size_t                       i;
   size_t                       modulo;
   struct mulle_pointerpair   *q;
   struct mulle_pointerpair   new_pair;

   assert( pair->_key != callback->keycallback.notakey);

   modulo     = _mulle_map_mask_for_depth( map->_depth);
   hole_index = 0xfeedface;  // for the analyzer
   i          = find_index( map->_storage, pair->_key, hash, modulo, &hole_index, &callback->keycallback);

   if( i != (size_t) mulle_not_found_e)
   {
      q = &map->_storage[ i];
      switch( mode)
      {
      case mulle_container_overwrite_e :
         q = &map->_storage[ i];
         if( q->_value == pair->_value && q->_key == pair->_key)
            return( q->_value);

         new_pair._key = (*callback->keycallback.retain)( &callback->keycallback, pair->_key, allocator);
         new_pair._value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value, allocator);

         (callback->keycallback.release)( &callback->keycallback, q->_key, allocator);
         (callback->valuecallback.release)( &callback->valuecallback, q->_value, allocator);

         *q = new_pair;
         return( q->_value);

      case mulle_container_insert_e :
         return( q->_value);
      }
   }

   new_pair._key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key, allocator);
   new_pair._value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value, allocator);

   i = hole_index;
   if( ! is_full( map, modulo))
   {
      map->_storage[ i] = new_pair;
      map->_count++;
      return( NULL);
   }

   modulo = grow( map, &callback->keycallback, allocator);

   i = _mulle_map_hash_for_modulo( hash, modulo);
   store_key_value_pair( map->_storage, i, modulo, &new_pair, &callback->keycallback);
   map->_count++;

   return( NULL);
}


void   *_mulle_map_get_with_hash( struct _mulle_map *map,
                         void *p,
                         uintptr_t hash,
                         struct mulle_container_keyvaluecallback *callback)
{
   struct mulle_pointerpair *q;
   int         (*f)( void *, void *, void *);
   size_t   modulo;
   size_t   limit;
   size_t   i;
   void         *no_key;

   modulo = _mulle_map_mask_for_depth( map->_depth);
   i      = _mulle_map_hash_for_modulo( hash, modulo);
   limit  = modulo + 1;
   no_key = callback->keycallback.notakey;

   f = (int (*)()) callback->keycallback.is_equal;
   while( (q = &map->_storage[ i])->_key != no_key)
   {
      if( p == q->_key)
         break;
      if( (*f)( callback, q->_key, p))
         break;
      if( ++i >= limit)
         i = 0;
   }

   return( q->_value);
}


void   *_mulle_map_get( struct _mulle_map *map,
                        void *p,
                        struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t     hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, p);
   return( _mulle_map_get_with_hash( map, p, hash, callback));
}


void   _mulle_map_set( struct _mulle_map *map,
                       struct mulle_pointerpair *pair,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
{
   uintptr_t   hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   _mulle_map_write( map, pair, hash, mulle_container_overwrite_e, callback, allocator);
}


void    *_mulle_map_insert( struct _mulle_map *map,
                            struct mulle_pointerpair *pair,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
{
   uintptr_t   hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   return( _mulle_map_write( map, pair, hash, mulle_container_insert_e, callback, allocator));
}


void   *_mulle_map_insert_known_absent( struct _mulle_map *map,
                                        struct mulle_pointerpair *pair,
                                        struct mulle_container_keyvaluecallback *callback,
                                        struct mulle_allocator *allocator)
{
   uintptr_t hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);
   return( _mulle_map_write( map, pair, hash, mulle_container_insert_e, callback, allocator));
}



int   _mulle_map_remove_with_hash( struct _mulle_map *map,
                                   void *key,
                                   uintptr_t hash,
                                   struct mulle_container_keyvaluecallback *callback,
                                   struct mulle_allocator *allocator)
{
   size_t                       hole_index;
   size_t                       i;
   size_t                       modulo;
   struct mulle_pointerpair   *q;
   size_t                       dst_index;
   size_t                       search_start;

   modulo = _mulle_map_mask_for_depth( map->_depth);
   i      = find_index( map->_storage, key, hash, modulo, &hole_index, &callback->keycallback);
   if( i == (size_t) mulle_not_found_e)
      return( 0);

   q = &map->_storage[ i];
   (callback->keycallback.release)( &callback->keycallback, q->_key, allocator);  // get rid of it
   (callback->valuecallback.release)( &callback->valuecallback, q->_value, allocator);  // get rid of it
   map->_count--;

   // now we may need to do a whole lot of shifting, if
   // the following object isn't in its proper hash index.
   // Because of the way objects are inserted and deleted
   // we have to possibly move EVERYTHING until we hit the
   // hole.

   dst_index = i;

   for(;;)
   {
      i = (i + 1) & modulo;

      q = &map->_storage[ i];
      if( q->_key == callback->keycallback.notakey)
      {
         map->_storage[ dst_index]._key   = callback->keycallback.notakey;
         map->_storage[ dst_index]._value = NULL;
         break;
      }

      // check if its in the slot it's expected
      // otherwise it won't be found because of
      // the "hole" in the front we just made
      // but keep going because "wrong" hashes might be behind
      // be sure not to move items that won't be found
      hash         = (*callback->keycallback.hash)( &callback->keycallback, q->_key);
      search_start = _mulle_map_hash_for_modulo( hash, modulo);

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


      if( search_start > i)  // object from wrapped insert ?
      {                          //
         if( dst_index > i)  // would this move to a non-wrapped position ?
         {
            if( search_start > dst_index)  // would it be found there ?
               continue;                   // no ->
         }
         // wrapped objects love movin ahead ...
      }
      else
      {
         if( dst_index > i)           // hmm search wrapped, object is as happy as a pig in a sty
            continue;
         if( search_start > dst_index)    // would it be found there ?
            continue;                     // no ->
      }
      // move it up
      map->_storage[ dst_index] = *q;
      dst_index = i;
   }
   return( 1);
}


int   _mulle_map_remove( struct _mulle_map *map,
                         void *key,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
{
   uintptr_t hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, key);

   return( _mulle_map_remove_with_hash( map, key, hash, callback, allocator));
}


void   _mulle_map_insert_values_for_keysv( struct _mulle_map *map,
                                           void *firstvalue,
                                           void *firstkey,
                                           va_list args,
                                           struct mulle_container_keyvaluecallback *callback,
                                           struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair._value = firstvalue;
   pair._key   = firstkey;
   while( pair._key != callback->keycallback.notakey)
   {
      _mulle_map_insert( map, &pair, callback, allocator);

      pair._value = va_arg( args, void *);
      pair._key   = va_arg( args, void *);
   }
}


#pragma mark -
#pragma mark copying

int   _mulle_map_copy_items( struct _mulle_map *dst,
                             struct _mulle_map *src,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator)
{
   struct _mulle_mapenumerator  rover;
   struct mulle_pointerpair   *item;
   int                          rval;

   rval  = 0;
   rover = _mulle_map_enumerate( src, callback);
   while( item = _mulle_mapenumerator_next( &rover))
      if( _mulle_map_insert( dst, item, callback, allocator))
      {
         rval = -1;
         break;
      }
   _mulle_mapenumerator_done( &rover);
   return( rval);
}


struct _mulle_map   *_mulle_map_copy( struct _mulle_map *set,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct _mulle_map   *other;

   other = _mulle_map_create( _mulle_map_get_count( set), 0, callback, allocator);
   if( _mulle_map_copy_items( other, set, callback, allocator))
   {
      _mulle_map_destroy( other, callback, allocator);
      other = NULL;
   }
   return( other);
}


// use this only for debugging
char   *_mulle_map_describe( struct _mulle_map *set,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator)
{
   char                          *result;
   char                          *key;
   char                          *value;
   int                           separate;
   size_t                        len;
   size_t                        key_len;
   size_t                        value_len;
   struct _mulle_mapenumerator   rover;
   struct mulle_pointerpair    *item;

   result = NULL;
   len  = 0;
   rover = _mulle_map_enumerate( set, callback);
   while( item = _mulle_mapenumerator_next( &rover))
   {
      key        = (*callback->keycallback.describe)( &callback->keycallback, item->_key, allocator);
      key_len    = strlen( key);
      value      = (*callback->valuecallback.describe)( &callback->valuecallback, item->_value, allocator);
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

      mulle_allocator_free( allocator, key);
      mulle_allocator_free( allocator, value);
   }
   _mulle_mapenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}

