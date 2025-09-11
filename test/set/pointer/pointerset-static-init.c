#include "include.h"

#include <stdio.h>


int   main( void)
{
   struct mulle__pointerset   set;
   void                       *storage[ 16];
   void                       *p;

   printf( "Test: static pointerset initialization\n");

   // Initialize with static storage
   _mulle__pointerset_init_with_static_pointers( &set, storage, 16);

   printf( "Initial count: %zu\n", _mulle__pointerset_get_count( &set));
   printf( "Initial size: %zu\n", _mulle__pointerset_get_size( &set));

   // Add some pointers
   p = (void *) 0x1234;
   _mulle__pointerset_set_generic( &set, p, &mulle_container_keycallback_nonowned_pointer_or_null, NULL);
   printf( "After adding 0x1234, count: %zu\n", _mulle__pointerset_get_count( &set));

   p = (void *) 0x5678;
   _mulle__pointerset_set_generic( &set, p, &mulle_container_keycallback_nonowned_pointer_or_null, NULL);
   printf( "After adding 0x5678, count: %zu\n", _mulle__pointerset_get_count( &set));

   // Check if they are there
   p = _mulle__pointerset_get_generic( &set, (void *) 0x1234, &mulle_container_keycallback_nonowned_pointer_or_null);
   printf( "Get 0x1234: %s\n", p ? "found" : "not found");

   p = _mulle__pointerset_get_generic( &set, (void *) 0x5678, &mulle_container_keycallback_nonowned_pointer_or_null);
   printf( "Get 0x5678: %s\n", p ? "found" : "not found");

   // Done
   _mulle__pointerset_done( &set, NULL);

   printf( "Test completed successfully\n");

   return( 0);
}