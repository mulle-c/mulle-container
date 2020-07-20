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


struct mulle__pointerarrayenumerator          mulle__pointerarrayenumerator_empty;
struct mulle__pointerarrayreverseenumerator   mulle__pointerarrayreverseenumerator_empty;


# pragma mark -
# pragma mark mechanisms

// intentionally not static inline
static void   _mulle__pointerarray_realloc( struct mulle__pointerarray *array,
                                            size_t new_size,
                                            struct mulle_allocator *allocator)
{
   size_t    used;

   used = _mulle__pointerarray_get_count( array);
   new_size = mulle_pow2round( new_size);
   if( new_size == 0)
      new_size = 4;

   array->storage = mulle_allocator_realloc( allocator,
                                             array->storage,
                                             sizeof( void *) * new_size);
   array->curr     = &array->storage[ used];
   array->sentinel = &array->storage[ new_size];
}


void   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                  struct mulle_allocator *allocator)
{
   _mulle__pointerarray_realloc( array, _mulle__pointerarray_get_size( array) * 2, allocator);
}



void   _mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                       size_t length,
                                       struct mulle_allocator *allocator)
{
   size_t   available;
   size_t   size;
   size_t   used;

   size      = _mulle__pointerarray_get_size( array);
   used      = _mulle__pointerarray_get_count( array);
   available = size - used;
   if( available < length)
      _mulle__pointerarray_realloc( array, size + (length - available), allocator);
}


void   _mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                     void *notakey)
{
   void   **p;
   void   **q;
   void   **sentinel;

   p        = array->storage;
   q        = p;
   sentinel = array->curr;

   for( ; p < sentinel; p++)
   {
      if( *p != notakey)
         continue;
      *q++ = *p;
   }

   array->curr = q;
}


uintptr_t
   _mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                       struct mulle_range range,
                                       void *p)
{
   void   **curr;
   void   **sentinel;

   curr     = array->storage[ range.location];
   sentinel = &curr[ range.length];
   while( curr < sentinel)
   {
      if( *curr == p)
         return( curr - array->storage);
      curr++;
   }
   return( mulle_not_found_e);
}


void
   _mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                         struct mulle_range range)
{
   range = mulle_range_validate_against_length( range,
                                                _mulle__pointerarray_get_count( array));
   memmove( &array->storage[ range.location],
            &array->storage[ range.location + range.length],
            range.length * sizeof( void *));

   array->curr -= range.length;
}

