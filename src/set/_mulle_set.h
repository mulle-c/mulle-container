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
#ifndef _mulle_set__h__
#define _mulle_set__h__

#include "_mulle_indexedbucket.h"
#include "mulle_container_operation.h"



/* 
 * this is a little hashtable, whose hashing and memory management
 * is governed by struct mulle_container_keycallback, which has to
 * be passed in with every call. The hashtable only stores a pointer
 * it does not store key-value pairs.
 */
#define _MULLE_SET_BASE          \
   void         **_storage;      \
   char         *_storageTypes;  \
   size_t       _count;          \
   short        _depth


struct _mulle_set
{
   _MULLE_SET_BASE;
};


#define _MULLE_SETENUMERATOR_BASE                           \
   struct _mulle_indexedbucketenumerator   _bucket_rover;   \
   size_t                                  _index;          \
   size_t                                  _left;           \
   struct _mulle_set                       *_table;         \
   struct mulle_container_keycallback      *_callback


struct  _mulle_setenumerator
{
   _MULLE_SETENUMERATOR_BASE;
};



struct _mulle_set   *_mulle_set_create( size_t capacity, struct mulle_container_keycallback *callback);

void   _mulle_set_init( struct _mulle_set *set, size_t capacity, struct mulle_container_keycallback *callback);
void   _mulle_set_done( struct _mulle_set *set, struct mulle_container_keycallback *callback);
void   _mulle_set_free( struct _mulle_set *set, struct mulle_container_keycallback *callback);
void   _mulle_set_reset( struct _mulle_set *set, struct mulle_container_keycallback *callback);

void   *_mulle_set_get( struct _mulle_set *set, void *p, struct mulle_container_keycallback *callback);
void   _mulle_set_remove( struct _mulle_set *set, void *p, struct mulle_container_keycallback *callback);


static inline size_t   _mulle_set_count( struct _mulle_set *set)
{
   return( set->_count);
}


struct _mulle_set   *_mulle_set_copy( struct _mulle_set *set,
                                      struct mulle_container_keycallback *callback);


# pragma mark -
# pragma mark Enumeration

static inline struct _mulle_setenumerator   _mulle_set_enumerate( struct _mulle_set *set,
                                                                  struct mulle_container_keycallback *callback)
{
   struct _mulle_setenumerator   rover;
   
   rover._bucket_rover._left = 0;
   
   rover._index    = 0;
   rover._left     = set->_count;
   rover._table    = set;
   rover._callback = callback;
   return( rover);
}


void   *_mulle_setenumerator_next( struct _mulle_setenumerator *rover);


static inline void   _mulle_setenumerator_done( struct _mulle_setenumerator *rover)
{
}


# pragma mark -
# pragma mark Convenience interface for insertion

void   *__mulle_set_put( struct _mulle_set *,
                         void *,
                         enum mulle_container_set_mode,
                         struct mulle_container_keycallback *);

static inline void   _mulle_set_put( struct _mulle_set *set,
                                     void *p,
                                     struct mulle_container_keycallback *callback)
{
   __mulle_set_put( set, p, mulle_container_put_e, callback);
}


static inline void   *_mulle_set_insert( struct _mulle_set *set,
                                         void *p,
                                         struct mulle_container_keycallback *callback)
{
   return( __mulle_set_put( set, p, mulle_container_insert_e, callback));
}

#endif

