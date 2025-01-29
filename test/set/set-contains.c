#include <stdio.h>
#include <mulle-container/mulle-container.h>

int main( void)
{
   struct mulle_set                        *set;
   struct mulle_container_keycallback      callback;

   callback = mulle_container_keycallback_copied_cstring;

   set = mulle_set_create( 0, &callback, NULL);
   
   printf( "Contains in NULL set: %d\n", mulle_set_member( NULL, "VfL"));
   printf( "Contains missing value: %d\n", mulle_set_member( set, "VfL"));
   
   mulle_set_set( set, "VfL");
   printf( "Contains existing value: %d\n", mulle_set_member( set, "VfL"));
   printf( "Contains other missing value: %d\n", mulle_set_member( set, "missing"));
   
   mulle_set_destroy( set);
   return( 0);
}
