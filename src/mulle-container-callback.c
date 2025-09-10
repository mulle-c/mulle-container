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


#include "mulle-container-callback.h"

#include "include-private.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


uintptr_t
   mulle_container_keycallback_pointer_hash( struct mulle_container_keycallback *callback,
                                             void *p)
{
   MULLE_C_UNUSED( callback);

   return( mulle_pointer_hash( p));
}


// just here so that we can distinguish between this and pointer types
int
   mulle_container_keycallback_intptr_is_equal( struct mulle_container_keycallback *callback,
                                                void *a,
                                                void *b)
{
   MULLE_C_UNUSED( callback);

   return( (intptr_t) a == (intptr_t) b);
}



int
   mulle_container_keycallback_pointer_is_equal( struct mulle_container_keycallback *callback,
                                                 void *a,
                                                 void *b)
{
   MULLE_C_UNUSED( callback);

   return( a == b);
}


void   mulle_container_valuecallback_nop( struct mulle_container_valuecallback *callback,
                                          void *p,
                                          struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( callback);
   MULLE_C_UNUSED( p);
   MULLE_C_UNUSED( allocator);
}


void   *mulle_container_valuecallback_self( struct mulle_container_valuecallback *callback,
                                            void *p,
                                            struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( callback);
   MULLE_C_UNUSED( allocator);

   return( p);
}


char   *mulle_container_valuecallback_no_description( struct mulle_container_valuecallback *callback,
                                                      void *p,
                                                      struct mulle_allocator **p_allocator)
{
   MULLE_C_UNUSED( callback);
   MULLE_C_UNUSED( p);

   assert( p_allocator);

   *p_allocator = &mulle_allocator_stdlib_nofree;
   return( "???");
}


void   _mulle_container_keycallback_pointer_free( struct mulle_container_keycallback *callback,
                                                  void *p,
                                                  struct mulle_allocator *allocator)
{
   if( p != callback->notakey)
      mulle_allocator_free( allocator, p);
}


void
   mulle_container_valuecallback_pointer_free( struct mulle_container_valuecallback  *callback,
                                               void *p,
                                               struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( callback);

   mulle_allocator_free( allocator, p);
}



char   *
   mulle_container_callback_int_describe( struct mulle_container_valuecallback *callback,
                                          void *p,
                                          struct mulle_allocator **p_allocator)
{
   char   buf[ 64];

   MULLE_C_UNUSED( callback);

   assert( p_allocator);

   snprintf( buf, sizeof( buf), "%d", (int) (intptr_t) p);
   return( mulle_allocator_strdup( *p_allocator, buf));
}


char   *
   mulle_container_callback_intptr_describe( struct mulle_container_valuecallback *callback,
                                             void *p,
                                             struct mulle_allocator **p_allocator)
{
   char   buf[ 64];

   MULLE_C_UNUSED( callback);

   assert( p_allocator);

   snprintf( buf, sizeof( buf), "%lld", (long long) (intptr_t) p);
   return( mulle_allocator_strdup( *p_allocator, buf));
}


char *
   mulle_container_callback_pointer_describe( struct mulle_container_valuecallback  *callback,
                                              void *p,
                                              struct mulle_allocator **p_allocator)
{
   char   buf[ 64];

   MULLE_C_UNUSED( callback);

   assert( p_allocator);

   snprintf( buf, sizeof( buf), "%p", p);
   return( mulle_allocator_strdup( *p_allocator, buf));
}


void *
   mulle_container_callback_cstring_copy( struct mulle_container_valuecallback *callback,
                                          void *s,
                                          struct mulle_allocator *allocator)
{
   size_t   len;
   char     *copy;

   MULLE_C_UNUSED( callback);

   len  = strlen( s) + 1;
   copy = mulle_allocator_malloc( allocator, len);
   memcpy( copy,  s, len);
   return( copy);
}


char
   *mulle_container_callback_cstring_describe( struct mulle_container_valuecallback *callback,
                                               void *p,
                                               struct mulle_allocator **p_allocator)
{
   assert( p_allocator);

   MULLE_C_UNUSED( callback);

   *p_allocator = NULL;
   return( p);
}


uintptr_t
   mulle_container_keycallback_cstring_hash( struct mulle_container_keycallback *callback,
                                             void *s)
{
   MULLE_C_UNUSED( callback);

   return( _mulle_fnv1a_inline( s, strlen( s)));
}


int
   mulle_container_keycallback_cstring_is_equal( struct mulle_container_keycallback *callback,
                                                 void *a,
                                                 void *b)
{
   MULLE_C_UNUSED( callback);

   assert( a && b);
   return( ! strcmp( a, b));
}


struct mulle_container_keycallback   mulle_container_keycallback_int =
{
   .hash     = mulle_container_keycallback_pointer_hash,
   .is_equal = mulle_container_keycallback_intptr_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = mulle_container_keycallback_nop,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_int_describe,
   .notakey  = mulle_not_an_int,
   .userinfo = NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_intptr =
{
   .hash     = mulle_container_keycallback_pointer_hash,
   .is_equal = mulle_container_keycallback_intptr_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = mulle_container_keycallback_nop,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_intptr_describe,
   .notakey  = mulle_not_an_intptr,
   .userinfo = NULL
};

/***/
struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer_or_null =
{
   .hash     = mulle_container_keycallback_pointer_hash,
   .is_equal = mulle_container_keycallback_pointer_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = mulle_container_keycallback_nop,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_pointer_describe,
   .notakey  = mulle_not_a_pointer,
   .userinfo = NULL,
};


struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer =
{
   .hash     = mulle_container_keycallback_pointer_hash,
   .is_equal = mulle_container_keycallback_pointer_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = mulle_container_keycallback_nop,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_pointer_describe,
   .notakey  = NULL,
   .userinfo = NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_owned_pointer =
{
   .hash     = mulle_container_keycallback_pointer_hash,
   .is_equal = mulle_container_keycallback_pointer_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = _mulle_container_keycallback_pointer_free,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_pointer_describe,
   .notakey  = NULL,
   .userinfo = NULL
};



/**/
struct mulle_container_keycallback   mulle_container_keycallback_copied_cstring =
{
   .hash     = mulle_container_keycallback_cstring_hash,
   .is_equal = mulle_container_keycallback_cstring_is_equal,
   .retain   = (mulle_container_keycallback_retain_t *) mulle_container_callback_cstring_copy,
   .release  = _mulle_container_keycallback_pointer_free,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_cstring_describe,
   .notakey  = NULL,
   .userinfo = NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_nonowned_cstring =
{
   .hash     = mulle_container_keycallback_cstring_hash,
   .is_equal = mulle_container_keycallback_cstring_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = mulle_container_keycallback_nop,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_cstring_describe,
   .notakey  = NULL,
   .userinfo = NULL
};


struct mulle_container_keycallback   mulle_container_keycallback_owned_cstring =
{
   .hash     = mulle_container_keycallback_cstring_hash,
   .is_equal = mulle_container_keycallback_cstring_is_equal,
   .retain   = mulle_container_keycallback_self,
   .release  = _mulle_container_keycallback_pointer_free,
   .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_cstring_describe,
   .notakey  = NULL,
   .userinfo = NULL
};


/**/
struct mulle_container_valuecallback   mulle_container_valuecallback_int =
{
   .retain   = mulle_container_valuecallback_self,
   .release  = mulle_container_valuecallback_nop,
   .describe = mulle_container_callback_int_describe,
   .userinfo = NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_intptr =
{
   .retain   = mulle_container_valuecallback_self,
   .release  = mulle_container_valuecallback_nop,
   .describe = mulle_container_callback_intptr_describe,
   .userinfo = NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_owned_pointer =
{
   .retain   = mulle_container_valuecallback_self,
   .release  = mulle_container_valuecallback_pointer_free,
   .describe = mulle_container_callback_pointer_describe,
   .userinfo = NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_pointer =
{
   .retain   = mulle_container_valuecallback_self,
   .release  = mulle_container_valuecallback_nop,
   .describe = mulle_container_callback_pointer_describe,
   .userinfo = NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_copied_cstring =
{
   .retain   = mulle_container_callback_cstring_copy,
   .release  = mulle_container_valuecallback_pointer_free,
   .describe = mulle_container_callback_cstring_describe,
   .userinfo = NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_cstring =
{
   .retain   = mulle_container_valuecallback_self,
   .release  = mulle_container_valuecallback_nop,
   .describe = mulle_container_callback_cstring_describe,
   .userinfo = NULL
};


struct mulle_container_valuecallback   mulle_container_valuecallback_owned_cstring =
{
   .retain   = mulle_container_valuecallback_self,
   .release  = mulle_container_valuecallback_pointer_free,
   .describe = mulle_container_callback_cstring_describe,
   .userinfo = NULL
};
