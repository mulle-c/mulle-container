//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
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
 
#ifndef _mulle_buffer__h__
#define _mulle_buffer__h__

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include "mulle_allocator.h"


#define _MULLE_BUFFER_BASE            \
   unsigned char   *_initial_storage; \
   unsigned char   *_storage;         \
   unsigned char   *_curr;            \
   unsigned char   *_sentinel;        \
   size_t          _size;


//
// _size will be -1 for a non-growing buffer
// this is like a possibly growing memory block (->NSData)
//
struct _mulle_buffer
{
   _MULLE_BUFFER_BASE;
};


//
// this is fairly conveniently, just like fwrite(  _storage, len, nElems, fp)
//
typedef size_t   _mulle_flushablebuffer_flusher( void *, size_t len, size_t, void *);

// _size will be -2 for a flushable buffer (always inflexable)


#define _MULLE_FLUSHABLEBUFFER_BASE             \
_MULLE_BUFFER_BASE;                          \
_mulle_flushablebuffer_flusher   *_flusher;  \
void                            *_userinfo


struct _mulle_flushablebuffer
{
   _MULLE_FLUSHABLEBUFFER_BASE;
};


#pragma mark -
#pragma mark creation destruction

struct _mulle_buffer     *_mulle_buffer_create( struct mulle_allocator *allocator);
void                     _mulle_buffer_free( struct _mulle_buffer *buffer,
                                            struct mulle_allocator *allocator);

#pragma mark -
#pragma mark initialization

static inline void    _mulle_flushablebuffer_init( struct _mulle_flushablebuffer *buffer,
                                                   void *storage,
                                                   size_t length,
                                                   _mulle_flushablebuffer_flusher *flusher,
                                                   void *userinfo)
{
   assert( storage && length);
   
   buffer->_initial_storage =
   buffer->_curr            =
   buffer->_storage         = storage;
   buffer->_sentinel        = &storage[ length];
   buffer->_size            = (size_t) -2;
   
   buffer->_flusher         = flusher;
   buffer->_userinfo        = userinfo;
}


static inline void    _mulle_buffer_init_with_static_bytes( struct _mulle_buffer *buffer,
                                                           void *storage,
                                                           size_t length)
{
   assert( length != (size_t) -1);
   
   buffer->_initial_storage =
   buffer->_curr            =
   buffer->_storage         = storage;
   buffer->_sentinel        = &((unsigned char *) storage)[ length];
   buffer->_size            = length;
}


static inline void    _mulle_buffer_init( struct _mulle_buffer *buffer)
{
   memset( buffer, 0, sizeof( struct _mulle_buffer));
}


static inline void    _mulle_buffer_set_initial_capacity( struct _mulle_buffer *buffer,
                                                         size_t capacity)
{
   assert( buffer->_storage  == 0);
   assert( buffer->_size == 0);
   buffer->_size =  capacity >> 1;
}


static inline void    _mulle_buffer_init_with_capacity( struct _mulle_buffer *buffer,
                                                       size_t capacity)
{
   buffer->_initial_storage  =
   buffer->_curr             =
   buffer->_storage          =
   buffer->_sentinel         = NULL;
   buffer->_size             = capacity >> 1;  // will grow to double _size
}


static inline void    _mulle_buffer_init_inflexable_with_static_bytes( struct _mulle_buffer *buffer,
                                                                      void *storage,
                                                                      size_t length)
{
   assert( length != (size_t) -1);
   
   buffer->_initial_storage  =
   buffer->_curr             =
   buffer->_storage          = storage;
   buffer->_sentinel         = &((unsigned char *) storage)[ length];
   buffer->_size             = (size_t) -1;
}


void   _mulle_buffer_done( struct _mulle_buffer *buffer,
                          struct mulle_allocator *allocator);


// _initial_storage storage will be lost
static inline void   _mulle_buffer_reset( struct _mulle_buffer *buffer,
                                         struct mulle_allocator *allocator)
{
   _mulle_buffer_done( buffer, allocator);
   _mulle_buffer_init( buffer);
}



#pragma mark -
#pragma mark change buffer type


void   _mulle_buffer_make_inflexable( struct _mulle_buffer *buffer,
                                     void *_storage,
                                     size_t length,
                                     struct mulle_allocator *allocator);

#pragma mark -
#pragma mark resize

int    _mulle_buffer_grow( struct _mulle_buffer *buffer,
                          size_t min_amount,
                          struct mulle_allocator *allocator);

void   _mulle_buffer_size_to_fit( struct _mulle_buffer *buffer,
                                 struct mulle_allocator *allocator);

void   _mulle_buffer_zero_to_length( struct _mulle_buffer *buffer,
                                    size_t length,
                                    struct mulle_allocator *allocator);

size_t   _mulle_buffer_set_length( struct _mulle_buffer *buffer,
                                  size_t length,
                                  struct mulle_allocator *allocator);


static inline void   *_mulle_buffer_advance( struct _mulle_buffer *buffer,
                                            size_t length,
                                            struct mulle_allocator *allocator)
{
   void             *old;
   unsigned char    *next;
   
   while( (next = &buffer->_curr[ length]) > buffer->_sentinel)
      if( _mulle_buffer_grow( buffer, length, allocator))
         return( NULL);
   
   old           = buffer->_curr;
   buffer->_curr = next;
   
   return( old);
}


static inline void   *_mulle_buffer_guarantee( struct _mulle_buffer *buffer,
                                              size_t length,
                                              struct mulle_allocator *allocator)
{
   while( &buffer->_curr[ length] > buffer->_sentinel)
      if( _mulle_buffer_grow( buffer, length, allocator))
         return( NULL);
   return( buffer->_curr);
}


#pragma mark -
#pragma mark retrieval
//
// you only do this once!, because you now own the malloc block
//
void   *_mulle_buffer_extract( struct _mulle_buffer *buffer,
                              struct mulle_allocator *allocator);


static inline void   *_mulle_buffer_get_bytes( struct _mulle_buffer *buffer)
{
   return( buffer->_storage);
}


static inline size_t   _mulle_buffer_get_length( struct _mulle_buffer *buffer)
{
   return( buffer->_curr - buffer->_storage);
}


static inline size_t   _mulle_buffer_get_static_bytes( struct _mulle_buffer *buffer)
{
   return( buffer->_storage == buffer->_initial_storage
          ? (buffer->_storage - buffer->_initial_storage)
          : 0);
}



#pragma mark -
#pragma mark removal

static inline void   _mulle_buffer_remove_all( struct _mulle_buffer *buffer)
{
   buffer->_curr = buffer->_storage;
}


#pragma mark -
#pragma mark query

static inline int     _mulle_buffer_is_inflexable( struct _mulle_buffer *buffer)
{
   return( buffer->_size == (size_t) -1 || buffer->_size == (size_t) -2);
}


static inline int     _mulle_buffer_is_flushable( struct _mulle_buffer *buffer)
{
   return( buffer->_size == (size_t) -2);
}


static inline int   _mulle_buffer_is_full( struct _mulle_buffer *buffer)
{
   return( buffer->_curr >= buffer->_sentinel);
}


static inline int   _mulle_buffer_is_empty( struct _mulle_buffer *buffer)
{
   return( buffer->_curr == buffer->_storage);
}


static inline int   _mulle_buffer_is_void( struct _mulle_buffer *buffer)
{
   return( buffer->_storage == buffer->_sentinel);
}


static inline int   _mulle_buffer_has_overflown( struct _mulle_buffer *buffer)
{
   return( buffer->_curr > buffer->_sentinel); // only ever yes for inflexable buffer
}


#pragma mark -
#pragma mark modification


// instead of checking every char, run it for a while and then check
// _mulle_buffer_has_overflown.
static inline void    _mulle_buffer_add_byte( struct _mulle_buffer *buffer,
                                              unsigned char c,
                                              struct mulle_allocator *allocator)
{
   if( _mulle_buffer_is_full( buffer))
      if( _mulle_buffer_grow( buffer, 1, allocator))
         return;

   *buffer->_curr++ = c;
}


static inline void    _mulle_buffer_add_char( struct _mulle_buffer *buffer,
                                              int c,
                                              struct mulle_allocator *allocator)
{
   assert( c <= CHAR_MAX && c >= CHAR_MIN);
   _mulle_buffer_add_byte( buffer, (unsigned char) c, allocator);
}


static inline void   _mulle_buffer_add_bytes( struct _mulle_buffer *buffer,
                                              void *bytes,
                                              size_t length,
                                              struct mulle_allocator *allocator)
{
   void   *s;
   
   s = _mulle_buffer_advance( buffer, length, allocator);
   if( s)
      memcpy( s, bytes, length);
}


static inline void   _mulle_buffer_add_string( struct _mulle_buffer *buffer,
                                               char *bytes,
                                               struct mulle_allocator *allocator)
{
   char   c;
   
   while( c = *bytes++)
      _mulle_buffer_add_byte( buffer, c, allocator);
}


static inline size_t   _mulle_buffer_add_string_with_length( struct _mulle_buffer *buffer,
                                                             char *bytes,
                                                             size_t length,
                                                             struct mulle_allocator *allocator)
{
   char   c;
   
   while( length && (c = *bytes++))
   {
      _mulle_buffer_add_byte( buffer, c, allocator);
      --length;
   }
   return( length);
}


static inline void   _mulle_buffer_memset( struct _mulle_buffer *buffer,
                                           int c,
                                           size_t length,
                                           struct mulle_allocator *allocator)
{
   void   *s;
   
   s = _mulle_buffer_advance( buffer, length, allocator);
   if( s)
      memset( s, c, length);
}


static inline void   _mulle_buffer_zero_last_byte( struct _mulle_buffer *buffer)
{
   if( ! _mulle_buffer_is_void( buffer))
   {
      if( _mulle_buffer_has_overflown( buffer))
         buffer->_curr[ -2] = 0;
      else
         if( _mulle_buffer_is_full( buffer))
            buffer->_curr[ -1] = 0;
         else
            buffer->_curr[ 0] = 0;
   }
}


static inline void    _mulle_buffer_add_buffer( struct _mulle_buffer *buffer,
                                                struct _mulle_buffer *other,
                                                struct mulle_allocator *allocator)
{
   assert( buffer != other);
   
   _mulle_buffer_add_bytes( buffer, _mulle_buffer_get_bytes( other), _mulle_buffer_get_length( other), allocator);
}

#pragma mark -
#pragma mark reading

/*
 * Limited read support for buffers
 * -1 == no more bytes
 */
static inline int   _mulle_buffer_next_byte( struct _mulle_buffer *buffer)
{
   if( _mulle_buffer_is_full( buffer))
      return( -1);
   return( *buffer->_curr++);
}


static inline int   _mulle_buffer_next_char( struct _mulle_buffer *buffer)
{
   if( _mulle_buffer_is_full( buffer))
      return( INT_MAX);
   return( (char) *buffer->_curr++);
}

#endif

