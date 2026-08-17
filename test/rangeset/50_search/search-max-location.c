//
// Test that rangeset greater-than search handles mulle_range_location_max
// correctly without overflow.
// Regression test for: location+1 overflow when location == mulle_range_location_max
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


static void   test_search_at_max_location( void)
{
   struct mulle__rangeset   set;
   uintptr_t               result;

   _mulle__rangeset_init( &set, 0, NULL);

   // Add a range that includes the maximum location
   _mulle__rangeset_insert( &set,
                            mulle_range_make( mulle_range_location_max - 2, 3),
                            NULL);

   // Search for greater-than at the maximum location — should return not_found
   // because there's nothing beyond max
   result = _mulle__rangeset_search_location( &set,
                                              mulle_range_location_max,
                                              mulle_rangeset_greater_than);
   printf( "greater_than at max: %s\n",
           result == mulle_not_found_e ? "not_found" : "WRONG");

   // Search for greater-than at max-1 — should find max (which is in the range)
   result = _mulle__rangeset_search_location( &set,
                                              mulle_range_location_max - 1,
                                              mulle_rangeset_greater_than);
   printf( "greater_than at max-1: %s\n",
           result == mulle_range_location_max ? "found_max" : "WRONG");

   // Search for greater-than at max-3 — should find max-2 (start of range)
   result = _mulle__rangeset_search_location( &set,
                                              mulle_range_location_max - 3,
                                              mulle_rangeset_greater_than);
   printf( "greater_than at max-3: %s\n",
           result == mulle_range_location_max - 2 ? "found_start" : "WRONG");

   _mulle__rangeset_done( &set, NULL);
}


static void   test_search_range_at_max_location( void)
{
   struct mulle__rangeset   set;
   struct mulle_range       result;

   _mulle__rangeset_init( &set, 0, NULL);

   // Add a range near max
   _mulle__rangeset_insert( &set,
                            mulle_range_make( mulle_range_location_max - 5, 3),
                            NULL);

   // greater_than range search at max — nothing beyond, should be invalid
   result = _mulle__rangeset_search_range( &set,
                                           mulle_range_location_max,
                                           mulle_rangeset_greater_than);
   printf( "range greater_than at max: %s\n",
           ! mulle_range_is_valid( result) ? "invalid" : "WRONG");

   _mulle__rangeset_done( &set, NULL);
}


int   main( void)
{
   test_search_at_max_location();
   test_search_range_at_max_location();
   return( 0);
}
