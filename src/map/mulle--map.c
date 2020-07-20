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

#include "mulle--map.h"

#include "mulle-container-operation.h"
#include "mulle-container-math.h"
#include "mulle-prime.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#define _MULLE_MAP_INITIAL_SIZE  4

// static const void   *dummy_notakey_storage[ _MULLE_MAP_INITIAL_SIZE * 2];

#pragma clang diagnostic ignored "-Wshift-count-overflow"


// assume we have size 4
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 or 2 (leave odd alone) for better insert perfomance
//
// (?)
static unsigned int   _mulle__map_hash_for_size( unsigned int hash, unsigned int size)
{
   assert( size >= 2);

   //
   // so if size is pow2, size-1 is the mask
   // now sub one again to kill lowest bit (which will be set)
   // if size is 0, this doesn't work though
   //
   return( hash & (size - 2)); // kill lowest bit (it will be)
}



void   *_mulle__map_write( struct mulle__map *map,
                           struct mulle_pointerpair *p,
                           unsigned int  hash,
                           enum mulle_container_write_mode mode,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator);
/*
 *
 */


static inline int   is_full( struct mulle__map *map, unsigned int size)
{
   return( map->count >= (size - (size >> 2)));  // full when only 25% free
}



#pragma mark - setup and teardown


static void   **allocate_storage( unsigned int n,
                                  void *notakey,
                                  struct mulle_allocator *allocator)
{
   void   **buf;
   void   **p;
   void   **sentinel;

   if( ! n)
      return( NULL);

   assert( n >= 2);
   assert( mulle_is_pow2( n));

   if( ! notakey)
      return( mulle_allocator_calloc( allocator, n, sizeof( void *) * 2));

   buf      = mulle_allocator_malloc( allocator, n * sizeof( void *) * 2);

   p        = &buf[ 0];
   sentinel = &p[ n];
   while( p < sentinel)
      *p++ = notakey;

   memset( &buf[ n], 0, n * sizeof( void *));
   return( buf);
}


void   _mulle__map_init( struct mulle__map *p,
                         unsigned int capacity,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
{
   assert_mulle_container_keyvaluecallback( callback);

   p->count   = 0;

   //
   // our map requires zeroes to find an end so give it ~25% holes
   // The code (especially get) depends on storage being there. For the case
   // that notakey is nil,
   // to be there already, so we can't have p->size == 0
   p->size    = capacity >= MULLE_MAP_MIN_SIZE
                     ? mulle_pow2round( capacity + (capacity >> MULLE_MAP_FILL_SHIFT))
                     : (callback->keycallback.notakey != 0 ? MULLE_MAP_MIN_SIZE : 0);
   p->storage = allocate_storage( p->size, callback->keycallback.notakey, allocator);
}


struct mulle__map   *_mulle__map_create( unsigned int capacity,
                                         size_t extra,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator)
{
   struct mulle__map   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__map) + extra);
   _mulle__map_init( p, capacity, callback, allocator);
   return( p);
}


static inline void _mulle__map_free_storage( struct mulle__map *map,
                                             struct mulle_allocator *allocator)
{
//   if( map->storage != (void **) dummy_notakey_storage)
      mulle_allocator_free( allocator, map->storage);
}


void   _mulle__map_done( struct mulle__map *map,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
{
   struct mulle__mapenumerator   rover;
   struct mulle_pointerpair      *pair;

   //
   // if keycallback and valuecallback are nop, we can just skip this
   //
   if( mulle_container_keyvaluecallback_releases( callback))
   {
      rover = mulle__map_enumerate( map, callback);
      while( pair = _mulle__mapenumerator_next( &rover))
      {
         (callback->keycallback.release)( &callback->keycallback, pair->_key, allocator);
         (callback->valuecallback.release)( &callback->valuecallback, pair->_value, allocator);
      }
      mulle__mapenumerator_done( &rover);
   }

   _mulle__map_free_storage( map, allocator);
}


void   _mulle__map_destroy( struct mulle__map *map,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle__map_done( map, callback, allocator);
   mulle_allocator_free( allocator, map);
}


void   _mulle__map_reset( struct mulle__map *map,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
{
   _mulle__map_done( map, callback, allocator);
   _mulle__map_init( map, 0, callback, allocator);
}


#pragma mark - mechanisms

static inline void   store_key_value( void **storage,
                                      unsigned int size,
                                      unsigned int i,
                                      void *key,
                                      void *value,
                                      void *notakey)
{
   while( storage[ i] != notakey)
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
                            unsigned int src_size,
                            struct mulle_container_keycallback *callback)
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
      if( key != callback->notakey)
      {
         hash = (unsigned int) (callback->hash)( callback, key);
         i    = _mulle__map_hash_for_size( hash, dst_size);
         store_key_value( dst, dst_size, i, key, src[ src_size], callback->notakey);
      }
      ++src;
   }
}


static void   grow( struct mulle__map *map,
                    struct mulle_container_keycallback *callback,
                    struct mulle_allocator *allocator)
{
   void           **buf;
   unsigned int   new_size;

   new_size = map->size * 2;
   if( new_size < map->size)
      abort();  // overflow

   assert( _MULLE_MAP_INITIAL_SIZE >= 2);

   if( new_size == 0)
      new_size = _MULLE_MAP_INITIAL_SIZE;

   buf = allocate_storage( new_size, callback->notakey, allocator);
   copy_storage( buf, new_size, map->storage, map->size, callback);
   _mulle__map_free_storage( map, allocator);

   map->storage = buf;
   map->size    = new_size;
}


static void   shrink( struct mulle__map *map,
                      struct mulle_container_keycallback *callback,
                      struct mulle_allocator *allocator)
{
   void           **buf;
   unsigned int   new_size;

   new_size = map->size / 2;
   if( new_size < _MULLE_MAP_INITIAL_SIZE)
      return;

   buf = allocate_storage( new_size, callback->notakey, allocator);
   copy_storage( buf, new_size, map->storage, map->size, callback);
   _mulle__map_free_storage( map, allocator);

   map->storage = buf;
   map->size    = new_size;
}



static unsigned long  _find_index( void **storage,
                                   unsigned int size,
                                   void *key,
                                   void *q,
                                   unsigned int i,
                                   unsigned int *hole_index,
                                   struct mulle_container_keycallback *callback)
{
   int            (*f)( void *, void *, void *);
   void           *param1;
   void           *param2;
   void           *notakey;
   unsigned int   mask;

   f       = (int (*)()) callback->is_equal;
   param1  = callback;
   param2  = key;
   notakey = callback->notakey;
   mask    = size - 1;

   do
   {
      if( (*f)( param1, param2, q))
         return( i);

      i = (i + 1) & mask;
      q = storage[ i];
   }
   while( q != notakey);

   *hole_index = i;
   return( mulle_not_found_e);
}


static inline unsigned long  find_index( void **storage,
                                         unsigned int size,
                                         void *key,
                                         unsigned int  hash,
                                         unsigned int *hole_index,
                                         struct mulle_container_keycallback *callback)
{
   void           *q;
   unsigned int   i;

   assert( storage);

   i = _mulle__map_hash_for_size( hash, size);
   q = storage[ i];
   if( q == callback->notakey)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }

   return( _find_index( storage, size, key, q, i, hole_index, callback));
}


#pragma mark - operations

void   *_mulle__map_write( struct mulle__map *map,
                           struct mulle_pointerpair *pair,
                           unsigned int hash,
                           enum mulle_container_write_mode mode,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   unsigned long              found;
   unsigned int               i;
   void                       **q;
   struct mulle_pointerpair   new_pair;

   assert( pair->_key != callback->keycallback.notakey);

   if( map->count)
   {
      unsigned int   size;
      void           **storage;  // only valid in this no-grow block
      unsigned int   hole_index;

      storage    = map->storage;
      size       = map->size;

      hole_index = 0xfeedface;  // for the analyzer
      found      = find_index( storage, size, pair->_key, hash, &hole_index, &callback->keycallback);

      if( found != mulle_not_found_e)
      {
         i = (unsigned int) found;
         q = &storage[ i];
         switch( mode)
         {
         case mulle_container_overwrite_e :
            q = &storage[ i];
            if( q[ size] == pair->_value && *q == pair->_key)
               return( q[ size]);

            new_pair._key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key, allocator);
            new_pair._value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value, allocator);

            (callback->keycallback.release)( &callback->keycallback, *q, allocator);
            (callback->valuecallback.release)( &callback->valuecallback, q[ size], allocator);

            *q       = new_pair._key;
            q[ size] = new_pair._value;
            return( q[ size]);

         case mulle_container_insert_e :
            return( q[ size]);
         }
      }

      i = hole_index;
      if( ! _mulle__map_is_full( map))
      {
         new_pair._key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key, allocator);
         new_pair._value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value, allocator);

         storage[ i]        = new_pair._key;
         storage[ i + size] = new_pair._value;
         map->count++;
         return( NULL);
      }
   }

   if( _mulle__map_is_full( map))
      grow( map, &callback->keycallback, allocator);

   i               = _mulle__map_hash_for_size( hash, map->size);
   new_pair._key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key, allocator);
   new_pair._value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value, allocator);

   store_key_value( map->storage, map->size, i, new_pair._key, new_pair._value, callback->keycallback.notakey);
   map->count++;

   return( NULL);
}


void   *_mulle__map_get_with_hash( struct mulle__map *map,
                                   void *key,
                                   unsigned int hash,
                                   struct mulle_container_keyvaluecallback *callback)
{
   int            (*f)( void *, void *, void *);
   unsigned int   i;
   unsigned int   size;
   unsigned int   mask;
   void           *found;
   void           **storage;
   void           *notakey;

   // important to not hit a NULL storage later
   // size must be > 2 for the hash to work, otherwise we could get
   if( map->count == 0)
      return( NULL);

   storage = map->storage;
   size    = map->size;
   i       = _mulle__map_hash_for_size( hash, size);
   f       = (int (*)()) callback->keycallback.is_equal;
   notakey = callback->keycallback.notakey;
   mask    = size - 1;

   for(;;)
   {
      found = storage[ i];
      if( found == notakey)
         break;
      if( key == found)
         break;
      if( (*f)( callback, found, key))
         break;
      i = (i + 1) & mask;
   }

   return( storage[ i + size]);
}


struct mulle_pointerpair   *_mulle__map_get_any_pair( struct mulle__map *map,
                                                      struct mulle_container_keyvaluecallback *callback,
                                                      struct mulle_pointerpair *space)
{
   unsigned int   i;
   unsigned int   size;
   unsigned int   mask;
   unsigned int   hash;
   void           *found;
   void           **storage;
   void           *notakey;

   if( ! map->count)
      return( NULL);

   storage = map->storage;
   size    = map->size;
   notakey = callback->keycallback.notakey;
   mask    = size - 1;

   //
   // we use a random starting point, it doesn't matter what space->_key is
   // if that happens to be the previous key, though this could be
   // fairly optimal ? when you are deleting contents with this in a loop
   // or ?
   //
   hash  = (unsigned int) (*callback->keycallback.hash)( &callback->keycallback, space->_key);
   i     = _mulle__map_hash_for_size( hash, size);

   for(;;)
   {
      found = storage[ i];
      if( found != notakey)
         return( storage[ i + size]);
      i = (i + 1) & mask;
   }
}


static void   *
   _mulle__map_pointerequalitysearch( struct mulle__map *map,
                                       void *key)
{
   void     **q;
   void     **sentinel;

   q        = map->storage;
   sentinel = &q[ map->size];

   //
   // first we search just for pointer equality of the key,
   // which can't be notakey so we don't care about notakey here
   //
   for( ; q < sentinel; q++)
      if( key == *q)
         return( q[ map->size]);

   return( NULL);
}



void   *_mulle__map_get( struct mulle__map *map,
                         void *key,
                         struct mulle_container_keyvaluecallback *callback)
{
   unsigned int   hash;
   void           *value;

   assert( map);
   if( map->size <= 32)  // if the dictionary is small try to easy match
   {
      value = _mulle__map_pointerequalitysearch( map, key);
      if( value)
         return( value);
      // else do regular search
   }

   hash  = (unsigned int) (*callback->keycallback.hash)( &callback->keycallback, key);
   value = _mulle__map_get_with_hash( map, key, hash, callback);
   return( value);
}


void   _mulle__map_set( struct mulle__map *map,
                        struct mulle_pointerpair *pair,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   _mulle__map_write( map, pair, hash, mulle_container_overwrite_e, callback, allocator);
}


void    *_mulle__map_insert( struct mulle__map *map,
                             struct mulle_pointerpair *pair,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator)
{
   uintptr_t   hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   return( _mulle__map_write( map, pair, hash, mulle_container_insert_e, callback, allocator));
}


void   *_mulle__map_insert_known_absent( struct mulle__map *map,
                                         struct mulle_pointerpair *pair,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int ) (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   return( _mulle__map_write( map, pair, hash, mulle_container_insert_e, callback, allocator));
}



int   _mulle__map_remove_with_hash( struct mulle__map *map,
                                    void *key,
                                    unsigned int  hash,
                                    struct mulle_container_keyvaluecallback *callback,
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

   found = find_index( storage, size, key, hash, &hole_index, &callback->keycallback);
   if( found == mulle_not_found_e)
      return( 0);

   i = (unsigned int) found;
   q = &storage[ i];
   (callback->keycallback.release)( &callback->keycallback, *q, allocator);  // get rid of it
   (callback->valuecallback.release)( &callback->valuecallback, q[ size], allocator);  // get rid of it
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
      if( *q == callback->keycallback.notakey)
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
      hash         = (unsigned int ) (*callback->keycallback.hash)( &callback->keycallback, *q);
      search_start = _mulle__map_hash_for_size( hash, size);

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


void   _mulle__map_shrink( struct mulle__map *map,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   assert( _mulle__map_is_sparse( map));
   shrink( map, &callback->keycallback, allocator);
}


int   _mulle__map_remove( struct mulle__map *map,
                          void *key,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->keycallback.hash)( &callback->keycallback, key);
   return( _mulle__map_remove_with_hash( map, key, hash, callback, allocator));
}


void   _mulle__map_insert_values_for_keysv( struct mulle__map *map,
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
      _mulle__map_insert( map, &pair, callback, allocator);

      pair._value = va_arg( args, void *);
      pair._key   = va_arg( args, void *);
   }
}


#pragma mark - copying

int   _mulle__map_copy_items( struct mulle__map *dst,
                              struct mulle__map *src,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator)
{
   struct mulle__mapenumerator  rover;
   struct mulle_pointerpair     *item;
   int                          rval;

   rval  = 0;
   rover = mulle__map_enumerate( src, callback);
   while( item = _mulle__mapenumerator_next( &rover))
      if( _mulle__map_insert( dst, item, callback, allocator))
      {
         rval = -1;
         break;
      }
   mulle__mapenumerator_done( &rover);
   return( rval);
}


struct mulle__map   *_mulle__map_copy( struct mulle__map *set,
                                       struct mulle_container_keyvaluecallback *callback,
                                       struct mulle_allocator *allocator)
{
   struct mulle__map   *other;

   other = _mulle__map_create( _mulle__map_get_count( set), 0, callback, allocator);
   if( _mulle__map_copy_items( other, set, callback, allocator))
   {
      _mulle__map_destroy( other, callback, allocator);
      other = NULL;
   }
   return( other);
}


// use this only for debugging
char   *_mulle__map_describe( struct mulle__map *set,
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
   struct mulle__mapenumerator   rover;
   struct mulle_pointerpair      *item;
   struct mulle_allocator        *key_allocator;
   struct mulle_allocator        *value_allocator;

   result = NULL;
   len    = 0;
   rover = mulle__map_enumerate( set, callback);
   while( item = _mulle__mapenumerator_next( &rover))
   {
      key_allocator   = allocator ? allocator : &mulle_default_allocator;
      value_allocator = key_allocator;

      key        = (*callback->keycallback.describe)( &callback->keycallback, item->_key, &key_allocator);
      key_len    = strlen( key);
      value      = (*callback->valuecallback.describe)( &callback->valuecallback, item->_value, &value_allocator);
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
   mulle__mapenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


// use this only for debugging
size_t   _mulle__map_count_collisions( struct mulle__map *set,
                                       struct mulle_container_keyvaluecallback *callback,
                                       size_t *perfects)
{
   unsigned int                  i;
   unsigned int                  search_start;
   struct mulle__mapenumerator   rover;
   size_t                        collisions;
   unsigned int                  hash;
   void                          **sentinel;
   void                          *key;
   unsigned int                  distance;
   size_t                        dummy;

   rover      = mulle__map_enumerate( set, callback);
   sentinel   = &rover._curr[ set->size];
   collisions = 0;
   if( ! perfects)
      perfects = &dummy;
   *perfects  = 0;
   for( i = 0 ;rover._curr < sentinel; ++rover._curr, i++)
   {
      key = *rover._curr;
      if( key == rover.notakey)
         continue;

      hash         = (unsigned int) (*callback->keycallback.hash)( &callback->keycallback, key);
      search_start = _mulle__map_hash_for_size( hash, set->size);
      // search_start is where the table  starts to search and i is where it
      // will find the current key
      if( search_start <= i)
      {
         distance = i - search_start;
         if( ! distance)
            ++*perfects;
      }
      else
         distance = set->size - search_start + i; // needs to wrap
      collisions += distance;
   }
   return( collisions);
}

