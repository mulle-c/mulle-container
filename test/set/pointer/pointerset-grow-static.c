#include "include.h"

#include <stdio.h>


int   main( void)
{
   struct mulle__pointerset   set;
   void                       *storage[ 4];  // Small static storage
   void                       *p;
   int                        i;

   printf( "Test: pointerset growing from static storage\n");

   // Initialize with small static storage
   _mulle__pointerset_init_with_static_pointers( &set, storage, 4);

   printf( "Initial count: %zu\n", _mulle__pointerset_get_count( &set));
   printf( "Initial size: %zu\n", _mulle__pointerset_get_size( &set));

   // Add enough pointers to trigger growth (more than 75% of capacity)
   for( i = 0; i < 5; i++)  // 5 > 4 * 0.75 = 3, so should grow
   {
      p = (void *) (uintptr_t) (0x1000 + i);
      _mulle__pointerset_set_generic( &set, p, &mulle_container_keycallback_nonowned_pointer_or_null, NULL);
      printf( "After adding 0x%x, count: %zu, size: %zu\n",
              (unsigned int) (uintptr_t) p,
              _mulle__pointerset_get_count( &set),
              _mulle__pointerset_get_size( &set));
   }

   // Verify all pointers are still there
   printf( "Verifying all pointers:\n");
   for( i = 0; i < 5; i++)
   {
      p = (void *) (uintptr_t) (0x1000 + i);
      void *found = _mulle__pointerset_get_generic( &set, p, &mulle_container_keycallback_nonowned_pointer_or_null);
      printf( "Get 0x%x: %s\n",
              (unsigned int) (uintptr_t) p,
              found ? "found" : "not found");
   }

   // Done
   _mulle__pointerset_done( &set, NULL);

   printf( "Test completed successfully\n");

   return( 0);
}