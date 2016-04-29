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


#include "mulle_container_callback.h"
#include "hash/mulle_hash.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


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


int   mulle_container_callback_pointer_is_equal( void *ignore, void *a, void *b)
{
   return( a == b);
}


void   mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback, void *p, struct mulle_allocator *allocator)
{
   if( p != callback->not_a_key_marker)
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
   if( ! copy)
   {
      fprintf( stderr, "out of memory: %ld", len);
      abort();
   }
   memcpy( copy,  s, len);
   return( copy);
}



static unsigned long   mulle_container_callback_cstring_hash( struct mulle_container_keycallback *ignore, void *s)
{
   return( mulle_hash( s, strlen( s)));
}


static int   mulle_container_callback_cstring_is_equal( struct mulle_container_keycallback *ignore, void *a, void *b)
{
   assert( a && b);
   return( ! strcmp( a, b));
}



struct mulle_container_keycallback   mulle_container_keycallback_int =
{
   (void *) mulle_hash_pointer,
   (void *) mulle_container_callback_pointer_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_int_describe,
   mulle_container_not_an_int_key,
   NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_intptr =
{
   (void *) mulle_hash_pointer,
   (void *) mulle_container_callback_pointer_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_intptr_describe,
   mulle_container_not_an_intptr_key,
   NULL
};

/***/
struct mulle_container_keycallback   mulle_container_keycallback_owned_pointer_or_null =
{
   (void *) mulle_hash_pointer,
   (void *) mulle_container_callback_pointer_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_keycallback_pointer_free,
   (void *) mulle_container_callback_pointer_describe,
   mulle_container_not_a_pointer_key,
   NULL,
};


struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer =
{
   (void *) mulle_hash_pointer,
   (void *) mulle_container_callback_pointer_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_pointer_describe,
   NULL,
   NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_owned_pointer =
{
   (void *) mulle_hash_pointer,
   (void *) mulle_container_callback_pointer_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_keycallback_pointer_free,
   (void *) mulle_container_callback_pointer_describe,
   NULL,
   NULL
};



/**/
struct mulle_container_keycallback   mulle_container_keycallback_copied_cstring =
{
   (void *) mulle_container_callback_cstring_hash,
   (void *) mulle_container_callback_cstring_is_equal,
   (void *) mulle_container_callback_cstring_copy,
   (void *) mulle_container_keycallback_pointer_free,
   (void *) mulle_container_callback_cstring_copy,
   NULL,
   NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_nonowned_cstring =
{
   (void *) mulle_container_callback_cstring_hash,
   (void *) mulle_container_callback_cstring_is_equal,
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_cstring_copy,
   NULL,
   NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_owned_cstring =
{
   (void *) mulle_container_callback_cstring_hash,
   (void *) mulle_container_callback_cstring_is_equal,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_keycallback_pointer_free,
   (void *) mulle_container_callback_cstring_copy,
   NULL,
   NULL
};


/**/
struct mulle_container_valuecallback   mulle_container_valuecallback_int =
{
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_int_describe,
   NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_intptr =
{
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_intptr_describe,
   NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_owned_pointer =
{
   (void *) mulle_container_callback_self,
   (void *) mulle_container_valuecallback_pointer_free,
   (void *) mulle_container_callback_pointer_describe,
   NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_pointer =
{
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_pointer_describe,
   NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_copied_cstring =
{
   (void *) mulle_container_callback_cstring_copy,
   (void *) mulle_container_valuecallback_pointer_free,
   (void *) mulle_container_callback_cstring_copy,
   NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_cstring =
{
   (void *) mulle_container_callback_self,
   (void *) mulle_container_callback_nop,
   (void *) mulle_container_callback_cstring_copy,
   NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_owned_cstring =
{
   (void *) mulle_container_callback_self,
   (void *) mulle_container_valuecallback_pointer_free,
   (void *) mulle_container_callback_cstring_copy,
   NULL
};
