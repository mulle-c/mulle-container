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
#include "_mulle_bigset.h"

#include "mulle_container_operation.h"
#include "mulle_container_callback.h"
#include "mulle_prime.h"

#include <assert.h>
#include <string.h>



int    __mulle_bigset_copy( struct _mulle_bigset *dst,
                            struct _mulle_bigset *src,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);

void   *_mulle_bigset_write( struct _mulle_bigset *set,
                             void *p,
                             enum mulle_container_write_mode mode,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);


#define _MULLE_SET_MAX_DEPTH              30

#define _MULLE_SET_MIN_INITIAL_DEPTH      0  
#define _MULLE_SET_INITIAL_DEPTH          3
#define _MULLE_SET_MAX_INITIAL_DEPTH      16  
#define _MULLE_HASH_STOP_SINGLE_DIMENSION_GROWTH 5

static short   good_depth_for_depth( short depth)
{
   assert( depth >= 0);
   return( depth < _MULLE_SET_INITIAL_DEPTH ? _MULLE_SET_INITIAL_DEPTH : depth); 
}


static short   depth_for_capacity( size_t capacity)
{
   unsigned short  i;
   
   if( ! capacity)
      return( _MULLE_SET_MIN_INITIAL_DEPTH);
      
   capacity >>= 2;
   for( i = _MULLE_SET_MIN_INITIAL_DEPTH; i <= _MULLE_SET_MAX_INITIAL_DEPTH; i++)
      if( capacity <= mulle_prime_for_depth( i))
         return( good_depth_for_depth( i));
   return( _MULLE_SET_MAX_INITIAL_DEPTH);
}


static void  _mulle_bigset_init_with_depth( struct _mulle_bigset *set,
                                            int depth,
                                            struct mulle_container_keycallback *callback,
                                            struct mulle_allocator *allocator)
{
   size_t   modulo;
   
   assert( depth >= 0 && depth < _MULLE_SET_MAX_DEPTH);
   assert( callback);
   
   set->_count        = 0;
   set->_storage      = NULL;
   set->_storageTypes = NULL;
   if( ! depth)
      return;
   
   modulo        = mulle_prime_for_depth( depth);
   set->_storage = mulle_allocator_calloc( allocator, modulo, (sizeof( void *) + sizeof( char)));
   if( set->_storage)
   {
      set->_storageTypes = (char *) &set->_storage[ modulo];
      set->_depth        = (short) depth;
   }
}


void  _mulle_bigset_init( struct _mulle_bigset *set,
                          size_t capacity,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   _mulle_bigset_init_with_depth( set, depth_for_capacity( capacity), callback, allocator);
}



struct _mulle_bigset   *_mulle_bigset_create( size_t capacity,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   struct _mulle_bigset *set;
   
   set  = mulle_allocator_malloc( allocator, sizeof( struct _mulle_bigset));
   if( set)
      _mulle_bigset_init( set, capacity, callback, allocator);
   return( set);
}


void   _mulle_bigset_destroy( struct _mulle_bigset *set,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator)
{
   _mulle_bigset_done( set, callback, allocator);
   mulle_allocator_free( allocator, set);
}


void   _mulle_bigset_reset( struct _mulle_bigset *set,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle_bigset_done( set, callback, allocator);
   _mulle_bigset_init( set, 0, callback, allocator);
}


void   _mulle_bigset_done( struct _mulle_bigset *set,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{
   void    **p;
   void    **sentinel;
   char    *q;

   sentinel = &set->_storage[ mulle_prime_for_depth( set->_depth)];
   for( q = set->_storageTypes, p = set->_storage; p < sentinel; p++, q++)
   {
      if( ! *p)
         continue;
      if( ! *q)
      {
         (*callback->release)( callback, *p, allocator);
         continue;
      }
      _mulle_set_destroy( *p, callback, allocator);
   }
   mulle_allocator_free( allocator, set->_storage);
}


int   __mulle_bigset_copy( struct _mulle_bigset *dst,
                        struct _mulle_bigset *src,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   struct _mulle_bigsetenumerator  rover;
   void                               *item;
   int                                rval;
   
   rval  = 0;
   rover = _mulle_bigset_enumerate( src, callback);
   while( item = _mulle_bigsetenumerator_next( &rover))
      if( _mulle_bigset_insert( dst, item, callback, allocator))
      {
         rval = -1;
         break;
      }
   _mulle_bigsetenumerator_done( &rover);
   return( rval);
}


struct _mulle_bigset   *_mulle_bigset_copy( struct _mulle_bigset *set,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct _mulle_bigset   *other;
   
   other = _mulle_bigset_create( _mulle_bigset_get_count( set), callback, allocator);
   if( __mulle_bigset_copy( other, set, callback, allocator))
   {
      _mulle_bigset_destroy( other, callback, allocator);
      other = NULL;
   }
   return( other);
}


extern void   *mulle_container_callback_self( void *, void *, struct mulle_allocator *allocator);
extern void   mulle_container_callback_nop( void *, void *, struct mulle_allocator *allocator);


//
// we "just" create a new hashtable with the new size and then copy
// the other set into it. The we move the ivars around and delted the
// new one...
//
static int   grow_vertically( struct _mulle_bigset *set,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator)
{
   struct _mulle_bigset                     copy;
   struct mulle_container_keycallback    tmpCallBacks;
   short                                 depth;
   int                                   rval;
   depth = set->_depth;
   if( depth < 0)
      return( 0);

   depth = good_depth_for_depth( depth + 1);
      
   tmpCallBacks         = *callback;
   tmpCallBacks.retain  = (void *(*)()) mulle_container_callback_self;
   tmpCallBacks.release = (void (*)()) mulle_container_callback_nop;
    
   _mulle_bigset_init( &copy, - depth, callback, allocator);  /* mark negative, so we don't grow within growing */
   rval = __mulle_bigset_copy( &copy, set, callback, allocator);
   if( rval)
   {
      _mulle_bigset_done( &copy, callback, allocator);
      return( 0);
   }
   
   _mulle_bigset_done( set, &tmpCallBacks, allocator);
   
   memcpy( set, &copy, sizeof( struct _mulle_bigset));
   set->_depth = depth;
   
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
void   *_mulle_bigset_write( struct _mulle_bigset *set,
                         void *p,
                         enum mulle_container_write_mode mode,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
{
   size_t              i;
   size_t              modulo;
   size_t              size;
   struct _mulle_set   *bucket;
   uintptr_t           hash;
   uintptr_t           other_hash;
   void                *q;
   
   assert(  p);

   p    = (*callback->retain)( callback, p, allocator);
   hash = (*callback->hash)( callback, p);
   
retry:
   modulo = mulle_prime_for_depth( set->_depth);
   if( ! modulo)
   {
      if( grow_vertically( set, callback, allocator))
         goto retry;
      return( NULL);
   }
      
   i = hash % modulo; 

   assert( set->_storage);
   
   q = set->_storage[ i];
   if( ! q)
   {
      set->_count++;
      set->_storage[ i] = p;
      return( NULL);
   }
   
   if( ! set->_storageTypes[ i])
   {  
      if( p == q)
         goto short_circuit;
         
      other_hash = (*callback->hash)( callback, q);
      if( hash == other_hash && (*callback->is_equal)( callback, p, q))
      {
short_circuit:      
         switch( mode)
         {
         case mulle_container_insert_e :
            return( q);
      
         case mulle_container_overwrite_e :
            (*callback->release)( callback, q, allocator);
            set->_storage[ i] = p;
            return( NULL);
         }
      }
      
      //
      // a) grow vertically or start a struct _mulle_set chain
      //
      if( set->_depth < _MULLE_HASH_STOP_SINGLE_DIMENSION_GROWTH)
         if( grow_vertically( set, callback, allocator))
            goto retry;
      
      // b) create a new bucket chain (quadruple size, of current vertical)
      // start small
      bucket = _mulle_set_create( 4, 0, callback, allocator);
      _mulle_set_write( bucket, q, (unsigned int) other_hash, mulle_container_insert_e, callback, allocator);  // analyzer: not true
      _mulle_set_write( bucket, p, (unsigned int) hash, mulle_container_insert_e, callback, allocator);
      set->_count++;
      
      set->_storage[ i]      = bucket;
      set->_storageTypes[ i] = 1;
      return( NULL);
   }
   
   // now make sure, the bucket don't grow to large
   // with respect to vertical size
   bucket = (struct _mulle_set *) q;
   if( _mulle_set_is_full( bucket))
   {
      size = _mulle_set_get_storagesize( bucket);
      if( size * 2 > modulo * 4)
         if( grow_vertically( set, callback, allocator))
            goto retry;
   }
   
   if( ! _mulle_set_write( bucket, p, (unsigned int) hash, mode, callback, allocator))
      set->_count++;
   return( NULL);
}


void   _mulle_bigset_remove( struct _mulle_bigset *set,
                          void *p,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   size_t   hash;
   size_t   i;
   size_t   modulo;
   struct _mulle_set  *bucket;
   void         *q;
   
   modulo = mulle_prime_for_depth( set->_depth);
   if( ! modulo)
      return;
      
   hash = (*callback->hash)( callback, p);
   i    = hash % modulo; 

   q = set->_storage[ i];
   if( ! q)
      return;
   
   if( ! set->_storageTypes[ i])
   {
      if( p == q || (hash == (*callback->hash)( callback, q) && (*callback->is_equal)( callback, p, q)))
      {
         (*callback->release)( callback, q, allocator);
         set->_storage[ i] = NULL;
         set->_count--;
      }
      return;
   }
   
   bucket = (struct _mulle_set *) q;
   if( __mulle_set_remove( bucket, p, (unsigned int) hash, callback, allocator))
      set->_count--;
}


void   *_mulle_bigset_get( struct _mulle_bigset *set,
                        void *p,
                        struct mulle_container_keycallback *callback)
{
   size_t   hash;
   size_t   modulo;
   size_t   i;
   void     *q;
   void     **storage;
   
   modulo = mulle_prime_for_depth( set->_depth);
   
   // for small hashtables try quick check
   storage = set->_storage;
   switch( modulo)
   {
   case 5 : if( *storage == p)
               return( p);
            ++storage;
            if( *storage == p)
               return( p);
            ++storage;
            if( *storage == p)
               return( p);
            ++storage;
   case 2 : if( *storage == p)
               return( p);
            ++storage;
   case 1 : if( *storage == p)
               return( p);
   }  
   
   hash = (*callback->hash)( callback, p);
   i    = mulle_prime_hash_for_depth( hash, set->_depth); 
   
   q = set->_storage[ i];
   if( ! set->_storageTypes[ i])
   {
      if( p == q || (hash == (*callback->hash)( callback, q) && (*callback->is_equal)( callback, p, q)))
         return( q);
      return( NULL);
   }
   
   return( __mulle_set_get( q, p, (unsigned int) hash, callback));
}


void   *_mulle_bigsetenumerator_next( struct _mulle_bigsetenumerator *rover)
{
   void   *q;
   void   **storage;
   
   if( ! rover->_left)
      return( NULL);

   --rover->_left;
retry:   
   q = _mulle_setenumerator_next( &rover->_bucket_rover);
   if( q == rover->_bucket_rover._notakey)
   {
      storage = rover->_table->_storage;
      while( ! (q = storage[ rover->_index]))
         ++rover->_index;

      if( rover->_table->_storageTypes[ rover->_index++])
      {
         rover->_bucket_rover = _mulle_set_enumerate( q, rover->_callback);
         goto retry;
      }
   }
   return( q);
}

