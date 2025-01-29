#include <stdio.h>
#include <mulle-container/mulle-container.h>

int main( void)
{
   struct mulle_set                        *set;
   struct mulle_container_keycallback      callback;

   callback = mulle_container_keycallback_copied_cstring;

   set = mulle_set_create( 0, &callback, NULL);
   
   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   printf( "Count before remove_all: %zu\n", mulle_set_get_count( set));
   printf( "Contains VfL before: %d\n", mulle_set_member( set, "VfL"));
   
   mulle_set_reset( set);  // reset is our remove_all
   printf( "Count after remove_all: %zu\n", mulle_set_get_count( set));
   printf( "Contains VfL after: %d\n", mulle_set_member( set, "VfL"));
   
   // Test NULL handling (should not crash)
   mulle_set_reset( NULL);
   printf( "remove_all on NULL completed\n");
   
   mulle_set_destroy( set);
   return( 0);
}
