#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main( void)
{
   struct mulle_container_keycallback   callback;
   
   callback = mulle_container_keycallback_nonowned_pointer_or_null;
   
   // Cover mulle_array_init with NULL array (line 88)
   mulle_array_init( NULL, 0, &callback, NULL);
   
   // Cover mulle__array_enumerate with NULL array (line 616)
   (void) mulle__array_enumerate( NULL, &callback);
   
   // Cover mulle__pointerarray_get with NULL array (line 517)
   (void) mulle__pointerarray_get( NULL, 0);
   
   // Cover mulle__rangeset_get with out of bounds (line 138)
   struct mulle__rangeset   set;
   
   _mulle__rangeset_init( &set, 0, NULL);
   (void) _mulle__rangeset_get( &set, 999);
   _mulle__rangeset_done( &set, NULL);
   
   // Cover mulle_pointerset_get_count with NULL (line 153)
   (void) mulle_pointerset_get_count( NULL);
   
   // Cover mulle_assoc_init with NULL (line 85)
   mulle_assoc_init( NULL, 0, (struct mulle_container_keyvaluecallback *) 0xDEAD, NULL, NULL);
   
   // Cover mulle__pointerpairarray_grow with size 0 (line 55)
   struct mulle__pointerpairarray   array;
   
   _mulle__pointerpairarray_init( &array, 0, NULL);
   _mulle__pointerpairarray_grow( &array, NULL);
   _mulle__pointerpairarray_done( &array, NULL);
   
   printf( "NULL safety checks passed\n");
   return( 0);
}
