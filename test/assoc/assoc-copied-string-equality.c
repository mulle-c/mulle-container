#include "include.h"
#include <stdio.h>

// Custom compare function that compares both key and value as strings
static int compare_string_pairs( struct mulle_pointerpair *a,
                                 struct mulle_pointerpair *b,
                                 void *userinfo)
{
   int cmp;
   
   cmp = strcmp( (char *) a->key, (char *) b->key);
   if( cmp != 0)
      return( cmp);
   
   return( strcmp( (char *) a->value, (char *) b->value));
}

int main( void)
{
   struct mulle_assoc                        *assoc1;
   struct mulle_assoc                        *assoc2;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   // Use compare function that compares both key AND value
   assoc1 = mulle_assoc_create( 0, &callback, compare_string_pairs, NULL);
   assoc2 = mulle_assoc_create( 0, &callback, compare_string_pairs, NULL);

   printf( "Empty assocs equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_set( assoc1, "key1", "value1");
   mulle_assoc_set( assoc2, "key1", "value1");
   printf( "Same string content equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_set( assoc2, "key2", "value2");
   printf( "Different content equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_destroy( assoc1);
   mulle_assoc_destroy( assoc2);

   return( 0);
}
