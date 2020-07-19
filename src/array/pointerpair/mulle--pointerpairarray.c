//
//  mulle_pointerpairarray.c
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
#include "mulle--pointerpairarray.h"

#include "mulle-container-math.h"
#include "mulle-container-operation.h"


# pragma mark - allocation


// intentionally not static inline
static void   _mulle__pointerpairarray_realloc( struct mulle__pointerpairarray *array,
                                                size_t new_size,
                                                struct mulle_allocator *allocator)
{
   new_size = mulle_pow2round( new_size);
   if( new_size == 0)
      new_size = 4;

   array->size  = new_size;
   array->storage = mulle_allocator_realloc( allocator,
                                            array->storage,
                                            sizeof( struct mulle_pointerpair) * new_size);
}


void   _mulle__pointerpairarray_grow( struct mulle__pointerpairarray *array,
                                      struct mulle_allocator *allocator)
{
   _mulle__pointerpairarray_realloc( array, array->size * 2, allocator);
}



void   _mulle__pointerpairarray_guarantee( struct mulle__pointerpairarray *array,
                                           size_t length,
                                           struct mulle_allocator *allocator)
{
   size_t   available;

   available = array->size - array->used;
   if( available < length)
      _mulle__pointerpairarray_realloc( array, array->size + (length - available), allocator);
}

# pragma mark - compaction


void   _mulle__pointerpairarray_compact( struct mulle__pointerpairarray *array,
                                         void *notakey)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *q;
   struct mulle_pointerpair   *sentinel;

   p        = array->storage;
   q        = p;
   sentinel = &p[ array->used];

   for( ;p < sentinel; p++)
   {
      if( p->_key != notakey)
         continue;
      *q++ = *p;
   }
   array->used = q - array->storage;
}


# pragma mark - searching


// need to find "compacted" index here

uintptr_t
   _mulle__pointerpairarray_find( struct mulle__pointerpairarray *array,
                                  void *key,
                                  void *notakey)
{
   struct mulle_pointerpair   *curr;
   struct mulle_pointerpair   *sentinel;
   size_t               n_empty;

   assert( key != notakey);

   n_empty  = 0;
   curr     = array->storage;
   sentinel = &curr[ array->used];
   while( curr < sentinel)
   {
      if( curr->_key == key)
         return( (curr - array->storage) - n_empty);

      if( curr->_key == notakey)
      {
         ++n_empty;
         continue;
      }
      curr++;
   }
   return( mulle_not_found_e);
}
