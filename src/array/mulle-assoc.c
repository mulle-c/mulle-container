//
//  mulle_assoc.c
//  mulle-container
//
//  Created by Nat! on 04/11/15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
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

#include "mulle-assoc.h"


struct mulle_assoc   *mulle_assoc_create( unsigned int capacity,
                                          struct mulle_container_keyvaluecallback *callback,
                                          mulle_pointerpair_compare_t *compare,
                                          struct mulle_allocator *allocator)
{
   struct mulle_assoc  *buffer;

   buffer = mulle_allocator_malloc( allocator, sizeof( struct mulle_assoc));
   mulle_assoc_init( buffer, capacity, callback, compare, allocator);
   return( buffer);
}


void   *mulle_assoc_get( struct mulle_assoc *assoc, void *key)
{
   int                        index;
   struct mulle_pointerpair   search;

   if( ! assoc)
      return( NULL);

   mulle_assoc_qsort_if_needed( assoc);

   search = mulle_pointerpair_make( key, NULL);
   index  = _mulle_pointerpair_bsearch( assoc->_storage,
                                        assoc->_curr - assoc->_storage,
                                        search,
                                        assoc->compare,
                                        assoc);
   if( index == -1)
      return( NULL);

   return( assoc->_storage[ index].value);
}


void   _mulle_assoc_set( struct mulle_assoc *assoc, void *key, void *value)
{
   int                        index;
   struct mulle_pointerpair   pair;
   struct mulle_pointerpair   old;

   mulle_assoc_qsort_if_needed( assoc);

   pair  = mulle_pointerpair_make( key, value);
   index = _mulle_pointerpair_bsearch( assoc->_storage,
                                       assoc->_curr - assoc->_storage,
                                       pair,
                                       assoc->compare,
                                       assoc);
   if( index == -1)
   {
      mulle__assoc_add( (struct mulle__assoc *) assoc,
                        key,
                        value,
                        assoc->callback,
                        assoc->allocator);
      return;
   }

   old                     = assoc->_storage[ index];
   assoc->_storage[ index] = mulle_pointerpair_retain( pair, assoc->callback, assoc->allocator);
   mulle_pointerpair_release( old, assoc->callback, assoc->allocator);
}
