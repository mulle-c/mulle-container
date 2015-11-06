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
#ifndef _MULLE_INDEXEDKEYVALUEBUCKET_H___
#define _MULLE_INDEXEDKEYVALUEBUCKET_H___     
 

#include "mulle_container_callback.h"
#include "_mulle_keyvaluepair.h"


#define _MULLE_INDEXEDKEYVALUEBUCKET_BASE   \
   struct _mulle_keyvaluepair *_storage;    \
   size_t         _count;                   \
   short          _depth;                   \
   short          _inZone

struct _mulle_indexedkeyvaluebucket
{
   _MULLE_INDEXEDKEYVALUEBUCKET_BASE;
};


#define _MULLE_INDEXEDKEYVALUEBUCKETENUMERATOR_BASE         \
   struct _mulle_keyvaluepair   *_curr;                     \
   size_t                        _left;                     \
   struct mulle_container_keyvaluecallback   *_callback


struct _mulle_indexedkeyvaluebucketenumerator
{
   _MULLE_INDEXEDKEYVALUEBUCKETENUMERATOR_BASE;
};


struct _mulle_indexedkeyvaluebucket   *_mulle_indexedkeyvaluebucket_create( size_t capacity,
                                                                            size_t extra,
                                                                            struct mulle_container_keyvaluecallback *callback);


void   _mulle_indexedkeyvaluebucket_free( struct _mulle_indexedkeyvaluebucket *bucket,
                                          struct mulle_container_keyvaluecallback *callback);

void   _mulle_indexedkeyvaluebucket_init( struct _mulle_indexedkeyvaluebucket *bucket,
                                          size_t capacity,
                                          struct mulle_container_keyvaluecallback *callback);


void   _mulle_indexedkeyvaluebucket_done( struct _mulle_indexedkeyvaluebucket *bucket,
                                            struct mulle_container_keyvaluecallback *callback);


static inline size_t   _mulle_indexedkeyvaluebucket_is_fuller_than( struct _mulle_indexedkeyvaluebucket *bucket,
                                                                    size_t size)
{
   return( bucket->_count >= (size - (size >> 1)));  // full when only 25% free 
}


static inline size_t   _mulle_indexedkeyvaluebucket_size_for_depth( int depth)
{
   return( 1UL << depth);
}


static inline size_t   _mulle_indexedkeyvaluebucket_is_full( struct _mulle_indexedkeyvaluebucket *bucket)
{
   return( _mulle_indexedkeyvaluebucket_is_fuller_than( bucket, _mulle_indexedkeyvaluebucket_size_for_depth( bucket->_depth)));
}


void   _mulle_indexedkeyvaluebucket_reset( struct _mulle_indexedkeyvaluebucket *bucket, 
                                           struct mulle_container_keyvaluecallback *callback);


void   _mulle_indexedkeyvaluebucket_put( struct _mulle_indexedkeyvaluebucket *bucket, 
                                            struct _mulle_keyvaluepair *pair,
                                            struct mulle_container_keyvaluecallback *callback);

void    *_mulle_indexedkeyvaluebucket_insert( struct _mulle_indexedkeyvaluebucket *bucket,
                                                        struct _mulle_keyvaluepair *pair,
                                                       struct mulle_container_keyvaluecallback *callback);

void   *_mulle_indexedkeyvaluebucket_insert_known_absent( struct _mulle_indexedkeyvaluebucket *bucket,
                                                          struct _mulle_keyvaluepair *pair,
                                                         struct mulle_container_keyvaluecallback *callback);

int   __mulle_indexedkeyvaluebucket_remove( struct _mulle_indexedkeyvaluebucket *bucket,
                                            void *key,
                                            uintptr_t hash,
                                            struct mulle_container_keyvaluecallback *callback);

int   _mulle_indexedkeyvaluebucket_remove( struct _mulle_indexedkeyvaluebucket *bucket,
                                           void *key,
                                           struct mulle_container_keyvaluecallback *callback);

void   *_mulle_indexedkeyvaluebucket_get( struct _mulle_indexedkeyvaluebucket *bucket,
                                          void *key,
                                          uintptr_t hash,
                                          struct mulle_container_keyvaluecallback *callback);


static inline size_t   _mulle_indexedkeyvaluebucket_count( struct _mulle_indexedkeyvaluebucket *bucket)
{
   return( bucket->_count);
}


static inline size_t   _mulle_indexedkeyvaluebucket_get_storagesize( struct _mulle_indexedkeyvaluebucket *bucket)
{
   return( 1UL << bucket->_depth);
}


# pragma mark -
# pragma mark enumeration

static inline struct _mulle_indexedkeyvaluebucketenumerator
   _mulle_indexedkeyvaluebucket_enumerate( struct _mulle_indexedkeyvaluebucket *bucket,
                                           struct mulle_container_keyvaluecallback *callback)
{
   struct _mulle_indexedkeyvaluebucketenumerator   rover;
   
   rover._left     = bucket->_count;
   rover._curr     = bucket->_storage;
   rover._callback = callback;
   
   return( rover);
}


static inline struct _mulle_keyvaluepair   *_mulle_indexedkeyvaluebucketenumerator_next( struct _mulle_indexedkeyvaluebucketenumerator *rover)
{
   struct _mulle_keyvaluepair   *p;
   
   if( ! rover->_left)
      return( 0);
      
   rover->_left--;
   for(;;)
   {
      p = rover->_curr++;
      if( p->_key != rover->_callback->keycallback.not_a_key_marker)
         return( p);
   }
}


static inline void   _mulle_indexedkeyvaluebucketenumerator_done( struct _mulle_indexedkeyvaluebucketenumerator *rover)
{
}                                    


#endif
