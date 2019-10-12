//
//  main.c
//  test-map
//
//  Created by Nat! on 05.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>

#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>


static void  simple( void)
{
   struct mulle_map              *map;
   struct mulle_mapenumerator    rover;
   void                          *key;
   void                          *value;
   int                           rval;
   int                           i;
   int                           state;
   struct mulle_container_keyvaluecallback      callback;
   char                                onstack[] = { 'V', 'f', 'L', 0 };

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   assert( mulle_map_get_count( map) == 0);
   assert( ! mulle_map_get( map, "VfL"));

   mulle_map_set( map, "VfL", "VFL");
   assert( ! mulle_map_get( map, "VFL"));
   assert( ! strcmp( "VFL", mulle_map_get( map, "VfL")));

   mulle_map_set( map, "VfL", "BOCHUM");
   assert( ! strcmp( "BOCHUM", mulle_map_get( map, "VfL")));

   mulle_map_remove( map, onstack);
   assert( ! mulle_map_get( map, "VfL"));

   mulle_map_destroy( map);
}


static void  enumerate( void)
{
   struct mulle_map              *map;
   struct mulle_mapenumerator    rover;
   void                          *key;
   void                          *value;
   int                           rval;
   int                           i;
   int                           state;
   struct mulle_container_keyvaluecallback      callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

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

   assert( mulle_map_get_count( map) == 3);
   mulle_map_remove( map, "xxx");
   assert( mulle_map_get_count( map) == 3);
   mulle_map_remove( map, "VfL");
   assert( mulle_map_get_count( map) == 2);
   mulle_map_remove( map, "Bochum");
   assert( mulle_map_get_count( map) == 1);
   mulle_map_remove( map, "1848");
   assert( mulle_map_get_count( map) == 0);

   mulle_map_destroy( map);
}



// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( void))
{
   mulle_testallocator_reset();
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( simple);
   run_test( enumerate);
   return( 0);
}
