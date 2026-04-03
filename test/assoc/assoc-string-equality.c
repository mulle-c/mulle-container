#include "include.h"
#include <stdio.h>

// Test string keys with int values to hit callback path (lines 187-191)
static void test_string_equality( void)
{
   struct mulle_assoc                        *assoc1;
   struct mulle_assoc                        *assoc2;
   struct mulle_container_keyvaluecallback   callback;

   // Use copied string KEYS but int VALUES (so values can be compared by pointer)
   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_int;

   assoc1 = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);
   assoc2 = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   printf( "Empty assocs equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_set( assoc1, "key1", (void *) 100);
   mulle_assoc_set( assoc2, "key1", (void *) 100);
   printf( "Same content equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_set( assoc2, "key2", (void *) 200);
   printf( "Different content equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_destroy( assoc1);
   mulle_assoc_destroy( assoc2);
}

int main( void)
{
   test_string_equality();
   return( 0);
}
