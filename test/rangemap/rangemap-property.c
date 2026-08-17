#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


#define DOMAIN_SIZE  128
#define STEP_COUNT   2500
#define RANGE_COUNT  128


struct expected_range
{
   struct mulle_range   range;
   void                 *value;
};


static uint32_t   next_random( uint32_t *state)
{
   *state = *state * UINT32_C( 1664525) + UINT32_C( 1013904223);
   return( *state);
}


static int   ranges_overlap( struct mulle_range a,
                             struct mulle_range b)
{
   return( a.location < b.location + b.length &&
           b.location < a.location + a.length);
}


static size_t   find_range( struct expected_range *expected,
                            size_t count,
                            struct mulle_range range)
{
   size_t   i;

   for( i = 0; i < count; i++)
      if( mulle_range_equals( expected[ i].range, range))
         return( i);
   return( count);
}


static void   check_rangemap( struct mulle__rangemap *map,
                              struct expected_range *expected,
                              size_t count)
{
   struct mulle__rangemapenumerator   rover;
   struct mulle_range                 range;
   struct mulle_range                 previous;
   struct mulle_range                 out_ranges[ 2];
   void                              *value;
   void                              *out_values[ 2];
   unsigned char                      seen[ RANGE_COUNT];
   uintptr_t                          found;
   uintptr_t                          i;
   uintptr_t                          location;
   size_t                             index;
   int                                have_previous;

   assert( _mulle__rangemap_get_count( map) == count);
   memset( seen, 0, sizeof( seen));
   have_previous = 0;
   rover = _mulle__rangemap_enumerate( map);
   while( _mulle__rangemapenumerator_next( &rover, &range, &value))
   {
      if( have_previous)
         assert( mulle_range_get_max( previous) <= range.location);
      previous = range;
      have_previous = 1;
      index = find_range( expected, count, range);
      assert( index < count);
      assert( ! seen[ index]);
      assert( value == expected[ index].value);
      seen[ index] = 1;
   }
   for( i = 0; i < count; i++)
      assert( seen[ i]);

   found = _mulle__rangemap_get_ranges_values( map,
                                               mulle_range_make( 0, DOMAIN_SIZE),
                                               0,
                                               NULL,
                                               NULL);
   assert( found == count);
   found = _mulle__rangemap_get_ranges_values( map,
                                               mulle_range_make( 0, DOMAIN_SIZE),
                                               2,
                                               out_ranges,
                                               out_values);
   assert( found == count);
   for( i = 0; i < count && i < 2; i++)
   {
      index = find_range( expected, count, out_ranges[ i]);
      assert( index < count);
      assert( out_values[ i] == expected[ index].value);
   }

   for( location = 0; location < DOMAIN_SIZE; location++)
   {
      index = count;
      for( i = 0; i < count; i++)
         if( mulle_range_contains_location( expected[ i].range, location))
         {
            index = i;
            break;
         }
      found = _mulle__rangemap_search( map, location);
      if( index == count)
      {
         assert( found == mulle_not_found_e);
         continue;
      }
      assert( found != mulle_not_found_e);
      range = _mulle__rangemap_get_range( map, found);
      value = _mulle__rangemap_get_value( map, found);
      assert( mulle_range_equals( range, expected[ index].range));
      assert( value == expected[ index].value);
   }
}


static void   test_randomized_ranges( void)
{
   struct mulle__rangemap   map;
   struct expected_range     expected[ RANGE_COUNT];
   struct mulle_range        range;
   uint32_t                  state;
   uintptr_t                 location;
   uintptr_t                 length;
   size_t                    count;
   size_t                    i;
   size_t                    index;
   size_t                    step;
   int                       overlap;
   int                       result;
   void                     *value;

   count = 0;
   _mulle__rangemap_init( &map, 1, NULL);
   state = UINT32_C( 0x3c6ef372);

   for( step = 0; step < STEP_COUNT; step++)
   {
      if( count && (next_random( &state) & 1))
      {
         index  = next_random( &state) % count;
         range  = expected[ index].range;
         result = _mulle__rangemap_remove( &map, range, NULL);
         assert( result == 0);
         expected[ index] = expected[ --count];
      }
      else
      {
         location = next_random( &state) % DOMAIN_SIZE;
         length   = 1 + next_random( &state) % 12;
         if( length > DOMAIN_SIZE - location)
            length = DOMAIN_SIZE - location;
         range = mulle_range_make( location, length);
         overlap = 0;
         for( i = 0; i < count; i++)
            overlap |= ranges_overlap( expected[ i].range, range);

         value = (void *) (uintptr_t) (step + 1);
         result = _mulle__rangemap_insert( &map, range, value, NULL);
         assert( result == (overlap ? EADDRINUSE : 0));
         if( ! overlap)
         {
            assert( count < RANGE_COUNT);
            expected[ count].range = range;
            expected[ count].value = value;
            ++count;
         }
      }
      if( (step & 0x1f) == 0)
         check_rangemap( &map, expected, count);
   }
   check_rangemap( &map, expected, count);

   _mulle__rangemap_reset( &map, NULL);
   assert( _mulle__rangemap_is_empty( &map));
   _mulle__rangemap_done( &map, NULL);
}


static void   test_boundaries( void)
{
   struct mulle__rangemap   map;
   struct mulle_range        range;
   uintptr_t                 found;
   uintptr_t                 index;
   int                       result;

   _mulle__rangemap_init( &map, 1, NULL);

   range = mulle_range_make( 0, 0);
   result = _mulle__rangemap_insert( &map, range, (void *) 1, NULL);
   assert( result == EINVAL);

   range = mulle_range_make( mulle_range_location_max, 2);
   result = _mulle__rangemap_insert( &map, range, (void *) 1, NULL);
   assert( result == EINVAL);

   range = mulle_range_make( mulle_range_location_max, 1);
   assert( mulle_range_is_valid( range));
   result = _mulle__rangemap_insert( &map, range, (void *) 2, NULL);
   assert( result == 0);
   index = _mulle__rangemap_search( &map, mulle_range_location_max);
   assert( index == 0);
   assert( _mulle__rangemap_get_value( &map, index) == (void *) 2);
   result = _mulle__rangemap_remove( &map, range, NULL);
   assert( result == 0);

   // Exercise queries that start inside a range, in a hole spanning the
   // next range, and after the last range.
   result = _mulle__rangemap_insert( &map,
                                     mulle_range_make( 10, 5),
                                     (void *) 3,
                                     NULL);
   assert( result == 0);
   result = _mulle__rangemap_insert( &map,
                                     mulle_range_make( 30, 5),
                                     (void *) 4,
                                     NULL);
   assert( result == 0);
   found = _mulle__rangemap_get_ranges_values( &map,
                                               mulle_range_make( 12, 20),
                                               0,
                                               NULL,
                                               NULL);
   assert( found == 2);
   found = _mulle__rangemap_get_ranges_values( &map,
                                               mulle_range_make( 16, 16),
                                               0,
                                               NULL,
                                               NULL);
   assert( found == 1);
   found = _mulle__rangemap_get_ranges_values( &map,
                                               mulle_range_make( 40, 5),
                                               0,
                                               NULL,
                                               NULL);
   assert( found == 0);
   _mulle__rangemap_remove( &map, mulle_range_make( 10, 5), NULL);
   _mulle__rangemap_remove( &map, mulle_range_make( 30, 5), NULL);

   range = mulle_range_make( mulle_not_found_e, 1);
   result = _mulle__rangemap_remove( &map, range, NULL);
   assert( result == EINVAL);
   assert( _mulle__rangemap_is_empty( &map));
   _mulle__rangemap_done( &map, NULL);
}


int   main( void)
{
   test_randomized_ranges();
   test_boundaries();
   return( 0);
}
