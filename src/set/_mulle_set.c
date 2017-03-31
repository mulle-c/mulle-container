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
#include "_mulle_set.h"
#include "mulle_container_operation.h"
#include "mulle_container_callback.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 *
 */
#define _MULLE_SET_S_INITIAL_DEPTH    1
#define _MULLE_SET_S_MAX_DEPTH        30

//
// with 4, large tables will be 1 MB pretty quickly even with little filling
// try 8KB sometime and see the kernel go on a VM rampage, because of some
// throttling mechanism its actually slowing down things...
//
#define _MULLE_SET_S_MIN_INITIAL_DEPTH    1
#define _MULLE_SET_S_MAX_INITIAL_DEPTH    3


static inline unsigned int   mask_for_depth( int depth)
{
   return( (1U << depth) - 1);
}


// assume we have
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 an 2 for better insert perfomance
//
// 0 2
static unsigned int   hash_for_modulo( unsigned int hash, unsigned int modulo)
{
   modulo &= ~1;
   hash   &= modulo;
   return( hash);
}


static short   depth_for_capacity( unsigned int capacity)
{
   unsigned int  i;

   capacity >>= 1;
   for( i = _MULLE_SET_S_MIN_INITIAL_DEPTH; i <= _MULLE_SET_S_MAX_INITIAL_DEPTH; i++)
      if( capacity <= _mulle_set_size_for_depth( i))
         return( (short) i);
   return( _MULLE_SET_S_MAX_INITIAL_DEPTH);
}


static inline unsigned int   _mulle_set_hash( struct mulle_container_keycallback *callback, void *p)
{
   return( (unsigned int) (*callback->hash)( callback, p));
}



static void   **allocate_pointers( size_t n,
                                   void *notakey,
                                   struct mulle_allocator *allocator)
{
   void   **buf;
   void   **p;
   void   **sentinel;

   if( ! notakey)
      return( mulle_allocator_calloc( allocator, n, sizeof( void *)));

   buf      = mulle_allocator_malloc( allocator, n * sizeof( void *));
   p        = &buf[ 0];
   sentinel = &buf[ n];
   while( p < sentinel)
      *p++ = notakey;

   return( buf);
}


void    _mulle_set_init( struct _mulle_set *p,
                         unsigned int capacity,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
{
   unsigned int   n;
   short          depth;

   depth       = depth_for_capacity( capacity);
   n           = _mulle_set_size_for_depth( depth) + 1;
   p->_count   = 0;
   p->_depth   = depth;
   p->_storage = allocate_pointers( n, callback->notakey, allocator);
}


struct _mulle_set   *_mulle_set_create( unsigned int capacity,
                                        size_t extra,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   struct _mulle_set   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct _mulle_set) + extra);
   _mulle_set_init( p, capacity, callback, allocator);
   return( p);
}


void   _mulle_set_done( struct _mulle_set *bucket,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   struct _mulle_setenumerator  rover;
   void   *item;

   rover = _mulle_set_enumerate( bucket, callback);
   while( item = _mulle_setenumerator_next( &rover))
      (*callback->release)( callback, item, allocator);
   _mulle_setenumerator_done( &rover);

   mulle_allocator_free( allocator, bucket->_storage);
}


// don't inline this (!)
void   _mulle_set_reset( struct _mulle_set *bucket,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
{
   _mulle_set_done( bucket, callback, allocator);
   _mulle_set_init( bucket, _mulle_set_size_for_depth( bucket->_depth), callback, allocator);
}



void    _mulle_set_destroy( struct _mulle_set *bucket,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle_set_done( bucket, callback, allocator);
   mulle_allocator_free( allocator, bucket);
}


#pragma mark -
#pragma mark operations

static inline void   store_pointer( void **data,
                                    unsigned int i,
                                    unsigned int modulo,
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
                            unsigned int modulo,
                            void **src,
                            unsigned int n,
                            struct mulle_container_keycallback *callback)
{
   void           *p;
   unsigned int   i;
   unsigned int   hash;

   assert( modulo + 1 >= n);

   for( ;n--;)
   {
      p = *src++;
      if( ! p)
         continue;

      hash = _mulle_set_hash( callback, p);
      i    = (unsigned int) hash_for_modulo( hash, modulo);
      store_pointer( dst, i, modulo, p);
   }
}


static unsigned int   grow( struct _mulle_set *bucket,
                      struct mulle_container_keycallback *callback,
                      struct mulle_allocator *allocator)
{
   unsigned int   modulo;
   unsigned int   new_modulo;
   short    depth;
   void     *buf;

   // for good "not found" performance, there should be a high possibility of
   // a NULL after each slot
   //
   depth  = bucket->_depth;
   modulo = mask_for_depth( depth);
   if( depth == _MULLE_SET_S_MAX_DEPTH)
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

   buf = allocate_pointers( new_modulo + 1, callback->notakey, allocator);
   if( ! buf)
      return( modulo);

   copy_buckets( buf, new_modulo, bucket->_storage, modulo + 1, callback);
   mulle_allocator_free( allocator, bucket->_storage);

   bucket->_depth   = depth;
   bucket->_storage = buf;

   return( new_modulo);
}


static unsigned long  _find_index( void  **storage,
                                   void *p,
                                   void *q,
                                   unsigned int limit,
                                   unsigned int i,
                                   unsigned int *hole_index,
                                   struct mulle_container_keycallback *callback)
{
   int   (*f)( void *, void *, void *);
   void   *param1;
   void   *param2;

   f      = (int (*)()) callback->is_equal;
   param1 = callback;
   param2 = p;

   do
   {
      if( (*f)( param1, param2, q))
         return( i);
      if( ++i >= limit)
         i = 0;
   }
   while( q = storage[ i]);

   *hole_index = i;
   return( mulle_not_found_e);
}


static inline unsigned long  find_index( void **storage,
                                         void *p,
                                         unsigned int hash,
                                         unsigned int modulo,
                                         unsigned int *hole_index,
                                         struct mulle_container_keycallback *callback)
{
   void        *q;
   unsigned int   i;

   i = hash_for_modulo( hash, modulo);
   q = storage[ i];
   if( ! q)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }
   return( _find_index( storage, p, q, modulo + 1, i, hole_index, callback));
}


void   *_mulle_set_write( struct _mulle_set *bucket,
                          void *p,
                          unsigned int hash,
                          enum mulle_container_write_mode mode,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   void             *q;
   unsigned long    found;
   unsigned int     i;
   unsigned int     modulo;
   unsigned int     hole_index;

   modulo = mask_for_depth( bucket->_depth);

   found = find_index( bucket->_storage, p, hash, modulo, &hole_index, callback);
   if( found != mulle_not_found_e)
   {
      i = (unsigned int) found;
      q = bucket->_storage[ i];
      switch( mode)
      {
      case mulle_container_overwrite_e  :
         q = bucket->_storage[ i];
         if( p != q)
         {
            p = (*callback->retain)( callback, p, allocator);
            (*callback->release)( callback, q, allocator);
            bucket->_storage[ i] = p;
         }
         return( NULL);

      case mulle_container_insert_e :
         return( q);
      }
   }

   p = (*callback->retain)( callback, p, allocator);

   i = hole_index;
   if( ! _mulle_set_is_fuller_than( bucket, modulo))
   {
      bucket->_storage[ i] = p;
      bucket->_count++;
      return( NULL);
   }

   modulo = grow( bucket, callback, allocator);

   i = hash_for_modulo( hash, modulo);
   store_pointer( bucket->_storage, i, modulo, p);
   bucket->_count++;

   return( NULL);
}


void    _mulle_set_set( struct _mulle_set *bucket,
                        void *p,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->hash)( callback, p);
   _mulle_set_write( bucket, p, hash, mulle_container_overwrite_e, callback, allocator);
}


void    *_mulle_set_insert( struct _mulle_set *bucket,
                                      void *p,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->hash)( callback, p);
   return( _mulle_set_write( bucket, p, hash, mulle_container_insert_e, callback, allocator));
}


void   *__mulle_set_get( struct _mulle_set *bucket,
                         void *p,
                         unsigned int hash,
                         struct mulle_container_keycallback *callback)
{
   void     *q;
   int      (*f)( void *, void *, void *);
   unsigned int   modulo;
   unsigned int   limit;
   unsigned int   i;

   modulo = mask_for_depth( bucket->_depth);
   i      = hash_for_modulo( hash, modulo);
   limit  = modulo + 1;

   f = (int (*)()) callback->is_equal;
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


int   __mulle_set_remove( struct _mulle_set *bucket,
                          void *p,
                          unsigned int hash,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   unsigned int   hole_index;
   unsigned long  found;
   unsigned int   i;
   unsigned int   modulo;
   unsigned int   dst_index;
   unsigned int   search_start;
   void     *q;

   modulo = mask_for_depth( bucket->_depth);
   found  = find_index( bucket->_storage, p, hash, modulo, &hole_index, callback);
   if( found == mulle_not_found_e)
      return( 0);

   i = (unsigned int) found;
   q = bucket->_storage[ i];
   (callback->release)( callback, q, allocator);  // get rid of it
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
      hash         = _mulle_set_hash( callback, q);
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


void   *_mulle_set_get( struct _mulle_set *set,
                        void *p,
                        struct mulle_container_keycallback *callback)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->hash)( callback, p);
   return( __mulle_set_get( set, p, hash, callback));
}


int  _mulle_set_remove( struct _mulle_set *set,
                        void *p,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->hash)( callback, p);
   return( __mulle_set_remove( set, p, hash, callback, allocator));
}


int   _mulle_set_copy_items( struct _mulle_set *dst,
                             struct _mulle_set *src,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator)
{
   struct _mulle_setenumerator  rover;
   void                         *item;
   int                          rval;

   rval  = 0;
   rover = _mulle_set_enumerate( src, callback);
   while( item = _mulle_setenumerator_next( &rover))
      if( _mulle_set_insert( dst, item, callback, allocator))
      {
         rval = -1;
         break;
      }
   _mulle_setenumerator_done( &rover);
   return( rval);
}


struct _mulle_set   *_mulle_set_copy( struct _mulle_set *set,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct _mulle_set   *other;

   other = _mulle_set_create( _mulle_set_get_count( set), 0, callback, allocator);
   if( _mulle_set_copy_items( other, set, callback, allocator))
   {
      _mulle_set_destroy( other, callback, allocator);
      other = NULL;
   }
   return( other);
}


// use this only for debugging
char   *_mulle_set_describe( struct _mulle_set *set,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator)
{
   char                          *result;
   char                          *s;
   int                           separate;
   size_t                        len;
   size_t                        s_len;
   struct _mulle_setenumerator   rover;
   void                          *item;

   result = NULL;
   len    = 0;

   rover = _mulle_set_enumerate( set, callback);
   while( item = _mulle_setenumerator_next( &rover))
   {
      s      = (*callback->describe)( callback, item, allocator);
      s_len  = strlen( s);

      separate = result != NULL;
      result   = mulle_allocator_realloc( allocator, result, len + (separate * 2) + s_len + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len += 2;
      }

      memcpy( &result[ len], s, s_len);
      len += s_len;

      mulle_allocator_free( allocator, s);
   }
   _mulle_setenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


