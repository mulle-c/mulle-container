//
//  main.c
//  test-array
//
//  Created by Nat! on 04.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//

#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void  simple( void)
{
   struct mulle_pointerpairarray             *array;
   struct mulle_pointerpairarrayenumerator   rover;
   struct mulle_pointerpair                  item;

   array = mulle_pointerpairarray_create( NULL);

   mulle_pointerpairarray_add_kv( array, "VfL", "Bochum");
   mulle_pointerpairarray_add_kv( array, "WSV", "Wuppertal");
   mulle_pointerpairarray_add_kv( array, "MSV", "Duisburg");
   mulle_pointerpairarray_add_kv( array, "KSC", "Karlsruhe");

   assert( mulle_pointerpairarray_get_count( array) == 4);

   printf( "Initial:\n");
   rover = mulle_pointerpairarray_enumerate( array);
   while( _mulle_pointerpairarrayenumerator_next( &rover, &item))
      printf( "\t{ %s, %s }\n", (char *) item.key, (char *) item.value);
   mulle_pointerpairarrayenumerator_done( &rover);

   mulle_pointerpairarray_qsort_r( array, _mulle_pointerpair_compare_string_key, NULL);

   printf( "Sorted:\n");
   rover = mulle_pointerpairarray_enumerate( array);
   while( _mulle_pointerpairarrayenumerator_next( &rover, &item))
      printf( "\t{ %s, %s }\n", (char *) item.key, (char *) item.value);
   mulle_pointerpairarrayenumerator_done( &rover);

   item = mulle_pointerpairarray_search_compare( array,
                                                 mulle_pointerpair_make( "VfL", NULL),
                                                 _mulle_pointerpair_compare_string_key,
                                                 NULL);
   printf( "Found:\n");
   printf( "\t{ %s, %s }\n", (char *) item.key, (char *) item.value);


   mulle_pointerpairarray_destroy( array);
}



// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
  // printf( "%s\n", name);
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( simple, "simple");
   return( 0);
}
