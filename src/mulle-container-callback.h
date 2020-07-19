//  mulle-container
//
//  Created by Nat! on 24.03.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
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

#ifndef mulle_container_callback_h__
#define mulle_container_callback_h__

#include "include.h"

#include "mulle-container-operation.h"

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>


#if MULLE_ALLOCATOR_VERSION < ((2 << 20) | (1 << 8) | 0)
# error "mulle_allocator is too old"
#endif


#pragma mark - callback schemes for containers

#define mulle_container_not_an_int_key      ((void *) INT_MIN)
#define mulle_container_not_an_intptr_key   ((void *) mulle_not_a_pointer)
#define mulle_container_not_a_pointer_key   ((void *) mulle_not_a_pointer)


//
// Describe returns a cstring! This string may be "autoreleased" depending
// on circumstances. This is indicated by NULLing p_allocator.
//
struct mulle_container_keycallback
{
   uintptr_t   (*hash)( struct mulle_container_keycallback *callback, void *p);
   int         (*is_equal)( struct mulle_container_keycallback *callback, void *p, void *q);
   void        *(*retain)( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   void        (*release)( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);
   char        *(*describe)( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator **p_allocator);

   void        *notakey;   // don't make it INTPTR_MAX as that's mulle_not_found_e
   void        *userinfo;
};

typedef uintptr_t
   mulle_container_keycallback_hash_t( struct mulle_container_keycallback *callback,
                                       void *p);
typedef int
   mulle_container_keycallback_is_equal_t( struct mulle_container_keycallback *callback,
                                           void *p,
                                           void *q);
typedef void *
   mulle_container_keycallback_retain_t( struct mulle_container_keycallback *callback,
                                         void *p,
                                         struct mulle_allocator *allocator);
typedef void
   mulle_container_keycallback_release_t( struct mulle_container_keycallback *callback,
                                          void *p,
                                          struct mulle_allocator *allocator);
typedef char *
   mulle_container_keycallback_describe_t( struct mulle_container_keycallback *callback,
                                           void *p,
                                           struct mulle_allocator **allocator);


static inline void  *mulle_container_keycallback_get_notakey( struct mulle_container_keycallback *callback)
{
   return( callback->notakey);
}


static inline void  *mulle_container_keycallback_get_userinfo( struct mulle_container_keycallback *callback)
{
   return( callback->userinfo);
}


static inline void   assert_mulle_container_keycallback( struct mulle_container_keycallback *callback)
{
   assert( callback->hash);
   assert( callback->is_equal);
   assert( callback->retain);
   assert( callback->release);
   assert( callback->describe);
}


// when a pointer is passed in:
//   it is "nonowned" -> don't copy or free
//         "owned"    -> don't copy but free
//         "copied"   -> copy and free
//


struct mulle_container_valuecallback
{
   void   *(*retain)( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   void   (*release)( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);
   char   *(*describe)( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator **p_allocator);

   void   *userinfo;
};

typedef void *
   mulle_container_valuecallback_retain_t( struct mulle_container_keycallback *callback,
                                           void *p,
                                           struct mulle_allocator *allocator);
typedef void
   mulle_container_valuecallback_release_t( struct mulle_container_keycallback *callback,
                                            void *p,
                                            struct mulle_allocator *allocator);

typedef char *
   mulle_container_valuecallback_describe_t( struct mulle_container_valuecallback *callback,
                                             void *p,
                                             struct mulle_allocator **allocator);


static inline void   assert_mulle_container_valuecallback( struct mulle_container_valuecallback *callback)
{
   assert( callback->retain);
   assert( callback->release);
   assert( callback->describe);
}

void   *mulle_container_valuecallback_self( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *a);
void   mulle_container_valuecallback_nop( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *a);
char   *mulle_container_valuecallback_no_description( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator **p_allocator);

#define mulle_container_keycallback_self \
   ((mulle_container_keycallback_retain_t *) mulle_container_valuecallback_self)

#define mulle_container_keycallback_nop \
   ((mulle_container_keycallback_release_t *) mulle_container_valuecallback_nop)

#define mulle_container_keycallback_no_description \
   ((mulle_container_keycallback_describe_t *)  mulle_container_valuecallback_no_description)


uintptr_t   mulle_container_keycallback_pointer_hash( struct mulle_container_keycallback *callback, void *p);
int         mulle_container_keycallback_pointer_is_equal(  struct mulle_container_keycallback *callback, void *a, void *b);
void        mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator);

void        mulle_container_valuecallback_pointer_free( struct mulle_container_valuecallback *callback, void *p, struct mulle_allocator *allocator);


struct mulle_container_keyvaluecallback
{
   struct mulle_container_keycallback     keycallback;
   struct mulle_container_valuecallback   valuecallback;
};


static inline void   assert_mulle_container_keyvaluecallback( struct mulle_container_keyvaluecallback *callback)
{
   assert_mulle_container_keycallback( &callback->keycallback);
   assert_mulle_container_valuecallback( &callback->valuecallback);
}


static inline int   mulle_container_keycallback_releases( struct mulle_container_keycallback *callback)
{
   return( callback->release != mulle_container_keycallback_nop);
}


static inline int   mulle_container_valuecallback_releases( struct mulle_container_valuecallback *callback)
{
   return( callback->release != mulle_container_valuecallback_nop);
}


static inline int   mulle_container_keyvaluecallback_releases( struct mulle_container_keyvaluecallback *callback)
{
   return( mulle_container_keycallback_releases( &callback->keycallback) ||
           mulle_container_valuecallback_releases( &callback->valuecallback));
}

#endif

