//
// Test for bug 4.1 fix: mulle_array_do_flexible must compile.
// The MULLE_ARRAY_DATA macro was casting to struct mulle_pointerarray
// instead of struct mulle_array, causing a compile error.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


int   main( void)
{
   mulle_array_do_flexible( array, 4, &mulle_container_keycallback_nonowned_cstring)
   {
      mulle_array_add( array, "hello");
      mulle_array_add( array, "world");

      printf( "count: %zu\n", mulle_array_get_count( array));
      printf( "element[0]: %s\n", (char *) mulle_array_get( array, 0));
      printf( "element[1]: %s\n", (char *) mulle_array_get( array, 1));
   }

   printf( "passed\n");
   return( 0);
}
