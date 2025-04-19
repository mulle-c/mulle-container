//  _mulle_assoc.c
//  mulle-container
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

#include "mulle--assoc.h"

#include "include-private.h"

#include "mulle-container-operation.h"
#include "mulle-container-callback.h"
#include "mulle-container-math.h"

#include <stdio.h>  // debug


#pragma mark - _mulle_assocrange internal helper

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__assoc_release( struct mulle__assoc *assoc,
                          struct mulle_range range,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   if( ! callback || ! mulle_container_keyvaluecallback_releases( callback))
      return;

   p        = &assoc->_storage[ range.location];
   sentinel = &p[ range.length];
   while( p < sentinel)
      mulle_pointerpair_release( *p++, callback, allocator);
}


#pragma mark - creation and destruction


void   _mulle__assoc_done( struct mulle__assoc *assoc,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   _mulle__assoc_release( assoc, mulle_range_make( 0, _mulle__assoc_get_count( assoc)), callback, allocator);
   _mulle__pointerpairarray_done( (struct mulle__pointerpairarray *) assoc, allocator);
}



void   _mulle__assoc_destroy( struct mulle__assoc *assoc,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator)
{
   _mulle__assoc_done( assoc, callback, allocator);
   mulle_allocator_free( allocator, assoc);
}



#pragma mark - removal

void   _mulle__assoc_reset( struct mulle__assoc *assoc,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle__assoc_release( assoc, mulle_range_make( 0, _mulle__assoc_get_count( assoc)), callback, allocator);

   mulle__pointerpairarray_reset( (struct mulle__pointerpairarray *) assoc);

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) assoc)->_n_mutations++;
#endif
}

void   _mulle__assoc_remove_in_range( struct mulle__assoc *assoc,
                                      struct mulle_range range,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
{
   range = mulle_range_validate_against_length( range, _mulle__assoc_get_count( assoc));
   _mulle__assoc_release( assoc, range, callback, allocator);

   _mulle__pointerpairarray_remove_in_range( (struct mulle__pointerpairarray *) assoc,
                                             range);
   assoc->_is_sorted = 0;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) assoc)->_n_mutations++;
#endif
}

void   _mulle__assoc_remove( struct mulle__assoc *assoc,
                             void *key,
                             struct mulle_container_keyvaluecallback *callback,
                             mulle_pointerpair_compare_t *compare,
                             void *userinfo,
                             struct mulle_allocator *allocator)
{
   size_t               i;
   struct mulle_pointerpair   item;

   //
   // Removal back to front is cool, as long as we are the only one
   // modifying the assoc. We don't use an enumerator here, but an index is
   // safe...
   //
   if( _mulle_container_keycallback_isbitequals( &callback->keycallback))
   {
      for( i = mulle__assoc_get_count( assoc); i;)
      {
         item = mulle__assoc_get_at_index( assoc, --i, compare, userinfo);
         if( key == item.key)
            _mulle__assoc_remove_in_range( assoc,
                                           mulle_range_make( i, 1),
                                           callback,
                                           allocator);
      }
   }
   else
   {
      for( i = mulle__assoc_get_count( assoc); i;)
      {
         item = mulle__assoc_get_at_index( assoc, --i, compare, userinfo);
         if( (callback->keycallback.is_equal)( &callback->keycallback, key, item.key))
            _mulle__assoc_remove_in_range( assoc,
                                           mulle_range_make( i, 1),
                                           callback,
                                           allocator);
      }
   }
}


int    _mulle__assoc_is_equal( struct mulle__assoc *assoc,
                               struct mulle__assoc *other,
                               struct mulle_container_keyvaluecallback *callback,
                               mulle_pointerpair_compare_t *compare,
                               void *userinfo)
{
   size_t               i, n;
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *q;

   n = _mulle__assoc_get_count( assoc);
   if( n != _mulle__assoc_get_count( other))
      return( 0);

   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
   _mulle__assoc_qsort_r_if_needed( other, compare, userinfo);

   if( _mulle_container_keycallback_isbitequals( &callback->keycallback))
      return( ! memcmp( assoc->_storage, other->_storage, n * sizeof( struct mulle_pointerpair)));

   p = assoc->_storage;
   q = other->_storage;
   for( i = 0; i < n; i++)
   {
      if( p->value != q->value)
         return( 0);
      if( ! (callback->keycallback.is_equal)( &callback->keycallback, p->key, q->key))
         return( 0);
   }

   return( 1);
}


void    _mulle__assoc_add( struct mulle__assoc *assoc,
                           void *key,
                           void *value,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   struct mulle_pointerpair  pair;
   struct mulle_pointerpair  retained;

   assert( ! _mulle__assoc_member( assoc, key, callback));

   pair     = mulle_pointerpair_make( key, value);
   retained = mulle_pointerpair_retain( pair, callback, allocator);
   _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *) assoc,
                                  retained,
                                  allocator);

   assoc->_is_sorted = 0;
}


// the index is in the "sorted" order, even though the array hasn't been sorted. Solution ? got to sort it on
// demand ..
void    _mulle__assoc_set_at_index( struct mulle__assoc *assoc,
                                    size_t i,
                                    void *key,
                                    void *value,
                                    struct mulle_container_keyvaluecallback *callback,
                                    mulle_pointerpair_compare_t *compare,
                                    void *userinfo,
                                    struct mulle_allocator *allocator)
{
   struct mulle_pointerpair  retained;
   struct mulle_pointerpair  pair;
   struct mulle_pointerpair  old;

   assert( key != callback->keycallback.notakey);

   // need to sort to have proper index to overwrite
   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);

   pair     = mulle_pointerpair_make( key, value);
   retained = mulle_pointerpair_retain( pair, callback, allocator);
   old      = _mulle__pointerpairarray_set( (struct mulle__pointerpairarray *)  assoc,
                                            i,
                                            retained);
   mulle_pointerpair_release( old, callback, allocator);

   assoc->_is_sorted = 0;
}


void
   _mulle__assoc_add_assoc_range( struct mulle__assoc *assoc,
                                  struct mulle__assoc *other,
                                  struct mulle_range range,
                                  struct mulle_container_keyvaluecallback *callback,
                                  struct mulle_allocator *allocator)
{
   size_t                     count;
   struct mulle_pointerpair   *q;
   struct mulle_pointerpair   *sentinel;
   uintptr_t                  i;
   struct mulle_range         all;

   count = _mulle__assoc_get_count( other);
   range = mulle_range_validate_against_length( range, count);
   if( ! range.length)
      return;

   all      = mulle_range_make( 0, _mulle__assoc_get_count( assoc));
   q        = &other->_storage[ range.location];
   sentinel = &q[ range.length];
   while( q < sentinel)
   {
      i = _mulle_pointerpair_find_in_range_callback( assoc->_storage,
                                                     *q,
                                                     all,
                                                     callback);      
      if( i == mulle_not_found_e)
      {
         _mulle__assoc_add( assoc, q->key, q->value, callback, allocator);
         all.length++;
      }
      ++q;
   }

   assoc->_is_sorted = 0;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) assoc)->_n_mutations++;
#endif
}


void _mulle__assoc_copy_items( struct mulle__assoc *dst,
                               struct mulle__assoc *src,
                               struct mulle_container_keyvaluecallback *callback,
                               struct mulle_allocator *allocator)
{
   size_t                    n;
   struct mulle_pointerpair  *p;
   struct mulle_pointerpair  *sentinel;

   n = mulle__assoc_get_count( src);
   if( ! n)
      return;

   assert( dst != src);

   _mulle__pointerpairarray_guarantee( (struct mulle__pointerpairarray *) dst, n, allocator);

   p        = dst->_curr;
   sentinel = dst->_curr + n;

   memcpy( p, src->_storage, n * sizeof( struct mulle_pointerpair));

   if( mulle_container_keyvaluecallback_retains( callback))
   {  
      while( p < sentinel)
      {
         *p = mulle_pointerpair_retain( *p, callback, allocator);
         ++p;
      }
   }

   dst->_curr      = sentinel;
   dst->_is_sorted = 0;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) dst)->_n_mutations++;
#endif
}

// AI Content

static uintptr_t
   _mulle__assoc_find_in_range_unsorted( struct mulle__assoc *assoc,
                                         void *key,
                                         struct mulle_range range,
                                         struct mulle_container_keyvaluecallback *callback)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   assert( key != mulle_pointerpair_notakey);

   range    = mulle_range_validate_against_length( range, _mulle__assoc_get_count(assoc));
   if( ! range.length)
      return( mulle_not_found_e);

   p        = &assoc->_storage[ range.location];
   sentinel = &p[ range.length];

   // NULL check needed for remap
   if( ! callback || _mulle_container_keycallback_isbitequals( &callback->keycallback))
   {
      for( ; p < sentinel; p++)
         if( p->key == key)
            return( p - assoc->_storage);
   }
   else
   {
      for( ; p < sentinel; p++)
         if( (*callback->keycallback.is_equal)( &callback->keycallback, p->key, key))
            return( p - assoc->_storage);
   }
   return( mulle_not_found_e);
}


uintptr_t
    _mulle__assoc_find_in_range( struct mulle__assoc *assoc,
                                 void *key,
                                 struct mulle_range range,
                                 struct mulle_container_keyvaluecallback *callback,
                                 mulle_pointerpair_compare_t *compare,
                                 void *userinfo)
{
   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);
   return( _mulle__assoc_find_in_range_unsorted( assoc, key, range, callback));
}

int   _mulle__assoc_member( struct mulle__assoc *assoc,
                           void *key,
                           struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t   result;

   result = _mulle__assoc_find_in_range_unsorted( assoc, key, mulle_range_make_all(), callback);
   return( result != mulle_not_found_e);
}


void   *_mulle__assoc_get( struct mulle__assoc *assoc,
                           void *key,
                           mulle_pointerpair_compare_t *compare,
                           void *userinfo)
{
   int                         index;
   struct mulle_pointerpair   search;

   _mulle__assoc_qsort_r_if_needed( assoc, compare, userinfo);

   search = mulle_pointerpair_make(key, NULL);
   index = _mulle_pointerpair_bsearch( assoc->_storage,
                                       assoc->_curr - assoc->_storage,
                                       search,
                                       compare,
                                       userinfo);
   if (index == -1)
      return( NULL);

   return assoc->_storage[index].value;
}


void   _mulle__assoc_set( struct mulle__assoc *assoc,
                          void *key,
                          void *value,
                          struct mulle_container_keyvaluecallback *callback,
                          mulle_pointerpair_compare_t *compare,
                          void *userinfo,
                          struct mulle_allocator *allocator)
{
   int                        index;
   struct mulle_pointerpair   pair;
   struct mulle_pointerpair   old;

   mulle__assoc_qsort_if_needed(assoc, compare, userinfo);

   pair  = mulle_pointerpair_make(key, value);
   index = _mulle_pointerpair_bsearch(assoc->_storage,
                                     assoc->_curr - assoc->_storage,
                                     pair,
                                     compare,
                                     userinfo);
   if (index == -1)
   {
      _mulle__assoc_add( assoc,
                         key,
                         value,
                         callback,
                         allocator);
   }
   else
   {
      old                     = assoc->_storage[index];
      assoc->_storage[index]  = mulle_pointerpair_retain( pair,
                                                          callback,
                                                          allocator);
      mulle_pointerpair_release( old, callback, allocator);
   }

   assoc->_is_sorted = 0;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) assoc)->_n_mutations++;
#endif
}


void   _mulle__assoc_remap_intptr_key_range( struct mulle__assoc *assoc,
                                             struct mulle_range range,
                                             intptr_t offset,
                                             struct mulle_container_keyvaluecallback *callback,
                                             mulle_pointerpair_compare_t *compare,
                                             void *userinfo)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;
   uintptr_t                  start;
   uintptr_t                  end;
   intptr_t                   value;

   assert( callback->keycallback.is_equal == mulle_container_keycallback_intptr_is_equal);

   range = mulle_range_validate_against_length(range, _mulle__assoc_get_count(assoc));
   if( ! range.length)
      return;

   // need stable start end points, so sort if unsorted
   mulle__assoc_qsort_if_needed(assoc, compare, userinfo);

   start = mulle__assoc_find(assoc,
                            (void *) range.location,
                            callback,
                            compare,
                            userinfo);
   if (start == mulle_not_found_e)
      return;

   end      = mulle_range_get_max(range);
   p        = &assoc->_storage[start];
   sentinel = assoc->_curr;

   for (; p < sentinel; p++)
   {
      value = (intptr_t) p->key;
      if ((uintptr_t) value >= end)
         break;

      value += offset;
      p->key = (void *) value;
   }

   assoc->_is_sorted = 0;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) assoc)->_n_mutations++;
#endif
}

void   _mulle__assoc_move_intptr_key_range( struct mulle__assoc *assoc,
                                            struct mulle_range range,
                                            intptr_t index,
                                            struct mulle_container_keyvaluecallback *callback,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo)
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

   assert(callback->keycallback.is_equal == mulle_container_keycallback_intptr_is_equal);

   range = mulle_range_validate_against_length( range, _mulle__assoc_get_count(assoc));
   if( ! range.length)
      return;

   if( mulle_range_contains_location( range, index))
      return;

   // need stable start end points, so sort if unsorted
   _mulle__assoc_qsort_r_if_needed(assoc, compare, userinfo);

   offset = index - (intptr_t) range.location;

   if (offset < 0)
   {
      //  |......................|
      //  |...........|rrrrrr|...| R=rrrrrr
      //  |..I........|rrrrrr|...|
      //  |   mmmmmmmm
      //  m += R.length
      //  r  = I.location + (r.location - R.location)

      move_away_range  = mulle_range_make(index, range.location - index);
      move_away_offset = (intptr_t) range.length;
      start = mulle__assoc_find(assoc,
                               (void *) index,
                               callback,
                               compare,
                               userinfo);
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

      range_end        = mulle_range_get_max(range);
      // below index, we move up, rest moves down
      move_away_range  = mulle_range_make(range_end,
                                         (intptr_t) index + 1 - range_end);
      move_away_offset = -(intptr_t) range.length;

      start = mulle__assoc_find(assoc,
                               (void *) range.location,
                               callback,
                               compare,
                               userinfo);
      end   = index + 1;
   }

   if (start == mulle_not_found_e)
      return;

   p        = &assoc->_storage[start];
   sentinel = assoc->_curr;

   for (; p < sentinel; p++)
   {
      value = (intptr_t) p->key;
      if ((uintptr_t) value > end)
         break;

      if (mulle_range_contains_location(move_away_range, value))
         value += move_away_offset;
      else
         value += offset;

      p->key = (void *) value;
   }

   assoc->_is_sorted = 0;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   ((struct mulle__pointerpairarray *) assoc)->_n_mutations++;
#endif
}



// use this only for debugging
char   *_mulle__assoc_describe( struct mulle__assoc *assoc,
                                struct mulle_container_keyvaluecallback *callback,
                                mulle_pointerpair_compare_t *compare,
                                void *userinfo,
                                struct mulle_allocator *allocator)
{
   char                            *result;
   char                            *key;
   char                            *value;
   int                             separate;
   size_t                          len;
   size_t                          key_len;
   size_t                          value_len;
   struct mulle__assocenumerator   rover;
   struct mulle_pointerpair        item;
   struct mulle_allocator          *key_allocator;
   struct mulle_allocator          *value_allocator;

   result = NULL;
   len    = 0;
   rover = mulle__assoc_enumerate( assoc, callback, compare, userinfo);
   while( _mulle__assocenumerator_next( &rover, &item.key, &item.value))
   {
      key_allocator   = allocator ? allocator : &mulle_default_allocator;
      value_allocator = key_allocator;

      key       = (*callback->keycallback.describe)( &callback->keycallback,
                                                     item.key,
                                                     &key_allocator);
      key_len   = strlen( key);

      value     = (*callback->valuecallback.describe)( &callback->valuecallback,
                                                       item.value,
                                                       &value_allocator);
      value_len = strlen( value);

      separate  = result != NULL;
      result    = mulle_allocator_realloc( allocator,
                                           result,
                                           len + (separate * 2) + 2 + key_len + 1 \
                                            + 2 + value_len + 2 + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len   += 2;
      }

      memcpy( &result[ len], "{ ", 2);
      len   += 2;

      memcpy( &result[ len], key, key_len);
      len += key_len;

      memcpy( &result[ len], ", ", 2);
      len   += 2;

      memcpy( &result[ len], value, value_len);
      len += value_len;

      memcpy( &result[ len], " }", 2);
      len   += 2;

      if( value_allocator)
         mulle_allocator_free( value_allocator, value);

      if( key_allocator)
         mulle_allocator_free( key_allocator, key);
   }
   mulle__assocenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}
