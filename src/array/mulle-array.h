//
//  mulle-array.h
//  mulle-container
//
//  Created by Nat! on 04/11/15.
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

#ifndef mulle_array__h__
#define mulle_array__h__

#include "mulle--array.h"
#include "mulle-container-operation.h"


#define MULLE_ARRAY_BASE                            \
   MULLE__ARRAY_BASE;                               \
   struct mulle_container_keycallback   *_callback; \
   struct mulle_allocator               *_allocator


struct mulle_array
{
   MULLE_ARRAY_BASE;
};

#define MULLE_ARRAYENUMERATOR_BASE   MULLE__ARRAYENUMERATOR_BASE


struct mulle_arrayenumerator
{
   MULLE_ARRAYENUMERATOR_BASE;
};


extern struct mulle_arrayenumerator   mulle_arrayenumerator_empty;


struct mulle_array    *mulle_array_create( struct mulle_container_keycallback *callback,
                                           struct mulle_allocator *allocator);


static inline void   mulle_array_destroy( struct mulle_array *array)
{
   if( ! array)
      return;
   _mulle__array_destroy( (struct mulle__array *) array, array->_callback, array->_allocator);
}


static inline void   mulle_array_done( struct mulle_array *array)
{
   if( ! array)
      return;
   _mulle__array_done( (struct mulle__array *) array, array->_callback, array->_allocator);
}


static inline int    mulle_array_grow( struct mulle_array *array)
{
   if( ! array)
      return( -1);
   return( _mulle__array_grow( (struct mulle__array *) array, array->_callback, array->_allocator));
}


static inline int    mulle_array_size_to_fit( struct mulle_array *array)
{
   if( ! array)
      return( -1);
   return( _mulle__array_size_to_fit( (struct mulle__array *) array, array->_callback, array->_allocator));
}


// TODO: update this to use mulle-range
static inline void   mulle_array_remove_in_range( struct mulle_array *array,
                                                  unsigned int location,
                                                  unsigned int length)
{
   if( ! array)
      return;

   _mulle__array_remove_in_range( (struct mulle__array *) array,
                                 location,
                                 length,
                                 array->_callback,
                                 array->_allocator);
}


// TODO: update this to use mulle-range
static inline void   mulle_array_zero_in_range( struct mulle_array *array,
                                                unsigned int location,
                                                unsigned int length)
{
   if( ! array)
      return;

   _mulle__array_zero_in_range( (struct mulle__array *) array,
                               location,
                               length,
                               array->_callback);
}


static inline void   mulle_array_compact_zeroes( struct mulle_array *array)
{
   if( ! array)
      return;
   _mulle__array_compact_zeroes( (struct mulle__array *) array, array->_callback);
}


static inline void   mulle_array_remove_all( struct mulle_array *array)
{
   if( ! array)
      return;
   _mulle__array_remove_all( (struct mulle__array *) array, array->_callback, array->_allocator);
}


static inline void    mulle_array_init( struct mulle_array *array,
                                        unsigned int length,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   if( ! array)
      return;
   if( ! allocator)
      allocator = &mulle_default_allocator;

   _mulle__array_init( (struct mulle__array *) array, length);

   array->_callback  = callback;
   array->_allocator = allocator;
}


static inline void   **mulle_array_get_all( struct mulle_array *array)
{
   return( array ? _mulle__array_get_all( (struct mulle__array *) array) : NULL);
}


static inline unsigned int   mulle_array_get_count( struct mulle_array *array)
{
   return( array ? _mulle__array_get_count( (struct mulle__array *) array) : 0);
}


static inline void   **mulle_array_advance( struct mulle_array *array,
                                            unsigned int length)
{
   return( array ? _mulle__array_advance( (struct mulle__array *) array,
                                          length,
                                          array->_callback,
                                          array->_allocator) : NULL);
}


static inline int   mulle_array_is_full( struct mulle_array *array)
{
   return( array ? _mulle__array_is_full( (struct mulle__array *) array) : 1);
}


// will use callbacks of array to determine equality
static inline int   mulle_array_is_equal( struct mulle_array *array, struct mulle_array *other)
{
   if( ! array || ! other)
      return( array == other);

   assert( array->_callback->is_equal == other->_callback->is_equal);
   return( _mulle__array_is_equal( (struct mulle__array *) array,
                                   (struct mulle__array *) other, array->_callback));
}



static inline unsigned int   mulle_array_guaranteedsize( struct mulle_array *array)
{
   if( ! array)
      return( 0);
   return( _mulle__array_guaranteedsize( (struct mulle__array *) array));
}


static inline void   **mulle_array_guarantee( struct mulle_array *array, unsigned int length)
{
   if( ! array)
      return( NULL);
   return( _mulle__array_guarantee( (struct mulle__array *) array, length, array->_callback, array->_allocator));
}


static inline void    mulle_array_add( struct mulle_array *array,
                                       void  *p)
{
   if( ! array)
      return;

   _mulle__array_add( (struct mulle__array *) array, p, array->_callback, array->_allocator);
}


static inline void   *mulle_array_get( struct mulle_array *array, unsigned int index)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_get( (struct mulle__array *) array, index));
}


static inline void   *mulle_array_get_last( struct mulle_array *array)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_get_last( (struct mulle__array *) array));
}


static inline void   mulle_array_remove_last( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle__array_remove_last( (struct mulle__array *) array, array->_callback, array->_allocator);
}


static inline void   mulle_array_add_multiple( struct mulle_array *array,
                                               void **pointers,
                                               unsigned int length)
{
   if( ! array || ! pointers)
      return;

   _mulle__array_add_multiple( (struct mulle__array *) array, pointers, length, array->_callback, array->_allocator);
}


static inline void    mulle_array_add_array( struct mulle_array *array,
                                             struct mulle_array *other)
{
   if( ! array || ! other)
      return;
   _mulle__array_add_array( (struct mulle__array *) array, (struct mulle__array *) other, array->_callback, array->_allocator);
}


static inline void   mulle_array_reset( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle__array_reset( (struct mulle__array *) array, array->_callback, array->_allocator);
}



#pragma mark -
#pragma mark grab contents destrutively
//
// you now own the allocated block now
//
static inline void   **mulle_array_extract_all( struct mulle_array *array)
{

   return( array ? _mulle__array_extract_all( (struct mulle__array *) array) : NULL);
}


#pragma mark -
#pragma mark search

static inline unsigned long  mulle_array_find_in_range_identical( struct mulle_array *array,
                                                           void *obj,
                                                           unsigned int location,
                                                           unsigned int length)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__array_find_in_range_identical( (struct mulle__array *) array,
                                                   obj,
                                                   location,
                                                   length));
}


static inline unsigned long  mulle_array_find_in_range( struct mulle_array *array,
                                                 void *obj,
                                                 unsigned int location,
                                                 unsigned int length)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__array_find_in_range( (struct mulle__array *) array,
                                        obj,
                                        location,
                                        length,
                                        array->_callback));
}



#pragma mark -
#pragma mark enumeration

static inline struct mulle_arrayenumerator   mulle_array_enumerate( struct mulle_array *array)
{
   struct mulle__arrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( *(struct mulle_arrayenumerator *) &rover);  // should be harmless
   }

   rover = _mulle__array_enumerate( (struct mulle__array *) array, array->_callback);
   return( *(struct mulle_arrayenumerator *) &rover);  // should be harmless
}


static inline void   *mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover)
{
   if( ! rover)
      return( NULL);
   return( _mulle__arrayenumerator_next( (struct mulle__arrayenumerator *) rover));
}


static inline void   mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
{
   if( rover)
      _mulle__arrayenumerator_done( (struct mulle__arrayenumerator *) rover);
}

#pragma mark -
#pragma mark integer convenience

static inline void    mulle_array_add_intptr( struct mulle_array *array,
                                              intptr_t v)
{
   if( ! array)
      return;
   _mulle__array_add( (struct mulle__array *) array, (void *) v, array->_callback, array->_allocator);
}


static inline void    mulle_array_add_int( struct mulle_array *array,
                                           int v)
{
   if( ! array)
      return;
   mulle_array_add_intptr( array, v);
}


static inline intptr_t    mulle_array_get_intptr( struct mulle_array *array, unsigned int index)
{
   if( ! array)
      return( 0);
   return( (intptr_t) mulle_array_get( array, index));
}


static inline int    mulle_array_get_int( struct mulle_array *array, unsigned int index)
{
   if( ! array)
      return( -1);
   return( (int) mulle_array_get_intptr( array, index));
}


static inline intptr_t   mulle_arrayenumerator_next_intptr( struct mulle_arrayenumerator *rover)
{
   if( ! rover)
      return( 0);
   return( (intptr_t) mulle_arrayenumerator_next( rover));
}


static inline int   mulle_arrayenumerator_next_int( struct mulle_arrayenumerator *rover)
{
   if( ! rover)
      return( 0);
   return( (int) mulle_arrayenumerator_next_intptr( rover));
}



static inline unsigned long  mulle_array_find_in_range_identical_intptr( struct mulle_array *array,
                                                                         intptr_t v,
                                                                         unsigned int location,
                                                                         unsigned int length)
{
   // ints are always identical
   if( ! array)
      return( mulle_not_found_e);

   return(  mulle_array_find_in_range_identical( array, (void *) v, location, length));
}


static inline unsigned long   mulle_array_find_in_range_identical_int( struct mulle_array *array,
                                                                       int v,
                                                                       unsigned int location,
                                                                       unsigned int length)
{
   if( ! array)
      return( mulle_not_found_e);

   return( mulle_array_find_in_range_identical_intptr( array, v, location, length));
}


#endif /* mulle_array_h */


