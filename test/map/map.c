//
//  main.c
//  test-map
//
//  Created by Nat! on 05.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-test-allocator/mulle-test-allocator.h>


static void  simple( void)
{
   struct mulle_bigmap              *map;
   struct mulle_bigmapenumerator    rover;
   void                          *key;
   void                          *value;
   int                           rval;
   int                           i;
   int                           state;
   struct mulle_container_keyvaluecallback      callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_bigmap_create( 0, &callback, NULL);
   mulle_bigmap_set( map, "VfL", "VFL");
   assert( "VFL" != mulle_bigmap_get( map, "VfL")); // must have been copied

   mulle_bigmap_set( map, "Bochum", "BOCHUM");
   mulle_bigmap_set( map, "1848", "1848");

   assert( mulle_bigmap_get_count( map) == 3);

   assert( ! strcmp( "VFL", mulle_bigmap_get( map, "VfL")));
   assert( ! strcmp( "BOCHUM", mulle_bigmap_get( map, "Bochum")));
   assert( ! strcmp( "1848", mulle_bigmap_get( map, "1848")));
   assert( ! mulle_bigmap_get( map, "xxx"));

   state = 0;
   rover = mulle_bigmap_enumerate( map);
   for( i = 0; i < 3; i++)
   {
      rval  = mulle_bigmapenumerator_next( &rover, &key, &value);
      assert( rval == 1);
      if( ! strcmp( "VfL", key))
      {
         state |= 0x1;
         assert( ! strcmp( "VFL", value));
         continue;
      }
      if( ! strcmp( "Bochum", key))
      {
         state |= 0x2;
         assert( ! strcmp( "BOCHUM", value));
         continue;
      }
      if( ! strcmp( "1848", key))
      {
         state |= 0x4;
         assert( ! strcmp( "1848", value));
         continue;
      }
   }
   assert( state == 0x7);

   rval  = mulle_bigmapenumerator_next( &rover, &key, &value);
   assert( ! rval);
   mulle_bigmapenumerator_done( &rover);

   assert( mulle_bigmap_get_count( map) == 3);
   mulle_bigmap_remove( map, "xxx");
   assert( mulle_bigmap_get_count( map) == 3);
   mulle_bigmap_remove( map, "VfL");
   assert( mulle_bigmap_get_count( map) == 2);
   mulle_bigmap_remove( map, "Bochum");
   assert( mulle_bigmap_get_count( map) == 1);
   mulle_bigmap_remove( map, "1848");
   assert( mulle_bigmap_get_count( map) == 0);

   mulle_bigmap_destroy( map);
}


// the mulle_test_allocator detects and aborts on leaks
static void  run_test( void (*f)( void))
{
   mulle_test_allocator_reset();
   (f)();
   mulle_test_allocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( simple);
   return( 0);
}
