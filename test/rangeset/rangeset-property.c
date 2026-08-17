#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


#define DOMAIN_SIZE  256
#define STEP_COUNT   3000


static uint32_t   next_random( uint32_t *state)
{
   *state = *state * UINT32_C( 1664525) + UINT32_C( 1013904223);
   return( *state);
}


static void   mark_range( unsigned char *model,
                          struct mulle_range range,
                          unsigned char value)
{
   uintptr_t   end;
   uintptr_t   i;

   end = range.location + range.length;
   if( end > DOMAIN_SIZE)
      end = DOMAIN_SIZE;
   for( i = range.location; i < end; i++)
      model[ i] = value;
}


static void   check_rangeset( struct mulle__rangeset *set,
                              unsigned char *model)
{
   unsigned char          covered[ DOMAIN_SIZE];
   struct mulle_range     range;
   struct mulle_range     previous;
   uintptr_t              range_count;
   uintptr_t              i;
   uintptr_t              location;
   uintptr_t              total;
   int                    have_previous;

   memset( covered, 0, sizeof( covered));
   range_count = _mulle__rangeset_get_rangecount( set);
   total       = 0;
   have_previous = 0;
   for( i = 0; i < range_count; i++)
   {
      range = _mulle__rangeset_get_range( set, i);
      assert( range.length);
      assert( mulle_range_is_valid( range));
      if( have_previous)
         assert( mulle_range_get_max( previous) <= range.location);
      previous = range;
      have_previous = 1;
      total += range.length;
      for( location = range.location;
           location < range.location + range.length && location < DOMAIN_SIZE;
           location++)
      {
         assert( model[ location]);
         assert( ! covered[ location]);
         covered[ location] = 1;
      }
   }

   for( location = 0; location < DOMAIN_SIZE; location++)
   {
      assert( covered[ location] == model[ location]);
      assert( _mulle__rangeset_contains( set, mulle_range_make( location, 1)) == model[ location]);
      assert( _mulle__rangeset_intersects( set, mulle_range_make( location, 1)) == model[ location]);
   }
   assert( total == _mulle__rangeset_sum_lengths( set));
}


static void   test_randomized_ranges( void)
{
   struct mulle__rangeset   set;
   unsigned char             model[ DOMAIN_SIZE];
   struct mulle_range        range;
   uint32_t                  state;
   uintptr_t                 location;
   uintptr_t                 length;
   size_t                    step;

   memset( model, 0, sizeof( model));
   _mulle__rangeset_init( &set, 1, NULL);
   state = UINT32_C( 0xbb67ae85);

   for( step = 0; step < STEP_COUNT; step++)
   {
      location = next_random( &state) % DOMAIN_SIZE;
      length   = next_random( &state) % 24;
      if( length > DOMAIN_SIZE - location)
         length = DOMAIN_SIZE - location;
      range = mulle_range_make( location, length);
      if( next_random( &state) & 1)
      {
         _mulle__rangeset_insert( &set, range, NULL);
         mark_range( model, range, 1);
      }
      else
      {
         _mulle__rangeset_remove( &set, range, NULL);
         mark_range( model, range, 0);
      }
      if( (step & 0x1f) == 0)
         check_rangeset( &set, model);
   }
   check_rangeset( &set, model);

   _mulle__rangeset_done( &set, NULL);
}


static void   test_boundaries( void)
{
   struct mulle__rangeset   set;
   struct mulle_range        range;

   _mulle__rangeset_init( &set, 1, NULL);

   range = mulle_range_make( mulle_range_location_max, 1);
   assert( mulle_range_is_valid( range));
   _mulle__rangeset_insert( &set, range, NULL);
   assert( _mulle__rangeset_get_first( &set) == mulle_range_location_max);
   assert( _mulle__rangeset_get_last( &set) == mulle_range_location_max);
   assert( _mulle__rangeset_contains( &set, range));

   range = mulle_range_make( mulle_range_location_max, 2);
   assert( ! mulle_range_is_valid( range));
   _mulle__rangeset_insert( &set, range, NULL);
   assert( _mulle__rangeset_get_rangecount( &set) == 1);

   range = mulle_range_make( mulle_not_found_e, 1);
   assert( ! mulle_range_is_valid( range));
   _mulle__rangeset_remove( &set, range, NULL);
   assert( _mulle__rangeset_get_rangecount( &set) == 1);

   _mulle__rangeset_remove( &set,
                            mulle_range_make( mulle_range_location_max, 1),
                            NULL);
   assert( _mulle__rangeset_get_rangecount( &set) == 0);
   _mulle__rangeset_done( &set, NULL);
}


int   main( void)
{
   test_randomized_ranges();
   test_boundaries();
   return( 0);
}
