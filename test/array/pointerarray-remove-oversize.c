//
// Test for bug 4.4: _mulle__pointerarray_remove_in_range with oversized range.
// An oversized range should be clamped to the array bounds, not cause
// out-of-bounds memmove / memory corruption.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


int   main( void)
{
   struct mulle_pointerarray   array;

   mulle_pointerarray_init( &array, 4, NULL);

   // Add 3 elements
   mulle_pointerarray_add( &array, (void *) 0x1);
   mulle_pointerarray_add( &array, (void *) 0x2);
   mulle_pointerarray_add( &array, (void *) 0x3);

   printf( "count before: %zu\n", mulle_pointerarray_get_count( &array));

   // Remove with an oversized range: location=0, length=100
   // This should clamp to count (3) and remove all elements, not crash.
   mulle_pointerarray_remove_in_range( &array, mulle_range_make( 0, 100));

   printf( "count after remove_in_range(0, 100): %zu\n",
           mulle_pointerarray_get_count( &array));

   mulle_pointerarray_done( &array);

   // Second test: remove from the middle with oversized length
   mulle_pointerarray_init( &array, 4, NULL);

   mulle_pointerarray_add( &array, (void *) 0xA);
   mulle_pointerarray_add( &array, (void *) 0xB);
   mulle_pointerarray_add( &array, (void *) 0xC);
   mulle_pointerarray_add( &array, (void *) 0xD);
   mulle_pointerarray_add( &array, (void *) 0xE);

   printf( "count before: %zu\n", mulle_pointerarray_get_count( &array));

   // Remove from location=2 with length=100 (way past end)
   // Should clamp to removing elements [2..4], leaving [0,1]
   mulle_pointerarray_remove_in_range( &array, mulle_range_make( 2, 100));

   printf( "count after remove_in_range(2, 100): %zu\n",
           mulle_pointerarray_get_count( &array));

   mulle_pointerarray_done( &array);

   printf( "passed\n");
   return( 0);
}
