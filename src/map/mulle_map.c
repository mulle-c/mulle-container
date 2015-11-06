//
//  mulle_map.h
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
#include "mulle_map.h"

#include "mulle_container_callback.h"

#include <string.h>


void   mulle_map_init_separated_callback( struct mulle_map *map,
                                          size_t capacity,
                                          struct mulle_container_keycallback *keycallback,
                                          struct mulle_container_valuecallback *valuecallback)
{
   if( keycallback)
      map->_callback.keycallback   = *keycallback;
   else
      memset( &map->_callback.keycallback, 0, sizeof( struct mulle_container_keycallback));
   
   if( valuecallback)
      map->_callback.valuecallback = *valuecallback;
   else
      memset( &map->_callback.valuecallback, 0, sizeof( struct mulle_container_valuecallback));

   mulle_container_keycallback_set_default_values( &map->_callback.keycallback);
   mulle_container_valuecallback_set_default_values( &map->_callback.valuecallback);

   _mulle_map_init( (struct _mulle_map *) map, capacity, &map->_callback);
}



struct mulle_map   *mulle_map_create_separated_callback( size_t capacity,
                                                         struct mulle_container_keycallback *keycallback,
                                                         struct mulle_container_valuecallback *valuecallback)
{
   struct mulle_map   *map;
   
   if( ! keycallback)
      keycallback = &mulle_container_keycallback_nonowned_pointer;
   
   map = (*keycallback->allocator->realloc)( NULL, sizeof( struct mulle_map));
   if( map)
      mulle_map_init_separated_callback( map, capacity, keycallback, valuecallback);
   return( map);
}


void   mulle_map_init( struct mulle_map *map,
                       size_t capacity,
                       struct mulle_container_keyvaluecallback *callback)
{
   mulle_map_init_separated_callback( map,
                                      capacity,
                                      callback ? &callback->keycallback : NULL,
                                      callback ? &callback->valuecallback : NULL);
}



struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback)
{
   return( mulle_map_create_separated_callback( capacity,
                                                callback ? &callback->keycallback   : NULL,
                                                callback ? &callback->valuecallback : NULL));
}


