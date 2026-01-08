//
//  mulle-rangemap.c
//  mulle-container
//
//  Created by Nat! on 10.07.25.
//  Copyright (C) 2023 Nat!, Mulle kybernetiK.
//  All rights reserved.
//
#include "mulle--rangemap.h"

#include "include-private.h"

#include <errno.h>


void   _mulle__rangemap_init( struct mulle__rangemap *map,
                             size_t capacity,
                             struct mulle_allocator *allocator)
{
   size_t   total_size;

   memset( map, 0, sizeof( *map));
   if( capacity)
   {
      total_size    = capacity * (sizeof( struct mulle_range) + sizeof( void *));
      map->_storage = mulle_allocator_malloc( allocator, total_size);
      map->_size    = capacity;
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
   size_t               old_size;
   size_t               new_size;
   size_t               total_size;
   struct mulle_range   *old_ranges;
   void                 **old_values;
   void                 **new_values;

   old_size = map->_size;
   new_size = old_size * 2;
   if( ! new_size)
      new_size = 2;

   total_size = new_size * (sizeof(struct mulle_range) + sizeof(void *));

   // Reallocate the combined buffer
   map->_storage = mulle_allocator_realloc( allocator, map->_storage, total_size);

   // Move the old values to their new location after expanded ranges
   old_ranges = (struct mulle_range *) map->_storage;
   old_values = (void **) &old_ranges[ old_size];
   new_values = (void **) &old_ranges[ new_size];

   memmove( new_values, old_values, map->_length * sizeof( void *));
   map->_size = new_size;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
}


/*
 *
 */
int   _mulle__rangemap_insert( struct mulle__rangemap *map,
                               struct mulle_range range,
                               void *value,
                               struct mulle_allocator *allocator)
{
   struct mulle_range   *ranges;
   struct mulle_range   *found;
   void                 **values;
   uintptr_t            index;

   if( ! range.length || ! mulle_range_is_valid( range))
      return( EINVAL);

   assert( value);

   ranges = _mulle__rangemap_get_ranges( map);
   found  = mulle_range_intersects_bsearch( ranges,
                                            (unsigned int) map->_length,
                                            range);
   if( found)
      return( EADDRINUSE);

   if( map->_length >= map->_size)
   {
      _mulle__rangemap_grow( map, allocator);
      // reread ranges
      ranges = _mulle__rangemap_get_ranges( map);
   }

   index  = _mulle_range_hole_bsearch( ranges, (unsigned int) map->_length, range.location);

   // Move everything up to make space for the new range
   memmove( &ranges[ index + 1],
            &ranges[ index],
            (map->_length - index) * sizeof( struct mulle_range));

   values = _mulle__rangemap_get_values( map);
   memmove( &values[ index + 1],
            &values[ index],
            (size_t) (map->_length - index) * sizeof( void *));

   // Insert new range and value
   ranges[ index] = range;
   values[ index] = value;

   map->_length++;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
   return( 0);
}


int   _mulle__rangemap_remove( struct mulle__rangemap *map,
                               struct mulle_range range,
                               struct mulle_allocator *allocator)
{
   struct mulle_range   *ranges;
   struct mulle_range   *found;
   void                 **values;
   size_t               index;
   size_t               n;

   // TODO: yikes ... should change API but dont wanna
   MULLE_C_UNUSED( allocator);

   if( ! range.length || ! mulle_range_is_valid( range))
      return( EINVAL);

   ranges = _mulle__rangemap_get_ranges( map);
   found  = mulle_range_intersects_bsearch( ranges,
                                            (unsigned int) map->_length,
                                            range);
   if( ! found)
      return( ENOENT);

   if( ! mulle_range_equals( *found, range))
      return( EACCES);

   values = _mulle__rangemap_get_values( map);

   index = found - ranges;
   n     = --map->_length;
   if( n)
   {
      memmove( &ranges[ index], &ranges[ index + 1],
               (size_t) (map->_length - index) * sizeof( struct mulle_range));
      memmove( &values[ index], &values[ index + 1],
               (size_t) (map->_length - index) * sizeof( void *));
   }

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif
   return( 0);
}


uintptr_t
   _mulle__rangemap_get_ranges_values( struct mulle__rangemap *map,
                                       struct mulle_range range,
                                       uintptr_t max_out,
                                       struct mulle_range *out_ranges,
                                       void **out_values)
{
   struct mulle_range   *ranges;
   void                 **values;
   struct mulle_range   *curr;
   uintptr_t            found_count;
   uintptr_t            i;

   if( ! range.length || ! mulle_range_is_valid( range))
      return( 0);

   ranges      = _mulle__rangemap_get_ranges(map);
   values      = _mulle__rangemap_get_values(map);

   // Find insertion point for range.location
   curr        = mulle_range_intersects_bsearch( ranges,
                                                 (unsigned int) map->_length,
                                                 mulle_range_make( range.location, 1));
   if( ! curr)
      return( 0);

   found_count = 0;
   // Scan forward intersecting ranges
   for( i = (uintptr_t) (curr - ranges); i < map->_length; i++)
   {
      if( ! mulle_range_intersects( ranges[ i], range))
         break;

      if( found_count < max_out)
      {
         if (out_ranges)
            out_ranges[ found_count] = ranges[ i];
         if (out_values)
            out_values[ found_count] = values[ i];
      }

      found_count++;
   }

   return( found_count);
}


uintptr_t   _mulle__rangemap_search( struct mulle__rangemap *map,
                                     uintptr_t location)
{
   struct mulle_range   *ranges;
   struct mulle_range   *curr;

   ranges = _mulle__rangemap_get_ranges(map);
   curr   = mulle_range_intersects_bsearch( ranges,
                                            (unsigned int) map->_length,
                                            mulle_range_make( location, 1));
   if( ! curr)
      return( mulle_not_found_e);

   // Scan forward intersecting ranges
   return( (uintptr_t) (curr - ranges));
}


void   *_mulle__rangemap_get_exact( struct mulle__rangemap *map,
                                    struct mulle_range range)
{
   struct mulle_range   *ranges;
   void                 **values;
   struct mulle_range   *curr;
   uintptr_t            i;

   ranges      = _mulle__rangemap_get_ranges(map);
   values      = _mulle__rangemap_get_values(map);

   // Find insertion point for range.location
   curr        = mulle_range_intersects_bsearch( ranges,
                                                 (unsigned int) map->_length,
                                                 mulle_range_make( range.location, 1));
   if( ! curr)
   {
      errno = ENOENT;
      return( NULL);
   }
   if( ! mulle_range_equals( *curr, range))
   {
      errno = EINVAL;
      return( NULL);
   }

   i = (uintptr_t) (curr - ranges);
   return( values[ i]);
}


void   _mulle__rangemap_reset( struct mulle__rangemap *map,
                               struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( allocator);

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
