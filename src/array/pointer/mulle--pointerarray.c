//
//  mulle_pointerarray.c
//  mulle-container
//
//  Created by Nat! on 03.11.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
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

#include "mulle--pointerarray.h"

#include "mulle-container-math.h"


# pragma mark - memory allocation

static void   _mulle__pointerarray_realloc( struct mulle__pointerarray *array,
                                            unsigned int new_size,
                                            struct mulle_allocator *allocator)
{
   unsigned int    used;

   used     = _mulle__pointerarray_get_count( array);
   new_size = mulle_pow2round( new_size);
   if( new_size == 0)
      new_size = 4;

   if( array->_storage == array->_initial_storage)
   {
      array->_storage = mulle_allocator_malloc( allocator,
                                                sizeof( void *) * new_size);
      memcpy( array->_storage, array->_initial_storage, sizeof( void *) * used);
   }
   else
      array->_storage = mulle_allocator_realloc( allocator,
                                                 array->_storage,
                                                 sizeof( void *) * new_size);
   array->_curr     = &array->_storage[ used];
   array->_sentinel = &array->_storage[ new_size];
}


void   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                  struct mulle_allocator *allocator)
{
   _mulle__pointerarray_realloc( array,
                                 _mulle__pointerarray_get_size( array) * 2,
                                 allocator);
}



void **   _mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                          unsigned int length,
                                          struct mulle_allocator *allocator)
{
   unsigned int   available;
   unsigned int   _size;
   unsigned int   _used;

   _size      = _mulle__pointerarray_get_size( array);
   _used      = _mulle__pointerarray_get_count( array);
   available = _size - _used;
   if( available < length)
      _mulle__pointerarray_realloc( array,
                                    _size + (length - available),
                                    allocator);
   return( array->_curr);
}


void   _mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                     void *notakey)
{
   void   **p;
   void   **q;
   void   **sentinel;

   p        = array->_storage;
   q        = p;
   sentinel = array->_curr;

   for( ; p < sentinel; p++)
   {
      if( *p != notakey)
         *q++ = *p;
   }

   array->_curr = q;
}


uintptr_t
   _mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                       void *q,
                                       struct mulle_range range)
{
   void   **p;
   void   **sentinel;

   range    = mulle_range_validate_against_length( range,
                                                   _mulle__pointerarray_get_count( array));
   p        = &array->_storage[ range.location];
   sentinel = &p[ range.length];

   assert( p >= array->_storage);
   assert( sentinel <= array->_sentinel);

   while( p < sentinel)
   {
      if( *p == q)
         return( p - array->_storage);
      p++;
   }
   return( mulle_not_found_e);
}


void
   _mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                         struct mulle_range range)
{
   unsigned int   count;
   unsigned int   tail;

   count  = _mulle__pointerarray_get_count( array);
   tail   = range.location + range.length;
   range  = mulle_range_validate_against_length( range, count);
   memmove( &array->_storage[ range.location],
            &array->_storage[ tail],
            (count - tail) * sizeof( void *));

   array->_curr -= range.length;
}



unsigned int
   _mulle__pointerarray_get_in_range( struct mulle__pointerarray *array,
                                      struct mulle_range range,
                                      void *buf)
{
   range = mulle_range_validate_against_length( range,
                                                _mulle__pointerarray_get_count( array));
   memcpy( buf, &array->_storage[ range.location], range.length * sizeof( void *));
   return( range.length);
}


MULLE_C_NONNULL_FIRST
struct mulle_pointers
   _mulle__pointerarray_extract_pointers( struct mulle__pointerarray *buffer,
                                          struct mulle_allocator *allocator)
{
   struct mulle_pointers   data;

   data.pointers = buffer->_storage;
   data.count    = _mulle__pointerarray_get_count( buffer);

   if( data.pointers && data.pointers == buffer->_initial_storage)
   {
      data.pointers = _mulle_allocator_malloc( allocator, data.count * sizeof( void *));
      memcpy( data.pointers, buffer->_storage, data.count * sizeof( void *));

      buffer->_curr    =
      buffer->_storage = buffer->_initial_storage;

      return( data);
   }

   buffer->_storage          =
   buffer->_curr             =
   buffer->_sentinel         =
   buffer->_initial_storage  = NULL;

   return( data);
}



void  _mulle__pointerarray_absorb( struct mulle__pointerarray *array,
                                   struct mulle_allocator *allocator,
                                   struct mulle__pointerarray *victim,
                                   struct mulle_allocator *victim_allocator)
{
   void           **reserved;
   unsigned int   n;

   if( ! allocator)
      allocator = &mulle_default_allocator;
   if( ! victim_allocator)
      victim_allocator = &mulle_default_allocator;

   if( ! array->_storage && allocator == victim_allocator)
   {
      array->_initial_storage = NULL;
      array->_storage         = victim->_storage;
      array->_sentinel        = victim->_sentinel;
      array->_curr            = victim->_curr;

      // wipe victim, but keep victim initial storage
      victim->_storage  = NULL;
      victim->_sentinel = NULL;
      victim->_curr     = NULL;
      return;
   }

   n        = _mulle__pointerarray_get_count( victim);
   reserved = _mulle__pointerarray_advance( array, n, allocator);
   memcpy( reserved, victim->_storage, sizeof( void *) * n);

   // clean out victim
   _mulle__pointerarray_reset( victim);
}

