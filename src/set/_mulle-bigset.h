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
#ifndef _mulle_bigset__h__
#define _mulle_bigset__h__

#include "_mulle-set.h"
#include "mulle-container-operation.h"



#define _MULLE_BIGSET_BASE       \
   void         **_storage;      \
   char         *_storageTypes;  \
   uintptr_t    _count;          \
   short        _depth


struct _mulle_bigset
{
   _MULLE_BIGSET_BASE;
};


#define _MULLE_BIGSETENUMERATOR_BASE                  \
   struct _mulle_setenumerator   _bucket_rover;       \
   size_t                                  _index;    \
   size_t                                  _left;     \
   struct _mulle_bigset                    *_table;   \
   struct mulle_container_keycallback      *_callback


struct  _mulle_bigsetenumerator
{
   _MULLE_BIGSETENUMERATOR_BASE;
};



struct _mulle_bigset   *_mulle_bigset_create( size_t capacity, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);

void   _mulle_bigset_init( struct _mulle_bigset *set, size_t capacity, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);
void   _mulle_bigset_done( struct _mulle_bigset *set, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);
void   _mulle_bigset_destroy( struct _mulle_bigset *set, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);
void   _mulle_bigset_reset( struct _mulle_bigset *set, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);

void   *_mulle_bigset_get( struct _mulle_bigset *set, void *p, struct mulle_container_keycallback *callback);
void   _mulle_bigset_remove( struct _mulle_bigset *set, void *p, struct mulle_container_keycallback *callback, struct mulle_allocator *allocator);


static inline size_t   _mulle_bigset_get_count( struct _mulle_bigset *set)
{
   return( set->_count);
}


struct _mulle_bigset   *_mulle_bigset_copy( struct _mulle_bigset *set,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);


# pragma mark -
# pragma mark Enumeration

static inline
struct _mulle_bigsetenumerator   _mulle_bigset_enumerate( struct _mulle_bigset *set,
                                                          struct mulle_container_keycallback *callback)
{
   struct _mulle_bigsetenumerator   rover;

   rover._bucket_rover._left = 0;

   rover._index    = 0;
   rover._left     = set->_count;
   rover._table    = set;
   rover._callback = callback;  // for not a key marker
   return( rover);
}


void   *_mulle_bigsetenumerator_next( struct _mulle_bigsetenumerator *rover);


static inline void   _mulle_bigsetenumerator_done( struct _mulle_bigsetenumerator *rover)
{
}


# pragma mark -
# pragma mark Convenience interface for insertion

void   *_mulle_bigset_write( struct _mulle_bigset *,
                             void *p,
                             enum mulle_container_write_mode,
                             struct mulle_container_keycallback *,
                             struct mulle_allocator *allocator);

static inline void   _mulle_bigset_set( struct _mulle_bigset *set,
                                        void *p,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   _mulle_bigset_write( set, p, mulle_container_overwrite_e, callback, allocator);
}


static inline void   *_mulle_bigset_insert( struct _mulle_bigset *set,
                                            void *p,
                                            struct mulle_container_keycallback *callback,
                                            struct mulle_allocator *allocator)
{
   return( _mulle_bigset_write( set, p, mulle_container_insert_e, callback, allocator));
}

#endif

