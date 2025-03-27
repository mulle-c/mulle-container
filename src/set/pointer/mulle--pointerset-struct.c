//
//  Copyright (C) 2011-2020 Nat!, Mulle kybernetiK.
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
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle--pointerset-struct.h"
#include "mulle--pointerset-generic.h"

#include "mulle-container-math.h"
#include "mulle-container-callback.h"
#include "mulle-container-callback-global.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/**
  * Bit twiddling code from
  * https://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
  * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 **/
//
// 0 is also a power of two for these purposes
//

void    _mulle__pointerset_init( struct mulle__pointerset *p,
                                 size_t capacity,
                                 struct mulle_allocator *allocator)
{
   memset( p, 0, sizeof( *p));

   //
   // our set requires mulle_not_a_pointer to find an end
   // so give it ~25% holes. For this to work though, we can not be smaller
   // than 4 items.
   //
   if( capacity)
   {
      p->_size  = capacity >= MULLE__POINTERSET_MIN_SIZE
                        ? mulle_pow2round( capacity + (capacity >> MULLE__POINTERSET_FILL_SHIFT))
                        : MULLE__POINTERSET_MIN_SIZE;
      p->_storage = _mulle__pointerset_allocate_storage_generic( p->_size, mulle_not_a_pointer, allocator);
   }
}


struct mulle__pointerset   *_mulle__pointerset_create( size_t capacity,
                                                       size_t extra,
                                                       struct mulle_allocator *allocator)
{
   struct mulle__pointerset   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointerset) + extra);
   _mulle__pointerset_init( p, capacity, allocator);
   return( p);
}


void   _mulle__pointerset_done( struct mulle__pointerset *set,
                                struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, set->_storage);
#ifdef DEBUG   
   mulle_memset_uint32( set, 0xDEADDEAD,sizeof( struct mulle__pointerset));
#endif   
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   set->_n_mutations++;
#endif
}


void    _mulle__pointerset_destroy( struct mulle__pointerset *set,
                                    struct mulle_allocator *allocator)
{
   _mulle__pointerset_done( set, allocator);
   mulle_allocator_free( allocator, set);
}

#pragma mark - optimized getter

void   *_mulle__pointerset__get( struct mulle__pointerset *set,
                                 void *key)
{
   uintptr_t      hash;
   size_t   i;
   size_t   mask;
   size_t   size;
   void           **storage;
   void           *found;

   hash    = mulle_pointer_hash( key);
   storage = set->_storage;
   size    = set->_size;
   i       = mulle__pointerset_hash_for_size( hash, size);
   mask    = size - 1;
   for(;;)
   {
      found = storage[ i];
      if( found == mulle_not_a_pointer)
         break;
      if( key == found)
         break;
      i = (i + 1) & mask;
   }

   return( found);
}


void   *_mulle__pointerset_get( struct mulle__pointerset *set,
                                void *key)
{
   void   *value;

   // important to not hit a NULL _storage later
   if( set->_count == 0)
      return( NULL);

   value = _mulle__pointerset__get( set, key);
   return( value == mulle_not_a_pointer ? NULL : value);
}


struct mulle__pointerset  *
   _mulle__pointerset_copy( struct mulle__pointerset *set,
                            struct mulle_allocator *allocator)
{
   struct mulle__pointerset   *other;

   other = _mulle__pointerset_create( _mulle__pointerset_get_count( set), 0, allocator);
   _mulle__pointerset_copy_items_generic( other,
                                          set,
                                          &mulle_container_keycallback_nonowned_pointer_or_null,
                                          allocator);
   return( other);
}
