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

#include "_mulle-array.h"


#define MULLE_ARRAY_BASE                            \
   _MULLE_ARRAY_BASE;                               \
   struct mulle_container_keycallback   *_callback; \
   struct mulle_allocator               *_allocator


struct mulle_array
{
   MULLE_ARRAY_BASE;
};

#define MULLE_ARRAYENUMERATOR_BASE   _MULLE_ARRAYENUMERATOR_BASE


struct mulle_arrayenumerator
{
   MULLE_ARRAYENUMERATOR_BASE;
};


extern struct mulle_arrayenumerator   mulle_arrayenumerator_empty;


struct mulle_array    *mulle_array_create( struct mulle_container_keycallback *callback,
                                           struct mulle_allocator *allocator);


static inline void   mulle_array_destroy( struct mulle_array *array)
{
   _mulle_array_destroy( (struct _mulle_array *) array, array->_callback, array->_allocator);
}


static inline void   mulle_array_done( struct mulle_array *array)
{
   _mulle_array_done( (struct _mulle_array *) array, array->_callback, array->_allocator);
}


static inline int    mulle_array_grow( struct mulle_array *array)
{
   return( _mulle_array_grow( (struct _mulle_array *) array, array->_callback, array->_allocator));
}


static inline int    mulle_array_size_to_fit( struct mulle_array *array)
{
   return( _mulle_array_size_to_fit( (struct _mulle_array *) array, array->_callback, array->_allocator));
}


static inline void   mulle_array_remove_in_range( struct mulle_array *array, unsigned int location, unsigned int length)
{
   _mulle_array_remove_in_range( (struct _mulle_array *) array,
                                 location,
                                 length,
                                 array->_callback,
                                 array->_allocator);
}


static inline void   mulle_array_zero_in_range( struct mulle_array *array, unsigned int location, unsigned int length)
{
   _mulle_array_zero_in_range( (struct _mulle_array *) array,
                               location,
                               length,
                               array->_callback);
}


static inline void   mulle_array_compact_zeroes( struct mulle_array *array)
{
   _mulle_array_compact_zeroes( (struct _mulle_array *) array, array->_callback);
}


static inline void   mulle_array_remove_all( struct mulle_array *array)
{
   _mulle_array_remove_all( (struct _mulle_array *) array, array->_callback, array->_allocator);
}


static inline void    mulle_array_init( struct mulle_array *array,
                                        unsigned int length,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   if( ! allocator)
      allocator = &mulle_default_allocator;

   _mulle_array_init( (struct _mulle_array *) array, length);

   array->_callback  = callback;
   array->_allocator = allocator;
}


static inline void   **mulle_array_get_all( struct mulle_array *array)
{
   return( _mulle_array_get_all( (struct _mulle_array *) array));
}


static inline unsigned int   mulle_array_get_count( struct mulle_array *array)
{
   return( _mulle_array_get_count( (struct _mulle_array *) array));
}


static inline void   **mulle_array_advance( struct mulle_array *array,
                                            unsigned int length)
{
   return( _mulle_array_advance( (struct _mulle_array *) array, length, array->_callback, array->_allocator));
}


static inline int   mulle_array_is_full( struct mulle_array *array)
{
   return( _mulle_array_is_full( (struct _mulle_array *) array));
}


// will use callbacks of array to determine equality
static inline int   mulle_array_is_equal( struct mulle_array *array, struct mulle_array *other)
{
   assert( ! other || array->_callback->is_equal == other->_callback->is_equal);
   return( _mulle_array_is_equal( (struct _mulle_array *) array, (struct _mulle_array *) other, array->_callback));
}



static inline unsigned int   mulle_array_guaranteedsize( struct mulle_array *array)
{
   return( _mulle_array_guaranteedsize( (struct _mulle_array *) array));
}


static inline void   **mulle_array_guarantee( struct mulle_array *array, unsigned int length)
{
   return( _mulle_array_guarantee( (struct _mulle_array *) array, length, array->_callback, array->_allocator));
}


static inline void    mulle_array_add( struct mulle_array *array,
                                       void  *p)
{
   _mulle_array_add( (struct _mulle_array *) array, p, array->_callback, array->_allocator);
}


static inline void   *mulle_array_get( struct mulle_array *array, unsigned int index)
{
   return( _mulle_array_get( (struct _mulle_array *) array, index));
}


static inline void   *mulle_array_get_last( struct mulle_array *array)
{
   return( _mulle_array_get_last( (struct _mulle_array *) array));
}


static inline void   mulle_array_remove_last( struct mulle_array *array)
{
   _mulle_array_remove_last( (struct _mulle_array *) array, array->_callback, array->_allocator);
}


static inline void   mulle_array_add_multiple( struct mulle_array *array,
                                               void **pointers,
                                               unsigned int length)
{
   _mulle_array_add_multiple( (struct _mulle_array *) array, pointers, length, array->_callback, array->_allocator);
}


static inline void    mulle_array_add_array( struct mulle_array *array,
                                             struct mulle_array *other)
{
   _mulle_array_add_array( (struct _mulle_array *) array, (struct _mulle_array *) other, array->_callback, array->_allocator);
}


static inline void   mulle_array_reset( struct mulle_array *array)
{
   _mulle_array_reset( (struct _mulle_array *) array, array->_callback, array->_allocator);
}



#pragma mark -
#pragma mark grab contents destrutively
//
// you now own the allocated block now
//
static inline void   **mulle_array_extract_all( struct mulle_array *array)
{
   return( _mulle_array_extract_all( (struct _mulle_array *) array));
}


#pragma mark -
#pragma mark search

static inline unsigned long  mulle_array_find_in_range_identical( struct mulle_array *array,
                                                           void *obj,
                                                           unsigned int location,
                                                           unsigned int length)
{
   return( _mulle_array_find_in_range_identical( (struct _mulle_array *) array, obj, location, length));
}


static inline unsigned long  mulle_array_find_in_range( struct mulle_array *array,
                                                 void *obj,
                                                 unsigned int location,
                                                 unsigned int length)
{
   return( _mulle_array_find_in_range( (struct _mulle_array *) array, obj, location, length, array->_callback));
}



#pragma mark -
#pragma mark enumeration

static inline struct mulle_arrayenumerator   mulle_array_enumerate( struct mulle_array *array)
{
   struct _mulle_arrayenumerator   rover;

   rover = _mulle_array_enumerate( (struct _mulle_array *) array, array->_callback);
   return( *(struct mulle_arrayenumerator *) &rover);  // should be harmless
}


static inline void   *mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover)
{
   return( _mulle_arrayenumerator_next( (struct _mulle_arrayenumerator *) rover));
}


static inline void   mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
{
   _mulle_arrayenumerator_done( (struct _mulle_arrayenumerator *) rover);
}

#pragma mark -
#pragma mark integer convenience

static inline void    mulle_array_add_intptr( struct mulle_array *array,
                                              intptr_t v)
{
   _mulle_array_add( (struct _mulle_array *) array, (void *) v, array->_callback, array->_allocator);
}


static inline void    mulle_array_add_int( struct mulle_array *array,
                                           int v)
{
   mulle_array_add_intptr( array, v);
}


static inline intptr_t    mulle_array_get_intptr( struct mulle_array *array, unsigned int index)
{
   return( (intptr_t) mulle_array_get( array, index));
}


static inline int    mulle_array_get_int( struct mulle_array *array, unsigned int index)
{
   return( (int) mulle_array_get_intptr( array, index));
}


static inline intptr_t   mulle_arrayenumerator_next_intptr( struct mulle_arrayenumerator *rover)
{
   return( (intptr_t) mulle_arrayenumerator_next( rover));
}


static inline int   mulle_arrayenumerator_next_int( struct mulle_arrayenumerator *rover)
{
   return( (int) mulle_arrayenumerator_next_intptr( rover));
}



static inline unsigned long  mulle_array_find_in_range_identical_intptr( struct mulle_array *array,
                                                                  intptr_t v,
                                                                  unsigned int location,
                                                                  unsigned int length)
{
   // ints are always identical
   return(  mulle_array_find_in_range_identical( array, (void *) v, location, length));
}


static inline unsigned long   mulle_array_find_in_range_identical_int( struct mulle_array *array,
                                                             int v,
                                                             unsigned int location,
                                                             unsigned int length)
{
   return( mulle_array_find_in_range_identical_intptr( array, v, location, length));
}


#endif /* mulle_array_h */


