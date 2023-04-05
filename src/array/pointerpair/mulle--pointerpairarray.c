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
                                                unsigned int new_size,
                                                struct mulle_allocator *allocator)
{
   unsigned int   used;

   new_size = mulle_pow2round( new_size);
   if( new_size == 0)
      new_size = 4;

   used            = _mulle__pointerpairarray_get_count( array);
   array->_storage = mulle_allocator_realloc( allocator,
                                              array->_storage,
                                              sizeof( struct mulle_pointerpair) * new_size);
   array->_curr     = &array->_storage[ used];
   array->_sentinel = &array->_storage[ new_size];
}


void   _mulle__pointerpairarray_grow( struct mulle__pointerpairarray *array,
                                      struct mulle_allocator *allocator)
{
   unsigned int   size;

   size = _mulle__pointerpairarray_get_size( array);
   _mulle__pointerpairarray_realloc( array, size * 2, allocator);
}


struct mulle_pointerpair  *
   _mulle__pointerpairarray_guarantee( struct mulle__pointerpairarray *array,
                                       unsigned int length,
                                       struct mulle_allocator *allocator)
{
   unsigned int   available;
   unsigned int   size;

   available = _mulle__pointerpairarray_get_guaranteed_size( array);
   if( available < length)
   {
      size = _mulle__pointerpairarray_get_size( array);
      _mulle__pointerpairarray_realloc( array, size + (length - available), allocator);
   }
   return( array->_curr);
}



# pragma mark - searching


// need to find "compacted" index here

uintptr_t
   _mulle__pointerpairarray_find( struct mulle__pointerpairarray *array,
                                  void *key)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   assert( key != mulle_pointerpair_notakey);

   p        = array->_storage;
   sentinel = array->_curr;
   while( p < sentinel)
   {
      if( p->key == key)
         return( p - array->_storage);

      p++;
   }
   return( mulle_not_found_e);
}



void
   _mulle__pointerpairarray_remove_in_range( struct mulle__pointerpairarray *array,
                                             struct mulle_range range)
{
   range = mulle_range_validate_against_length( range,
                                                _mulle__pointerpairarray_get_count( array));
   memmove( &array->_storage[ range.location],
            &array->_storage[ range.location + range.length],
            range.length * sizeof( struct mulle_pointerpair));

   array->_curr -= range.length;
}



unsigned int
   _mulle__pointerpairarray_get_in_range( struct mulle__pointerpairarray *array,
                                          struct mulle_range range,
                                          void *buf)
{
   range = mulle_range_validate_against_length( range,
                                                mulle__pointerpairarray_get_count( array));
   memcpy( buf, &array->_storage[ range.location], range.length * sizeof( struct mulle_pointerpair));
   return( range.length);
}
