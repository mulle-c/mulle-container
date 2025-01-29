#include <stdio.h>
#include <mulle-container/mulle-container.h>

int main( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   
   printf( "Contains in NULL map: %d\n", mulle_map_contains( NULL, "VfL"));
   printf( "Contains missing key: %d\n", mulle_map_contains( map, "VfL"));
   
   mulle_map_set( map, "VfL", "VFL");
   printf( "Contains existing key: %d\n", mulle_map_contains( map, "VfL"));
   printf( "Contains other missing key: %d\n", mulle_map_contains( map, "missing"));
   
   mulle_map_destroy( map);
   return( 0);
}
