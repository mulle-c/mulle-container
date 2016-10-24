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
 
#include "mulle_container_callback.h"
#include "mulle_container_operation.h"


/* set is a primitive growing hashtable */
#define _MULLE_SET_BASE              \
   void             **_storage;      \
   unsigned int     _count;          \
   short            _depth

// NSSet/NSMutableSet/NSHashTable

struct _mulle_set
{
   _MULLE_SET_BASE;
};


#define _MULLE_SETENUMERATOR_BASE   \
   void           **_curr;          \
   unsigned int   _left;            \
   void           *_notakey


struct _mulle_setenumerator
{
   _MULLE_SETENUMERATOR_BASE;
};


#pragma mark -
#pragma mark setup and takedown

struct _mulle_set   *_mulle_set_create( unsigned int capacity,
                                        size_t extra,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator);

void    _mulle_set_destroy( struct _mulle_set *set,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);


void    _mulle_set_init( struct _mulle_set *set, 
                         unsigned int capacity,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator);

void    _mulle_set_done( struct _mulle_set *set, 
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator);

// don't inline this (!)
void   _mulle_set_reset( struct _mulle_set *set,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator);


#pragma mark -
#pragma mark copying

int   _mulle_set_copy_items( struct _mulle_set *dst,
                             struct _mulle_set *src,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);

struct _mulle_set   *_mulle_set_copy( struct _mulle_set *set,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

#pragma mark -
#pragma mark debugging

char   *_mulle_set_describe( struct _mulle_set *set,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);

#pragma mark -
#pragma mark petty accessors

static inline unsigned int   _mulle_set_is_fuller_than( struct _mulle_set *set, unsigned int size)
{
   return( set->_count >= (size - (size >> 1)));  // full when only 25% free 
}


static inline unsigned int   _mulle_set_size_for_depth( int depth)
{
   return( 1U << depth);
}


static inline unsigned int   _mulle_set_is_full( struct _mulle_set *set)
{
   return( _mulle_set_is_fuller_than( set, _mulle_set_size_for_depth( set->_depth)));
}


static inline unsigned int   _mulle_set_get_storagesize( struct _mulle_set *set)
{
   return( 1U << set->_depth);
}


static inline unsigned int   _mulle_set_get_count( struct _mulle_set *set)
{
   return( set->_count);
}


#pragma mark -
#pragma mark operations

void   _mulle_set_set( struct _mulle_set *set,
                       void *p,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);

void    *_mulle_set_insert( struct _mulle_set *set,
                            void *p,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator);


void   *_mulle_set_get( struct _mulle_set *set,
                        void *p,
                        struct mulle_container_keycallback *callback);

int  _mulle_set_remove( struct _mulle_set *set,
                        void *p,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator);
          
#pragma mark -
#pragma mark interface for mulle_bigset

int   __mulle_set_remove( struct _mulle_set *set,
                          void *p,
                          unsigned int hash,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator);

void   *__mulle_set_get( struct _mulle_set *set,
                         void *p,
                         unsigned int hash,
                         struct mulle_container_keycallback *callback);

void   *_mulle_set_write( struct _mulle_set *set,
                          void *p,
                          unsigned int hash,
                          enum mulle_container_write_mode mode,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator);


#pragma mark -
#pragma mark enumeration

static inline struct
   _mulle_setenumerator   _mulle_set_enumerate( struct _mulle_set *set,
                                                struct mulle_container_keycallback *callback)
{
   struct _mulle_setenumerator   rover;
   
   rover._left             = set->_count;
   rover._curr             = set->_storage;
   rover._notakey = callback->not_a_key_marker;
   return( rover);
}


static inline void   *_mulle_setenumerator_next( struct _mulle_setenumerator *rover)
{
   void   *p;
   
   if( ! rover->_left)
      return( rover->_notakey);
      
   for(;;)
   {
      p = *rover->_curr++;
      if( p != rover->_notakey)
      {
         rover->_left--;
         return( p);
      }
   }
}


static inline void   _mulle_setenumerator_done( struct _mulle_setenumerator *rover)
{
}                                    

#endif
