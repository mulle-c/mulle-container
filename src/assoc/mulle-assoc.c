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
   assoc->_storage[ index] = mulle_pointerpair_retain( pair,
                                                       assoc->callback,
                                                       assoc->allocator);
   mulle_pointerpair_release( old, assoc->callback, assoc->allocator);
}


void   _mulle_assoc_remap_intptr_key_range( struct mulle_assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t offset)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;
   uintptr_t                  start;
   uintptr_t                  end;
   intptr_t                   value;

   assert( assoc->callback->keycallback.is_equal == mulle_container_keycallback_intptr_is_equal);

   // shabby hack to determin if these are integer callbacks
   if( ! range.length)
      return;

   // need stable start end points, so sort if unsorted
   mulle_assoc_qsort_if_needed( assoc);

   start = mulle__assoc_find( (struct mulle__assoc *) assoc,
                              (void *) range.location,
                              assoc->callback);
   if( start == mulle_not_found_e)
      return;

   end      = mulle_range_get_max( range);
   p        = &assoc->_storage[ start];
   sentinel = assoc->_curr;

   for( ; p < sentinel; p++)
   {
      value  = (intptr_t) p->key;
      if( value >= end)
         break;

      value += offset;
      p->key = (void *) value;
   }

   assoc->_is_sorted = 0;
}


void   _mulle_assoc_move_intptr_key_range( struct mulle_assoc *assoc,
                                           struct mulle_range range,
                                           intptr_t index)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;
   struct mulle_range         move_away_range;
   uintptr_t                  start;
   uintptr_t                  end;
   uintptr_t                  range_end;
   intptr_t                   value;
   intptr_t                   move_away_offset;
   intptr_t                   offset;

   assert( assoc->callback->keycallback.is_equal == mulle_container_keycallback_intptr_is_equal);

   if( mulle_range_contains_location( range, index))
      return;
   if( ! range.length)
      return;

   // need stable start end points, so sort if unsorted
   mulle_assoc_qsort_if_needed( assoc);

   offset = index - (intptr_t) range.location;

   if( offset < 0)
   {
      //  |......................|
      //  |...........|rrrrrr|...| R=rrrrrr
      //  |..I........|rrrrrr|...|
      //  |   mmmmmmmm
      //  m += R.length
      //  r  = I.location + (r.location - R.location)

      move_away_range  = mulle_range_make( index, range.location - index);
      move_away_offset = (intptr_t) range.length;
//      mulle_assoc_remap_intptr_key_range( &_viewsByRows,
//                                          moveUpRange,
//                                          (intptr_t) range.length);
      start = mulle__assoc_find( (struct mulle__assoc *) assoc,
                                 (void *) index,
                                 assoc->callback);
      end   = range.location + range.length - 1;

   }
   else
   {
      //  |......................|
      //  |...|rrrrrr|...........| R=rrrrrr
      //  |...|rrrrrr|...I.......|
      //  |           mmmm
      //  m -= R.length
      //  r  = I.location + (r.location - R.location)

      range_end        = mulle_range_get_max( range);
      // below index, we move up, rest moves down
      move_away_range  = mulle_range_make( range_end,
                                           (intptr_t) index + 1 - range_end);
      move_away_offset = - (intptr_t) range.length;

      start = mulle__assoc_find( (struct mulle__assoc *) assoc,
                                 (void *) range.location,
                                 assoc->callback);
      end   = index + 1;
   }

   if( start == mulle_not_found_e)
      return;

   // fprintf( stderr, "range<%td-%td> index=%td | start=%u end=%u | offset=%td | range=<%td-%td> offset=%td\n",
   //                     range.location, range.length, index,
   //                     start, end, offset, move_away_range.location, move_away_range.length, move_away_offset);

   p        = &assoc->_storage[ start];
   sentinel = assoc->_curr;

   for( ; p < sentinel; p++)
   {
      value  = (intptr_t) p->key;
      if( value > end)
         break;

      if( mulle_range_contains_location( move_away_range, value))
         value += move_away_offset;
      else
         value += offset;

      p->key = (void *) value;
   }

   assoc->_is_sorted = 0;
}
