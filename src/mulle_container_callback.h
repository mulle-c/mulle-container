//  mulle-container
//
//  Created by Nat! on 24.03.15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
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
#include <limits.h>

#include <mulle_allocator/mulle_allocator.h>


#pragma mark -
#pragma mark callback schemes for containers

#define mulle_container_not_an_int_key          ((void *) INT_MIN)
#define mulle_container_not_an_intptr_key	((void *) INTPTR_MIN)
#define mulle_container_not_a_pointer_key	((void *) UINTPTR_MAX)

struct mulle_container_keycallback
{
   uintptr_t   (*hash)( struct mulle_container_keycallback *callback, void *p);
   int         (*is_equal)( struct mulle_container_keycallback *callback, void *p, void *q);
   void        *(*retain)( struct mulle_container_keycallback *callback, void *p);
   void        (*release)( struct mulle_container_keycallback *callback, void *p);
   void        *(*describe)( struct mulle_container_keycallback *callback, void *p);
   
   void        *userinfo;
   void        *not_a_key_marker;
   
   struct mulle_allocator   *allocator;
};


// when a pointer is passed in:
//   it is "nonowned" -> don't copy or free
//         "owned"    -> don't copy but free
//         "copied"   -> copy and free
//
extern struct mulle_container_keycallback   mulle_container_keycallback_int;
extern struct mulle_container_keycallback   mulle_container_keycallback_intptr;
extern struct mulle_container_keycallback   mulle_container_keycallback_copied_cstring;
extern struct mulle_container_keycallback   mulle_container_keycallback_nonowned_cstring;
extern struct mulle_container_keycallback   mulle_container_keycallback_owned_cstring;
extern struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer;
extern struct mulle_container_keycallback   mulle_container_keycallback_owned_pointer;
extern struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer_or_null;
extern struct mulle_container_keycallback   mulle_container_keycallback_pointer_or_null;


struct mulle_container_valuecallback
{
   void   *(*retain)( struct mulle_container_valuecallback *callback, void *p);
   void   (*release)( struct mulle_container_valuecallback *callback, void *p);
   void   *(*describe)( struct mulle_container_valuecallback *callback, void *p);

   void   *userinfo;

   struct mulle_allocator   *allocator;
};


extern struct mulle_container_valuecallback   mulle_container_valuecallback_int;
extern struct mulle_container_valuecallback   mulle_container_valuecallback_intptr;
extern struct mulle_container_valuecallback   mulle_container_valuecallback_copied_cstring;
extern struct mulle_container_valuecallback   mulle_container_valuecallback_owned_cstring;
extern struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_cstring;
extern struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_pointer;
extern struct mulle_container_valuecallback   mulle_container_valuecallback_owned_pointer;


void            mulle_container_keycallback_set_default_values( struct mulle_container_keycallback *p);
void            mulle_container_valuecallback_set_default_values( struct mulle_container_valuecallback *p);

void           *mulle_container_callback_self( void *ignore, void *p);
void            mulle_container_callback_nop( void *ignore, void *p);
void           *mulle_container_callback_no_value( void *ignore, void *p);
unsigned long   mulle_container_callback_pointer_hash( void *ignore, void *p);
int             mulle_container_callback_pointer_is_equal( void *ignore, void *a, void *b);

void            mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback, void *p);
void            mulle_container_valuecallback_pointer_free( struct mulle_container_valuecallback *callback, void *p);


struct mulle_container_keyvaluecallback
{
   struct mulle_container_keycallback       keycallback;
   struct mulle_container_valuecallback     valuecallback;
};

#endif
