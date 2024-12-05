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


static void   create( void)
{
   struct mulle__pointermap   *map;

   map = mulle__pointermap_create( 0, 10, NULL);

   assert( map != NULL);
   assert( mulle__pointermap_get_count( map) == 0);

   mulle__pointermap_destroy( map, NULL);
}


static void  simple( void)
{
   char   onstack[] = { 'V', 'f', 'L', 0 };
   void   *old;
   int    state;

   mulle__pointermap_do( map)
   {
      assert( mulle__pointermap_get_count( map) == 0);
      assert( ! mulle__pointermap_get( map, "VfL"));

      mulle__pointermap_set( map, "VfL", "VFL", NULL);
      assert( ! mulle__pointermap_get( map, "VFL"));
      assert( ! strcmp( "VFL", mulle__pointermap_get( map, "VfL")));

      state = mulle__pointermap_insert( map, "VfL", "xxx", NULL);
      assert( ! state);
      assert( ! strcmp( "VFL", mulle__pointermap_get( map, "VfL")));

      mulle__pointermap_set( map, "VfL", "BOCHUM", NULL);
      assert( ! strcmp( "BOCHUM", mulle__pointermap_get( map, "VfL")));

      // should NOT  remove
      mulle__pointermap_remove( map, onstack, NULL);
      assert( mulle__pointermap_get( map, "VfL"));
   }
}


static void   map_reset( void)
{
   struct mulle__pointermap    *map;

   map = mulle__pointermap_create( 0, 9, NULL);

   mulle__pointermap_set( map, "VfL", "VFL", NULL);
   mulle__pointermap_set( map, "Bochum", "BOCHUM", NULL);
   mulle__pointermap_set( map, "1848", "1848", NULL);

   mulle__pointermap_reset( map, NULL);
   assert( mulle__pointermap_get_count( map) == 0);

   mulle__pointermap_destroy( map, NULL);
}



static void   map_remove( void)
{
   struct mulle__pointermap   *map;

   map = mulle__pointermap_create( 3, 0, NULL);

   mulle__pointermap_set( map, "VfL", "VFL", NULL);
   mulle__pointermap_set( map, "Bochum", "BOCHUM", NULL);
   mulle__pointermap_set( map, "1848", "1848", NULL);

   mulle__pointermap_remove( map, "Bochum", NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, "Bochum", NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, "1848", NULL);
   assert( mulle__pointermap_get_count( map) == 1);

   mulle__pointermap_remove( map, "VfL", NULL);
   assert( mulle__pointermap_get_count( map) == 0);

   mulle__pointermap_destroy( map, NULL);
}


static void   map_copy( void)
{
   struct mulle__pointermap   *map;
   struct mulle__pointermap   *copy;

   map = mulle__pointermap_create( 3, 0, NULL);

   copy = _mulle__pointermap_copy( map, NULL);
   mulle__pointermap_destroy( copy, NULL);

   mulle__pointermap_set( map, "VfL", "VFL", NULL);
   mulle__pointermap_set( map, "Bochum", "BOCHUM", NULL);
   mulle__pointermap_set( map, "1848", "1848", NULL);

   copy = _mulle__pointermap_copy( map, NULL);
   mulle__pointermap_destroy( copy, NULL);

   mulle__pointermap_remove( map, "Bochum", NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, "Bochum", NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, "1848", NULL);
   assert( mulle__pointermap_get_count( map) == 1);

   mulle__pointermap_remove( map, "VfL", NULL);
   assert( mulle__pointermap_get_count( map) == 0);

   mulle__pointermap_destroy( map, NULL);
}



static void  enumerate( void)
{
   struct mulle__pointermapenumerator   rover;
   void                                 *key;
   void                                 *value;
   int                                  rval;
   int                                  i;
   int                                  state;
   struct mulle__pointermap             *map;
   struct mulle_pointerpair             pair;

   map = mulle__pointermap_create( 100, 0, NULL);

   mulle__pointermap_set( map, "VfL", "VFL", NULL);
   mulle__pointermap_set( map, "Bochum", "BOCHUM", NULL);
   mulle__pointermap_set( map, "1848", "1848", NULL);

   assert( mulle__pointermap_get_count( map) == 3);

   state = 0;
   rover = mulle__pointermap_enumerate( map);
   for( i = 0; i < 3; i++)
   {
      rval  = _mulle__pointermapenumerator_next( &rover, &key, &value);
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
   mulle__pointermapenumerator_done( &rover);
   rval  = _mulle__pointermapenumerator_next( &rover, &key, &value);
   assert( ! rval);

   assert( _mulle__pointermap_is_sparse( map));

   _mulle__pointermap_shrink_generic( map, &mulle__pointermap_keyvaluecallback, NULL);
   mulle__pointermap_destroy( map, NULL);
}


static void  enumerate_pairs( void)
{
   struct mulle__pointermapenumerator   rover;
   void                                 *key;
   void                                 *value;
   int                                  rval;
   int                                  i;
   struct mulle__pointermap             *map;
   struct mulle_pointerpair             pair;

   map = mulle__pointermap_create( 0, 0, NULL);

   mulle__pointermap_set( map, "VfL", "VFL", NULL);
   mulle__pointermap_set( map, "Bochum", "BOCHUM", NULL);
   mulle__pointermap_set( map, "1848", "1848", NULL);

   assert( mulle__pointermap_get_count( map) == 3);

   rover = mulle__pointermap_enumerate( map);
   for( i = 0; i < 3; i++)
   {
      rval  = _mulle__pointermapenumerator_next_pair( &rover, &pair);
      assert( rval == 1);
      if( ! strcmp( "VfL", pair.key))
      {
         assert( ! strcmp( "VFL", pair.value));
         continue;
      }
      if( ! strcmp( "Bochum", pair.key))
      {
         assert( ! strcmp( "BOCHUM", pair.value));
         continue;
      }
      if( ! strcmp( "1848", pair.key))
      {
         assert( ! strcmp( "1848", pair.value));
         continue;
      }
   }
   mulle__pointermapenumerator_done( &rover);

   rval  = _mulle__pointermapenumerator_next_pair( &rover, &pair);
   assert( ! rval);
   assert( mulle_pointerpair_is_invalid( pair));

   mulle__pointermap_destroy( map, NULL);
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


int   main( int argc, char * argv[])
{
   run_test( create, "create");
   run_test( simple, "simple");
   run_test( enumerate, "enumerate");
   run_test( enumerate_pairs, "enumerate_pairs");
   run_test( map_reset, "reset");
   run_test( map_remove, "remove");
   run_test( map_copy, "copy");

   return( 0);
}
