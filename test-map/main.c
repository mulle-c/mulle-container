//
//  main.c
//  test-map
//
//  Created by Nat! on 05.11.15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
//
#include <stdio.h>
#include <mulle_container/mulle_container.h>
#include <mulle_test_allocator/mulle_test_allocator.h>


static void  simple( void)
{
   struct mulle_map              *map;
   struct mulle_mapenumerator    rover;
   void                          *key;
   void                          *value;
   int                           rval;
   int                           i;
   int                           state;
   
   map = mulle_map_create_separated_callback( 0, &mulle_container_keycallback_copied_cstring, &mulle_container_valuecallback_copied_cstring);
   mulle_map_put( map, "VfL", "VFL");
   assert( "VFL" != mulle_map_get( map, "VfL")); // must have been copied
   
   mulle_map_put( map, "Bochum", "BOCHUM");
   mulle_map_put( map, "1848", "1848");
   
   assert( mulle_map_count( map) == 3);
   
   assert( ! strcmp( "VFL", mulle_map_get( map, "VfL")));
   assert( ! strcmp( "BOCHUM", mulle_map_get( map, "Bochum")));
   assert( ! strcmp( "1848", mulle_map_get( map, "1848")));
   assert( ! mulle_map_get( map, "xxx"));
   
   state = 0;
   rover = mulle_map_enumerate( map);
   for( i = 0; i < 3; i++)
   {
      rval  = mulle_mapenumerator_next( &rover, &key, &value);
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

   rval  = mulle_mapenumerator_next( &rover, &key, &value);
   assert( ! rval);
   mulle_mapenumerator_done( &rover);
   
   assert( mulle_map_count( map) == 3);
   mulle_map_remove( map, "xxx");
   assert( mulle_map_count( map) == 3);
   mulle_map_remove( map, "VfL");
   assert( mulle_map_count( map) == 2);
   mulle_map_remove( map, "Bochum");
   assert( mulle_map_count( map) == 1);
   mulle_map_remove( map, "1848");
   assert( mulle_map_count( map) == 0);
      
   mulle_map_free( map);
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
