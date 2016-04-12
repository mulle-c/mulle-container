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
#ifndef mulle_indexedbucket__h__
#define mulle_indexedbucket__h__
 
#include "mulle_container_callback.h"
#include "mulle_container_operation.h"


/* indexedbucket is a primitive growing hashtable */
#define _MULLE_INDEXEDBUCKET_BASE    \
   void             **_storage;      \
   size_t           _count;          \
   short            _depth


struct _mulle_indexedbucket
{
   _MULLE_INDEXEDBUCKET_BASE;
};


struct _mulle_indexedbucketenumerator
{
   void     **_curr;
   size_t   _left;
   void     *_not_a_key_marker;
};


struct _mulle_indexedbucket   *_mulle_indexedbucket_create( size_t capacity,
                                                            size_t extra,
                                                            struct mulle_container_keycallback *callback,
                                                            struct mulle_allocator *allocator);

void    _mulle_indexedbucket_free( struct _mulle_indexedbucket *bucket,
                                   struct mulle_container_keycallback *callback,
                                   struct mulle_allocator *allocator);


void    _mulle_indexedbucket_init( struct _mulle_indexedbucket *bucket, 
                                   size_t capacity,
                                   struct mulle_container_keycallback *callback,
                                   struct mulle_allocator *allocator);

void    _mulle_indexedbucket_done( struct _mulle_indexedbucket *bucket, 
                                   struct mulle_container_keycallback *callback,
                                   struct mulle_allocator *allocator);


static inline size_t   _mulle_indexedbucket_is_fuller_than( struct _mulle_indexedbucket *bucket, size_t size)
{
   return( bucket->_count >= (size - (size >> 1)));  // full when only 25% free 
}


static inline size_t   _mulle_indexedbucket_size_for_depth( int depth)
{
   return( 1UL << depth);
}


static inline size_t   _mulle_indexedbucket_is_full( struct _mulle_indexedbucket *bucket)
{
   return( _mulle_indexedbucket_is_fuller_than( bucket, _mulle_indexedbucket_size_for_depth( bucket->_depth)));
}


// don't inline this (!)
void   _mulle_indexedbucket_reset( struct _mulle_indexedbucket *bucket,
                                   struct mulle_container_keycallback *callback,
                                   struct mulle_allocator *allocator);


void   *_mulle_indexedbucket_set_with_mode( struct _mulle_indexedbucket *bucket,
                                            void *p,
                                            uintptr_t hash,
                                            enum mulle_container_set_mode mode,
                                            struct mulle_container_keycallback *callback,
                                            struct mulle_allocator *allocator);

void   _mulle_indexedbucket_put( struct _mulle_indexedbucket *bucket,
                                 void *p,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator);

void    *_mulle_indexedbucket_insert( struct _mulle_indexedbucket *bucket,
                                      void *p,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

int   _mulle_indexedbucket_remove( struct _mulle_indexedbucket *bucket,
                                   void *p,
                                   uintptr_t hash,
                                   struct mulle_container_keycallback *callback,
                                   struct mulle_allocator *allocator);

void   *_mulle_indexedbucket_get( struct _mulle_indexedbucket *bucket,
                                  void *p,
                                  uintptr_t hash,
                                  struct mulle_container_keycallback *callback);


static inline size_t   _mulle_indexedbucket_get_storagesize( struct _mulle_indexedbucket *bucket)
{
   return( 1UL << bucket->_depth);
}


static inline struct
   _mulle_indexedbucketenumerator   _mulle_indexedbucket_enumerate( struct _mulle_indexedbucket *bucket,
                                                                    struct mulle_container_keycallback *callback)
{
   struct _mulle_indexedbucketenumerator   rover;
   
   rover._left             = bucket->_count;
   rover._curr             = bucket->_storage;
   rover._not_a_key_marker = callback->not_a_key_marker;
   return( rover);
}


static inline void   *_mulle_indexedbucketenumerator_next( struct _mulle_indexedbucketenumerator *rover)
{
   void   *p;
   
   if( ! rover->_left)
      return( rover->_not_a_key_marker);
      
   for(;;)
   {
      p = *rover->_curr++;
      if( p != rover->_not_a_key_marker)
      {
         rover->_left--;
         return( p);
      }
   }
}


static inline void   _mulle_indexedbucketenumerator_done( struct _mulle_indexedbucketenumerator *rover)
{
}                                    


static inline size_t   _mulle_indexedbucket_get_count( struct _mulle_indexedbucket *bucket)
{
   return( bucket->_count);
}
   
#endif
