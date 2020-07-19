//
//  mulle_pointers.c
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

#include "mulle--pointers.h"

#include "mulle-container-math.h"



# pragma mark -
# pragma mark mechanisms

// intentionally not static inline
static void   _mulle__pointers_realloc( struct mulle__pointers *pointers,
                                        size_t new_size,
                                        struct mulle_allocator *allocator)
{
   new_size = mulle_pow2round( new_size);
   if( new_size == 0)
      new_size = 4;

   pointers->size     = new_size;
   pointers->storage = mulle_allocator_realloc( allocator,
                                                pointers->storage,
                                                sizeof( void *) * new_size);
}



void   _mulle__pointers_grow( struct mulle__pointers *pointers,
                              struct mulle_allocator *allocator)
{
   _mulle__pointers_realloc( pointers, pointers->size * 2, allocator);
}


void   _mulle__pointers_guarantee( struct mulle__pointers *pointers,
                                   size_t length,
                                   struct mulle_allocator *allocator)
{
   size_t   available;

   available = pointers->size - pointers->used;
   if( available < length)
      _mulle__pointers_realloc( pointers, pointers->size + (length - available), allocator);
}


