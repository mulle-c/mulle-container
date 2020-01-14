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
#pragma clang diagnostic ignored "-Wparentheses"

#include "_mulle-set.h"

#include "mulle-container-math.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 *
 */
#define _MULLE_SET_INITIAL_SIZE  4


/**
  * Bit twiddling code from
  * https://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
  * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 **/
//
// 0 is also a power of two for these purposes
//

#pragma clang diagnostic ignored "-Wshift-count-overflow"



// assume we have
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 an 2 for better insert perfomance
//
// 0 2
static unsigned int   _mulle_set_hash_for_size( unsigned int hash, unsigned int size)
{
   assert( size >= 2);
   return( hash & (size - 2));
}


static inline unsigned int   _mulle_set_hash( struct mulle_container_keycallback *callback, void *p)
{
   return( (unsigned int) (*callback->hash)( callback, p));
}


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
   assert_mulle_container_keycallback( callback);

   p->_count   = 0;
   //
   // our set requires zeroes to find an end
   // so give it ~25% holes. For this to work though, we can not be smaller
   // than 4 items.
   //
   p->_size    = capacity >= _MULLE_SET_MIN_SIZE
                     ? mulle_pow2round( capacity + (capacity >> _MULLE_SET_FILL_SHIFT))
                     : (callback->notakey != 0 ? _MULLE_SET_MIN_SIZE : 0);
   p->_storage = allocate_storage( p->_size, callback->notakey, allocator);
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


void   _mulle_set_done( struct _mulle_set *set,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   struct _mulle_setenumerator  rover;
   void   *item;

   if( mulle_container_keycallback_releases( callback))
   {
      rover = _mulle_set_enumerate( set, callback);
      while(  _mulle_setenumerator_next( &rover, &item))
      {
         (*callback->release)( callback, item, allocator);
      }
      _mulle_setenumerator_done( &rover);
   }
   mulle_allocator_free( allocator, set->_storage);
}


// don't inline this (!)
void   _mulle_set_reset( struct _mulle_set *set,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
{
   _mulle_set_done( set, callback, allocator);
   _mulle_set_init( set, 0, callback, allocator);
}



void    _mulle_set_destroy( struct _mulle_set *set,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle_set_done( set, callback, allocator);
   mulle_allocator_free( allocator, set);
}


#pragma mark -
#pragma mark operations

static inline void   store_pointer( void **data,
                                    unsigned int size,
                                    unsigned int i,
                                    void *p,
                                    void *notakey)
{
   while( data[ i] != notakey)
   {
      if( ++i < size)
         continue;
      i = 0;
   }

   data[ i] = p;
}


static void   copy_storage( void **dst,
                            unsigned int size,
                            void **src,
                            unsigned int n,
                            struct mulle_container_keycallback *callback)
{
   void           *p;
   unsigned int   i;
   unsigned int   hash;
   void           *notakey;

   assert( size >= n);

   notakey = callback->notakey;
   for( ;n--;)
   {
      p = *src++;
      if( p == notakey)
         continue;

      hash = _mulle_set_hash( callback, p);
      i    = _mulle_set_hash_for_size( hash, size);
      store_pointer( dst, size, i, p, callback->notakey);
   }
}


static void   grow( struct _mulle_set *set,
                    struct mulle_container_keycallback *callback,
                    struct mulle_allocator *allocator)
{
   unsigned int   new_size;
   void           **buf;

   // for good "not found" performance, there should be a high possibility of
   // a NULL after each slot
   //
   new_size = set->_size * 2;
   if( new_size < set->_size)
      abort();  // overflow

   if( new_size < _MULLE_SET_INITIAL_SIZE)
      new_size = _MULLE_SET_INITIAL_SIZE;

   buf = allocate_storage( new_size, callback->notakey, allocator);
   copy_storage( buf, new_size, set->_storage, set->_size, callback);
   mulle_allocator_free( allocator, set->_storage);

   set->_storage = buf;
   set->_size    = new_size;
}


static void   shrink( struct _mulle_set *set,
                      struct mulle_container_keycallback *callback,
                      struct mulle_allocator *allocator)
{
   void           **buf;
   unsigned int   new_size;

   new_size = set->_size / 2;
   if( new_size < _MULLE_SET_INITIAL_SIZE)
      return;

   buf = allocate_storage( new_size, callback->notakey, allocator);
   copy_storage( buf, new_size, set->_storage, set->_size, callback);
   mulle_allocator_free( allocator, set);

   set->_storage = buf;
   set->_size    = new_size;
}


static unsigned long  _find_index( void  **storage,
                                   unsigned int size,
                                   void *p,
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
   param2  = p;
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
                                         void *p,
                                         unsigned int hash,
                                         unsigned int *hole_index,
                                         struct mulle_container_keycallback *callback)
{
   void           *q;
   unsigned int   i;

   assert( storage);

   i = _mulle_set_hash_for_size( hash, size);
   q = storage[ i];
   if( q == callback->notakey)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }
   return( _find_index( storage, size, p, q, i, hole_index, callback));
}


void   *_mulle_set_write( struct _mulle_set *set,
                          void *p,
                          unsigned int hash,
                          enum mulle_container_write_mode mode,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   unsigned int   i;

   if( set->_count)
   {
      void             *q;
      unsigned long    found;
      unsigned int     hole_index;

      found = find_index( set->_storage, set->_size, p, hash, &hole_index, callback);
      if( found != mulle_not_found_e)
      {
         i = (unsigned int) found;
         q = set->_storage[ i];
         switch( mode)
         {
         case mulle_container_overwrite_e  :
            q = set->_storage[ i];
            if( p != q)
            {
               p = (*callback->retain)( callback, p, allocator);
               (*callback->release)( callback, q, allocator);
               set->_storage[ i] = p;
            }
            return( NULL);

         case mulle_container_insert_e :
            return( q);
         }
      }


      i = hole_index;
      if( ! _mulle_set_is_full( set))
      {
         p = (*callback->retain)( callback, p, allocator);
         set->_storage[ i] = p;
         set->_count++;
         return( NULL);
      }
   }

   if( _mulle_set_is_full( set))
      grow( set, callback, allocator);

   i = _mulle_set_hash_for_size( hash, set->_size);
   p = (*callback->retain)( callback, p, allocator);
   store_pointer( set->_storage, set->_size, i, p, callback->notakey);
   set->_count++;

   return( NULL);
}


void    _mulle_set_set( struct _mulle_set *set,
                        void *p,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->hash)( callback, p);
   _mulle_set_write( set, p, hash, mulle_container_overwrite_e, callback, allocator);
}


void    *_mulle_set_insert( struct _mulle_set *set,
                            void *p,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   unsigned int   hash;

   hash = (unsigned int) (*callback->hash)( callback, p);
   return( _mulle_set_write( set, p, hash, mulle_container_insert_e, callback, allocator));
}



void   *_mulle_set_get_with_hash( struct _mulle_set *set,
                                  void *key,
                                  unsigned int hash,
                                  struct mulle_container_keycallback *callback)
{
   int            (*f)( void *, void *, void *);
   unsigned int   i;
   unsigned int   size;
   unsigned int   mask;
   void           *found;
   void           **storage;
   void           *notakey;

   storage = set->_storage;
   size    = set->_size;
   i       = _mulle_set_hash_for_size( hash, size);
   f       = (int (*)()) callback->is_equal;
   notakey = callback->notakey;
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

   return( found);
}


static void   *
   _mulle_set_pointerequality_search( struct _mulle_set *set,
                                      void *key)
{
   void     **q;
   void     **sentinel;

   q        = set->_storage;
   sentinel = &q[ set->_size];

   //
   // first we search just for pointer equality of the key,
   // which can't be notakey so we don't care about notakey here
   //
   for( ; q < sentinel; q++)
      if( key == *q)
         return( key);  // pointer equality: can use key

   return( NULL);
}


void   *_mulle_set_get( struct _mulle_set *set,
                        void *key,
                        struct mulle_container_keycallback *callback)
{
   unsigned int   hash;
   void           *value;

   // important to not hit a NULL storage later
   if( set->_count == 0)
      return( NULL);

   assert( set);
   if( set->_size <= 32)  // if the dictionary is small try to easy match
   {
      value = _mulle_set_pointerequality_search( set, key);
      if( value)
         return( value);
   }

   hash  = (unsigned int) (*callback->hash)( callback, key);
   value = _mulle_set_get_with_hash( set, key, hash, callback);
   return( value);
}


void   *__mulle_set_get( struct _mulle_set *set,
                         void *p,
                         unsigned int hash,
                         struct mulle_container_keycallback *callback)
{
   void           *q;
   void           *notakey;
   int            (*f)( void *, void *, void *);
   unsigned int   size;
   unsigned int   i;
   unsigned int   mask;

   size    = set->_size;
   i       = _mulle_set_hash_for_size( hash, size);
   f       = (int (*)()) callback->is_equal;
   mask    = size - 1;
   notakey = callback->notakey;

   for(;;)
   {
      q = set->_storage[ i];
      if( p == q)
         break;
      if( q == notakey)
         break;
      if( (*f)( callback, p, q))
         break;
      i = (i + 1) & mask;
   }

   return( q);
}



void   _mulle_set_shrink( struct _mulle_set *set,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   assert( _mulle_set_is_sparse( set));
   shrink( set, callback, allocator);
}


int   __mulle_set_remove( struct _mulle_set *set,
                          void *p,
                          unsigned int hash,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   unsigned int   hole_index;
   unsigned long  found;
   unsigned int   i;
   unsigned int   size;
   unsigned int   dst_index;
   unsigned int   search_start;
   void           *q;
   void           *notakey;
   unsigned int   mask;

   found = find_index( set->_storage, set->_size, p, hash, &hole_index, callback);
   if( found == mulle_not_found_e)
      return( 0);

   i = (unsigned int) found;
   q = set->_storage[ i];
   (callback->release)( callback, q, allocator);  // get rid of it
   set->_count--;

   // now we may need to do a whole lot of shifting, if
   // the following object isn't in its proper hash index.
   // Because of the way objects are inserted and deleted
   // we have to possibly move EVERYTHING until we hit the
   // hole.

   dst_index = i;
   size      = set->_size;
   notakey   = callback->notakey;
   mask      = size - 1;

   for(;;)
   {
      i = (i + 1) & mask;
      q = set->_storage[ i];
      if( q == notakey)
      {
         set->_storage[ dst_index] = notakey;
         break;
      }

      // check if its in the slot it's expected
      // otherwise it won't be found because of
      // the "hole" in the front we just made
      // but keep going because "wrong" hashes might be behind
      // be sure not to move items that won't be found
      hash         = _mulle_set_hash( callback, q);
      search_start = _mulle_set_hash_for_size( hash, size);

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
      set->_storage[ dst_index] = q;
      dst_index                 = i;
   }
   return( 1);
}


int  _mulle_set_remove( struct _mulle_set *set,
                        void *p,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   unsigned int   hash;

   if( ! set->_count)
      return( 0);

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
   while( _mulle_setenumerator_next( &rover, &item))
   {
      if( _mulle_set_insert( dst, item, callback, allocator))
      {
         rval = -1;
         break;
      }
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
   struct mulle_allocator        *value_allocator;
   void                          *item;

   result = NULL;
   len    = 0;

   rover = _mulle_set_enumerate( set, callback);
   while( _mulle_setenumerator_next( &rover, &item))
   {
      value_allocator = allocator ? allocator : &mulle_default_allocator;

      s      = (*callback->describe)( callback, item, &value_allocator);
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

      if( value_allocator)
         mulle_allocator_free( value_allocator, s);
   }
   _mulle_setenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


