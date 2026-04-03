#include <mulle-container/mulle-container.h>
#include <stdio.h>
#include <stdlib.h>

int main( void)
{
   char   *s;
   
   // Test int describe (line 136)
   {
      struct mulle_array   *array;
      
      array = mulle_array_create( 0, &mulle_container_keycallback_int, NULL);
      mulle_array_add( array, mulle_int_as_pointer( 42));
      mulle_array_add( array, mulle_int_as_pointer( 100));
      
      s = mulle_array_describe( array);
      printf( "Int array: %s\n", s);
      mulle_free( s);
      
      mulle_array_destroy( array);
   }
   
   // Test intptr describe (line 146-147)
   {
      struct mulle_array   *array;
      
      array = mulle_array_create( 0, &mulle_container_keycallback_intptr, NULL);
      mulle_array_add( array, mulle_int_as_pointer( 12345));
      
      s = mulle_array_describe( array);
      printf( "Intptr array: %s\n", s);
      mulle_free( s);
      
      mulle_array_destroy( array);
   }
   
   // Test pointer describe (line 168, 178-179)
   {
      struct mulle_array   *array;
      void                 *ptr1 = (void *) 0x1000;
      void                 *ptr2 = (void *) 0x2000;
      
      array = mulle_array_create( 0, &mulle_container_keycallback_nonowned_pointer, NULL);
      mulle_array_add( array, ptr1);
      mulle_array_add( array, ptr2);
      
      s = mulle_array_describe( array);
      printf( "Pointer array: %s\n", s);
      mulle_free( s);
      
      mulle_array_destroy( array);
   }
   
   // Test no_description callback (line 100, 109-110)
   {
      struct mulle_container_valuecallback   callback;
      struct mulle_allocator                 *allocator;
      
      callback = mulle_container_valuecallback_nonowned_pointer;
      callback.describe = mulle_container_valuecallback_no_description;
      
      s = callback.describe( &callback, (void *) 0x1234, &allocator);
      printf( "No description: %s\n", s);
   }
   
   printf( "Callback describe tests passed\n");
   return( 0);
}
