//
//  mulle-container
//
//  Copyright (C) 2011-2025 Nat!, Mulle kybernetiK. All rights reserved.
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
#ifndef mulle__maptinyenumerator_h__
#define mulle__maptinyenumerator_h__

#include "mulle--map.h"


// this is a private interface for _MulleObjCDictionary
// - countByEnumeratingWithState:(NSFastEnumerationState *) rover
//                       objects:(id *) buffer
//                         count:

/*
 * a different and smaller interface, where you have to pass in space
 * to store the enumeration result each iteration and where notakey
 * must be NULL
 */

#define MULLE__MAPTINYENUMERATOR_BASE  \
   void     **_curr;                   \
   size_t   _left;                     \
   size_t   _offset


struct mulle__maptinyenumerator
{
   MULLE__MAPTINYENUMERATOR_BASE;
};


static inline struct mulle__maptinyenumerator
   _mulle__map_tinyenumerate_nil( struct mulle__map *map)
{
   struct mulle__maptinyenumerator   rover;

   assert( map);
   rover._left   = map->_count;
   rover._curr   = map->_storage;
   rover._offset = _mulle__map_get_size( map);

   return( rover);
}



static inline struct mulle__maptinyenumerator
   mulle__map_tinyenumerate_nil( struct mulle__map *map)
{
   if( map)
      return( _mulle__map_tinyenumerate_nil( map));

   return( (struct mulle__maptinyenumerator) { 0 }); // less sanitizer warnings
}


MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline int
   _mulle__maptinyenumerator_next( struct mulle__maptinyenumerator *rover,
                                   void **key,
                                   void **value)
{
   void   **p;

   if( ! rover->_left)
   {
      *key   = NULL;
      *value = NULL;
      return( 0);
   }

   rover->_left--;
   for(;;)
   {
      p = rover->_curr++;
      if( *p)
      {
         *key   = *p;
         *value = p[ rover->_offset];
         return( 1);
      }
   }
}



MULLE_C_NONNULL_FIRST
static inline void
   _mulle__maptinyenumerator_done( struct mulle__maptinyenumerator *rover)
{
}


static inline void
   mulle__maptinyenumerator_done( struct mulle__maptinyenumerator *rover)
{
}

#endif
