#include <stdio.h>
#include <mulle-container/mulle-container.h>

int main( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   
   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   printf( "Count before remove_all: %zu\n", mulle_map_get_count( map));
   printf( "Contains VfL before: %d\n", mulle_map_contains( map, "VfL"));
   
   mulle_map_remove_all( map);
   printf( "Count after remove_all: %zu\n", mulle_map_get_count( map));
   printf( "Contains VfL after: %d\n", mulle_map_contains( map, "VfL"));
   
   // Test NULL handling (should not crash)
   mulle_map_remove_all( NULL);
   printf( "remove_all on NULL completed\n");
   
   mulle_map_destroy( map);
   return( 0);
}
