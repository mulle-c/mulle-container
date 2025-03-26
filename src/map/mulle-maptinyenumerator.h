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
#ifndef mulle_maptinyenumerator_h__
#define mulle_maptinyenumerator_h__

#include "mulle--maptinyenumerator.h"


// this is a private interface for _MulleObjCDictionary
// - countByEnumeratingWithState:(NSFastEnumerationState *) rover
//                       objects:(id *) buffer
//                         count:
//
// this does mutation checks on its own and only checks for nil,
// so this enumerator can be slightly more simplistic.
//
#pragma mark - tiny enumeration


#define MULLE_MAPTINYENUMERATOR_BASE    MULLE__MAPTINYENUMERATOR_BASE


struct mulle_maptinyenumerator
{
   MULLE_MAPTINYENUMERATOR_BASE;
};



static inline struct mulle_maptinyenumerator
   mulle_map_tinyenumerate_nil( struct mulle_map *map)
{
   struct mulle_maptinyenumerator    rover;
   struct mulle__maptinyenumerator   tmp;

   tmp = mulle__map_tinyenumerate_nil( (struct mulle__map *) map);
   memcpy( &rover, &tmp, sizeof( struct mulle__maptinyenumerator));
   return( rover);
}


static inline int
   mulle_maptinyenumerator_next( struct mulle_maptinyenumerator *rover,
                                 void **key,
                                 void **value)
{
   if( ! rover)
      return( 0);
   return( _mulle__maptinyenumerator_next( (struct mulle__maptinyenumerator *) rover,
                                           key,
                                           value));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_maptinyenumerator_done( struct mulle_maptinyenumerator *rover)
{
   _mulle__maptinyenumerator_done( (struct mulle__maptinyenumerator *) rover);
}


static inline void
   mulle_maptinyenumerator_done( struct mulle_maptinyenumerator *rover)
{
   mulle__maptinyenumerator_done( (struct mulle__maptinyenumerator *) rover);
}

#endif
