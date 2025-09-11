#include "include.h"

#include <stdio.h>


int   main( void)
{
   struct mulle_pointerset   set;
   void                      *storage[ 8];
   void                      *p;

   printf( "Testing mulle_pointerset_init_with_static_pointers\n");

   // Initialize with static storage
   mulle_pointerset_init_with_static_pointers( &set, storage, 8, &mulle_stdlib_allocator);

   printf( "Initial count: %zu\n", mulle_pointerset_get_count( &set));
   printf( "Initial size: %zu\n", mulle_pointerset_get_size( &set));

   // Add some pointers
   p = (void *) 0x1234;
   mulle_pointerset_set( &set, p);
   printf( "After adding 0x1234, count: %zu\n", mulle_pointerset_get_count( &set));

   p = (void *) 0x5678;
   mulle_pointerset_set( &set, p);
   printf( "After adding 0x5678, count: %zu\n", mulle_pointerset_get_count( &set));

   // Check if we can find them
   p = mulle_pointerset_get( &set, (void *) 0x1234);
   printf( "Found 0x1234: %s\n", p ? "YES" : "NO");

   p = mulle_pointerset_get( &set, (void *) 0x5678);
   printf( "Found 0x5678: %s\n", p ? "YES" : "NO");

   p = mulle_pointerset_get( &set, (void *) 0x9ABC);
   printf( "Found 0x9ABC: %s\n", p ? "YES" : "NO");

   mulle_pointerset_done( &set);

   return( 0);
}