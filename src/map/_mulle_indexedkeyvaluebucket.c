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

#include "_mulle_indexedkeyvaluebucket.h"

#include "mulle_container_operation.h"
#include "mulle_prime.h"
#include <assert.h>
#include <stdio.h>


static inline size_t   _mulle_indexedkeyvaluebucket_mask_for_depth( short depth)
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
static uintptr_t   _mulle_indexedkeyvaluebucket_hash_for_modulo( uintptr_t hash, size_t modulo)
{
   modulo &= ~1;
   hash   &= modulo;
   return( hash);
}



void   *__mulle_indexedkeyvaluebucket_insert_with_mode( struct _mulle_indexedkeyvaluebucket *bucket, 
                                                        struct _mulle_keyvaluepair *p,
                                                        uintptr_t hash,
                                                        enum mulle_container_set_mode mode,
                                                        struct mulle_container_keyvaluecallback *callback);
/*
 *
 */
#define _MULLE_INDEXEDKEYVALUEBUCKET_INITIAL_DEPTH    1
#define _MULLE_INDEXEDKEYVALUEBUCKET_MAX_DEPTH        30

//
// with 4, large tables will be 1 MB pretty quickly even with little filling
// try 8KB sometime and see the kernel go on a VM rampage, because of some
// throttling mechanism its actually slowing down things...
//
#define _MULLE_INDEXEDKEYVALUEBUCKET_MIN_INITIAL_DEPTH    1  
#define _MULLE_INDEXEDKEYVALUEBUCKET_MAX_INITIAL_DEPTH    3  


static inline size_t   is_full( struct _mulle_indexedkeyvaluebucket *bucket, size_t size)
{
   return( bucket->_count >= (size - (size >> 1)));  // full when only 25% free 
}



static short   depth_for_capacity( size_t capacity)
{
   unsigned int  i;
   
   capacity >>= 1;
   for( i = _MULLE_INDEXEDKEYVALUEBUCKET_MIN_INITIAL_DEPTH; i <= _MULLE_INDEXEDKEYVALUEBUCKET_MAX_INITIAL_DEPTH; i++)
      if( capacity <= _mulle_indexedkeyvaluebucket_size_for_depth( i))
         return( (short) i);
   return( _MULLE_INDEXEDKEYVALUEBUCKET_MAX_INITIAL_DEPTH);
}


void   _mulle_indexedkeyvaluebucket_init( struct _mulle_indexedkeyvaluebucket *p,
                                          size_t capacity,
                                          struct mulle_container_keyvaluecallback *callback)
{
   size_t   n;
   short    depth;
      
   depth       = depth_for_capacity( capacity);
   n           = _mulle_indexedkeyvaluebucket_size_for_depth( depth) + 1;
   p->_count   = 0;
   p->_depth   = depth;
   p->_storage = mulle_allocator_calloc( callback->keycallback.allocator, n, sizeof( struct _mulle_keyvaluepair));
   
   if( callback->keycallback.not_a_key_marker)
   {
      struct _mulle_keyvaluepair   *start;
      struct _mulle_keyvaluepair   *end;
      
      // set to ununused, if using special not_a_key_marker
      start = &p->_storage[ 0];
      end   = &start[ n];
      while( start < end)
      {
         start->_key = callback->keycallback.not_a_key_marker;
         start++;
      }
   }
}


struct _mulle_indexedkeyvaluebucket   *_mulle_indexedkeyvaluebucket_create( size_t capacity,
                                                                            size_t extra,
                                                                            struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_indexedkeyvaluebucket   *p;
   
   p = mulle_allocator_calloc( callback->keycallback.allocator, 1, sizeof( struct _mulle_indexedkeyvaluebucket) + extra);
   if( p)
      _mulle_indexedkeyvaluebucket_init( p, capacity, callback);
   return( p);
}


void   _mulle_indexedkeyvaluebucket_done( struct _mulle_indexedkeyvaluebucket *bucket,
                                            struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_indexedkeyvaluebucketenumerator   rover;
   struct _mulle_keyvaluepair                       *pair;
   
   rover = _mulle_indexedkeyvaluebucket_enumerate( bucket, callback);
   while( pair = _mulle_indexedkeyvaluebucketenumerator_next( &rover))
   {
      (callback->keycallback.release)( &callback->keycallback, pair->_key);
      (callback->valuecallback.release)( &callback->valuecallback, pair->_value);
   }
   _mulle_indexedkeyvaluebucketenumerator_done( &rover); 
   
   _mulle_allocator_free( callback->keycallback.allocator, bucket->_storage);
}


void   _mulle_indexedkeyvaluebucket_free( struct _mulle_indexedkeyvaluebucket *bucket, 
                                          struct mulle_container_keyvaluecallback *callback)
{
   _mulle_indexedkeyvaluebucket_done( bucket, callback);
   _mulle_allocator_free( callback->keycallback.allocator, bucket);
}


static inline void   store_key_value_pair( struct _mulle_keyvaluepair *data, 
                                           size_t i,
                                           size_t modulo,
                                           struct _mulle_keyvaluepair *p,
                                           struct mulle_container_keycallback *callback)
{
   size_t     limit;
   
   limit = modulo + 1;
   while( data[ i]._key != callback->not_a_key_marker)
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
static void   copy_buckets( struct _mulle_keyvaluepair *dst,
                            size_t modulo,
                            struct _mulle_keyvaluepair *src, 
                            size_t n,
                            struct mulle_container_keycallback *callback)
{
   struct _mulle_keyvaluepair *p;
   size_t     i;
   uintptr_t  hash;
   
   assert( modulo + 1 >= n);
   
   for( ;n--;)
   {
      p = src++;
      if( p->_key == callback->not_a_key_marker)
         continue;

      hash = (callback->hash)( callback, p->_key);
      i    = _mulle_indexedkeyvaluebucket_hash_for_modulo( hash, modulo);
      store_key_value_pair( dst, i, modulo, p, callback);
   }
}


static size_t   grow( struct _mulle_indexedkeyvaluebucket *bucket,
                      struct mulle_container_keycallback *callback)
{
   size_t                       modulo;
   size_t                       new_modulo;
   struct _mulle_keyvaluepair   *buf;
   short                        depth;
   
   // for good "not found" performance, there should be a high possibility of 
   // a NULL after each slot
   //
   depth  = bucket->_depth;
   modulo = _mulle_indexedkeyvaluebucket_mask_for_depth( depth);
   if( depth == _MULLE_INDEXEDKEYVALUEBUCKET_MAX_DEPTH)
   {
      static int  complain;
      
      if( ! complain)
      {
         fprintf( stderr, "%s: Your hash is so weak, that you reached the end of the rope", __PRETTY_FUNCTION__);
         ++complain;
      }
      return( modulo);
   }
   
   new_modulo = _mulle_indexedkeyvaluebucket_mask_for_depth( ++depth);

   buf = mulle_allocator_calloc( callback->allocator, (new_modulo + 1), sizeof( struct _mulle_keyvaluepair));
   if( callback->not_a_key_marker)
   {
      struct _mulle_keyvaluepair   *start;
      struct _mulle_keyvaluepair   *end;
      
      // set fresh portion to unused, if using special _not_a_key_marker
      start = &buf[ modulo + 1];
      end   = &buf[ new_modulo + 1];
      while( start < end)
      {
         start->_key = callback->not_a_key_marker;
         start++;
      }
   }
   
   copy_buckets( buf, new_modulo, bucket->_storage, modulo + 1, callback);
   mulle_allocator_free( callback->allocator,  bucket->_storage);
   
   bucket->_depth   = depth;
   bucket->_storage = buf;
   
   return( new_modulo);
}


static size_t  _find_index( struct _mulle_keyvaluepair  *storage, 
                            void *key,
                            struct _mulle_keyvaluepair *q,
                            size_t limit,
                            size_t i,
                            size_t *hole_index,
                            struct mulle_container_keycallback *callback)
{
   int   (*f)( void *, void *, void *);
   void   *param1;
   void   *param2;
   void   *notAKey;
   
   f      = (void *) callback->is_equal;
   param1 = callback;
   param2 = key;
   notAKey = callback->not_a_key_marker;
   do
   {
      if( (*f)( param1, param2, q->_key))
         return( i);
      if( ++i < limit)
         continue;
      i = 0;
   }
   while( (q = &storage[ i])->_key == notAKey);
   
   *hole_index = i;
   return( mulle_not_found_e);
}


static inline size_t  find_index( struct _mulle_keyvaluepair *storage,
                                  void *key,
                                  uintptr_t hash,
                                  size_t modulo,
                                  size_t *hole_index,
                                  struct mulle_container_keycallback *callback)
{
   struct _mulle_keyvaluepair   *q;
   size_t       i;
   
   i = _mulle_indexedkeyvaluebucket_hash_for_modulo( hash, modulo);
   q = &storage[ i];
   if( q->_key == callback->not_a_key_marker)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }
   return( _find_index( storage, key, q, modulo + 1, i, hole_index, callback));
}


void   *__mulle_indexedkeyvaluebucket_insert_with_mode( struct _mulle_indexedkeyvaluebucket *bucket,
                                                        struct _mulle_keyvaluepair *pair,
                                                        uintptr_t hash,
                                                        enum mulle_container_set_mode mode,
                                                        struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_keyvaluepair   *q; 
   size_t       i;
   size_t       modulo;
   size_t       hole_index;

   modulo = _mulle_indexedkeyvaluebucket_mask_for_depth( bucket->_depth);
   
   i = find_index( bucket->_storage, pair->_key, hash, modulo, &hole_index, &callback->keycallback);
   if( i != (size_t) mulle_not_found_e)
   {
      q = &bucket->_storage[ i];
      switch( mode)
      {
      case mulle_container_put_e :
         q = &bucket->_storage[ i];
         if( q->_value == pair->_value && q->_key == pair->_key)
            return( q->_value);
   
         (callback->keycallback.release)( &callback->keycallback, q->_key);
         (callback->valuecallback.release)( &callback->valuecallback, q->_value);
         *q = *pair;
         return( q->_value);  
         
      case mulle_container_insert_e :
         return( q->_value);
      }
   }
   
   i = hole_index;
   if( ! is_full( bucket, modulo))
   {
      bucket->_storage[ i] = *pair;
      bucket->_count++;
      return( NULL);
   }

   modulo = grow( bucket, &callback->keycallback);

   i = _mulle_indexedkeyvaluebucket_hash_for_modulo( hash, modulo);
   store_key_value_pair( bucket->_storage, i, modulo, pair, &callback->keycallback);
   bucket->_count++;
   
   return( NULL);
}


void   *_mulle_indexedkeyvaluebucket_get( struct _mulle_indexedkeyvaluebucket *bucket,
                                          void *p,
                                          uintptr_t hash,
                                          struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_keyvaluepair *q;
   int         (*f)( void *, void *, void *);
   size_t   modulo;
   size_t   limit;
   size_t   i;
   void         *no_key;
   
   modulo = _mulle_indexedkeyvaluebucket_mask_for_depth( bucket->_depth);
   i      = _mulle_indexedkeyvaluebucket_hash_for_modulo( hash, modulo);
   limit  = modulo + 1;
   no_key = callback->keycallback.not_a_key_marker;
   
   f = (void *) callback->keycallback.is_equal;
   while( (q = &bucket->_storage[ i])->_key != no_key)
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


void   _mulle_indexedkeyvaluebucket_put( struct _mulle_indexedkeyvaluebucket *bucket, 
                                            struct _mulle_keyvaluepair *pair,
                                            struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t   hash;
   
   pair->_key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key);
   pair->_value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value);
   hash         = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   __mulle_indexedkeyvaluebucket_insert_with_mode( bucket, pair, hash, mulle_container_put_e, callback);
}


void    *_mulle_indexedkeyvaluebucket_insert( struct _mulle_indexedkeyvaluebucket *bucket, 
                                                        struct _mulle_keyvaluepair *pair,
                                                        struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t   hash;
   
   pair->_key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key);
   pair->_value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value);
   hash         = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   return( __mulle_indexedkeyvaluebucket_insert_with_mode( bucket, pair, hash, mulle_container_insert_e, callback));
}                                       


void   *_mulle_indexedkeyvaluebucket_insert_known_absent( struct _mulle_indexedkeyvaluebucket *bucket,
                                                          struct _mulle_keyvaluepair *pair,
                                                          struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t hash;

   pair->_key   = (*callback->keycallback.retain)( &callback->keycallback, pair->_key);
   pair->_value = (*callback->valuecallback.retain)( &callback->valuecallback, pair->_value);
   hash         = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);

   return( __mulle_indexedkeyvaluebucket_insert_with_mode( bucket, pair, hash, mulle_container_insert_e, callback));
}                                       



int   __mulle_indexedkeyvaluebucket_remove( struct _mulle_indexedkeyvaluebucket *bucket,
                                            void *key,
                                            uintptr_t hash,
                                            struct mulle_container_keyvaluecallback *callback)
{
   size_t                       hole_index;
   size_t                       i;
   size_t                       modulo;
   struct _mulle_keyvaluepair   *q;
   size_t                       dst_index;
   size_t                       search_start;
   
   modulo = _mulle_indexedkeyvaluebucket_mask_for_depth( bucket->_depth);
   i      = find_index( bucket->_storage, key, hash, modulo, &hole_index, &callback->keycallback);
   if( i == (size_t) mulle_not_found_e)
      return( 0);

   q = &bucket->_storage[ i];
   (callback->keycallback.release)( &callback->keycallback, q->_key);  // get rid of it
   (callback->valuecallback.release)( &callback->valuecallback, q->_value);  // get rid of it
   bucket->_count--;
   
   // now we may need to do a whole lot of shifting, if 
   // the following object isn't in its proper hash index.
   // Because of the way objects are inserted and deleted
   // we have to possibly move EVERYTHING until we hit the
   // hole.
   
   dst_index = i;
   
   for(;;)
   {
      i = (i + 1) & modulo;
      
      q = &bucket->_storage[ i];
      if( q->_key == callback->keycallback.not_a_key_marker)
      {
         bucket->_storage[ dst_index]._key   = callback->keycallback.not_a_key_marker;
         bucket->_storage[ dst_index]._value = NULL;
         break;
      }
   
      // check if its in the slot it's expected
      // otherwise it won't be found because of
      // the "hole" in the front we just made
      // but keep going because "wrong" hashes might be behind
      // be sure not to move items that won't be found
      hash         = (*callback->keycallback.hash)( &callback->keycallback, q->_key);
      search_start = _mulle_indexedkeyvaluebucket_hash_for_modulo( hash, modulo);

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
      bucket->_storage[ dst_index] = *q;
      dst_index = i;
   }
   return( 1);
}


int   _mulle_indexedkeyvaluebucket_remove( struct _mulle_indexedkeyvaluebucket *bucket,
                                           void *key,
                                           struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t hash;

   hash = (*callback->keycallback.hash)( &callback->keycallback, key);

   return( __mulle_indexedkeyvaluebucket_remove( bucket, key, hash, callback));
}


void   _mulle_indexedkeyvaluebucket_reset( struct _mulle_indexedkeyvaluebucket *bucket, 
                                           struct mulle_container_keyvaluecallback *callback)
{
   _mulle_indexedkeyvaluebucket_done( bucket, callback);
   _mulle_indexedkeyvaluebucket_init( bucket,
                                      _mulle_indexedkeyvaluebucket_size_for_depth( bucket->_depth),
                                      callback);
}


