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

// A pointer map identifies its keys by address, not by content. Identical
// string literals are not guaranteed to be merged (tcc does not merge them),
// so a key needs storage with a stable address for as long as it is in the
// map. Literals would work here only because gcc and clang happen to merge
// them.
static char   vfl[]       = "VfL";
static char   vfl_upper[] = "VFL";
static char   bochum[]    = "Bochum";
static char   year[]      = "1848";


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
   int    state;

   mulle__pointermap_do( map)
   {
      assert( mulle__pointermap_get_count( map) == 0);
      assert( ! mulle__pointermap_get( map, vfl));

      mulle__pointermap_set( map, vfl, "VFL", NULL);
      assert( ! mulle__pointermap_get( map, vfl_upper));
      assert( ! strcmp( "VFL", mulle__pointermap_get( map, vfl)));

      state = mulle__pointermap_insert( map, vfl, "xxx", NULL);
      assert( ! state);
      assert( ! strcmp( "VFL", mulle__pointermap_get( map, vfl)));

      mulle__pointermap_set( map, vfl, "BOCHUM", NULL);
      assert( ! strcmp( "BOCHUM", mulle__pointermap_get( map, vfl)));

      // should NOT  remove
      mulle__pointermap_remove( map, onstack, NULL);
      assert( mulle__pointermap_get( map, vfl));
   }
}


static void   map_reset( void)
{
   struct mulle__pointermap    *map;

   map = mulle__pointermap_create( 0, 9, NULL);

   mulle__pointermap_set( map, vfl, "VFL", NULL);
   mulle__pointermap_set( map, bochum, "BOCHUM", NULL);
   mulle__pointermap_set( map, year, "1848", NULL);

   mulle__pointermap_reset( map, NULL);
   assert( mulle__pointermap_get_count( map) == 0);

   mulle__pointermap_destroy( map, NULL);
}



static void   map_remove( void)
{
   struct mulle__pointermap   *map;

   map = mulle__pointermap_create( 3, 0, NULL);

   mulle__pointermap_set( map, vfl, "VFL", NULL);
   mulle__pointermap_set( map, bochum, "BOCHUM", NULL);
   mulle__pointermap_set( map, year, "1848", NULL);

   mulle__pointermap_remove( map, bochum, NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, bochum, NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, year, NULL);
   assert( mulle__pointermap_get_count( map) == 1);

   mulle__pointermap_remove( map, vfl, NULL);
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

   mulle__pointermap_set( map, vfl, "VFL", NULL);
   mulle__pointermap_set( map, bochum, "BOCHUM", NULL);
   mulle__pointermap_set( map, year, "1848", NULL);

   copy = _mulle__pointermap_copy( map, NULL);
   mulle__pointermap_destroy( copy, NULL);

   mulle__pointermap_remove( map, bochum, NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, bochum, NULL);
   assert( mulle__pointermap_get_count( map) == 2);

   mulle__pointermap_remove( map, year, NULL);
   assert( mulle__pointermap_get_count( map) == 1);

   mulle__pointermap_remove( map, vfl, NULL);
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

   map = mulle__pointermap_create( 100, 0, NULL);

   mulle__pointermap_set( map, vfl, "VFL", NULL);
   mulle__pointermap_set( map, bochum, "BOCHUM", NULL);
   mulle__pointermap_set( map, year, "1848", NULL);

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
   int                                  rval;
   int                                  i;
   struct mulle__pointermap             *map;
   struct mulle_pointerpair             pair;

   map = mulle__pointermap_create( 0, 0, NULL);

   mulle__pointermap_set( map, vfl, "VFL", NULL);
   mulle__pointermap_set( map, bochum, "BOCHUM", NULL);
   mulle__pointermap_set( map, year, "1848", NULL);

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


// the mulle_allocator_default detects and aborts on leaks
static void   run_test( void (*f)( void), char *name)
{
#ifdef DEBUG
   fprintf( stderr, "%s\n", name);
#endif
   (f)();
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
