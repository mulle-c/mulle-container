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

#include "_mulle_buffer.h"

#if DEBUG
# define MULLE_BUFFER_MIN_GROW_SIZE    16      // minimum realloc increment
# define MULLE_BUFFER_MIN_DOUBLE_SIZE  512     // when we start doubling the buffer
#else
# define MULLE_BUFFER_MIN_GROW_SIZE    64       // minimum realloc increment
# define MULLE_BUFFER_MIN_DOUBLE_SIZE  1024    // when we start doubling the buffer
#endif


void   *_mulle_buffer_extract( struct _mulle_buffer *buffer, struct mulle_allocator *allocator)
{
   size_t   size;
   void     *block;
   
   block = buffer->_storage;
   if( buffer->_storage == buffer->_initial_storage)
   {
      size  = _mulle_buffer_get_length( buffer);
      block = (*allocator->realloc)( NULL, size);
      memcpy( block, buffer->_storage, size);
   }
   
   buffer->_storage          = 
   buffer->_curr             = 
   buffer->_sentinel         = 
   buffer->_initial_storage  = NULL;

   return( block);
}


void    _mulle_buffer_size_to_fit( struct _mulle_buffer *buffer, struct mulle_allocator *allocator)
{
   size_t   length;
   
   if( _mulle_buffer_is_inflexable( buffer))
      return;
      
   if( buffer->_storage == buffer->_initial_storage)
      return;
      
   length  = _mulle_buffer_get_length( buffer);

   buffer->_storage  = (*allocator->realloc)( buffer->_storage, sizeof( unsigned char) * length);
   if( buffer->_storage)
   {
      buffer->_curr     = &buffer->_storage[ length];
      buffer->_sentinel = &buffer->_storage[ length];
      buffer->_size     = length;
   }
   else
   {
      buffer->_curr     = 0;
      buffer->_sentinel = 0;
      buffer->_size     = 0;
   }
}


int   _mulle_buffer_grow( struct _mulle_buffer *buffer, size_t min_amount, struct mulle_allocator *allocator)
{
   void     *malloc_block;
   size_t   new_size;
   size_t   diff;
   size_t   len;
   
   if( _mulle_buffer_is_inflexable( buffer))
   {  
      struct _mulle_flushablebuffer  *ibuffer;
      
      if( ! _mulle_buffer_is_flushable( buffer))
      {
         buffer->_curr = buffer->_sentinel + 1;  // set "overflowed"
         return( -1);
      }
         
      ibuffer = (struct _mulle_flushablebuffer *) buffer;
      if( 1 != (*ibuffer->_flusher)( ibuffer->_storage, ibuffer->_curr - ibuffer->_storage, 1, ibuffer->_userinfo))
      {
         ibuffer->_curr = ibuffer->_sentinel + 1;  // set "overflowed"
         return( -2);
      }
      
      ibuffer->_curr = ibuffer->_storage;
      return( 0);
   }
   
   malloc_block = NULL;
   if( buffer->_storage != buffer->_initial_storage)
      malloc_block = buffer->_storage;
   
   //
   // assume realloc is slow enough, to warrant all this code :)
   //
   diff = MULLE_BUFFER_MIN_GROW_SIZE;
   if( min_amount > MULLE_BUFFER_MIN_GROW_SIZE)
      diff = min_amount;

   if( min_amount >= MULLE_BUFFER_MIN_DOUBLE_SIZE && diff < buffer->_size)
      diff = buffer->_size;
      
   new_size          = buffer->_size + diff;
   
   assert( new_size >= min_amount);
   
   len               = buffer->_curr - buffer->_storage;
   buffer->_storage  = (*allocator->realloc)( malloc_block, sizeof( unsigned char) * new_size);
   if( buffer->_storage)
   {
      buffer->_curr     = &buffer->_storage[ len];
      buffer->_sentinel = &buffer->_storage[ new_size];
      buffer->_size     = new_size;
   }
   else
   {
      buffer->_curr     = NULL;
      buffer->_sentinel = NULL;
      buffer->_size     = 0;
      return( -3);
   }
      
   assert( buffer->_sentinel >= buffer->_curr);

   return( 0);
}


//
// this transforms a buffer
// into a inflexableBuffer 
//
void   _mulle_buffer_make_inflexable( struct _mulle_buffer *buffer,
                                     void *buf,
                                     size_t length,
                                     struct mulle_allocator *allocator)
{
   if( buffer->_storage != buffer->_initial_storage)
      (*allocator->free)( buffer->_storage);
   
   buffer->_storage         = 
   buffer->_initial_storage = buf;
   
   buffer->_curr            = 
   buffer->_sentinel        = &buffer->_storage[ length];
   buffer->_size            = (size_t) -1;
}


void   _mulle_buffer_done( struct _mulle_buffer *buffer,
                          struct mulle_allocator *allocator)
{
   if( buffer->_storage != buffer->_initial_storage)
      (*allocator->free)( buffer->_storage);
}


struct _mulle_buffer   *_mulle_buffer_create( struct mulle_allocator *allocator)
{
   struct _mulle_buffer  *buffer;
   
   buffer = (*allocator->realloc)( NULL, sizeof( struct _mulle_buffer));
   _mulle_buffer_init( buffer);
   return( buffer);
}


void   _mulle_buffer_free( struct _mulle_buffer *buffer,
                          struct mulle_allocator *allocator)
{
   (*allocator->free)( buffer);
}


size_t   _mulle_buffer_set_length( struct _mulle_buffer *buffer,
                                  size_t length,
                                  struct mulle_allocator *allocator)
{
   long   diff;
   
   diff = (long) length - (long) _mulle_buffer_get_length( buffer);
   if( diff <= 0)
   {
      buffer->_curr = &buffer->_curr[ diff];
      _mulle_buffer_size_to_fit( buffer, allocator);
      return( 0);
   }
   
   _mulle_buffer_advance( buffer, (size_t) diff, allocator);
   return( (size_t) diff);
}


void   _mulle_buffer_zero_to_length( struct _mulle_buffer *buffer,
                                    size_t length,
                                    struct mulle_allocator *allocator)
{
   long   diff;
   
   diff = _mulle_buffer_set_length( buffer, length, allocator);
   memset( &buffer->_curr[ -diff], 0, (size_t) diff);
}

