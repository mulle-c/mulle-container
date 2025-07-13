//
//  mulle-rangemap.c
//  mulle-container
//
//  Created by Nat! on 10.07.25.
//  Copyright (C) 2023 Nat!, Mulle kybernetiK.
//  All rights reserved.
//
#include "mulle-rangemap.h"

#include "include-private.h"

#ifdef __has_include
# if __has_include( "_mulle-container-versioncheck.h")
#  include "_mulle-container-versioncheck.h"
# endif
#endif

// Public API implementation
struct mulle_rangemap   *mulle_rangemap_create( size_t capacity,
                                                struct mulle_allocator *allocator)
{
   struct mulle_rangemap   *map;

   if( ! allocator)
      allocator = &mulle_default_allocator;

   map = mulle_allocator_malloc( allocator, sizeof( struct mulle_rangemap));
   mulle_rangemap_init( map, capacity, allocator);
   return( map);
}

void   mulle_rangemap_init( struct mulle_rangemap *map,
                           size_t capacity,
                           struct mulle_allocator *allocator)
{
   if( ! allocator)
      allocator = &mulle_default_allocator;

   _mulle__rangemap_init( &map->_base, capacity, allocator);
   map->allocator = allocator;
}

void   _mulle__rangemap_init( struct mulle__rangemap *map,
                             size_t capacity,
                             struct mulle_allocator *allocator)
{
   size_t   total_size;

   memset( map, 0, sizeof( *map));
   if( capacity)
   {
      total_size = capacity * (sizeof( struct mulle_range) + sizeof( void *));
      map->_storage = mulle_allocator_malloc( allocator, total_size);
      map->_size = capacity;
   }
}


void   _mulle__rangemap_done( struct mulle__rangemap *map,
                             struct mulle_allocator *allocator)
{
   if( map->_storage)
      mulle_allocator_free( allocator, map->_storage);
#ifdef DEBUG
   mulle_memset_uint32( map, 0xDEADDEAD, sizeof( struct mulle__rangemap));
#endif
}


static void   _mulle__rangemap_grow( struct mulle__rangemap *map,
                                    struct mulle_allocator *allocator)
{
   size_t   new_size;
   size_t   total_size;

   new_size = map->_size * 2;
   if( ! new_size)
      new_size = 2;

   total_size = new_size * (sizeof( struct mulle_range) + sizeof( void *));
   map->_storage = mulle_allocator_realloc( allocator, map->_storage, total_size);
   map->_size = new_size;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
}


void   __mulle__rangemap_insert( struct mulle__rangemap *map,
                                struct mulle_range range,
                                void *value,
                                void **merged,
                                struct mulle_allocator *allocator)
{
   struct mulle_range   *ranges;
   void                **values;
   struct mulle_range   *prev;
void                *prev_value;
   struct mulle_range   *next;
   void                *next_value;
   uintptr_t           index;

   if( ! range.length || ! mulle_range_is_valid( range))
      return;

   prev_value = NULL;  // Initialize to avoid warning

   ranges = _mulle__rangemap_get_ranges( map);
   values = _mulle__rangemap_get_values( map);
   
   index = _mulle_range_hole_bsearch( ranges, map->_length, range.location);

   if( map->_length)
   {
      if( index)
      {
         prev = &ranges[ index - 1];
         prev_value = values[ index - 1];
         
if( mulle_range_get_max( *prev) == range.location)
            {
               // Keep prev_value (leftmost), return merged value
               if( merged)
                  *merged = value;  // our value gets merged away
               prev->length += range.length;
               values[ index - 1] = prev_value;  // ensure leftmost value is kept
               return;
            }
      }

      if( index < map->_length)
      {
         next = &ranges[ index];
         next_value = values[ index];
         
         if( mulle_range_get_max( range) == next->location)
         {
            if( prev)
            {
               // Three-way merge: keep prev_value (leftmost)
               if( merged)
               {
                  *merged = value;  // first merged value
                  merged = next_value;  // second merged value
               }
               prev->length += next->length;
               memmove( next, next + 1,
                     (map->_length - (next - ranges) - 1) * sizeof( struct mulle_range));
               memmove( &values[ next - ranges], &values[ next - ranges + 1],
                     (map->_length - (next - ranges) - 1) * sizeof( void *));
               map->_length--;
            }
            else
            {
               // Two-way merge: keep new value (leftmost)
               if( merged)
                  *merged = next_value;
               next->location -= range.length;
               next->length += range.length;
               values[ index] = value;
            }
            return;
         }
      }
   }

   // Need more space?
   if( map->_length >= map->_size)
      _mulle__rangemap_grow( map, allocator);

   // Move everything up
   memmove( &ranges[ index + 1], 
           &ranges[ index], 
           (map->_length - index) * sizeof( struct mulle_range));
   memmove( &values[ index + 1],
           &values[ index],
           (map->_length - index) * sizeof( void *));

   // Insert new range and value
   ranges[ index] = range;
   values[ index] = value;
   map->_length++;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
}


void   __mulle__rangemap_remove( struct mulle__rangemap *map,
                                struct mulle_range range,
                                void **removed_values,
                                size_t *n_removed,
                                struct mulle_allocator *allocator)
{
   struct mulle_range   *ranges;
   void                **values;
   struct mulle_range   *found;
   struct mulle_range   newRange;
   uintptr_t           range_end;
   uintptr_t           found_end;
   size_t              removed_count = 0;

   if( ! range.length || ! mulle_range_is_valid( range))
      return;

   ranges = _mulle__rangemap_get_ranges( map);
   values = _mulle__rangemap_get_values( map);

   for(;;)
   {
      found = mulle_range_intersects_bsearch( ranges,
                                           (unsigned int) map->_length,
                                           range);
      if( ! found)
         break;

      // Complete overlap: remove found range
      if( mulle_range_contains( range, *found))
      {
         if( removed_values)
            removed_values[ removed_count] = values[ found - ranges];
         removed_count++;

         memmove( found, found + 1,
                (map->_length - (found - ranges) - 1) * sizeof( struct mulle_range));
         memmove( &values[ found - ranges], &values[ found - ranges + 1],
                (map->_length - (found - ranges) - 1) * sizeof( void *));
         map->_length--;
         continue;
      }

      found_end = mulle_range_get_max( *found);
      range_end = mulle_range_get_max( range);

      // Make a hole
      if( range.location > found->location && range_end < found_end)
      {
         // front part
         found->length = range.location - found->location;

         // end part
         newRange = mulle_range_make( range_end, found_end - range_end);
         if( map->_length >= map->_size)
            _mulle__rangemap_grow( map, allocator);

         memmove( found + 2, found + 1,
                (map->_length - (found - ranges) - 1) * sizeof( struct mulle_range));
         memmove( &values[ found - ranges + 2], &values[ found - ranges + 1],
                (map->_length - (found - ranges) - 1) * sizeof( void *));

         found[ 1] = newRange;
         values[ found - ranges + 1] = values[ found - ranges];
         map->_length++;
         continue;
      }

      // Remove front part
      if( range_end < found_end)
      {
         found->length = found_end - range_end;
         found->location = range_end;
         continue;
      }

      // Remove back part
      found->length = range.location - found->location;
   }

   if( n_removed)
      *n_removed = removed_count;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
}


struct mulle_range   _mulle__rangemap_search_exact( struct mulle__rangemap *map,
                                                   uintptr_t location,
                                                   void **value)
{
   struct mulle_range   *ranges;
   void                **values;
   struct mulle_range   *found;

   ranges = _mulle__rangemap_get_ranges( map);
   values = _mulle__rangemap_get_values( map);

   found = mulle_range_intersects_bsearch( ranges,
                                         (unsigned int) map->_length,
                                         mulle_range_make( location, 1));
   if( found)
   {
      if( value)
         *value = values[ found - ranges];
      return( *found);
   }
   return( mulle_range_make_invalid());
}


struct mulle_range   _mulle__rangemap_search_nearest( struct mulle__rangemap *map,
                                                     uintptr_t location,
                                                     void **value)
{
   struct mulle_range   *ranges;
   void                **values;
   struct mulle_range   *curr;
   struct mulle_range   *prev;
   uintptr_t           index;

   ranges = _mulle__rangemap_get_ranges( map);
   values = _mulle__rangemap_get_values( map);

   curr = mulle_range_intersects_bsearch( ranges,
                                       (unsigned int) map->_length,
                                       mulle_range_make( location, 1));
   if( ! curr)
   {
      if( ! map->_length)
         return( mulle_range_make_invalid());

      // get the best fitting hole index
      index = _mulle_range_hole_bsearch( ranges,
                                       (unsigned int) map->_length,
                                       location);
      index = index == map->_length ? index - 1 : index;
      curr = &ranges[ index];
      if( index)
      {
         prev = &ranges[ index - 1];
         if( mulle_range_distance_to_location( *curr, location) >
             mulle_range_distance_to_location( *prev, location))
         {
            curr = prev;
         }
      }
   }

   if( curr)
   {
      if( value)
         *value = values[ curr - ranges];
      return( *curr);
   }
   return( mulle_range_make_invalid());
}


void   _mulle__rangemap_reset( struct mulle__rangemap *map,
                              struct mulle_allocator *allocator)
{
   map->_length = 0;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
}


void   _mulle__rangemap_print( struct mulle__rangemap *map,
                              FILE *fp)
{
   struct mulle_range   *ranges;
   void                **values;
   size_t              i;

   ranges = _mulle__rangemap_get_ranges( map);
   values = _mulle__rangemap_get_values( map);

   for( i = 0; i < map->_length; i++)
   {
      fprintf( fp, "#%lu: %lu,%lu -> %p\n",
              (unsigned long) i,
              (unsigned long) ranges[ i].location,
              (unsigned long) ranges[ i].length,
              values[ i]);
   }
}
