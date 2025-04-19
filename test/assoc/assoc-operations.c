//
//  assoc-operations.c
//  test-assoc
//
//  Created by Codeon GmbH.
//  Copyright (c) 2024 Mulle kybernetiK.
//  All rights reserved.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>

#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>


static void  test_is_equal( void)
{
   struct mulle_assoc                        *assoc1;
   struct mulle_assoc                        *assoc2;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_int;
   callback.valuecallback = mulle_container_valuecallback_int;

   assoc1 = mulle_assoc_create( 0, &callback, NULL, NULL);
   assoc2 = mulle_assoc_create( 0, &callback, NULL, NULL);

   printf( "Empty assocs equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_set( assoc1, (void *) 1, (void *) 10);
   mulle_assoc_set( assoc2, (void *) 1, (void *) 10);
   printf( "Same content equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_set( assoc2, (void *) 2, (void *) 20);
   printf( "Different content equal: %d\n", mulle_assoc_is_equal( assoc1, assoc2));

   mulle_assoc_destroy( assoc1);
   mulle_assoc_destroy( assoc2);
}


static void  test_index_access( void)
{
   struct mulle_assoc                        *assoc;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_pointerpair                  pair;

   callback.keycallback   = mulle_container_keycallback_int;
   callback.valuecallback = mulle_container_valuecallback_int;

   assoc = mulle_assoc_create( 0, &callback, NULL, NULL);

   mulle_assoc_set( assoc, (void *) 1, (void *) 10);
   mulle_assoc_set( assoc, (void *) 2, (void *) 20);
   mulle_assoc_set( assoc, (void *) 3, (void *) 30);

   pair = mulle_assoc_get_at_index( assoc, 0);
   printf( "Index 0: key=%ld value=%ld\n", (long) pair.key, (long) pair.value);

   // Replace key=1 at index 0 with key=2,value=20
   // After sorting, key=2 should be at index 1 since keys are [1,2,3]
   mulle_assoc_set_at_index( assoc, 0, (void *) 2, (void *) 20);
   pair = mulle_assoc_get_at_index( assoc, 1);  // Check index 1 where key=2 should be
   printf( "After set index 0: key=%ld value=%ld\n", (long) pair.key, (long) pair.value);

   mulle_assoc_destroy( assoc);
}


static void  run_test( void (*f)( void))
{
   mulle_testallocator_reset();
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( test_is_equal);
   run_test( test_index_access);
   return( 0);
}