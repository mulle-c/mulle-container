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

#include "mulle-container-callback.h"
#include "mulle-container-operation.h"


#define _MULLE_SET_FILL_SHIFT    2
#define _MULLE_SET_MIN_SIZE      (1 << _MULLE_SET_FILL_SHIFT)

// counts are unsigned int, the result multiplied by sizeof is size_t */

/* set is a primitive growing hashtable */
#define _MULLE_SET_BASE              \
   void             **_storage;      \
   unsigned int     _count;          \
   unsigned int     _size

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


#pragma mark - setup and takedown

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


#pragma mark - copying

int   _mulle_set_copy_items( struct _mulle_set *dst,
                             struct _mulle_set *src,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);

struct _mulle_set   *_mulle_set_copy( struct _mulle_set *set,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator);

#pragma mark - debugging

char   *_mulle_set_describe( struct _mulle_set *set,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);

#pragma mark - petty accessors


static inline unsigned int   _mulle_set_get_size( struct _mulle_set *set)
{
   return( set->_size);
}


static inline unsigned int   _mulle_set_get_count( struct _mulle_set *set)
{
   return( set->_count);
}


#pragma mark - query


static inline int  _mulle_set_is_full( struct _mulle_set *set)
{
   unsigned int    size;

   size = set->_size;
   size = (size - (size >> _MULLE_SET_FILL_SHIFT));  // full when 75% occupied
   return( set->_count >= size);
}

static inline int  _mulle_set_is_sparse( struct _mulle_set *set)
{
   unsigned int    size;

   size = set->_size / 2;
   size = (size - (size >> _MULLE_SET_FILL_SHIFT));  // full when 75% occupied
   return( set->_count < size);
}



#pragma mark - operations


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

#pragma mark - interface for mulle_bigset

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


static inline void  _mulle_set_shrink_if_needed( struct _mulle_set *set,
                                                 struct mulle_container_keycallback *callback,
                                                 struct mulle_allocator *allocator)
{
   void   _mulle_set_shrink( struct _mulle_set *set,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);

   if( _mulle_set_is_sparse( set))
      _mulle_set_shrink( set, callback, allocator);
}


#pragma mark - enumeration


static inline struct _mulle_setenumerator
   _mulle_set_enumerate( struct _mulle_set *set,
                         struct mulle_container_keycallback *callback);

static inline struct _mulle_setenumerator
   _mulle_set_enumerate( struct _mulle_set *set,
                         struct mulle_container_keycallback *callback)
{
   struct _mulle_setenumerator   rover;

   rover._left    = set->_count;
   rover._curr    = set->_storage;
   rover._notakey = callback->notakey;
   return( rover);
}


static inline void   *_mulle_setenumerator_next_nil( struct _mulle_setenumerator *rover)
{
   void   *p;

   assert( rover->_notakey == NULL);

   if( ! rover->_left)
      return( NULL);

   for(;;)
   {
      p = *rover->_curr++;
      if( p)
      {
         rover->_left--;
         return( p);
      }
   }
}


static inline int   _mulle_setenumerator_next( struct _mulle_setenumerator *rover,
                                               void **item)
{
   void   *p;

   if( ! rover->_left)
   {
      if( item)
         *item = rover->_notakey;  // useful for NSHashTableEnumeration
      return( 0);
   }

   for(;;)
   {
      p = *rover->_curr++;
      if( p != rover->_notakey)
      {
         rover->_left--;
         if( item)
            *item = p;
         return( 1);
      }
   }
}


static inline void   _mulle_setenumerator_done( struct _mulle_setenumerator *rover)
{
}

#endif
