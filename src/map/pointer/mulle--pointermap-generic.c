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

#include "mulle--pointermap-generic.h"

#include "mulle-container-operation.h"
#include "mulle-container-math.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


void   **mulle__pointermap_allocate_storage_generic( size_t n,
                                                     void *notakey,
                                                     struct mulle_allocator *allocator)
{
   void   **buf;
   void   **p;
   void   **sentinel;

   assert( n >= 2);
   assert( mulle_is_pow2( n));

   if( notakey == NULL)
      return( mulle_allocator_calloc( allocator, n, sizeof( void *) * 2));

   buf      = mulle_allocator_malloc( allocator, n * sizeof( void *) * 2);

   // only wipe keys
   p        = &buf[ 0];
   sentinel = &buf[ n];
   while( p < sentinel)
      *p++ = notakey;

   return( buf);
}


//
// could reuse _mulle__pointerset_reset_generic for this
//
void   _mulle__pointermap_reset_generic( struct mulle__pointermap *map,
                                         void *notakey)
{
   void   **p;
   void   **sentinel;

   p        = map->_storage;
   sentinel = &p[ map->_size];
   while( p < sentinel)
      *p++ = notakey;

   map->_count = 0;
}



#pragma mark - insert / remove / get

static inline void   store_key_value_generic( void **storage,
                                              size_t size,
                                              size_t i,
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
// disappear after copy, so values won't be retained
//
static void   copy_storage_generic( void **dst,
                                    size_t dst_size,
                                    void **src,
                                    size_t src_size,
                                    struct mulle_container_keycallback *callback)
{
   void        *key;
   void        **sentinel;
   size_t      i;
   uintptr_t   hash;

   assert( mulle_is_pow2( dst_size));
   assert( mulle_is_pow2( src_size));

   sentinel = &src[ src_size];
   while( src < sentinel)
   {
      key = *src;
      if( key != callback->notakey)
      {
         hash = (callback->hash)( callback, key);
         i    = mulle__pointermap_hash_for_size( hash, dst_size);
         store_key_value_generic( dst,
                                  dst_size,
                                  i,
                                  key,
                                  src[ src_size],
                                  callback->notakey);
      }
      ++src;
   }
}


static void   grow_generic( struct mulle__pointermap *map,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   void           **buf;
   size_t   new_size;

   new_size = map->_size * 2;
   if( new_size < map->_size)
      abort();  // overflow

   assert( MULLE__POINTERMAP_INITIAL_SIZE >= 2);

   if( new_size == 0)
      new_size = MULLE__POINTERMAP_INITIAL_SIZE;

   buf = mulle__pointermap_allocate_storage_generic( new_size, callback->notakey, allocator);
   copy_storage_generic( buf, new_size, map->_storage, map->_size, callback);
   mulle_allocator_free( allocator, map->_storage);

   map->_storage = buf;
   map->_size    = new_size;
}


static unsigned long  _find_index_generic( void **storage,
                                           size_t size,
                                           void *key,
                                           void *q,
                                           size_t i,
                                           size_t *hole_index,
                                           struct mulle_container_keycallback *callback)
{
   int      (*f)( void *, void *, void *);
   void     *param1;
   void     *param2;
   void     *notakey;
   size_t   mask;

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


static inline uintptr_t  find_index_generic( void **storage,
                                             size_t size,
                                             void *key,
                                             uintptr_t  hash,
                                             size_t *hole_index,
                                             struct mulle_container_keycallback *callback)
{
   void           *q;
   size_t   i;

   assert( storage);

   i = mulle__pointermap_hash_for_size( hash, size);
   q = storage[ i];
   if( q == callback->notakey)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }

   return( _find_index_generic( storage, size, key, q, i, hole_index, callback));
}


#pragma mark - operations

void   *_mulle__pointermap_write_pair_generic( struct mulle__pointermap *map,
                                               struct mulle_pointerpair *pair,
                                               enum mulle_container_write_mode mode,
                                               struct mulle_container_keyvaluecallback *callback,
                                               struct mulle_allocator *allocator)
{
   uintptr_t                  found;
   size_t                     i;
   void                       **q;
   void                       *old_value;
   struct mulle_pointerpair   new_pair;
   uintptr_t                  hash;

   assert( pair->key != callback->keycallback.notakey);

   hash = (*callback->keycallback.hash)( &callback->keycallback, pair->key);

   if( map->_count)
   {
      size_t   size;
      void     **storage;  // only valid in this no-grow block
      size_t   hole_index;

      storage    = map->_storage;
      size       = map->_size;

      hole_index = 0xfeedface;  // for the analyzer
      found      = find_index_generic( storage, size, pair->key, hash, &hole_index, &callback->keycallback);

      if( found != mulle_not_found_e)
      {
         i         = (size_t) found;
         q         = &storage[ i];
         old_value = q[ size];         // old_value
         if( mode == mulle_container_insert_e)
            return( old_value);

         if( old_value == pair->value && *q == pair->key)
            return( NULL);

         new_pair.key   = (*callback->keycallback.retain)( &callback->keycallback, pair->key, allocator);
         new_pair.value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->value, allocator);

         (callback->keycallback.release)( &callback->keycallback, *q, allocator);
         (callback->valuecallback.release)( &callback->valuecallback, old_value, allocator);

         *q       = new_pair.key;
         q[ size] = new_pair.value;

         // if we do replace, we return the old_value value. If the release is a nop,
         // or an autorelease, then this will be still value. This will crash
         // if the callback if strdup/free
         return( mode == mulle_container_update_e ? old_value : NULL);
      }

      i = hole_index;
      if( ! _mulle__pointermap_is_full( map))
      {
         new_pair.key       = (*callback->keycallback.retain)( &callback->keycallback, pair->key, allocator);
         new_pair.value     = (*callback->valuecallback.retain)( &callback->valuecallback, pair->value, allocator);

         storage[ i]        = new_pair.key;
         storage[ i + size] = new_pair.value;
         map->_count++;
         return( NULL);
      }
   }

   if( _mulle__pointermap_is_full( map))
      grow_generic( map, &callback->keycallback, allocator);

   i              = mulle__pointermap_hash_for_size( hash, map->_size);
   new_pair.key   = (*callback->keycallback.retain)( &callback->keycallback, pair->key, allocator);
   new_pair.value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->value, allocator);

   store_key_value_generic( map->_storage,
                            map->_size,
                            i,
                            new_pair.key,
                            new_pair.value,
                            callback->keycallback.notakey);
   map->_count++;

   return( NULL);
}


//
// If the hash of the key is already known, this can be quite a timesaver
// returns NULL if not found. Since value is always NULL not found.
// notakey is just for key.
//
void   *_mulle__pointermap__get_generic_knownhash( struct mulle__pointermap *map,
                                                   void *key,
                                                   uintptr_t hash,
                                                   struct mulle_container_keyvaluecallback *callback)
{
   int       (*f)( void *, void *, void *);
   size_t    i;
   size_t    size;
   size_t    mask;
   void      *found;
   void      **storage;
   void      *notakey;

   notakey  = callback->keycallback.notakey;
   // important to not hit a NULL storage later
   // size must be > 2 for the hash to work, otherwise we could get
   if( map->_count == 0)
      return( NULL);

   storage  = map->_storage;
   size     = map->_size;
   i        = mulle__pointermap_hash_for_size( hash, size);
   f        = (int (*)()) callback->keycallback.is_equal;
   mask     = size - 1;

   for(;;)
   {
      found = storage[ i];
      if( found == notakey)
         return( NULL);
      if( key == found)
         break;
      if( (*f)( callback, found, key))
         break;
      i = (i + 1) & mask;
   }

   return( storage[ i + size]);
}


//
// returns NULL if nothing found.
//
void   *_mulle__pointermap__get_generic( struct mulle__pointermap *map,
                                         void *key,
                                         struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t   hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, key);
   return( _mulle__pointermap__get_generic_knownhash( map, key, hash, callback));
}


struct mulle_pointerpair   *
   _mulle__pointermap__get_pair_generic_knownhash( struct mulle__pointermap *map,
                                                   void *key,
                                                   uintptr_t hash,
                                                   struct mulle_container_keyvaluecallback *callback,
                                                   struct mulle_pointerpair *pair)
{
   int       (*f)( void *, void *, void *);
   size_t    i;
   size_t    size;
   size_t    mask;
   void      *found;
   void      **storage;
   void      *notakey;

   // important to not hit a NULL storage later
   // size must be > 2 for the hash to work, otherwise we could get
   if( map->_count == 0)
      return( NULL);

   notakey  = callback->keycallback.notakey;
   storage  = map->_storage;
   size     = map->_size;
   i        = mulle__pointermap_hash_for_size( hash, size);
   f        = (int (*)()) callback->keycallback.is_equal;
   mask     = size - 1;

   for(;;)
   {
      found = storage[ i];
      if( found == notakey)
         return( NULL);
      if( key == found)
         break;
      if( (*f)( callback, found, key))
         break;
      i = (i + 1) & mask;
   }

   pair->key   = found;
   pair->value = storage[ i + size];
   return( pair);
}


struct mulle_pointerpair   *_mulle__pointermap__get_pair_generic( struct mulle__pointermap *map,
                                                                  void *key,
                                                                  struct mulle_container_keyvaluecallback *callback,
                                                                  struct mulle_pointerpair *space)
{
   uintptr_t   hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, key);
   return( _mulle__pointermap__get_pair_generic_knownhash( map, key, hash, callback, space));
}


struct mulle_pointerpair   *
   _mulle__pointermap_get_any_pair_generic( struct mulle__pointermap *map,
                                            struct mulle_container_keyvaluecallback *callback,
                                            struct mulle_pointerpair *space)
{
   size_t   i;
   size_t   size;
   size_t   mask;
   void           *found;
   void           **storage;
   void           *notakey;

   notakey = callback->keycallback.notakey;
   if( ! map->_count)
      return( NULL);

   storage = map->_storage;
   size    = map->_size;
   mask    = size - 1;

   i = 0; // just any start
   for(;;)
   {
      found = storage[ i];
      if( found != notakey)
      {
         space->key   = found;
         space->value = storage[ i + size];
         return( space);
      }

      i = (i + 1) & mask;
   }
}


static void   *
   _mulle__pointermap_pointerequalitysearch( struct mulle__pointermap *map,
                                             void *key)
{
   void   **q;
   void   **sentinel;

   q        = map->_storage;
   sentinel = &q[ map->_size];

   //
   // first we search just for pointer equality of the key,
   for( ; q < sentinel; q++)
      if( key == *q)
         return( q[ map->_size]);

   return( NULL);
}


void   *_mulle__pointermap_get_generic_knownhash( struct mulle__pointermap *map,
                                                  void *key,
                                                  uintptr_t hash,
                                                  struct mulle_container_keyvaluecallback *callback)
{
   void   *value;

   assert( map);

   if( key == callback->keycallback.notakey)
      return( NULL);

   if( map->_size <= 32)  // if the dictionary is small try to easy match
   {
      value = _mulle__pointermap_pointerequalitysearch( map, key);
      if( value)
         return( value);
      // else do regular search
   }

   value = _mulle__pointermap__get_generic_knownhash( map, key, hash, callback);
   return( value);
}


void   *_mulle__pointermap_get_generic( struct mulle__pointermap *map,
                                        void *key,
                                        struct mulle_container_keyvaluecallback *callback)
{
   void   *value;

   if( key == callback->keycallback.notakey)
      return( NULL);

   assert( map);
   if( map->_size <= 32)  // if the dictionary is small try to easy match
   {
      value = _mulle__pointermap_pointerequalitysearch( map, key);
      if( value)
         return( value);
      // fallback to regular search
   }

   value = _mulle__pointermap__get_generic( map, key, callback);
   return( value);
}


int   _mulle__pointermap_remove_generic( struct mulle__pointermap *map,
                                         void *key,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator)
{
   size_t      hole_index;
   size_t      i;
   uintptr_t         found;
   void              **storage;
   void              **q;
   size_t      dst_index;
   size_t      search_start;
   size_t      size;
   size_t      mask;
   uintptr_t         hash;

   // important to not hit a NULL storage later
   if( map->_count == 0)
      return( 0);

   hash    = (callback->keycallback.hash)( &callback->keycallback, key);
   storage = map->_storage;
   size    = map->_size;

   found = find_index_generic( storage, size, key, hash, &hole_index, &callback->keycallback);
   if( found == mulle_not_found_e)
      return( 0);

   i = (size_t) found;
   q = &storage[ i];
   (callback->keycallback.release)( &callback->keycallback, *q, allocator);  // get rid of it
   (callback->valuecallback.release)( &callback->valuecallback, q[ size], allocator);  // get rid of it
   map->_count--;

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
      hash         = (*callback->keycallback.hash)( &callback->keycallback, *q);
      search_start = mulle__pointermap_hash_for_size( hash, size);

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



void   _mulle__pointermap_shrink_generic( struct mulle__pointermap *map,
                                          struct mulle_container_keyvaluecallback *callback,
                                          struct mulle_allocator *allocator)
{
   void           **buf;
   size_t   new_size;

   assert( _mulle__pointermap_is_sparse( map));

   new_size = map->_size / 2;
   while( _mulle__pointermap_is_sparse_size( map, new_size))
   {
      new_size = new_size / 2;
      if( new_size < MULLE__POINTERMAP_INITIAL_SIZE)
         break;
   }

   if( new_size < MULLE__POINTERMAP_INITIAL_SIZE)
      new_size = MULLE__POINTERMAP_INITIAL_SIZE;
   if( new_size >= map->_size)
      return;

   assert( map->_count < new_size);

   buf = mulle__pointermap_allocate_storage_generic( new_size, callback->keycallback.notakey, allocator);
   copy_storage_generic( buf, new_size, map->_storage, map->_size, &callback->keycallback);
   mulle_allocator_free( allocator, map->_storage);

   map->_storage = buf;
   map->_size    = new_size;
}


void  _mulle__pointermap_copy_items_generic( struct mulle__pointermap *dst,
                                             struct mulle__pointermap *src,
                                             struct mulle_container_keyvaluecallback *callback,
                                             struct mulle_allocator *allocator)
{
   struct mulle__genericpointermapenumerator  rover;
   struct mulle_pointerpair                   *item;

   assert( dst);
   assert( dst != src);

   rover = mulle__pointermap_enumerate_generic( src, callback);
   while( item = _mulle__genericpointermapenumerator_next_pair( &rover))
   {
      _mulle__pointermap_set_pair_generic( dst, item, callback, allocator);
   }
   mulle__genericpointermapenumerator_done( &rover);
}


// use this only for debugging
size_t
   _mulle__pointermap_count_collisions_generic( struct mulle__pointermap *set,
                                                struct mulle_container_keyvaluecallback *callback,
                                                size_t *perfects)
{
   size_t                                collisions;
   size_t                                distance;
   size_t                                dummy;
   size_t                                i;
   size_t                                search_start;
   struct mulle__genericpointermapenumerator   rover;
   uintptr_t                                   hash;
   void                                        **sentinel;
   void                                        *key;

   rover      = mulle__pointermap_enumerate_generic( set, callback);
   sentinel   = &rover._curr[ set->_size];
   collisions = 0;
   if( ! perfects)
      perfects = &dummy;
   *perfects  = 0;

   for( i = 0; rover._curr < sentinel; ++rover._curr, i++)
   {
      key = *rover._curr;
      if( key == rover._notakey)
         continue;

      hash         = (*callback->keycallback.hash)( &callback->keycallback, key);
      search_start = mulle__pointermap_hash_for_size( hash, set->_size);
      // search_start is where the table  starts to search and i is where it
      // will find the current key
      if( search_start <= i)
      {
         distance = i - search_start;
         if( ! distance)
            ++*perfects;
      }
      else
         distance = set->_size - search_start + i; // needs to wrap
      collisions += distance;
   }
   return( collisions);
}

