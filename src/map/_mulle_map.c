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
#include "mulle_prime.h"

#include "_mulle_indexedkeyvaluebucket.h"
#include "mulle_container_attributes.h"
#include <assert.h>
#include <string.h>


void   *__mulle_map_put( struct _mulle_map *map,
                         struct _mulle_keyvaluepair *pair,
                         enum mulle_container_set_mode mode,
                         struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second_fourth;

void   __mulle_map_copy( struct _mulle_map *dst,
                         struct _mulle_map *src,
                         struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_third;

extern void   *__mulle_indexedkeyvaluebucket_insert_with_mode( struct _mulle_indexedkeyvaluebucket *,
                                                               struct _mulle_keyvaluepair *,
                                                               size_t,
                                                               enum mulle_container_set_mode,
                                                               struct mulle_container_keyvaluecallback *) mulle_nonnull_first_second_fifth;

#ifndef NDEBUG
static void   _mulle_map_sanity_check( struct _mulle_map *map,
                                       struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_mapenumerator   rover;
   size_t                        count;
   
   count = 0;
   rover = _mulle_map_enumerate( map, callback);
   while( _mulle_mapenumerator_next( &rover, NULL, NULL))
      ++count;
   _mulle_mapenumerator_done( &rover);
   assert( count == _mulle_map_count( map));
}
#endif

//static unsigned int   suggest_bucket_depth( unsigned int capacity)
//{
//   unsigned int  i;
//      
//   capacity >>= 1;
//   for( i = 0; i <= MULLE_INDEXED_BUCKETS_MAX_INITIAL_DEPTH; i++)
//      if( capacity < __NSIndexedBucketPrimes[ i])
//         return( i); 
//   return( MULLE_INDEXED_BUCKETS_MAX_INITIAL_DEPTH);
//}


#define MULLE_MAP_MAX_DEPTH              30

#define MULLE_MAP_MIN_INITIAL_DEPTH      0  
#define MULLE_MAP_INITIAL_DEPTH          3
#define MULLE_MAP_MAX_INITIAL_DEPTH      16  
#define MULLE_MAP_STOP_SINGLE_DIMENSION_GROWTH 5


static short   good_depth_for_depth( int depth)
{
   assert( depth >= 0);
   return( depth < MULLE_MAP_INITIAL_DEPTH ? MULLE_MAP_INITIAL_DEPTH : (short) depth);
}


static short   depth_for_capacity( size_t capacity)
{
   int  i;
   
   if( ! capacity)
      return( MULLE_MAP_MIN_INITIAL_DEPTH);
      
   capacity >>= 2;
   for( i = MULLE_MAP_MIN_INITIAL_DEPTH; i <= MULLE_MAP_MAX_INITIAL_DEPTH; i++)
      if( capacity <= mulle_prime_for_depth( i))
         return( good_depth_for_depth( i));
   return( MULLE_MAP_MAX_INITIAL_DEPTH);
}


static void   _mulle_map_init_with_depth( struct _mulle_map *map,
                                          int depth,
                                          struct mulle_container_keyvaluecallback *callback)
{
   size_t                        modulo;
   struct _mulle_keyvaluepair   *pair;
   struct _mulle_keyvaluepair   *sentinel;
   
   map->_storage = NULL;
   map->_count   = 0;
   map->_depth   = (short) depth;
   modulo        = mulle_prime_for_depth( map->_depth);
   map->_storage = map->_depth
                      ? mulle_allocator_calloc( callback->keycallback.allocator,
                                                modulo,
                                                sizeof( struct _mulle_keyvaluepair))
                      : NULL;

   if( map->_storage)
   {
      pair     = map->_storage;
      sentinel = &map->_storage[ modulo];
      while( pair < sentinel)
      {
         pair->_key   = callback->keycallback.not_a_key_marker;
         pair->_value = NULL;
         pair++;
      }
   }
}


void  _mulle_map_init( struct _mulle_map *map, size_t capacity, struct mulle_container_keyvaluecallback *callback)
{
   _mulle_map_init_with_depth( map, depth_for_capacity( capacity), callback);
}


void   _mulle_map_free( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback)
{
   _mulle_map_done( map, callback);
   (*callback->keycallback.allocator->free)( map);
}


struct _mulle_map   *_mulle_map_create( size_t capacity,
                                        struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_map   *map;
   
   map = mulle_allocator_calloc( callback->keycallback.allocator, 1, sizeof( struct _mulle_map));
   if( map)
      _mulle_map_init_with_depth( map, depth_for_capacity( capacity), callback);
   return( map);
}


void   __mulle_map_copy( struct _mulle_map *dst,
                         struct _mulle_map *src,
                         struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_mapenumerator   rover;
   void              *key;
   void              *value;
   
   rover = _mulle_map_enumerate( src, callback);
   while( _mulle_mapenumerator_next( &rover, &key, &value))
      _mulle_map_insert( dst, key, value, callback);
   _mulle_mapenumerator_done( &rover);
}


struct _mulle_map *_mulle_map_copy( struct _mulle_map *map,
                                    struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_map   *other;

   other = _mulle_map_create( _mulle_map_count( map), callback);
   __mulle_map_copy( other, map, callback);
   return( other);
}



void   _mulle_map_done( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_keyvaluepair   *pair;
   struct _mulle_keyvaluepair   *sentinel;
   
   if( ! map->_depth)
      return;
      
   sentinel = &map->_storage[ mulle_prime_for_depth( map->_depth)];
   for( pair = map->_storage; pair < sentinel; pair++)
   {
      if( _mulle_keyvaluepair_is_empty( pair, callback))
         continue;
      if( _mulle_keyvaluepair_is_storage( pair, callback))
      {
         (callback->keycallback.release)( &callback->keycallback, pair->_key);
         (callback->valuecallback.release)( &callback->valuecallback, pair->_value);
         continue;
      }
      _mulle_indexedkeyvaluebucket_free( pair->_value, callback);
   }
   (*callback->keycallback.allocator->free)( map->_storage);
}


int   _mulle_mapenumerator_next( struct _mulle_mapenumerator *rover, void **key, void  **value)
{
   struct _mulle_keyvaluepair   *pair;
   struct _mulle_keyvaluepair   *storage;
   
   if( ! rover->_left)
      return( 0);

   --rover->_left;
retry:   
   pair = _mulle_indexedkeyvaluebucketenumerator_next( &rover->_bucketsRover);
   if( ! pair)
   {
      storage = rover->_map->_storage;
      for(;;)
      {
         assert( rover->_index < mulle_prime_for_depth( rover->_map->_depth));
         
         pair = &storage[ rover->_index++];
         if( ! _mulle_keyvaluepair_is_empty( pair, rover->_callback))
            break;
      }

      if( _mulle_keyvaluepair_is_queue( pair, rover->_callback))
      {
         rover->_bucketsRover = _mulle_indexedkeyvaluebucket_enumerate( pair->_value, rover->_callback);
         goto retry;
      }
   }  
   
   assert( pair->_key != rover->_callback->keycallback.not_a_key_marker);
   
   if( key)
      *key = pair->_key;
   if( value)
      *value = pair->_value;

   return( 1);
}


void   *_mulle_map_get( struct _mulle_map *map, void *key, struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t        hash;
   size_t           modulo;
   size_t           i;
   struct _mulle_keyvaluepair   *pair;
   
#ifndef NDEBUG      
   _mulle_map_sanity_check( map, callback);
#endif    
   
   modulo  = mulle_prime_for_depth( map->_depth);
   if( ! modulo)
      return( NULL);
      
   pair = map->_storage;
   // for small maps try quick check
   switch( modulo)
   {
   case 5 : if( pair->_key == key)
               return( pair->_value);
            ++pair;
            if( pair->_key == key)
               return( pair->_value);
            ++pair;
            if( pair->_key == key)
               return( pair->_value);
            ++pair;
   case 2 : if( pair->_key == key)
               return( pair->_value);
            ++pair;
   case 1 : if( pair->_key == key)
               return( pair->_value);
   }
   
   hash = (*callback->keycallback.hash)( &callback->keycallback, key);
   i    = mulle_prime_hash_for_depth( hash, map->_depth);
   pair = &map->_storage[ i];
   
   if( _mulle_keyvaluepair_is_empty( pair, callback))
      return( NULL);
   
   if( _mulle_keyvaluepair_is_storage( pair, callback))
   {
      if( pair->_key == key)
         return( pair->_value);
      if( (*callback->keycallback.is_equal)( &callback->keycallback, pair->_key, key))
         return( pair->_value);
      return( NULL);
   }

   return( _mulle_indexedkeyvaluebucket_get( (struct _mulle_indexedkeyvaluebucket *) pair->_value,
                                             key,
                                             hash,
                                             callback));
}


//
// we "just" create a new hashmap with the new size and then copy
// the other map into it. The we move the ivars around and delete the
// new one...
//
static int   grow_vertically( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_map                         copy;
   struct mulle_container_keyvaluecallback   tmpcallback;
   short                                     depth;
   
   depth = map->_depth;
   if( depth < 0)
      return( 0);

   depth = good_depth_for_depth( depth + 1);
      
   tmpcallback = *callback;
   
   tmpcallback.keycallback.retain    = (void *) mulle_container_callback_self;
   tmpcallback.valuecallback.retain  = (void *) mulle_container_callback_self;
   tmpcallback.keycallback.release   = (void *) mulle_container_callback_nop;
   tmpcallback.valuecallback.release = (void *) mulle_container_callback_nop;
    
   _mulle_map_init_with_depth( &copy, - depth, &tmpcallback);  /* mark negative, so we don't grow within growing */
   __mulle_map_copy( &copy, map, &tmpcallback);
   _mulle_map_done( map, &tmpcallback);

   memcpy( map, &copy, sizeof( struct _mulle_map));
   map->_depth = depth;

   return( 1);
}


//
// what's the strategy ?
//  1.) first we prefer vertical growth until we reach 31, to keep smallish
//  2.) after that we prefer rectangular growth
//       if an array becomes four times as large as the vertical plane
//  3.) we quadruple the vertical plane, but then we have to reorg completely
//      so, 
//  4.) we do this until the vertical plane hits 509 * 1036 = 1 mio objects... 
//      not too shabby. After that we don't want to reorg so much anymore
//      we only reorg twice at 127 and 509 thefore 
//  5.) this can be completely avoided by specifing a capacity, so that enough
//      vertical space is preallocated
//      
void   *__mulle_map_put( struct _mulle_map *map,
                         struct _mulle_keyvaluepair *new_pair,
                         enum mulle_container_set_mode mode,
                         struct mulle_container_keyvaluecallback *callback)
{

   int                                   same_key;
   size_t                                i;
   size_t                                modulo;
   size_t                                other_hash;
   size_t                                size;
   struct _mulle_indexedkeyvaluebucket   *bucket;
   struct _mulle_keyvaluepair            *pair;
   uintptr_t                             hash;
   void                                  *value;
   
   assert( new_pair);
   assert( new_pair->_key != callback->keycallback.not_a_key_marker);

   new_pair->_key   = (*callback->keycallback.retain)( &callback->keycallback, new_pair->_key);
   new_pair->_value = (*callback->valuecallback.retain)( &callback->valuecallback, new_pair->_value);
   hash             = (*callback->keycallback.hash)( &callback->keycallback, new_pair->_key);
   
retry:
#ifndef NDEBUG      
   _mulle_map_sanity_check( map, callback);
#endif 

   modulo = mulle_prime_for_depth( map->_depth);
   if( ! modulo)
   {
      grow_vertically( map, callback);
      goto retry;
   }
      
   assert( map->_storage);

   i    = hash % modulo;
   pair = &map->_storage[ i];
   if( _mulle_keyvaluepair_is_empty( pair, callback))
   {
      map->_count++;
      *pair = *new_pair;
#ifndef NDEBUG      
      _mulle_map_sanity_check( map, callback);
#endif      
      return( NULL);
   }
   
   //
   // if the key is not the "not_a_key_marker" then this is really a 
   // key/value pair and not a queue
   //
   
   if( _mulle_keyvaluepair_is_storage( pair, callback))
   {
      same_key = new_pair->_key == pair->_key;
      if( same_key)
         goto match;
         
      other_hash = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);
      if( hash == other_hash && (*callback->keycallback.is_equal)( &callback->keycallback, new_pair->_key, pair->_key))
      {
      match:      
         switch( mode)
         {
         case mulle_container_put_e :
            if( new_pair->_value == pair->_value && same_key)
               return( pair->_value);
               
            (*callback->keycallback.release)( &callback->keycallback, pair->_key);
            (*callback->valuecallback.release)( &callback->valuecallback, pair->_value);
            *pair = *new_pair;
            return( pair->_value);
            
         case mulle_container_insert_e :
            return( pair->_value);
         }
      }
      
      //
      // a) grow vertically or start a _mulle_indexedkeyvaluebucket chain
      //
      if( map->_depth < MULLE_MAP_STOP_SINGLE_DIMENSION_GROWTH)
         if( grow_vertically( map, callback))
            goto retry;
      
      // b) create a new bucket chain (quadruple size, of current vertical)
      // start small
      bucket = _mulle_indexedkeyvaluebucket_create( 4, 0, callback);
      __mulle_indexedkeyvaluebucket_insert_with_mode( bucket, 
                                                      pair,
                                                      other_hash,   // analyzer: wrong!
                                                      mulle_container_insert_e,
                                                      callback);
      __mulle_indexedkeyvaluebucket_insert_with_mode( bucket,
                                                      new_pair,
                                                      hash,
                                                      mulle_container_insert_e,
                                                      callback);
      
      pair->_key   = callback->keycallback.not_a_key_marker;
      pair->_value = bucket;

      map->_count++;
#ifndef NDEBUG      
      _mulle_map_sanity_check( map, callback);
#endif      
      return( NULL);
   }
   
   // now make sure, the bucket don't grow to large
   // with respect to vertical size
   bucket = (struct _mulle_indexedkeyvaluebucket *) pair->_value;
   if( _mulle_indexedkeyvaluebucket_is_full( bucket))
   {
      size = _mulle_indexedkeyvaluebucket_get_storagesize( bucket);
      if( size * 2 > modulo * 4)
         if( grow_vertically( map, callback))
            goto retry;
   }
   
   value  = __mulle_indexedkeyvaluebucket_insert_with_mode( bucket, 
                                                            new_pair,
                                                            hash,
                                                            mode,
                                                            callback);
   if( ! value)
   {
      map->_count ++;
#ifndef NDEBUG      
      _mulle_map_sanity_check( map, callback);
#endif      
   }
   return( value);
}


void   _mulle_map_remove( struct _mulle_map *map, void *key, struct mulle_container_keyvaluecallback *callback)
{
   size_t                                hash;
   size_t                                i;
   size_t                                modulo;
   size_t                                other_hash;
   struct _mulle_indexedkeyvaluebucket   *bucket;
   struct _mulle_keyvaluepair            *pair;
   
   modulo = mulle_prime_for_depth( map->_depth);
   if( ! modulo)
      return;
      
   hash = (*callback->keycallback.hash)( &callback->keycallback, key);
   i    = hash % modulo; 

   pair = &map->_storage[ i];
   if( _mulle_keyvaluepair_is_empty( pair, callback))
      return;
   
   if( _mulle_keyvaluepair_is_storage( pair, callback))
   {
      if( key == pair->_key)
         goto match;
         
      other_hash = (*callback->keycallback.hash)( &callback->keycallback, pair->_key);
      if( hash == other_hash && (*callback->keycallback.is_equal)( &callback->keycallback, key, pair->_key))
      {
match:      
         (*callback->keycallback.release)( &callback->keycallback, pair->_key);
         (*callback->valuecallback.release)( &callback->valuecallback, pair->_value);
         pair->_key   = NULL;
         pair->_value = NULL;
         map->_count--;
#ifndef NDEBUG      
      _mulle_map_sanity_check( map, callback);
#endif    
      }
      return;
   }
   
   bucket = (struct _mulle_indexedkeyvaluebucket *) pair->_value;
   if( __mulle_indexedkeyvaluebucket_remove( bucket, key, hash, callback))
   {
      map->_count--;  // could check that bucket are empty, and throw them away
#ifndef NDEBUG      
      _mulle_map_sanity_check( map, callback);
#endif    
   }
}


void   _mulle_map_insert_keys_and_valuesv( struct _mulle_map *map,
                                                     void *firstvalue,
                                                     void *firstkey,
                                                     va_list args,
                                                     struct mulle_container_keyvaluecallback *callback)
{
   void  *value;
   void  *key;
   
   value = firstvalue;
   key   = firstkey;
   while( value)
   {
      _mulle_map_insert( map, key, value, callback);

      value = va_arg( args, void *);
      key   = va_arg( args, void *);
   }
}


void    _mulle_map_reset( struct _mulle_map *map, struct mulle_container_keyvaluecallback *callback)
{
   _mulle_map_done( map, callback);
   _mulle_map_init_with_depth( map, 0, callback);
}
