#include <stdio.h>
#include <mulle-container/mulle-container.h>

int main( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);
   
   printf( "Empty map: %d\n", mulle_map_is_empty( map));
   printf( "NULL map: %d\n", mulle_map_is_empty( NULL));
   
   mulle_map_set( map, "VfL", "VFL");
   printf( "After adding element: %d\n", mulle_map_is_empty( map));
   
   mulle_map_destroy( map);
   return( 0);
}
