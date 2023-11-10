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



static void  find( void)
{
   struct mulle_pointerpairarray             *array;
   struct mulle_pointerpairarrayenumerator   rover;
   struct mulle_pointerpair                  item;
   struct mulle_pointerpair                  pairs[ 3];
   uintptr_t                                 index;

   array = mulle_pointerpairarray_create( NULL);

   mulle_pointerpairarray_add_kv( array, "VfL", "Bochum");
   mulle_pointerpairarray_add_kv( array, "WSV", "Wuppertal");
   mulle_pointerpairarray_add_kv( array, "MSV", "Duisburg");
   mulle_pointerpairarray_add_kv( array, "KSC", "Karlsruhe");

   mulle_pointerpairarray_get_in_range( array, mulle_range_make( 0, 3), pairs);
   assert( ! strcmp( pairs[ 2].value, "Duisburg"));

   item = mulle_pointerpair_make_invalid();
   assert( mulle_pointerpair_is_invalid( item));

   index = mulle_pointerpairarray_find_in_range( NULL, mulle_pointerpair_make_invalid(), mulle_range_make_invalid());
   assert( index == mulle_not_found_e);

   index = mulle_pointerpairarray_find_in_range( array, mulle_pointerpair_make_invalid(), mulle_range_make_invalid());
   assert( index == mulle_not_found_e);

   index = mulle_pointerpairarray_find_in_range( array, mulle_pointerpair_make_invalid(), mulle_range_make_all());
   assert( index == mulle_not_found_e);

   index = mulle_pointerpairarray_find_in_range( array, item, mulle_range_make_all());
   assert( index == mulle_not_found_e);

   item  = mulle_pointerpair_make( "MSV", "Duisburg");
   index = mulle_pointerpairarray_find( array, item);
   assert( index != mulle_not_found_e);

   item  = mulle_pointerpair_make( "MSV", "Duisburg");
   index = mulle_pointerpairarray_find_callback( array, item, NULL);
   assert( index != mulle_not_found_e);

   item  = mulle_pointerpair_make( "MSV", "Duisburg");
   index = mulle_pointerpairarray_find_compare( NULL, item, 0, NULL);
   assert( index == mulle_not_found_e);

   item  = mulle_pointerpair_make( "MSV", "Duisburg");
   index = mulle_pointerpairarray_find_compare( array, item, 0, NULL);
   assert( index != mulle_not_found_e);

   mulle_pointerpairarray_qsort_r( array, _mulle_pointerpair_compare_pointer_key, NULL);
   mulle_pointerpairarray_qsort( array, _mulle_pointerpair_compare_string_key);

   index = 0;
   mulle_pointerpairarray_for( array, item)
   {
      index++;
   }
   assert( index == mulle_pointerpairarray_get_count( array));

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
   run_test( find, "find");
   return( 0);
}
