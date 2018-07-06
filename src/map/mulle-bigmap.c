//
//  mulle-bigmap.h
//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright (C) 2011 Nat!, Mulle kybernetiK. All rights reserved.
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
#include "mulle-bigmap.h"

#include "mulle-container-callback.h"

#include "include-private.h"
#include <string.h>


void   mulle_bigmap_init( struct mulle_bigmap *map,
                       size_t capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
{
   if( ! allocator)
      allocator = &mulle_default_allocator;

   _mulle_bigmap_init( (struct _mulle_bigmap *) map, capacity, callback, allocator);

   map->_callback  = callback;
   map->_allocator = allocator;
}



struct mulle_bigmap   *mulle_bigmap_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct mulle_bigmap   *map;

   map = mulle_allocator_malloc( allocator, sizeof( struct mulle_bigmap));
   mulle_bigmap_init( map, capacity, callback, allocator);
   return( map);
}


