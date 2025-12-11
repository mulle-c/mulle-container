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
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle--set.h"

#include "mulle-container-math.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


void    _mulle__set_init( struct mulle__set *p,
                          size_t capacity,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   memset( p, 0, sizeof( *p));

   //
   // our set requires mulle_not_a_pointer to find an end
   // so give it ~25% holes. For this to work though, we can not be smaller
   // than 4 items.
   //
   p->_size  = capacity >= MULLE__POINTERSET_MIN_SIZE
                     ? mulle_pow2round( capacity + (capacity >> MULLE__POINTERSET_FILL_SHIFT))
                     : MULLE__POINTERSET_MIN_SIZE;
   p->_storage = _mulle__pointerset_allocate_storage_generic( p->_size, callback->notakey, allocator);
}



struct mulle__set   *_mulle__set_create( size_t capacity,
                                         size_t extra,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator)
{
   struct mulle__set   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__set) + extra);
   _mulle__set_init( p, capacity, callback, allocator);
   return( p);
}


void   _mulle__set_release_all( struct mulle__set *set,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator)
{
   struct mulle__setenumerator  rover;
   void                         *item;

   if( callback && _mulle_container_keycallback_releases( callback))
   {
      rover = _mulle__set_enumerate( set, callback);
      while( _mulle__setenumerator_next( &rover, &item))
         (*callback->release)( callback, item, allocator);
      _mulle__setenumerator_done( &rover);
   }
}


void   _mulle__set_done( struct mulle__set *set,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
{
   _mulle__set_release_all( set, callback, allocator);
   _mulle__pointerset_done( (struct mulle__pointerset *) set, allocator);
}


void   _mulle__set_reset( struct mulle__set *set,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   _mulle__set_release_all( set, callback, allocator);
   _mulle__pointerset_reset_generic( (struct mulle__pointerset *) set,
                                     callback ? callback->notakey : NULL);
}


void    _mulle__set_destroy( struct mulle__set *set,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator)
{
   _mulle__set_done( set, callback, allocator);
   mulle_allocator_free( allocator, set);
}


struct mulle__set   *_mulle__set_copy( struct mulle__set *set,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct mulle__set   *other;

   other = _mulle__set_create( _mulle__set_get_count( set), 0, callback, allocator);
   _mulle__set_copy_items( other, set, callback, allocator);
   return( other);
}

# pragma mark - debugging

// use this only for debugging
char   *_mulle__set_describe( struct mulle__set *set,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator)
{
   char                          *result;
   char                          *s;
   int                           separate;
   size_t                        len;
   size_t                        s_len;
   struct mulle__setenumerator   rover;
   struct mulle_allocator        *value_allocator;
   void                          *item;

   result = NULL;
   len    = 0;

   rover = _mulle__set_enumerate( set, callback);
   while( _mulle__setenumerator_next( &rover, &item))
   {
      value_allocator = allocator ? allocator : &mulle_stdlib_allocator;

      s      = (*callback->describe)( callback, item, &value_allocator);
      s_len  = strlen( s);

      separate = result != NULL;
      result   = mulle_allocator_realloc( allocator, result, len + (separate * 2) + s_len + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len += 2;
      }

      memcpy( &result[ len], s, s_len);
      len += s_len;

      // else it static
      if( value_allocator)
         _mulle_allocator_free( value_allocator, s);
   }
   mulle__setenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


