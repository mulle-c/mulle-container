//
//  main.c
//  test-array
//
//  Created by Nat! on 04.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>

#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>





static void   create( void)
{
   struct mulle_array   *array;

   array = mulle_array_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   assert( array != NULL);
   assert( mulle_array_get_count( array) == 0);

   mulle_array_destroy( array);
}


static void   array_null( void)
{
   mulle_array_init( NULL, 10, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_array_add( NULL, "foo");

   assert( mulle_array_get_count( NULL) == 0);
   assert( mulle_array_get_size( NULL) == 0);
   assert( mulle_array_get_guaranteed_size( NULL) == 0);

   mulle_array_remove( NULL, "bar");

   mulle_array_done( NULL);
}



static void   init( void)
{
   struct mulle_array   array;

   mulle_array_init( &array, 10, &mulle_container_keycallback_copied_cstring, NULL);

   assert( mulle_array_get_count( &array) == 0);
   assert( mulle_array_get_size( &array) >= 10);

   mulle_array_done( &array);
}


static void   guarantee( void)
{
   struct mulle_array   array;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   assert( mulle_array_get_count( &array) == 0);

   mulle_array_guarantee( &array, 32);
   assert( mulle_array_get_count( &array) == 0);
   assert( mulle_array_get_size( &array) >= 32);
   assert( mulle_array_get_guaranteed_size( &array) >= 32);

   mulle_array_done( &array);
}


static void   array_reset( void)
{
   struct mulle_array   *array;

   array = mulle_array_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   mulle_array_add( array, "VfL");
   mulle_array_add( array, "Bochum");
   mulle_array_add( array, "1848");

   mulle_array_reset( array);
   assert( mulle_array_get_count( array) == 0);

   mulle_array_destroy( array);
}



static void   array_remove( void)
{
   struct mulle_array   *array;

   array = mulle_array_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   mulle_array_add( array, "VfL");
   mulle_array_add( array, "Bochum");
   mulle_array_add( array, "1848");

   mulle_array_remove( array, "unknown");
   assert( mulle_array_get_count( array) == 3);
   mulle_array_remove( array, "Bochum");
   assert( mulle_array_get_count( array) == 2);
   mulle_array_remove( array, "Bochum");
   assert( mulle_array_get_count( array) == 2);

   mulle_array_destroy( array);
}



static void   simple( void)
{
   struct mulle_array              *array;
   struct mulle_arrayenumerator    rover;
   void                            *item;

   array = mulle_array_create( 0, &mulle_container_keycallback_copied_cstring, NULL);
   mulle_array_add( array, "VfL");
   assert("VfL" != mulle_array_get( array, 0)); // must have been copied

   mulle_array_add( array, "Bochum");
   mulle_array_add( array, "1848");

   assert( mulle_array_get_count( array) == 3);

   assert( ! strcmp( "VfL", mulle_array_get( array, 0)));
   assert( ! strcmp( "Bochum", mulle_array_get( array, 1)));
   assert( ! strcmp( "1848", mulle_array_get( array, 2)));

   rover = mulle_array_enumerate( array);
   mulle_arrayenumerator_next( &rover, &item);
   assert( item != NULL);
   assert( ! strcmp( "VfL", item));
   mulle_arrayenumerator_next( &rover, &item);
   assert( item != NULL);
   assert( ! strcmp( "Bochum", item));
   mulle_arrayenumerator_next( &rover, &item);
   assert( item != NULL);
   assert( ! strcmp( "1848", item));
   assert( ! mulle_arrayenumerator_next( &rover, &item));
   mulle_arrayenumerator_done( &rover);

   mulle_array_remove_last( array);
   assert( mulle_array_get_count( array) == 2);
   mulle_array_remove_last( array);
   assert( mulle_array_get_count( array) == 1);
   mulle_array_remove_last( array);
   assert( mulle_array_get_count( array) == 0);

   mulle_array_destroy( array);
}


static void  ephemeral( void)
{
   struct mulle_array   array;

   mulle_array_init( &array, 10, &mulle_container_keycallback_copied_cstring, NULL);
   mulle_array_add( &array, "VfL");
   mulle_array_is_full( &array);
   mulle_array_done( &array);
}


static void   grow( void)
{
   struct mulle_array   array;
   size_t               memo_size;
   void                 *memo_storage;
   size_t               i, n;

   mulle_array_init( &array, 10, &mulle_container_keycallback_copied_cstring, NULL);
   _mulle_array_guarantee( &array, 20);
   assert( mulle_array_get_guaranteed_size( &array) >= 20);

   memo_size    = mulle_array_get_size( &array);
   memo_storage = array._storage;

   n = mulle_array_get_guaranteed_size( &array);
   for( i = 0; i < n; i++)
   {
      mulle_array_add( &array, "VfL");
   }

   // guarantee means unchanging internal _storage
   assert( mulle_array_get_guaranteed_size( &array) == 0);
   assert( mulle_array_get_size( &array) == memo_size);
   assert( array._storage == memo_storage);

   assert( mulle_array_is_full( &array) == 1);
   _mulle_array_grow( &array);
   assert( mulle_array_is_full( &array) == 0);

   mulle_array_done( &array);
}


static void   find_in_range_identical( void)
{
   struct mulle_array              *array;
   struct mulle_arrayenumerator    rover;
   int                             item;
   size_t                          index;
   void                            *pointer;

   array = mulle_array_create( 0, &mulle_container_keycallback_int, NULL);

   mulle_array_add( array, mulle_int_as_pointer( 0));
   mulle_array_add( array, mulle_int_as_pointer( 1));
   mulle_array_add( array, mulle_int_as_pointer( 2));

   assert( 0 == mulle_pointer_as_int( mulle_array_get( array, 0)));
   assert( 1 == mulle_pointer_as_int( mulle_array_get( array, 1)));
   assert( 2 == mulle_pointer_as_int( mulle_array_get( array, 2)));

   assert( mulle_array_get_count( array) == 3);

   rover = mulle_array_enumerate( array);
   mulle_arrayenumerator_next( &rover, &pointer);
   item = mulle_pointer_as_int( pointer);
   assert( 0 == item);
   mulle_arrayenumerator_next( &rover, &pointer);
   item = mulle_pointer_as_int( pointer);
   assert( 1 == item);
   mulle_arrayenumerator_next( &rover, &pointer);
   item = mulle_pointer_as_int( pointer);
   assert( 2 == item);

   index = mulle_array_find_in_range_identical( array, mulle_int_as_pointer( 1), mulle_range_make( 0, 3));
   assert( index == 1);
   index = mulle_array_find_in_range_identical( array, mulle_int_as_pointer( 1), mulle_range_make( 0, 1));
   assert( index == mulle_not_found_e);
   mulle_arrayenumerator_done( &rover);
   mulle_array_destroy( array);
}


static void   array_is_equal( void)
{
   struct mulle_array   array;
   struct mulle_array   copy;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   assert( mulle_array_is_equal( NULL, NULL));
   assert( ! mulle_array_is_equal( NULL, &array));
   assert( ! mulle_array_is_equal( &array, NULL));

   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");

   mulle_array_init( &copy, 0, &mulle_container_keycallback_copied_cstring, NULL);
   assert( ! mulle_array_is_equal( &copy, &array));

   mulle_array_add_array( &copy, &array, mulle_range_make_all());
   assert( mulle_array_is_equal( &copy, &array));

   mulle_array_done( &copy);
   mulle_array_done( &array);
}


static void   copy( void)
{
   struct mulle_array   array;
   struct mulle_array   copy;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");
   assert( mulle_array_get_count( &array) == 3);

   mulle_array_init( &copy, 0, &mulle_container_keycallback_copied_cstring, NULL);
   assert( mulle_array_get_count( &copy) == 0);
   mulle_array_add_array( &copy, &array, mulle_range_make_all());
   assert( mulle_array_get_count( &copy) == 3);
   assert( mulle_array_get_count( &array) == 3);
   assert( mulle_array_is_equal( &copy, &array));

   mulle_array_done( &copy);
   mulle_array_done( &array);
}


static void   find( void)
{
   struct mulle_array   array;
   unsigned long        found;
   char                 *s1;
   char                 *s2;
   char                 *s3;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   found = mulle_array_find_in_range_identical( &array, "xxx", mulle_range_make( 0, 0));
   assert( found == mulle_not_found_e);

   mulle_array_add( &array, s1 = "VfL");
   mulle_array_add( &array, s2 = "Bochum");
   mulle_array_add( &array, s3 = "1848");

   found = mulle_array_find_in_range( &array, s1, mulle_range_make( 0, 0));
   assert( found ==  mulle_not_found_e);

   // can't happen since we use copied cstring!
   found = mulle_array_find_in_range_identical( &array, s1, mulle_range_make( 0, 3));
   assert( found ==  mulle_not_found_e);

   found = mulle_array_find_in_range( &array, s1, mulle_range_make( 0, 3));
   assert( found == 0);
   found = mulle_array_find_in_range( &array, s2, mulle_range_make( 0, 3));
   assert( found == 1);
   found = mulle_array_find_in_range( &array, s3, mulle_range_make( 0, 3));
   assert( found == 2);

   found = mulle_array_find_in_range( &array, s3, mulle_range_make( 2, 1));
   assert( found == 2);

   found = mulle_array_find_in_range( &array, s1, mulle_range_make( 1, 0));
   assert( found == mulle_not_found_e);

   mulle_array_done( &array);
}


static void   enumerate( void)
{
   struct mulle_array             array;
   struct mulle_arrayenumerator   rover;
   void                           *item;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");

   rover = mulle_array_enumerate( &array);
   while( mulle_arrayenumerator_next( &rover, &item))
      printf( "%s\n", (char *) item);
   mulle_arrayenumerator_done( &rover);

   mulle_array_done( &array);
}


static void   reverse_enumerate( void)
{
   struct mulle_array                     array;
   struct mulle_arrayreverseenumerator   rover;
   void                                  *item;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");

   rover = mulle_array_reverseenumerate( &array);
   while( mulle_arrayreverseenumerator_next( &rover, &item))
      printf( "%s\n", (char *) item);
   mulle_arrayreverseenumerator_done( &rover);

   mulle_array_done( &array);
}


static void   get_in_range( void)
{
   struct mulle_array             array;
   struct mulle_arrayenumerator   rover;
   void                           *item;
   char                           *buf[ 32];
   unsigned int                   length;

   mulle_array_init( &array, 0, &mulle_container_keycallback_copied_cstring, NULL);

   length = mulle_array_get_in_range( &array, mulle_range_make_all(), buf);
   assert( length == 0);

   mulle_array_add( &array, "VfL");
   mulle_array_add( &array, "Bochum");
   mulle_array_add( &array, "1848");

   length = mulle_array_get_in_range( &array, mulle_range_make_all(), buf);
   assert( length == 3);

   assert( ! strcmp( "VfL", buf[ 0]));
   assert( ! strcmp( "Bochum", buf[ 1]));
   assert( ! strcmp( "1848", buf[ 2]));

   length = mulle_array_get_in_range( &array, mulle_range_make( 1, 2), buf);
   assert( length == 2);

   assert( ! strcmp( "Bochum", buf[ 0]));
   assert( ! strcmp( "1848", buf[ 1]));

   length = mulle_array_get_in_range( &array, mulle_range_make( 1, 3), buf);
   assert( length == 0);

   mulle_array_done( &array);
}



// the mulle_testallocator detects and aborts on leaks
static void   run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
#ifdef DEBUG
   fprintf( stderr, "%s\n", name);
#endif
   (f)();
   mulle_testallocator_reset();
}


int   main(int argc, char * argv[])
{
   run_test( array_null, "null");
   run_test( create, "create");
   run_test( init, "init");
   run_test( array_remove, "remove");
   run_test( simple, "simple");
   run_test( guarantee, "guarantee");
   run_test( ephemeral, "ephemeral");
   run_test( array_is_equal, "is_equal");
   run_test( array_reset, "reset");
   run_test( grow, "grow");
   run_test( find_in_range_identical, "find_in_range_identical");
   run_test( get_in_range, "get_in_range");
   run_test( copy, "copy");
   run_test( find, "find");
   run_test( enumerate, "enumerate");
   run_test( reverse_enumerate, "reverseenumerate");

   return( 0);
}
