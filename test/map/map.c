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
   struct mulle_map                             *map;
   struct mulle_container_keyvaluecallback      callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   assert( map != NULL);
   assert( mulle_map_get_count( map) == 0);

   mulle_map_destroy( map);
}


static void   map_null( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_mapenumerator                rover;
   int                                       rval;
   void                                      *key;
   void                                      *value;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_map_init( NULL, 10, &callback, NULL);

   mulle_map_set( NULL, "foo", "bar");

   assert( mulle_map_get_count( NULL) == 0);
   assert( mulle_map_get_size( NULL) == 0);

   mulle_map_remove( NULL, "bar");

   assert( mulle_map_get_count( NULL) == 0);

   rover = mulle_map_enumerate( NULL);
   rval  = mulle_mapenumerator_next( &rover, &key, &value);
   assert( rval == 0);
   assert( key == mulle_map_get_notakey( NULL));
   assert( ! value);
   mulle_mapenumerator_done( &rover);

   mulle_map_done( NULL);
}


static void  simple( void)
{
   struct mulle_container_keyvaluecallback   callback;
   char                                      onstack[] = { 'V', 'f', 'L', 0 };

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_map_do( map, &callback)
   {
      assert( mulle_map_get_count( map) == 0);
      assert( ! mulle_map_get( map, "VfL"));

      assert( ! mulle_map_get( map, NULL));

      mulle_map_set( map, "VfL", "VFL");
      assert( ! mulle_map_get( map, "VFL"));
      assert( ! strcmp( "VFL", mulle_map_get( map, "VfL")));

      mulle_map_set( map, "VfL", "BOCHUM");
      assert( ! strcmp( "BOCHUM", mulle_map_get( map, "VfL")));

      // should remove
      mulle_map_remove( map, onstack);
      assert( ! mulle_map_get( map, "VfL"));
   }
}


static void  insert( void)
{
   struct mulle_container_keyvaluecallback      callback;
   void    *value;
   int     state;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_map_do( map, &callback)
   {
      state = mulle_map_insert( map, "city", "Bochum");
      assert( state);
      state = mulle_map_insert( map, "city", "Witten");
      assert( ! state);
      value = mulle_map_get( map, "city");
      assert( value && ! strcmp( "Bochum", value));
   }
}


static void  do_register( void)
{
   struct mulle_container_keyvaluecallback   callback;
   void   *old;

   callback.keycallback   = mulle_container_keycallback_nonowned_cstring;
   callback.valuecallback = mulle_container_valuecallback_nonowned_cstring;

   mulle_map_do( map, &callback)
   {
      old = mulle_map_update( map, "city", "Bochum");
      assert( ! old);
      old = mulle_map_update( map, "city", "Witten");
      assert( old && ! strcmp( "Bochum", old));
      old = mulle_map_update( map, "city", "Hattingen");
      assert( old && ! strcmp( "Witten", old));
   }
}



static void  insert_argv( void)
{
   struct mulle_container_keyvaluecallback      callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_map_do( map, &callback)
   {
      mulle_map_insert_key_values( map, "city", "Bochum",
                                        "animal", "Beetle",
                                         NULL);
      assert( ! mulle_map_get( map, "Bochum"));
      assert( ! strcmp( "Bochum", mulle_map_get( map, "city")));
      assert( ! strcmp( "Beetle", mulle_map_get( map, "animal")));
   }
}


static void   map_reset( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   mulle_map_reset( map);
   assert( mulle_map_get_count( map) == 0);

   mulle_map_destroy( map);
}



static void   map_misc( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_pointerpair                  *any;
   struct mulle_pointerpair                  anySpace;
   struct mulle_pointerpair                  *p;
   struct mulle_pointerpair                  pSpace;
   void                                      *value;
   uintptr_t                                 hash;
   size_t                              perfects;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assert( ! mulle__map_is_sparse( NULL));

   map = mulle_map_create( 1000, &callback, NULL);

   any = _mulle__map_get_any_pair( (struct mulle__map *) map, &callback, &anySpace);
   assert( any == NULL);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   _mulle__map_count_collisions( (struct mulle__map *) map, &callback, NULL);

   any = _mulle__map_get_any_pair( (struct mulle__map *) map, &callback, &anySpace);
   assert( any);
   p   = _mulle__map__get_pair( (struct mulle__map *) map, any->key, &callback, &pSpace);
   assert( p->key == any->key);

   hash  = (*callback.keycallback.hash)( &callback.keycallback, any->key);
   value = _mulle__map__get_knownhash( (struct mulle__map *) map, any->key, hash, &callback);
   assert( value == any->value);

   _mulle__map_count_collisions( (struct mulle__map *) map, &callback, &perfects);
   _mulle__map_count_collisions( (struct mulle__map *) map, &callback, NULL);

   assert( mulle__map_is_sparse( (struct mulle__map *) map));
   mulle_map_shrink_if_needed( map);

   mulle_map_destroy( map);
}


static void   map_remove( void)
{
   struct mulle_map                          *map;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   mulle_map_remove( map, "Bochum");
   assert( mulle_map_get_count( map) == 0);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   mulle_map_remove( map, "Noone");
   assert( mulle_map_get_count( map) == 3);

   mulle_map_remove( map, "Bochum");
   assert( mulle_map_get_count( map) == 2);

   mulle_map_remove( map, "Bochum");
   assert( mulle_map_get_count( map) == 2);

   mulle_map_remove( map, "1848");
   assert( mulle_map_get_count( map) == 1);

   mulle_map_remove( map, "VfL");
   assert( mulle_map_get_count( map) == 0);

   mulle_map_destroy( map);
}


static void   map_add_map( void)
{
   struct mulle_map                          *map;
   struct mulle_map                          *add;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;


   map = mulle_map_create( 0, &callback, NULL);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   add = mulle_map_create( 0, &callback, NULL);
   mulle_map_set( add, "Foo", "foo");
   mulle_map_set( add, "VfL", "VFL");

   mulle_map_add_map( map, add);

   assert( mulle_map_get_count( map) == 4);
   assert( mulle_map_get_count( add) == 2);

   mulle_map_add_map( map, NULL);
   assert( mulle_map_get_count( map) == 4);

   mulle_map_add_map( map, map);
   assert( mulle_map_get_count( map) == 4);

   mulle_map_destroy( map);
   mulle_map_destroy( add);
}


static void   map_copy( void)
{
   struct mulle_map                          *map;
   struct mulle_map                          *copy;
   struct mulle__map                         *copy2;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   copy = mulle_map_copy( map);

   assert( mulle_map_get_count( copy) == 3);
   mulle_map_destroy( copy);

   copy2 = _mulle__map_copy( (struct mulle__map *) map,
                             &callback,
                             NULL);
   assert( _mulle__map_get_count( copy2) == 3);
   _mulle__map_destroy( copy2,
                        &callback,
                        NULL);

   mulle_map_destroy( map);
}


static void  enumerate( void)
{
   struct mulle_map                          *map;
   struct mulle_mapenumerator                rover;
   void                                      *key;
   void                                      *value;
   int                                       rval;
   int                                       i;
   int                                       state;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   map = mulle_map_create( 0, &callback, NULL);

   mulle_map_set( map, "VfL", "VFL");
   mulle_map_set( map, "Bochum", "BOCHUM");
   mulle_map_set( map, "1848", "1848");

   assert( mulle_map_get_count( map) == 3);

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


static void   describe( void)
{
   char   *s;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_map_do( map, &callback)
   {
      s = mulle_map_describe( map);
      printf( "%s\n", s);
      mulle_free( s);

      mulle_map_set( map, "Club", "VfL Bochum 1848");
      mulle_map_set( map, "Car", "VW Beetle");

      s = mulle_map_describe( map);
      printf( "%s\n", s);
      mulle_free( s);
   }
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
   run_test( map_null, "map_null");
   run_test( create, "create");
   run_test( simple, "simple");
   run_test( insert, "insert");
   run_test( insert_argv, "insert_argv");
   run_test( enumerate, "enumerate");
   run_test( do_register, "register");
   run_test( map_reset, "reset");
   run_test( map_remove, "remove");
   run_test( map_copy, "copy");
   run_test( map_add_map, "add_map");
   run_test( describe, "describe");
   run_test( map_misc, "misc");

   return( 0);
}
