#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mulle-container/mulle-container.h>


int   main( void)
{
   struct mulle__rangemap   map;
   int                     ret;

   _mulle__rangemap_init( &map, 0, NULL);

   // Test 1: Insert first range (should succeed)
   ret = _mulle__rangemap_insert( &map, mulle_range_make( 10, 5), (void *) 0x1, NULL);
   printf( "Insert 10,5: %s\n", ret ? "Failure" : "Success");
   _mulle__rangemap_print( &map, stdout);

   // Test 2: Insert non-overlapping range (should succeed)
   ret = _mulle__rangemap_insert( &map, mulle_range_make( 20, 5), (void *) 0x2, NULL);
   printf( "Insert 20,5: %s\n", ret ? "Failure" : "Success");
   _mulle__rangemap_print( &map, stdout);

   // Test 3: Insert overlapping range (should fail)
   ret = _mulle__rangemap_insert( &map, mulle_range_make( 12, 3), (void *) 0x3, NULL);
   printf( "Insert 12,3 (overlap): %s\n", ret ? "Failure" : "Success");
   _mulle__rangemap_print( &map, stdout);

   // Test 4: Insert partially overlapping range (should fail)
   ret = _mulle__rangemap_insert( &map, mulle_range_make( 8, 5), (void *) 0x4, NULL);
   printf( "Insert 8,5 (partial overlap): %s\n", ret ? "Failure" : "Success");
   _mulle__rangemap_print( &map, stdout);

   // Test 5: Insert fully containing range (should fail)
   ret = _mulle__rangemap_insert( &map, mulle_range_make( 9, 8), (void *) 0x5, NULL);
   printf( "Insert 9,8 (full overlap): %s\n", ret ? "Failure" : "Success");
   _mulle__rangemap_print( &map, stdout);

   // Test 6: Insert range that contains existing ranges (should fail)
   ret = _mulle__rangemap_insert( &map, mulle_range_make( 5, 30), (void *) 0x6, NULL);
   printf( "Insert 5,30 (contains existing): %s\n", ret ? "Failure" : "Success");
   _mulle__rangemap_print( &map, stdout);

   _mulle__rangemap_done( &map, NULL);

   return( 0);
}