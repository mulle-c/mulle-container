//
// Test for bug 4.5: _mulle__structarray_remove_in_range with oversized range.
// An oversized range should be clamped to the array bounds, not abort
// or cause memory corruption.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


int   main( void)
{
   struct mulle_structarray   array;
   int                        value;

   mulle_structarray_init( &array, sizeof( int), alignof( int), 4, NULL);

   // Add 3 elements
   value = 10;
   mulle_structarray_add( &array, &value);
   value = 20;
   mulle_structarray_add( &array, &value);
   value = 30;
   mulle_structarray_add( &array, &value);

   printf( "count before: %zu\n", mulle_structarray_get_count( &array));

   // Remove with an oversized range: location=0, length=100
   // This should clamp to count (3) and remove all, not abort/crash.
   mulle_structarray_remove_in_range( &array, mulle_range_make( 0, 100));

   printf( "count after remove_in_range(0, 100): %zu\n",
           mulle_structarray_get_count( &array));

   mulle_structarray_done( &array);

   // Second test: remove from the middle with oversized length
   mulle_structarray_init( &array, sizeof( int), alignof( int), 4, NULL);

   value = 100;
   mulle_structarray_add( &array, &value);
   value = 200;
   mulle_structarray_add( &array, &value);
   value = 300;
   mulle_structarray_add( &array, &value);
   value = 400;
   mulle_structarray_add( &array, &value);
   value = 500;
   mulle_structarray_add( &array, &value);

   printf( "count before: %zu\n", mulle_structarray_get_count( &array));

   // Remove from location=2 with length=100 (way past end)
   // Should clamp to removing elements [2..4], leaving [0,1]
   mulle_structarray_remove_in_range( &array, mulle_range_make( 2, 100));

   printf( "count after remove_in_range(2, 100): %zu\n",
           mulle_structarray_get_count( &array));

   // Verify remaining elements are intact
   printf( "element[0]: %d\n", *(int *) mulle_structarray_get( &array, 0));
   printf( "element[1]: %d\n", *(int *) mulle_structarray_get( &array, 1));

   mulle_structarray_done( &array);

   printf( "passed\n");
   return( 0);
}
