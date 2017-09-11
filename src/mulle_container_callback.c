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


#include "mulle_container_callback.h"
#include "hash/mulle_hash.h"

#include <mulle_c11/mulle_c11.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


#define MULLE_CONTAINER_GLOBAL  MULLE_C_GLOBAL

uintptr_t   mulle_container_keycallback_pointer_hash( struct mulle_container_keycallback *callback, void *p)
{
   return( mulle_hash_pointer( p));
}


int   mulle_container_keycallback_pointer_is_equal( struct mulle_container_keycallback *callback, void *a, void *b)
{
   return( a == b);
}


void   mulle_container_callback_nop( void *ignore, void *p, struct mulle_allocator *a)
{
}


void   *mulle_container_callback_self( void *ignore, void *p, struct mulle_allocator *a)
{
   return( p);
}


void   *mulle_container_callback_no_value( void *ignore, void *p, struct mulle_allocator *allocator)
{
   return( 0);
}


void   mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator)
{
   if( p != callback->notakey)
      mulle_allocator_free( allocator, p);
}


void   mulle_container_valuecallback_pointer_free( struct mulle_container_valuecallback  *callback, void *p, struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, p);
}



static void   *mulle_container_callback_int_describe( struct mulle_container_valuecallback  *callback, void *p, struct mulle_allocator *allocator)
{
   char     *s;

   s = mulle_allocator_malloc( allocator, 64);
   sprintf( s, "%d", (int) (uintptr_t) p);
   return( s);
}


static void   *mulle_container_callback_intptr_describe( void *ignore, void *p, struct mulle_allocator *allocator)
{
   char     *s;

   s = mulle_allocator_malloc( allocator, 64);
   sprintf( s, "%lld", (long long) (uintptr_t) p);
   return( s);
}


static void   *mulle_container_callback_pointer_describe( void *ignore, void *p, struct mulle_allocator *allocator)
{
   char     *s;

   s = mulle_allocator_malloc( allocator, 64);
   sprintf( s, "%p", p);
   return( s);
}


static void   *mulle_container_callback_cstring_copy( void *ignore, void *s, struct mulle_allocator *allocator)
{
   size_t   len;
   char     *copy;

   len = strlen( s) + 1;
   copy = mulle_allocator_malloc( allocator, len);
   memcpy( copy,  s, len);
   return( copy);
}



static uintptr_t   mulle_container_keycallback_cstring_hash( struct mulle_container_keycallback *ignore, void *s)
{
   return( mulle_hash( s, strlen( s)));
}


static int   mulle_container_keycallback_cstring_is_equal( struct mulle_container_keycallback *ignore, void *a, void *b)
{
   assert( a && b);
   return( ! strcmp( a, b));
}


MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_int =
{
   mulle_container_keycallback_pointer_hash,
   mulle_container_keycallback_pointer_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_nop,
   (void *(*)()) mulle_container_callback_int_describe,
   mulle_container_not_an_int_key,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_intptr =
{
   mulle_container_keycallback_pointer_hash,
   mulle_container_keycallback_pointer_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_nop,
   (void *(*)()) mulle_container_callback_intptr_describe,
   mulle_container_not_an_intptr_key,
   NULL
};

/***/
MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer_or_null =
{
   mulle_container_keycallback_pointer_hash,
   mulle_container_keycallback_pointer_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_nop,
   (void *(*)()) mulle_container_callback_pointer_describe,
   mulle_container_not_a_pointer_key,
   NULL,
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer =
{
   mulle_container_keycallback_pointer_hash,
   mulle_container_keycallback_pointer_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_nop,
   (void *(*)()) mulle_container_callback_pointer_describe,
   NULL,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_owned_pointer =
{
   mulle_container_keycallback_pointer_hash,
   mulle_container_keycallback_pointer_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_pointer_free,
   (void *(*)()) mulle_container_callback_pointer_describe,
   NULL,
   NULL
};



/**/
MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_copied_cstring =
{
   mulle_container_keycallback_cstring_hash,
   mulle_container_keycallback_cstring_is_equal,
   (void *(*)()) mulle_container_callback_cstring_copy,
   mulle_container_keycallback_pointer_free,
   (void *(*)()) mulle_container_callback_cstring_copy,
   NULL,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_nonowned_cstring =
{
   mulle_container_keycallback_cstring_hash,
   mulle_container_keycallback_cstring_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_nop,
   (void *) mulle_container_callback_cstring_copy,
   NULL,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_keycallback   mulle_container_keycallback_owned_cstring =
{
   mulle_container_keycallback_cstring_hash,
   mulle_container_keycallback_cstring_is_equal,
   mulle_container_keycallback_self,
   mulle_container_keycallback_pointer_free,
   (void *(*)()) mulle_container_callback_cstring_copy,
   NULL,
   NULL
};


/**/
MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_int =
{
   mulle_container_valuecallback_self,
   mulle_container_valuecallback_nop,
   (void *(*)()) mulle_container_callback_int_describe,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_intptr =
{
   mulle_container_valuecallback_self,
   mulle_container_valuecallback_nop,
   (void *(*)()) mulle_container_callback_intptr_describe,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_owned_pointer =
{
   mulle_container_valuecallback_self,
   mulle_container_valuecallback_pointer_free,
   (void *(*)()) mulle_container_callback_pointer_describe,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_pointer =
{
   mulle_container_valuecallback_self,
   mulle_container_valuecallback_nop,
   (void *(*)()) mulle_container_callback_pointer_describe,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_copied_cstring =
{
   (void *(*)()) mulle_container_callback_cstring_copy,
   mulle_container_valuecallback_pointer_free,
   (void *(*)()) mulle_container_callback_cstring_copy,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_cstring =
{
   mulle_container_valuecallback_self,
   mulle_container_valuecallback_nop,
   (void *(*)()) mulle_container_callback_cstring_copy,
   NULL
};


MULLE_CONTAINER_GLOBAL
struct mulle_container_valuecallback   mulle_container_valuecallback_owned_cstring =
{
   mulle_container_valuecallback_self,
   mulle_container_valuecallback_pointer_free,
   (void *(*)())  mulle_container_callback_cstring_copy,
   NULL
};
