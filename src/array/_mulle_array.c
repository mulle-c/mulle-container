//  _mulle_array.c
//  mulle-container
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

#include "_mulle_array.h"

#include <mulle_allocator/mulle_allocator.h>

#include "mulle_container_operation.h"
#include "mulle_container_callback.h"


#pragma mark -
#pragma mark _mulle_arrayrange internal helper


struct _mulle_arrayrange
{
   void   **p;
   void   **sentinel;
};


static inline struct _mulle_arrayrange   _mulle_arrayrange_make( void **p, void **sentinel)
{
   return( (struct _mulle_arrayrange){ .p = p, .sentinel = sentinel });
}


static inline struct _mulle_arrayrange   _mulle_array_get_arrayrange( struct _mulle_array *array,
                                                                      unsigned int location,
                                                                      unsigned int length)
{
   struct _mulle_arrayrange   range;

   range.p        = &array->_storage[ location];
   range.sentinel = &range.p[ length];
   
   assert( range.sentinel <= array->_sentinel);

   return( range);
}


static inline void   _mulle_arrayrange_release( struct _mulle_arrayrange range,
                                                struct mulle_container_keycallback *callback,
                                                struct mulle_allocator *allocator)
{
   while( range.p < range.sentinel)
      (*callback->release)( callback, *range.p++, allocator);
}

#pragma mark -
#pragma mark grow and shrink


//
// all allocation is done in .c
//
static int  _mulle_array_set_size( struct _mulle_array *array,
                                   unsigned int new_size,
                                   struct mulle_container_keycallback *callback,
                                   struct mulle_allocator *allocator)
{
   unsigned int    count;
   
   count = _mulle_array_get_count( array);
   
   array->_storage = mulle_allocator_realloc( allocator, array->_storage, sizeof( void *) * new_size);
   if( ! array->_storage)
   {
      array->_curr     = NULL;
      array->_sentinel = mulle_container_not_a_pointer_key;
      
      return( -1);
   }

   array->_curr     = &array->_storage[ count];
   array->_sentinel = &array->_storage[ new_size];
   array->_size     = new_size;
   
   return( 0);
}


int  _mulle_array_grow( struct _mulle_array *array,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   unsigned int   new_size;
   
   new_size = array->_size * 2;
   if( new_size < 16)
      new_size = 16;
   
   return( _mulle_array_set_size( array, new_size, callback, allocator));
}


int  _mulle_array_size_to_fit( struct _mulle_array *array,
                               struct mulle_container_keycallback *callback,
                               struct mulle_allocator *allocator)
{
   unsigned int   new_size;
   
   new_size = _mulle_array_get_count( array);
   return( _mulle_array_set_size( array, new_size, callback, allocator));
}


#pragma mark -
#pragma mark creation and destruction

struct _mulle_array    *_mulle_array_create( struct mulle_allocator *allocator)
{
   struct _mulle_array  *array;
   
   array = mulle_allocator_malloc( allocator, sizeof( struct _mulle_array));
   if( array)
      _mulle_array_init( array, 0);
   return( array);
}



void   _mulle_array_done( struct _mulle_array *array,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   _mulle_array_remove_all( array, callback, allocator);
   mulle_allocator_free( allocator, array->_storage);
   array->_storage  = NULL;
}


void   _mulle_array_destroy( struct _mulle_array *array,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   _mulle_array_done( array, callback, allocator);
   mulle_allocator_free( allocator, array);
}


#pragma mark -
#pragma mark search


static void  assert_index( struct _mulle_array *array, unsigned int i)
{
   assert( ! _mulle_array_has_overflown( array));
   assert( i < _mulle_array_get_count( array));
}



//
// range must have been checked already
//
unsigned long   _mulle_array_find_in_range( struct _mulle_array *array,
                                            void *obj,
                                            unsigned int location,
                                            unsigned int length,
                                            struct mulle_container_keycallback *callback)
{
   unsigned int   i, n;
   
   // allow obj == nil
   if( ! obj || ! _mulle_array_get_count( array))
      return( mulle_not_found_e);
      
   // quick check for first 32 pointers
   n = length;
   if( n > 32)
      n = 32;
   n += location;
   
   assert_index( array, n - 1);
   
   for( i = location; i < n; i++)
      if( array->_storage[ i] == obj)
         return( i);
   
   n  = length + location;
   for( i = location; i < n; i++)
      if( (*callback->is_equal)( callback, obj, array->_storage[ i]))
         return( i);

   return( mulle_not_found_e);
}

unsigned long  _mulle_array_find_in_range_identical( struct _mulle_array *array,
                                                     void *obj,
                                                     unsigned int location,
                                                     unsigned int length)
{
   unsigned int   i, n;
   
   n = length + location;
   
   assert_index( array, n - 1);
   assert( n >= location);
   
   for( i = location; i < n; i++)
      if( array->_storage[ i] == obj)
         return( i);
   
   return( mulle_not_found_e);
}


#pragma mark -
#pragma mark removal

void   _mulle_array_zero_in_range( struct _mulle_array *array,
                                   unsigned int location,
                                   unsigned int length,
                                   struct mulle_container_keycallback *callback)
{
   struct _mulle_arrayrange   range;
   
   assert( ! _mulle_array_has_overflown( array));

   range = _mulle_array_get_arrayrange( array, location, length);
   if( ! callback->notakey)
   {
      memset( range.p, 0, length * sizeof( void *));
      return;
   }

   while( range.p < range.sentinel)
      *range.p++= callback->notakey;
}



void   _mulle_array_compact_zeroes( struct _mulle_array *array,
                                    struct mulle_container_keycallback *callback)
{
   void   **p;
   void   **q;
   void   **sentinel;
   
   assert( ! _mulle_array_has_overflown( array));

   p        = array->_storage;
   q        = p;
   sentinel = array->_curr;
   
   for( ;p < sentinel; p++)
   {
      if( *p != callback->notakey)
         continue;
      *q++ = *p;
   }
   array->_curr = q;
}




void   _mulle_array_remove_all( struct _mulle_array *array,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator)
{
   assert( ! _mulle_array_has_overflown( array));
   if( callback->release != (void (*)()) mulle_container_callback_nop)
      _mulle_arrayrange_release( _mulle_arrayrange_make( array->_storage, array->_curr), callback, allocator);

   array->_curr = array->_storage;
}


void   _mulle_array_remove_in_range( struct _mulle_array *array,
                                     unsigned int location,
                                     unsigned int length,
                                     struct mulle_container_keycallback *callback,
                                     struct mulle_allocator *allocator)
{
   unsigned int   n;
   
   assert( ! _mulle_array_has_overflown( array));
   _mulle_arrayrange_release( _mulle_array_get_arrayrange( array, location, length), callback, allocator);

   n = _mulle_array_get_count( array) - (location + length - 1);
   memcpy( &array->_storage[ location],
           &array->_storage[ location + length],
           n * sizeof( void *));
   
   array->_curr = array->_storage[ location + n];
}


int   _mulle_array_add_multiple( struct _mulle_array *array,
                                 void   **pointers,
                                 unsigned int length,
                                 struct mulle_container_keycallback *callback,
                                 struct mulle_allocator *allocator)
{
   void     **p;
   void     **sentinel;
   
   assert( pointers || ! length);
   
   p = _mulle_array_advance( array, length, callback, allocator);
   if( ! p)
      return( -1);

   if( callback->retain == (void *(*)()) mulle_container_callback_self)
   {
      memcpy( p, pointers, length * sizeof( void *));
      return( 0);
   }
   
   sentinel = &pointers[ length];
   while( pointers < sentinel)
   {
      *p++ = (*callback->retain)( callback, *pointers++, allocator);
      assert( p[ -1] != callback->notakey);
   }
   return( 0);
}


int    _mulle_array_is_equal( struct _mulle_array *array,
                              struct _mulle_array *other,
                              struct mulle_container_keycallback *callback)
{
   unsigned int   i, n;
   
   assert( ! _mulle_array_has_overflown( array));
   if( ! other)
      return( 0);
   
   assert( ! _mulle_array_has_overflown( other));

   n = _mulle_array_get_count( array);
   if( n != _mulle_array_get_count( other))
      return( 0);
   
   if( callback->is_equal == mulle_container_keycallback_pointer_is_equal)
      return( ! memcmp( array->_storage, other->_storage, n * sizeof( void *)));
   
   for( i = 0; i < n; i++)
      if( ! (callback->is_equal)( callback, array->_storage[ i], other->_storage[ i]))
         return( 0);

   return( 1);
}
