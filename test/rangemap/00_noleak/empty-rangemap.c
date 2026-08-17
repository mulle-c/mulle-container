//
// Test that empty rangemap operations don't crash (no NULL pointer arithmetic).
// Regression test for: get_ranges_values and get_exact computed pointer offsets
// from NULL storage before checking _length == 0.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <errno.h>


static void   test_empty_get_ranges_values( void)
{
   struct mulle__rangemap   map;
   struct mulle_range       out_ranges[4];
   void                     *out_values[4];
   uintptr_t               count;

   _mulle__rangemap_init( &map, 0, NULL);  // zero capacity, _storage is NULL

   count = _mulle__rangemap_get_ranges_values( &map,
                                               mulle_range_make( 0, 100),
                                               4,
                                               out_ranges,
                                               out_values);
   printf( "empty get_ranges_values: %lu\n", (unsigned long) count);

   _mulle__rangemap_done( &map, NULL);
}


static void   test_empty_get_exact( void)
{
   struct mulle__rangemap   map;
   void                     *value;

   _mulle__rangemap_init( &map, 0, NULL);

   errno = 0;
   value = _mulle__rangemap_get_exact( &map, mulle_range_make( 5, 3));
   printf( "empty get_exact: %s (errno=%d)\n",
           value == NULL ? "NULL" : "WRONG",
           errno);

   _mulle__rangemap_done( &map, NULL);
}


static void   test_empty_search( void)
{
   struct mulle__rangemap   map;
   uintptr_t               result;

   _mulle__rangemap_init( &map, 0, NULL);

   result = _mulle__rangemap_search( &map, 42);
   printf( "empty search: %s\n",
           result == mulle_not_found_e ? "not_found" : "WRONG");

   _mulle__rangemap_done( &map, NULL);
}


static void   test_empty_enumerate( void)
{
   struct mulle__rangemap            map;
   struct mulle__rangemapenumerator  rover;
   struct mulle_range                range;
   void                              *value;
   int                               count;

   _mulle__rangemap_init( &map, 0, NULL);

   count = 0;
   rover = _mulle__rangemap_enumerate( &map);
   while( _mulle__rangemapenumerator_next( &rover, &range, &value))
      count++;

   printf( "empty enumerate count: %d\n", count);

   _mulle__rangemap_done( &map, NULL);
}


int   main( void)
{
   test_empty_get_ranges_values();
   test_empty_get_exact();
   test_empty_search();
   test_empty_enumerate();
   return( 0);
}
