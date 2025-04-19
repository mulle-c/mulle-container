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
                                            size_t new_size,
                                            struct mulle_allocator *allocator)
{
   size_t    used;

   used     = _mulle__pointerarray_get_count( array);
   new_size = mulle_pow2round( new_size);
   if( new_size < 4)
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

// memo its not directly a mutation, but an enumerator will get garbage now
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif   
}


void   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                  struct mulle_allocator *allocator)
{
   _mulle__pointerarray_realloc( array,
                                 _mulle__pointerarray_get_size( array) * 2,
                                 allocator);
}



void **   _mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                          size_t length,
                                          struct mulle_allocator *allocator)
{
   size_t   available;
   size_t   _size;
   size_t   _used;

   _size     = _mulle__pointerarray_get_size( array);
   _used     = _mulle__pointerarray_get_count( array);
   available = _size - _used;
   if( available < length)
      _mulle__pointerarray_realloc( array,
                                    _size + (length - available),
                                    allocator);

   // we only guarantee, so someone can write into the array, so assume this
   // is a mutation
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
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

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
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


int    _mulle__pointerarray_is_equal( struct mulle__pointerarray *array,
                                      struct mulle__pointerarray *other)
{
   size_t   n;

   n = _mulle__pointerarray_get_count( array);
   if( n != _mulle__pointerarray_get_count( other))
      return( 0);

   return( ! memcmp( array->_storage, other->_storage, n * sizeof( void *)));
}


void
   _mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                         struct mulle_range range)
{
   size_t   count;
   size_t   tail;

   count  = _mulle__pointerarray_get_count( array);
   tail   = range.location + range.length;
   range  = mulle_range_validate_against_length( range, count);
   memmove( &array->_storage[ range.location],
            &array->_storage[ tail],
            (count - tail) * sizeof( void *));

   array->_curr -= range.length;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}



size_t
   _mulle__pointerarray_get_in_range( struct mulle__pointerarray *array,
                                      struct mulle_range range,
                                      void *buf)
{
   range = mulle_range_validate_against_length( range,
                                                _mulle__pointerarray_get_count( array));
   memcpy( buf, &array->_storage[ range.location], range.length * sizeof( void *));
   return( range.length);
}


struct mulle_pointers
   _mulle__pointerarray_extract_pointers( struct mulle__pointerarray *array,
                                          struct mulle_allocator *allocator)
{
   struct mulle_pointers   data;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif

   data = mulle_pointers_make( array->_storage,
                               _mulle__pointerarray_get_count( array));

   if( data.pointers && data.pointers == array->_initial_storage)
   {
      data.pointers = mulle_allocator_malloc( allocator, data.count * sizeof( void *));
      memcpy( data.pointers, array->_storage, data.count * sizeof( void *));

      array->_curr    =
      array->_storage = array->_initial_storage;
      return( data);
   }

   array->_storage          =
   array->_curr             =
   array->_sentinel         =
   array->_initial_storage  = NULL;

   return( data);
}


void   _mulle__pointerarray_add_pointerarray( struct mulle__pointerarray *array,
                                              struct mulle__pointerarray *other,
                                              struct mulle_range range,
                                              struct mulle_allocator *allocator)
{
   size_t   count;
   void           **q;
   void           **dst;

   assert( array);

   count = mulle__pointerarray_get_count( other);
   range = mulle_range_validate_against_length( range, count);
   if( ! range.length)
      return;

   dst =  _mulle__pointerarray_advance( array, range.length, allocator);
   q   = &other->_storage[ range.location];
   // even if you add self to self, the memory areas dont overlap
   memcpy( dst, q, range.length * sizeof( void *));

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}



void  _mulle__pointerarray_absorb_array( struct mulle__pointerarray *array,
                                         struct mulle_allocator *allocator,
                                         struct mulle__pointerarray *victim,
                                         struct mulle_allocator *victim_allocator)
{
   void     **reserved;
   size_t   n;

   assert( array != victim);

   if( ! allocator)
      allocator = &mulle_default_allocator;
   if( ! victim_allocator)
      victim_allocator = &mulle_default_allocator;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif

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

