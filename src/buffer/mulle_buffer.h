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


#define MULLE_BUFFER_BASE              \
   _MULLE_BUFFER_BASE;                 \
   struct mulle_allocator  allocator


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
   struct mulle_allocator  allocator

struct mulle_flushablebuffer
{
   MULLE_FLUSHABLEBUFFER_BASE;
};


struct mulle_buffer   *mulle_buffer_create( struct mulle_allocator *allocator);


static inline void   mulle_buffer_free( struct mulle_buffer *buffer)
{
   _mulle_buffer_free( (struct _mulle_buffer *) buffer, &buffer->allocator);
}

static inline void   mulle_buffer_done( struct mulle_buffer *buffer)
{
   _mulle_buffer_done( (struct _mulle_buffer *) buffer, &buffer->allocator);
}

static inline int    mulle_buffer_grow( struct mulle_buffer *buffer,
                                        size_t min_amount)
{
   return( _mulle_buffer_grow( (struct _mulle_buffer *) buffer, min_amount, &buffer->allocator));
}


static inline void   mulle_buffer_size_to_fit( struct mulle_buffer *buffer)
{
   _mulle_buffer_size_to_fit( (struct _mulle_buffer *) buffer, &buffer->allocator);
}


static inline void   mulle_buffer_make_inflexable( struct mulle_buffer *buffer,
                                                   void *storage,
                                                   size_t length)
{
   _mulle_buffer_make_inflexable( (struct _mulle_buffer *) buffer, storage, length, &buffer->allocator);
}


static inline void   mulle_buffer_zero_to_length( struct mulle_buffer *buffer,
                                                  size_t length)
{
   _mulle_buffer_zero_to_length( (struct _mulle_buffer *) buffer, length, &buffer->allocator);
}


static inline size_t   mulle_buffer_set_length( struct mulle_buffer *buffer,
                                                size_t length)
{
   return( _mulle_buffer_set_length( (struct _mulle_buffer *) buffer, length, &buffer->allocator));
}


//
// you only do this once!, because you now own the malloc block
//
static inline void   *mulle_buffer_extract( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_extract( (struct _mulle_buffer *) buffer, &buffer->allocator));
}


static inline void   mulle_buffer_remove_all( struct mulle_buffer *buffer)
{
   _mulle_buffer_remove_all( (struct _mulle_buffer *) buffer);
}


static inline int     mulle_buffer_is_inflexable( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_inflexable( (struct _mulle_buffer *) buffer));
}


static inline int     mulle_buffer_is_flushable( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_flushable( (struct _mulle_buffer *) buffer));
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


static inline void    mulle_buffer_init_with_static_bytes( struct mulle_buffer *buffer,
                                                           void *storage,
                                                           size_t length)
{
   _mulle_buffer_init_with_static_bytes( (struct _mulle_buffer *) buffer, storage, length);
}


static inline void    mulle_buffer_init( struct mulle_buffer *buffer, struct mulle_allocator *allocator)
{
   assert( allocator);
   
   _mulle_buffer_init( (struct _mulle_buffer *) buffer);

   buffer->allocator = *allocator;
}


static inline void    mulle_buffer_set_initial_capacity( struct mulle_buffer *buffer,
                                                         size_t capacity)
{
   _mulle_buffer_set_initial_capacity( (struct _mulle_buffer *) buffer, capacity);
}


static inline void    mulle_buffer_init_with_capacity( struct mulle_buffer *buffer,
                                                       size_t capacity)
{
   _mulle_buffer_init_with_capacity( (struct _mulle_buffer *) buffer, capacity);
}


static inline void    mulle_buffer_init_inflexable_with_static_bytes( struct mulle_buffer *buffer,
                                                                      void *storage,
                                                                      size_t length)
{
   _mulle_buffer_init_inflexable_with_static_bytes( (struct _mulle_buffer *) buffer, storage, length);
}


static inline void   *mulle_buffer_get_bytes( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_bytes( (struct _mulle_buffer *) buffer));
}


static inline size_t   mulle_buffer_get_length( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_length( (struct _mulle_buffer *) buffer));
}


static inline void   *mulle_buffer_advance( struct mulle_buffer *buffer,
                                            size_t length,
                                            struct mulle_allocator *allocator)
{
   return( _mulle_buffer_advance( (struct _mulle_buffer *) buffer,
                                  length,
                                  &buffer->allocator));
}


static inline int   mulle_buffer_is_full( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_is_full( (struct _mulle_buffer *) buffer));
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


static inline void   *mulle_buffer_guarantee( struct mulle_buffer *buffer,
                                              size_t length)
{
   return( _mulle_buffer_guarantee( (struct _mulle_buffer *) buffer, length, &buffer->allocator));
}


static inline void    mulle_buffer_add_byte( struct mulle_buffer *buffer,
                                             unsigned char c)
{
   _mulle_buffer_add_byte( (struct _mulle_buffer *) buffer, c, &buffer->allocator);
}


static inline void    mulle_buffer_add_char( struct mulle_buffer *buffer,
                                             int c)
{
   _mulle_buffer_add_char( (struct _mulle_buffer *) buffer, c, &buffer->allocator);
}


static inline void   mulle_buffer_add_bytes( struct mulle_buffer *buffer,
                                             void *bytes,
                                             size_t length)
{
   _mulle_buffer_add_bytes( (struct _mulle_buffer *) buffer, bytes, length, &buffer->allocator);
}


static inline void   mulle_buffer_add_string( struct mulle_buffer *buffer,
                                              char *bytes)
{
   _mulle_buffer_add_string( (struct _mulle_buffer *) buffer, bytes, &buffer->allocator);
}


static inline size_t   mulle_buffer_add_string_with_length( struct mulle_buffer *buffer,
                                                            char *bytes,
                                                            size_t length)
{
   return( _mulle_buffer_add_string_with_length( (struct _mulle_buffer *) buffer,
                                                 bytes,
                                                 length,
                                                 &buffer->allocator));
}


static inline void   mulle_buffer_memset( struct mulle_buffer *buffer,
                                          int c,
                                          size_t length)
{
   _mulle_buffer_memset( (struct _mulle_buffer *) buffer, c, length, &buffer->allocator);
}


static inline void   mulle_buffer_zero_last_byte( struct mulle_buffer *buffer)
{
   _mulle_buffer_zero_last_byte( (struct _mulle_buffer *) buffer);
}


static inline size_t   mulle_buffer_get_static_bytes( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_get_static_bytes( (struct _mulle_buffer *) buffer));
}


static inline void    mulle_buffer_add_buffer( struct mulle_buffer *buffer,
                                               struct mulle_buffer *other)
{
   return( _mulle_buffer_add_buffer( (struct _mulle_buffer *) buffer, (struct _mulle_buffer *) other, &buffer->allocator));
}


// _initial_storage storage will be lost
static inline void   mulle_buffer_reset( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_reset( (struct _mulle_buffer *) buffer, &buffer->allocator));
}




static inline int   mulle_buffer_next_byte( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_next_byte( (struct _mulle_buffer *) buffer));
}


static inline int   mulle_buffer_next_char( struct mulle_buffer *buffer)
{
   return( _mulle_buffer_next_char( (struct _mulle_buffer *) buffer));
}


#endif /* mulle_buffer_h */
