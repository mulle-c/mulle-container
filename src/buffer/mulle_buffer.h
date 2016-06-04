//
//  mulle_buffer.h
//  mulle-container
//
//  Created by Nat! on 04/11/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
//
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

#ifndef mulle_buffer__h__
#define mulle_buffer__h__

#include "_mulle_buffer.h"

#include <mulle_allocator/mulle_allocator.h>


// stupidities to fix:
// non "_" prefixed functions should check for NULL buffer
//
#define MULLE_BUFFER_BASE              \
   _MULLE_BUFFER_BASE;                 \
   struct mulle_allocator  *_allocator


//
// _size will be -1 for a non-growing buffer
// this is like a possibly growing memory block (->NSData)
//
struct mulle_buffer
{
   MULLE_BUFFER_BASE;
};



#define MULLE_FLUSHABLEBUFFER_BASE              \
   _MULLE_FLUSHABLEBUFFER_BASE;                 \
   struct mulle_allocator  *_allocator

struct mulle_flushablebuffer
{
   MULLE_FLUSHABLEBUFFER_BASE;
};


struct mulle_buffer   *mulle_buffer_create( struct mulle_allocator *allocator);


static inline void   mulle_buffer_destroy( struct mulle_buffer *buffer)
{
   _mulle_buffer_destroy( (struct _mulle_buffer *) buffer, buffer->_allocator);
}

static inline void   mulle_buffer_done( struct mulle_buffer *buffer)
{
   _mulle_buffer_done( (struct _mulle_buffer *) buffer, buffer->_allocator);
}


typedef _mulle_flushablebuffer_flusher   mulle_flushablebuffer_flusher;

static inline void    mulle_flushablebuffer_init( struct mulle_flushablebuffer *buffer,
                                                  void *storage,
                                                  size_t length,
                                                  mulle_flushablebuffer_flusher *flusher,
                                                  void *userinfo)
{
   _mulle_flushablebuffer_init( (struct _mulle_flushablebuffer *) buffer, storage, length, flusher, userinfo);
}



static inline void    mulle_buffer_set_allocator( struct mulle_buffer *buffer,
                                                  struct mulle_allocator *allocator)
{
   assert( buffer);
   
   // always have an allocator
   buffer->_allocator = allocator ? allocator : &mulle_default_allocator;
}


/* a growing buffer, that starts with a memory region, allocated by the user.
   Useful if you already malloced something and dont't need it anymore.
   The storage is overwritten from the start.
*/
static inline void    mulle_buffer_init_with_allocated_bytes( struct mulle_buffer *buffer,
                                                              void *storage,
                                                              size_t length,
                                                              struct mulle_allocator *allocator)
{
   _mulle_buffer_init_with_allocated_bytes( (struct _mulle_buffer *) buffer, storage, length);
   mulle_buffer_set_allocator( buffer, allocator);
}


/* a growing buffer, that starts with a non freeable memory region, allocated 
   by the user. Useful if you want to supply some stack memory for mulle_buffer
   so that it need not necessarily malloc, if the contents remain small enough.
   The storage is overwritten from the start.
 */
static inline void    mulle_buffer_init_with_static_bytes( struct mulle_buffer *buffer,
                                                           void *storage,
                                                           size_t length,
                                                           struct mulle_allocator *allocator)
{
   _mulle_buffer_init_with_static_bytes( (struct _mulle_buffer *) buffer, storage, length);
   mulle_buffer_set_allocator( buffer, allocator);
}


static inline void    mulle_buffer_init( struct mulle_buffer *buffer,
                                         struct mulle_allocator *allocator)
{
   _mulle_buffer_init( (struct _mulle_buffer *) buffer);
   mulle_buffer_set_allocator( buffer, allocator);
}


static inline void    mulle_buffer_init_with_capacity( struct mulle_buffer *buffer,
                                                       size_t capacity,
                                                       struct mulle_allocator *allocator)
{
   _mulle_buffer_init_with_capacity( (struct _mulle_buffer *) buffer, capacity);
   mulle_buffer_set_allocator( buffer, allocator);
}


/* this buffer does not grow. storage is supplied. This is useful for 
   guaranteeing that a buffer can not be overrun. 
   The storage is overwritten from the start.
 */
static inline void    mulle_buffer_init_inflexable_with_static_bytes( struct mulle_buffer *buffer,
                                                                      void *storage,
                                                                      size_t length)
{
   _mulle_buffer_init_inflexable_with_static_bytes( (struct _mulle_buffer *) buffer, storage, length);

   buffer->_allocator = NULL; // don't allocate
}


#pragma mark -
#pragma mark sizing


static inline int    mulle_buffer_grow( struct mulle_buffer *buffer,
                                        size_t min_amount)
{
   return( _mulle_buffer_grow( (struct _mulle_buffer *) buffer, min_amount, buffer->_allocator));
}


static inline void   mulle_buffer_size_to_fit( struct mulle_buffer *buffer)
{
   _mulle_buffer_size_to_fit( (struct _mulle_buffer *) buffer, buffer->_allocator);
}


/* the given storage replaces the previous contents.
   the buffer is not flexable anymore. 
   The insertion point is moved to &storage[length]
 */
static inline void   mulle_buffer_make_inflexable( struct mulle_buffer *buffer,
                                                   void *storage,
                                                   size_t length)
{
   _mulle_buffer_make_inflexable( (struct _mulle_buffer *) buffer, storage, length, buffer->_allocator);
}


static inline void   mulle_buffer_zero_to_length( struct mulle_buffer *buffer,
                                                  size_t length)
{
   _mulle_buffer_zero_to_length( (struct _mulle_buffer *) buffer, length, buffer->_allocator);
}


static inline size_t   mulle_buffer_set_length( struct mulle_buffer *buffer,
                                                size_t length)
{
   return( _mulle_buffer_set_length( (struct _mulle_buffer *) buffer, length, buffer->_allocator));
}


//
// you only do this once!, because you now own the malloc block
//
static inline void   *mulle_buffer_extract_bytes( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_extract_bytes( (struct _mulle_buffer *) buffer, buffer->_allocator));
}


static inline void   mulle_buffer_remove_all( struct mulle_buffer *buffer)
{
   _mulle_buffer_remove_all( (struct _mulle_buffer *) buffer);
}


static inline void    mulle_buffer_set_initial_capacity( struct mulle_buffer *buffer,
                                                         size_t capacity)
{
   _mulle_buffer_set_initial_capacity( (struct _mulle_buffer *) buffer, capacity);
}


#pragma mark -
#pragma mark accessors

static inline void   *mulle_buffer_get_bytes( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_bytes( (struct _mulle_buffer *) buffer));
}


static inline size_t   mulle_buffer_get_length( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_length( (struct _mulle_buffer *) buffer));
}


static inline size_t   mulle_buffer_get_static_bytes_length( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_static_bytes_length( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_set_seek( struct mulle_buffer *buffer, int mode, off_t seek)
{
   return( _mulle_buffer_set_seek( (struct _mulle_buffer *) buffer, mode, seek));
}


static inline off_t   mulle_buffer_get_seek( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_seek( (struct _mulle_buffer *) buffer));
}


static inline struct mulle_allocator  *mulle_buffer_get_allocator( struct mulle_buffer *buffer)
{
   return( buffer->_allocator);
}


static inline void   *mulle_buffer_advance( struct mulle_buffer *buffer,
                                            size_t length)
{
   return( _mulle_buffer_advance( (struct _mulle_buffer *) buffer,
                                  length,
                                  buffer->_allocator));
}


#pragma mark -
#pragma mark query

static inline int   mulle_buffer_is_inflexable( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_inflexable( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_is_flushable( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_flushable( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_is_full( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_full( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_is_big_enough( struct mulle_buffer *buffer, size_t len)
{
   return( _mulle_buffer_is_big_enough( (struct _mulle_buffer *) buffer, len));
}


static inline int   mulle_buffer_is_empty( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_empty( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_is_void( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_void( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_has_overflown( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_has_overflown( (struct _mulle_buffer *) buffer));
}

#pragma mark -
#pragma mark additions

static inline int   mulle_buffer_guarantee( struct mulle_buffer *buffer,
                                            size_t length)
{
   return( _mulle_buffer_guarantee( (struct _mulle_buffer *) buffer, length, buffer->_allocator));
}


static inline void    mulle_buffer_add_byte( struct mulle_buffer *buffer,
                                             unsigned char c)
{
   _mulle_buffer_add_byte( (struct _mulle_buffer *) buffer, c, buffer->_allocator);
}


static inline void    mulle_buffer_remove_last_byte( struct mulle_buffer *buffer)
{
   _mulle_buffer_remove_last_byte( (struct _mulle_buffer *) buffer);
}


static inline void    mulle_buffer_add_char( struct mulle_buffer *buffer,
                                             int c)
{
   _mulle_buffer_add_char( (struct _mulle_buffer *) buffer, c, buffer->_allocator);
}


static inline void    mulle_buffer_add_uint16( struct mulle_buffer *buffer,
                                               uint16_t c)
{
   _mulle_buffer_add_uint16( (struct _mulle_buffer *) buffer, c, buffer->_allocator);
}


static inline void    mulle_buffer_add_uint32( struct mulle_buffer *buffer,
                                               uint32_t c)
{
   _mulle_buffer_add_uint32( (struct _mulle_buffer *) buffer, c, buffer->_allocator);
}


#pragma mark -
#pragma mark add memory ranges

static inline int   mulle_buffer_intersects_bytes( struct mulle_buffer *buffer,
                                                   void *bytes,
                                                   size_t length)
{
   return( _mulle_buffer_intersects_bytes( (struct _mulle_buffer *) buffer, bytes, length));
}


static inline void   mulle_buffer_add_bytes( struct mulle_buffer *buffer,
                                             void *bytes,
                                             size_t length)
{
   _mulle_buffer_add_bytes( (struct _mulle_buffer *) buffer, bytes, length, buffer->_allocator);
}


static inline void   mulle_buffer_add_string( struct mulle_buffer *buffer,
                                              char *bytes)
{
   _mulle_buffer_add_string( (struct _mulle_buffer *) buffer, bytes, buffer->_allocator);
}


static inline size_t   mulle_buffer_add_string_with_maxlength( struct mulle_buffer *buffer,
                                                            char *bytes,
                                                            size_t length)
{
   return( _mulle_buffer_add_string_with_maxlength( (struct _mulle_buffer *) buffer,
                                                     bytes,
                                                     length,
                                                     buffer->_allocator));
}


static inline void   mulle_buffer_memset( struct mulle_buffer *buffer,
                                          int c,
                                          size_t length)
{
   _mulle_buffer_memset( (struct _mulle_buffer *) buffer, c, length, buffer->_allocator);
}


static inline void   mulle_buffer_zero_last_byte( struct mulle_buffer *buffer)
{
   _mulle_buffer_zero_last_byte( (struct _mulle_buffer *) buffer);
}



static inline void    mulle_buffer_add_buffer( struct mulle_buffer *buffer,
                                               struct mulle_buffer *other)
{
   _mulle_buffer_add_buffer( (struct _mulle_buffer *) buffer, (struct _mulle_buffer *) other, buffer->_allocator);
}


// _initial_storage storage will be lost
static inline void   mulle_buffer_reset( struct mulle_buffer *buffer)
{
   _mulle_buffer_reset( (struct _mulle_buffer *) buffer, buffer->_allocator);
}


#pragma mark -
#pragma mark reading

/*
 * Limited read support for buffers
 * -1 == no more bytes
 */
static inline int   mulle_buffer_next_bytes( struct mulle_buffer *buffer,
                                             void *buf,
                                             size_t len)
{
   return( _mulle_buffer_next_bytes( (struct _mulle_buffer *) buffer, buf, len));
}


static inline void   *mulle_buffer_reference_bytes( struct mulle_buffer *buffer, size_t len)
{
   return( _mulle_buffer_reference_bytes( (struct _mulle_buffer *) buffer, len));
}


static inline int   mulle_buffer_next_byte( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_next_byte( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_next_char( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_next_char( (struct _mulle_buffer *) buffer));
}


#pragma mark -
#pragma mark hexdump fixed to 16 bytes per line

// dumps only for n >= 1 && n <= 16
void  mulle_buffer_dump_hex_16( struct mulle_buffer *buffer,
                                uint8_t *bytes,
                                unsigned int n,
                                size_t counter,
                                unsigned int options);

// dumps all, does not append a \0
void  mulle_buffer_dump_hex( struct mulle_buffer *buffer,
                             uint8_t *bytes,
                             size_t length,
                             size_t counter,
                             unsigned int options);

#endif /* mulle_buffer_h */
