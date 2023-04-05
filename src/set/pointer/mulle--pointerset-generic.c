//
//  Copyright (C) 2011-2020 Nat!, Mulle kybernetiK.
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
#include "mulle--pointerset-generic.h"

#include "mulle-container-math.h"
#include "mulle-container-callback.h"
#include "mulle-container-callback-global.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


# pragma mark - generic operations

// suitable for sue by mulle--set and mulle--pointerset


void   **_mulle__pointerset_allocate_storage_generic( unsigned int n,
                                                      void *notakey,
                                                      struct mulle_allocator *allocator)
{
   void   **buf;
   void   **p;
   void   **sentinel;

   assert( n >= 2);
   assert( mulle_is_pow2( n));

   if( notakey == NULL)
      return( mulle_allocator_calloc( allocator, n, sizeof( void *)));

   buf      = mulle_allocator_malloc( allocator, n * sizeof( void *));
   p        = &buf[ 0];
   sentinel = &buf[ n];
   while( p < sentinel)
      *p++ = notakey;

   return( buf);
}


// don't inline this (!)
void   _mulle__pointerset_reset_generic( struct mulle__pointerset *set,
                                         void *notakey)
{
   void   **p;
   void   **sentinel;

   p        = set->_storage;
   sentinel = &p[ set->_size];
   while( p < sentinel)
      *p++ = notakey;

   set->_count = 0;
}


static inline void   store_pointer_generic( void **data,
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


static void   copy_storage_generic( void **dst,
                                    unsigned int dst_size,
                                    void **src,
                                    unsigned int src_size,
                                    struct mulle_container_keycallback *callback)
{
   void           *p;
   void           **sentinel;
   unsigned int   i;
   uintptr_t      hash;

   sentinel = &src[ src_size];
   while( src < sentinel)
   {
      p = *src++;
      if( p == callback->notakey)
         continue;

      hash = (*callback->hash)( callback, p);
      i    = (unsigned int) mulle__pointerset_hash_for_size( hash, dst_size);
      store_pointer_generic( dst, dst_size, i, p, callback->notakey);
   }
}


static void   grow_generic( struct mulle__pointerset *set,
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

   if( new_size == 0)
      new_size = MULLE__POINTERSET_INITIAL_SIZE;

   assert( MULLE__POINTERSET_INITIAL_SIZE >= 2);

   buf = _mulle__pointerset_allocate_storage_generic( new_size, callback->notakey, allocator);
   copy_storage_generic( buf, new_size, set->_storage, set->_size, callback);
   mulle_allocator_free( allocator, set->_storage);

   set->_storage = buf;
   set->_size    = new_size;
}


static void   shrink_generic( struct mulle__pointerset *set,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator)
{
   void           **buf;
   unsigned int   new_size;

   new_size = set->_size / 2;
   if( new_size < MULLE__POINTERSET_INITIAL_SIZE)
      return;

   assert( set->_count < new_size);

   buf = _mulle__pointerset_allocate_storage_generic( new_size, callback->notakey, allocator);
   copy_storage_generic( buf, new_size, set->_storage, set->_size, callback);
   mulle_allocator_free( allocator, set);

   set->_storage = buf;
   set->_size    = new_size;
}


static uintptr_t  _find_index_generic( void  **storage,
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


static inline uintptr_t   find_index_generic( void **storage,
                                              unsigned int size,
                                              void *p,
                                              uintptr_t hash,
                                              unsigned int *hole_index,
                                              struct mulle_container_keycallback *callback)
{
   void           *q;
   unsigned int   i;

   assert( storage);

   i = mulle__pointerset_hash_for_size( hash, size);
   q = storage[ i];
   if( q == callback->notakey)
   {
      *hole_index = i;
      return( mulle_not_found_e);
   }
   return( _find_index_generic( storage, size, p, q, i, hole_index, callback));
}


//
// returns previous value, if something is overwritten or
// notakey if its a fresh insert
//
void   *_mulle__pointerset_write_generic( struct mulle__pointerset *set,
                                          void *p,
                                          enum mulle_container_write_mode mode,
                                          struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator)
{
   unsigned int   i;
   uintptr_t      hash;

   hash = (*callback->hash)( callback, p);
   if( set->_count)
   {
      void           *q;
      uintptr_t      found;
      unsigned int   hole_index;

      hole_index = 0xfeedface; // for the analyzer
      found = find_index_generic( set->_storage, set->_size, p, hash, &hole_index, callback);
      if( found != mulle_not_found_e)
      {
         i = (unsigned int) found;
         q = set->_storage[ i];
         switch( mode)
         {
         case mulle_container_overwrite_e  :
            q = set->_storage[ i];
            if( p == q)
               return( callback->notakey);

            p = (*callback->retain)( callback, p, allocator);
            set->_storage[ i] = p;
            (*callback->release)( callback, q, allocator);
            return( q);

         case mulle_container_insert_e :
            return( q);
         }
      }

      i = hole_index;
      if( ! _mulle__pointerset_is_full( set))
      {
         p = (*callback->retain)( callback, p, allocator);
         set->_storage[ i] = p;
         set->_count++;
         return( callback->notakey);
      }
   }

   if( _mulle__pointerset_is_full( set))
      grow_generic( set, callback, allocator);

   i = mulle__pointerset_hash_for_size( hash, set->_size);
   p = (*callback->retain)( callback, p, allocator);
   store_pointer_generic( set->_storage, set->_size, i, p, callback->notakey);
   set->_count++;

   return( callback->notakey);
}


static void   *
   _mulle__pointerset_pointerequalitysearch_generic( struct mulle__pointerset *set,
                                                     void *key,
                                                     void *notakey)
{
   void   **q;
   void   **sentinel;

   q        = set->_storage;
   sentinel = &q[ set->_size];

   //
   // first we search just for pointer equality of the key,
   // which can't be notakey so we don't care about notakey here
   //
   for( ; q < sentinel; q++)
      if( key == *q)
         return( key);  // pointer equality: can use key

   return( notakey);
}


void   *_mulle__pointerset__get_generic( struct mulle__pointerset *set,
                                         void *key,
                                         struct mulle_container_keycallback *callback)
{
   int            (*f)( void *, void *, void *);
   uintptr_t      hash;
   unsigned int   i;
   unsigned int   mask;
   unsigned int   size;
   void           **storage;
   void           *found;
   void           *notakey;

   hash    = (*callback->hash)( callback, key);
   storage = set->_storage;
   size    = set->_size;
   i       = mulle__pointerset_hash_for_size( hash, size);
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


void   *_mulle__pointerset_get_generic( struct mulle__pointerset *set,
                                        void *key,
                                        struct mulle_container_keycallback *callback)
{
   void   *value;

   // important to not hit a NULL _storage later
   if( set->_count == 0)
      return( callback->notakey);

   assert( set);
   if( set->_size <= 32)  // if the dictionary is small try to easy match
   {
      value = _mulle__pointerset_pointerequalitysearch_generic( set, key, callback->notakey);
      if( value != callback->notakey)
         return( value);
   }

   value = _mulle__pointerset__get_generic( set, key, callback);
   return( value);
}


void   _mulle__pointerset_shrink_generic( struct mulle__pointerset *set,
                                           struct mulle_container_keycallback *callback,
                                           struct mulle_allocator *allocator)
{
   assert( _mulle__pointerset_is_sparse( set));
   shrink_generic( set, callback, allocator);
}


int   _mulle__pointerset_remove_generic( struct mulle__pointerset *set,
                                         void *p,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator)
{
   uintptr_t      found;
   uintptr_t      hash;
   unsigned int   dst_index;
   unsigned int   hole_index;
   unsigned int   i;
   unsigned int   mask;
   unsigned int   search_start;
   unsigned int   size;
   void           *notakey;
   void           *q;

   if( ! set->_count)
      return( 0);

   hash  = (*callback->hash)( callback, p);
   found = find_index_generic( set->_storage, set->_size, p, hash, &hole_index, callback);
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
      hash         = (*callback->hash)( callback, q);
      search_start = mulle__pointerset_hash_for_size( hash, size);

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


int   _mulle__pointerset_copy_items_generic( struct mulle__pointerset *dst,
                                             struct mulle__pointerset *src,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator)
{
   struct mulle__genericpointersetenumerator  rover;
   void                                        *item;
   int                                         rval;

   rval  = 0;
   rover = _mulle__pointerset_enumerate_generic( src, callback);
   while( _mulle__genericpointersetenumerator_next( &rover, &item))
   {
      if( _mulle__pointerset_insert_generic( dst, item, callback, allocator))
      {
         rval = -1;
         break;
      }
   }
   mulle__genericpointersetenumerator_done( &rover);
   return( rval);
}
