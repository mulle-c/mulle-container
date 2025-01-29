#include <stdio.h>
#include <mulle-container/mulle-container.h>

int main( void)
{
   struct mulle_set                        *set1;
   struct mulle_set                        *set2;
   struct mulle_container_keycallback      callback;

   callback = mulle_container_keycallback_copied_cstring;

   // Create two sets
   set1 = mulle_set_create( 0, &callback, NULL);
   set2 = mulle_set_create( 0, &callback, NULL);
   
   // Add elements to set1
   mulle_set_set( set1, "VfL");
   mulle_set_set( set1, "Bochum");
   mulle_set_set( set1, "1848");
   printf( "Set1 count: %zu\n", mulle_set_get_count( set1));
   
   // Add elements to set2
   mulle_set_set( set2, "VfL");
   mulle_set_set( set2, "Bochum");
   mulle_set_set( set2, "Football");
   printf( "Set2 count: %zu\n", mulle_set_get_count( set2));
   
   // Test intersection
   mulle_set_intersect( set1, set1, set2);
   printf( "Intersection count: %zu\n", mulle_set_get_count( set1));
   printf( "Contains VfL: %d\n", mulle_set_member( set1, "VfL"));
   printf( "Contains Bochum: %d\n", mulle_set_member( set1, "Bochum"));
   printf( "Contains 1848: %d\n", mulle_set_member( set1, "1848"));
   printf( "Contains Football: %d\n", mulle_set_member( set1, "Football"));
   
   // Test NULL handling
   mulle_set_intersect( NULL, NULL, set2);
   mulle_set_intersect( NULL, set1, NULL);
   mulle_set_intersect( NULL, NULL, NULL);
   printf( "NULL handling completed\n");
   
   mulle_set_destroy( set1);
   mulle_set_destroy( set2);
   return( 0);
}
