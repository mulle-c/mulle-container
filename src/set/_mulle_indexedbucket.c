//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
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
#include "_mulle_indexedbucket.h"
#include "mulle_container_operation.h"
#include "mulle_container_callback.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>


void   *_mulle_indexedbucket_set_with_mode( struct _mulle_indexedbucket *bucket,
                                               void *p,
                                               uintptr_t hash,
                                               enum mulle_container_set_mode mode,
                                               struct mulle_container_keycallback *callback);
/*
 *
 */
#define _MULLE_INDEXEDBUCKET_S_INITIAL_DEPTH    1
#define _MULLE_INDEXEDBUCKET_S_MAX_DEPTH        30

//
// with 4, large tables will be 1 MB pretty quickly even with little filling
// try 8KB sometime and see the kernel go on a VM rampage, because of some
// throttling mechanism its actually slowing down things...
//
#define _MULLE_INDEXEDBUCKET_S_MIN_INITIAL_DEPTH    1
#define _MULLE_INDEXEDBUCKET_S_MAX_INITIAL_DEPTH    3


static inline size_t   mask_for_depth( int depth)
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
static uintptr_t   hash_for_modulo( uintptr_t hash, size_t modulo)
{
   modulo &= ~1;
   hash   &= modulo;
   return( hash);
}


static short   depth_for_capacity( size_t capacity)
{
   unsigned int  i;
   
   capacity >>= 1;
   for( i = _MULLE_INDEXEDBUCKET_S_MIN_INITIAL_DEPTH; i <= _MULLE_INDEXEDBUCKET_S_MAX_INITIAL_DEPTH; i++)
      if( capacity <= _mulle_indexedbucket_size_for_depth( i))
         return( (short) i);
   return( _MULLE_INDEXEDBUCKET_S_MAX_INITIAL_DEPTH);
}


static inline uintptr_t   _mulle_indexedbucket_hash( struct mulle_container_keycallback *callback, void *p)
{
   return( (*callback->hash)( callback, p));
}




void    _mulle_indexedbucket_init( struct _mulle_indexedbucket *p,
                                   size_t capacity,
                                   struct mulle_container_keycallback *callback)
{
   size_t   n;
   short    depth;
      
   depth       = depth_for_capacity( capacity);
   n           = _mulle_indexedbucket_size_for_depth( depth) + 1;
   p->_count   = 0;
   p->_depth   = depth;
   p->_storage = mulle_allocator_calloc( callback->allocator, n, sizeof( void *));
}


struct _mulle_indexedbucket   *_mulle_indexedbucket_create( size_t capacity,
                                                            size_t extra,
                                                            struct mulle_container_keycallback *callback)
{
   struct _mulle_indexedbucket   *p;
   
   p = mulle_allocator_calloc( callback->allocator, 1, sizeof( struct _mulle_indexedbucket) + extra);
   if( p)
      _mulle_indexedbucket_init( p, capacity, callback);
   return( p);
}


void   _mulle_indexedbucket_done( struct _mulle_indexedbucket *bucket,
                                    struct mulle_container_keycallback *callback)
{
   struct _mulle_indexedbucketenumerator  rover;
   void   *item;
   
   rover = _mulle_indexedbucket_enumerate( bucket, callback);
   while( item = _mulle_indexedbucketenumerator_next( &rover))
      (*callback->release)( callback, item);
   _mulle_indexedbucketenumerator_done( &rover);
   
   mulle_allocator_free( callback->allocator, bucket->_storage);
}


void    _mulle_indexedbucket_free( struct _mulle_indexedbucket *bucket, 
                                   struct mulle_container_keycallback *callback)
{
   _mulle_indexedbucket_done( bucket, callback);
   mulle_allocator_free( callback->allocator, bucket);
}


static inline void   store_pointer( void **data, 
                                    size_t i,
                                    size_t modulo,
                                    void *p)
{
   while( data[ i])
   {
      if( ++i <= modulo)
         continue;
      i = 0;
   }
   
   data[ i] = p;
}


static void   copy_buckets( void **dst,
                            size_t modulo,
                            void **src, 
                            size_t n,
                            struct mulle_container_keycallback *callback)
{
   void       *p;
   size_t     i;
   size_t     hash;
   
   assert( modulo + 1 >= n);
   
   for( ;n--;)
   {
      p = *src++;
      if( ! p)
         continue;

      hash = _mulle_indexedbucket_hash( callback, p);
      i    = (size_t) hash_for_modulo( hash, modulo);
      store_pointer( dst, i, modulo, p);
   }
}


static size_t   grow( struct _mulle_indexedbucket *bucket,
                      struct mulle_container_keycallback *callback)
{
   size_t   modulo;
   size_t   new_modulo;
   short    depth;
   void     *buf;
   
   // for good "not found" performance, there should be a high possibility of 
   // a NULL after each slot
   //
   depth  = bucket->_depth;
   modulo = mask_for_depth( depth);
   if( depth == _MULLE_INDEXEDBUCKET_S_MAX_DEPTH)
   {
      static int  complain;
      
      if( ! complain)
      {
         fprintf( stderr, "Your hash is so weak, that you reached the end of the rope");
         complain = 1;
      }
      return( modulo);
   }
   
   new_modulo = mask_for_depth( ++depth);

   buf = mulle_allocator_calloc( callback->allocator, (new_modulo + 1), sizeof( void *));
   if( ! buf)
      return( modulo);
   
   copy_buckets( buf, new_modulo, bucket->_storage, modulo + 1, callback);
   mulle_allocator_free( callback->allocator, bucket->_storage);
   
   bucket->_depth   = depth;
   bucket->_storage = buf;
   
   return( new_modulo);
}


static size_t  _find_index( void  **storage,
                            void *p,
                            void *q,
                            size_t limit,
                            size_t i,
                            size_t *hole_index,
                            struct mulle_container_keycallback *callback)
{
   int   (*f)( void *, void *, void *);
   void   *param1;
   void   *param2;
   
   f      = (void *) callback->is_equal;
   param1 = callback;
   param2 = p;
   
   do
   {
      if( (*f)( param1, param2, q))
         return( i);
      if( ++i < limit)
         continue;
      i = 0;
   }
   while( q = storage[ i]);
   
   *hole_index = i;
   return( mulle_not_found_e);
}


static inline size_t  find_index( void **storage,
                                  void *p,
                                  uintptr_t hash,
                                  size_t modulo,
                                  size_t *hole_index,
                                  struct mulle_container_keycallback *callback)
{
   void        *q;
   size_t   i;
   
   i = hash_for_modulo( hash, modulo);
   q = storage[ i];
   if( ! q)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }
   return( _find_index( storage, p, q, modulo + 1, i, hole_index, callback));
}


void   *_mulle_indexedbucket_set_with_mode( struct _mulle_indexedbucket *bucket,
                                               void *p,
                                               uintptr_t hash,
                                               enum mulle_container_set_mode mode,
                                               struct mulle_container_keycallback *callback)
{
   void       *q;
   size_t     i;
   size_t     modulo;
   size_t     hole_index;

   modulo = mask_for_depth( bucket->_depth);
   
   i = find_index( bucket->_storage, p, hash, modulo, &hole_index, callback);
   if( i != (size_t) mulle_not_found_e)
   {
      q = bucket->_storage[ i];
      switch( mode)
      {
      case mulle_container_put_e  :
         q = bucket->_storage[ i];
         (*callback->release)( callback, q);
         bucket->_storage[ i] = p;
         return( NULL);  
         
      case mulle_container_insert_e :
         return( q);
      }
   }
      
   i = hole_index;
   if( ! _mulle_indexedbucket_is_fuller_than( bucket, modulo))
   {
      bucket->_storage[ i] = p;
      bucket->_count++;
      return( NULL);
   }

   modulo = grow( bucket, callback);

   i = hash_for_modulo( hash, modulo);
   store_pointer( bucket->_storage, i, modulo, p);
   bucket->_count++;

   return( NULL);
}


void    _mulle_indexedbucket_put( struct _mulle_indexedbucket *bucket, 
                                     void *p,
                                     struct mulle_container_keycallback *callback)
{
   size_t   hash;

   p    = (*callback->retain)( callback, p);
   hash = (*callback->hash)( callback, p);
   
   _mulle_indexedbucket_set_with_mode( bucket, p, hash, mulle_container_put_e, callback);
}


void    *_mulle_indexedbucket_insert( struct _mulle_indexedbucket *bucket,
                                                void *p,
                                                struct mulle_container_keycallback *callback)
{
   size_t   hash;
   
   p    = (*callback->retain)( callback, p);
   hash = (*callback->hash)( callback, p);
   
   return( _mulle_indexedbucket_set_with_mode( bucket, p, hash, mulle_container_insert_e, callback));
}                                       


void   *_mulle_indexedbucket_get( struct _mulle_indexedbucket *bucket,
                                  void *p,
                                  uintptr_t hash,
                                  struct mulle_container_keycallback *callback)
{
   void     *q;
   int      (*f)( void *, void *, void *);
   size_t   modulo;
   size_t   limit;
   size_t   i;
   
   modulo = mask_for_depth( bucket->_depth);
   i      = hash_for_modulo( hash, modulo);
   limit  = modulo + 1;

   f = (void *) callback->is_equal;
   while( q = bucket->_storage[ i])
   {
      if( p == q)
         break;
      if( (*f)( callback, p, q))
         break;
      if( ++i >= limit)
         i = 0;
   }

   return( q);
}


int   _mulle_indexedbucket_remove( struct _mulle_indexedbucket *bucket,
                                   void *p,
                                   uintptr_t hash,
                                   struct mulle_container_keycallback *callback)
{
   size_t   hole_index;
   size_t   i;
   size_t   modulo;
   size_t   dst_index;
   size_t   search_start;
   void     *q;
   
   modulo = mask_for_depth( bucket->_depth);
   i      = find_index( bucket->_storage, p, hash, modulo, &hole_index, callback);
   if( i == (size_t) mulle_not_found_e)
      return( 0);

   q = bucket->_storage[ i];
   (callback->release)( callback, q);  // get rid of it
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
      
      q = bucket->_storage[ i];
      if( ! q)
      {
         bucket->_storage[ dst_index] = NULL;
         break;
      }
   
      // check if its in the slot it's expected
      // otherwise it won't be found because of
      // the "hole" in the front we just made
      // but keep going because "wrong" hashes might be behind
      // be sure not to move items that won't be found
      hash         = _mulle_indexedbucket_hash( callback, q);
      search_start = hash_for_modulo( hash, modulo);

      // object better off where it is ?
      // CASE:   
      // [
      //    
      //   
      //    2 0x5205a0 (2 0xda000000)  *delete*
      //    3 0x531ae0 (2 0xda000000)  comulleider
      //   ]
      // dst_index=2, search_start=2, index=3
      //
      // CASE:   
      // [
      //    
      //    1 0x5671b0 (1 0xcad00000)  *delete*
      //    2 0x5205a0 (2 0xda000000)  
      //    3 0x531ae0 (3 0xdbfc0000)  comulleider
      //   ]
      // dst_index=1, search_start=3, index=3
      //
      // CASE:   
      // [
      //    
      //    1 0x5671b0 (1 0xcad00000)  *delete*
      //    2 0x5205a0 (2 0xda000000)  comulleider
      //    
      //   ]
      // dst_index=1, search_start=2, index=2
      //
      // CASE:   
      // [
      //    0 0x537a40 (0 0xdbfc0000)  comulleider
      //   
      //    
      //    3 0x531ae0 (3 0xdbfc0000)  *delete*
      //   ]
      // dst_index=3, search_start=0, index=0
      //
      // CASE:   
      // [
      //    0 0x537a40 (2 0xda000000)  comulleider
      //   
      //    2 0x5205a0 (2 0xda000000)
      //    3 0x531ae0 (2 0xda000000)  *delete*
      //   ]
      // dst_index=3, search_start=2, index=0
      //
      // CASE:   
      // [
      //    0 0x537a40 (0 0xdbfc0000)
      //    1 0x5205a0 (3 0xda000000)  comulleider
      //   
      //    3 0x531ae0 (3 0xdbfc0000)  *delete* 
      //   ]
      // dst_index=3, search_start=3, index=1
      //
      // CASE:   
      // [
      //    0 0x537a40 (0 0xdbfc0000)  *delete*
      //    1 0x5205a0 (3 0xda000000)  comulleider
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
      bucket->_storage[ dst_index] = q;
      dst_index = i;
   }
   return( 1);
}


// don't inline this (!)
void   _mulle_indexedbucket_reset( struct _mulle_indexedbucket *bucket,
                                   struct mulle_container_keycallback *callback)
{
   _mulle_indexedbucket_done( bucket, callback);
   _mulle_indexedbucket_init( bucket, _mulle_indexedbucket_size_for_depth( bucket->_depth), callback);
}


