//
// Test for bug 4.2 fix: mulle_set_get_size must accept struct mulle_set *
// (not struct mulle__set *). Also tests NULL-safety.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


int   main( void)
{
   struct mulle_set   *set;
   size_t             size;

   // NULL-safety: must not crash
   size = mulle_set_get_size( NULL);
   printf( "mulle_set_get_size( NULL): %zu\n", size);

   // Create a set and check size
   set = mulle_set_create( 8,
                           &mulle_container_keycallback_nonowned_pointer_or_null,
                           NULL);
   size = mulle_set_get_size( set);
   printf( "mulle_set_get_size( empty set): %zu\n", size);

   mulle_set_destroy( set);

   printf( "passed\n");
   return( 0);
}
