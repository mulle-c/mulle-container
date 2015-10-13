//
//  mulle_callback_type.h
//  mulle-types
//
//  Created by Nat! on 24.03.15.
//  Copyright (c) 2015 Mulle kybernetiK. All rights reserved.
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

#ifndef mulle_callback_type_h__
#define mulle_callback_type_h__

#include <stdint.h>
#include <stddef.h>


#pragma mark -
#pragma mark _mulle_allocator

//
// _mulle_allocator just a way to pass around the memory scheme du jour
// to containers. Eventually this header will wind up in a separate
// mulle_c_containers project,
//
struct _mulle_allocator
{
   void   *(*calloc)( size_t, size_t);
   void   (*free)( void *);
};

extern struct _mulle_allocator   mulle_stdlib_allocator;


typedef void   *mulle_lockfree_calloc_t( size_t n, size_t size);
typedef void   mulle_lockfree_free_t( void *);


static inline mulle_lockfree_calloc_t  *_mulle_allocator_get_calloc( struct _mulle_allocator *p)
{
   return( p ? p->calloc : mulle_stdlib_allocator.calloc);
}


static inline mulle_lockfree_free_t  *_mulle_allocator_get_free( struct _mulle_allocator *p)
{
   return( p ? p->free : mulle_stdlib_allocator.free);
}


static inline void  *__mulle_allocator_calloc( struct _mulle_allocator *p, size_t n, size_t size)
{
   return( (*_mulle_allocator_get_calloc( p))( n, size));
}


static inline void  __mulle_allocator_free( struct _mulle_allocator *p, void *block)
{
   (*_mulle_allocator_get_free( p))( block);
}


#pragma mark -
#pragma mark _mulle_comparator
//
// _mulle_comparator just a way to pass around memory comparison
// schemes to containers
//
struct _mulle_comparator
{
   unsigned int   (*hash)( intptr_t);
   int            (*compare)( void *, void *);
};

typedef unsigned int   mulle_lockfree_hash_t( intptr_t);
typedef int            mulle_lockfree_compare_t( void *, void *);

extern struct _mulle_comparator   mulle_default_comparator;


static inline mulle_lockfree_hash_t  *_mulle_comparator_get_hash( struct _mulle_comparator *p)
{
   return( p ? p->hash : mulle_default_comparator.hash);
}


static inline mulle_lockfree_compare_t  *_mulle_comparator_get_compare( struct _mulle_comparator *p)
{
   return( p ? p->compare : mulle_default_comparator.compare);
}


static inline unsigned int  _mulle_comparator_hash( struct _mulle_comparator *comparator, intptr_t key)
{
   return( (*_mulle_comparator_get_hash( comparator))( key));
}


static inline int  _mulle_comparator_compare( struct _mulle_comparator *comparator, void *a, void *b)
{
   return( (*_mulle_comparator_get_compare( comparator))( a, b));
}


#pragma mark -
#pragma mark _mulle_callback
//
// _mulle_callback combines allocation and comparison into a
// callback scheme for mulle_lockfree containers
//
struct _mulle_callback
{
   struct _mulle_allocator     allocator;
   struct _mulle_comparator    comparator;
};

extern struct _mulle_callback    mulle_default_callback;


static inline mulle_lockfree_calloc_t  *_mulle_callback_get_calloc( struct _mulle_callback *p)
{
   return( p ? p->allocator.calloc : mulle_default_callback.allocator.calloc);
}


static inline mulle_lockfree_free_t  *_mulle_callback_get_free( struct _mulle_callback *p)
{
   return( p ? p->allocator.free : mulle_default_callback.allocator.free);
}


static inline mulle_lockfree_hash_t  *_mulle_callback_get_hash( struct _mulle_callback *p)
{
   return( p ? p->comparator.hash : mulle_default_callback.comparator.hash);
}


static inline mulle_lockfree_compare_t  *_mulle_callback_get_compare( struct _mulle_callback *p)
{
   return( p ? p->comparator.compare : mulle_default_callback.comparator.compare);
}



static inline void  *_mulle_callback_calloc( struct _mulle_callback *p, size_t n, size_t size)
{
   return( (*_mulle_callback_get_calloc( p))( n, size));
}


static inline void  _mulle_callback_free( struct _mulle_callback *p, void *block)
{
   (*_mulle_callback_get_free( p))( block);
}


static inline unsigned int  _mulle_callback_hash( struct _mulle_callback *p, intptr_t key)
{
   return( (*_mulle_callback_get_hash)( p)( key));
}


static inline int  _mulle_callback_compare( struct _mulle_callback *p, void *a, void *b)
{
   return( (*_mulle_callback_get_compare( p))( a, b));
}

#endif
