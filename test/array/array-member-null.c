//
// Test for bug 4.3: mulle_array_member(NULL, ...) should not crash.
// The mulle_ prefix guarantees NULL-safety per the documented contract.
// Expected: returns 0 (not found) without crashing.
//
#include <mulle-container/mulle-container.h>
#include <stdio.h>


int   main( void)
{
   int   result;

   // mulle_array_member with NULL array must not crash
   result = mulle_array_member( NULL, (void *) 0x1);
   printf( "mulle_array_member( NULL, 0x1): %d\n", result);

   result = mulle_array_member( NULL, NULL);
   printf( "mulle_array_member( NULL, NULL): %d\n", result);

   printf( "passed\n");
   return( 0);
}
