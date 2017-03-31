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
#ifndef mulle_bigset__h__
#define mulle_bigset__h__

#include "_mulle_bigset.h"
#include "mulle_container_callback.h"
#include <mulle_allocator/mulle_allocator.h>


//
// a mulle_bigset is a mulle_hashtable, with callbacks added
// this makes a mulle_bigset more convenient.
//
#define MULLE_BIGSET_BASE                             \
   _MULLE_SET_BASE;                                   \
   struct mulle_container_keycallback   *_callback;   \
   struct mulle_allocator               *_allocator


struct mulle_bigset
{
   MULLE_BIGSET_BASE;
};


#define MULLE_BIGSETENUMERATOR_BASE   \
   _MULLE_BIGSETENUMERATOR_BASE


struct  mulle_bigsetenumerator
{
   MULLE_BIGSETENUMERATOR_BASE;
};


struct mulle_bigset   *mulle_bigset_create( size_t capacity,
                                            struct mulle_container_keycallback *callback,
                                            struct mulle_allocator *allocator);

void   mulle_bigset_init( struct mulle_bigset *set,
                          size_t capacity,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator);


static inline void   mulle_bigset_done( struct mulle_bigset *set)
{
   _mulle_bigset_done( (struct _mulle_bigset *) set, set->_callback, set->_allocator);
}


static inline void   mulle_bigset_destroy( struct mulle_bigset *set)
{
   _mulle_bigset_destroy( (struct _mulle_bigset *) set, set->_callback, set->_allocator);
}


static inline void   mulle_bigset_reset( struct mulle_bigset *set)
{
   _mulle_bigset_reset( (struct _mulle_bigset *) set, set->_callback, set->_allocator);
}


static inline void   *mulle_bigset_get( struct mulle_bigset *set, void *p)
{
   return( _mulle_bigset_get( (struct _mulle_bigset *) set, p, set->_callback));
}


static inline void   mulle_bigset_remove( struct mulle_bigset *set, void *p)
{
   _mulle_bigset_remove( (struct _mulle_bigset *) set, p, set->_callback, set->_allocator);
}


static inline size_t   mulle_bigset_get_count( struct mulle_bigset *set)
{
   return( _mulle_bigset_get_count( (struct _mulle_bigset *) set));
}


static inline struct mulle_bigset   *mulle_bigset_copy( struct mulle_bigset *set)
{
   return( (struct mulle_bigset *) _mulle_bigset_copy( (struct _mulle_bigset *) set, set->_callback, set->_allocator));
}


static inline struct mulle_allocator   *mulle_bigset_get_allocator( struct mulle_bigset *set)
{
   return( set->_allocator);
}


static inline struct mulle_container_keycallback   *mulle_bigset_get_keycallback( struct mulle_bigset *set)
{
   return( set->_callback);
}


#pragma mark -
#pragma mark enumerator


static inline struct mulle_bigsetenumerator   mulle_bigset_enumerate( struct mulle_bigset *set)
{
   struct _mulle_bigsetenumerator  rover;

   rover = _mulle_bigset_enumerate( (struct _mulle_bigset *) set, set->_callback);
   return( *(struct mulle_bigsetenumerator *) &rover);
}


static inline void   *mulle_bigsetenumerator_next( struct mulle_bigsetenumerator *rover)
{
   return( _mulle_bigsetenumerator_next( (struct _mulle_bigsetenumerator *) rover));
}


static inline void   mulle_bigsetenumerator_done( struct mulle_bigsetenumerator *rover)
{
   _mulle_bigsetenumerator_done( (struct _mulle_bigsetenumerator *) rover);
}


static inline void   mulle_bigset_set( struct mulle_bigset *set, void *p)
{
   _mulle_bigset_set( (struct _mulle_bigset *) set, p, set->_callback, set->_allocator);
}


static inline void   *mulle_bigset_insert( struct mulle_bigset *set,
                                        void *p)
{
   return( _mulle_bigset_write( (struct _mulle_bigset *) set, p, mulle_container_insert_e, set->_callback, set->_allocator));
}

#endif

