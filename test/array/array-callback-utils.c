#include <mulle-container/mulle-container.h>
#include <stdio.h>

int main( void)
{
   struct mulle_container_keyvaluecallback   callback;
   int                                       retains;
   int                                       releases;
   
   // Test with owned pointers (should retain/release)
   callback.keycallback   = mulle_container_keycallback_owned_pointer;
   callback.valuecallback = mulle_container_valuecallback_owned_pointer;
   
   retains = mulle_container_keyvaluecallback_retains( &callback);
   releases = mulle_container_keyvaluecallback_releases( &callback);
   printf( "Owned pointers - retains: %d, releases: %d\n", retains, releases);
   
   // Test with nonowned pointers (should not retain/release)
   callback.keycallback   = mulle_container_keycallback_nonowned_pointer;
   callback.valuecallback = mulle_container_valuecallback_nonowned_pointer;
   
   retains = mulle_container_keyvaluecallback_retains( &callback);
   releases = mulle_container_keyvaluecallback_releases( &callback);
   printf( "Nonowned pointers - retains: %d, releases: %d\n", retains, releases);
   
   // Test with NULL callback (lines 318, 329)
   retains = mulle_container_keyvaluecallback_retains( NULL);
   releases = mulle_container_keyvaluecallback_releases( NULL);
   printf( "NULL callback - retains: %d, releases: %d\n", retains, releases);
   
   // Test value callback helpers (lines 301, 303)
   retains = _mulle_container_valuecallback_retains( &callback.valuecallback);
   releases = _mulle_container_valuecallback_releases( &callback.valuecallback);
   printf( "Value callback - retains: %d, releases: %d\n", retains, releases);
   
   // Test with copied cstrings (line 321)
   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;
   
   retains = mulle_container_keyvaluecallback_retains( &callback);
   releases = mulle_container_keyvaluecallback_releases( &callback);
   printf( "Copied cstrings - retains: %d, releases: %d\n", retains, releases);
   
   printf( "Callback utility tests passed\n");
   return( 0);
}
