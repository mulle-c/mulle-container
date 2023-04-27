//
//  main.c
//  test-assoc
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
   struct mulle_assoc                             *assoc;
   struct mulle_container_keyvaluecallback      callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   assert( assoc != NULL);
   assert( mulle_assoc_get_count( assoc) == 0);

   mulle_assoc_destroy( assoc);
}


static void   assoc_null( void)
{
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_assocenumerator                rover;
   int                                       rval;
   void                                      *key;
   void                                      *value;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_assoc_init( NULL, 10, &callback, _mulle_pointerpair_compare_string_key, NULL);

   mulle_assoc_set( NULL, "foo", "bar");

   assert( mulle_assoc_get_count( NULL) == 0);
   assert( mulle_assoc_get_size( NULL) == 0);

   mulle_assoc_remove( NULL, "bar");

   assert( mulle_assoc_get_count( NULL) == 0);

   rover = mulle_assoc_enumerate( NULL);
   rval  = mulle_assocenumerator_next( &rover, &key, &value);
   assert( rval == 0);
   assert( key == mulle_assoc_get_notakey( NULL));
   assert( ! value);
   mulle_assocenumerator_done( &rover);

   mulle_assoc_done( NULL);
}


static void  simple( void)
{
   struct mulle_assoc              *assoc;
   struct mulle_assocenumerator    rover;
   void                          *key;
   void                          *value;
   int                           rval;
   int                           i;
   int                           state;
   struct mulle_container_keyvaluecallback      callback;
   char                          onstack[] = { 'V', 'f', 'L', 0 };

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   assert( mulle_assoc_get_count( assoc) == 0);
   assert( ! mulle_assoc_get( assoc, "VfL"));

   mulle_assoc_set( assoc, "VfL", "VFL");
   assert( ! mulle_assoc_get( assoc, "VFL"));
   assert( ! strcmp( "VFL", mulle_assoc_get( assoc, "VfL")));

   mulle_assoc_set( assoc, "VfL", "BOCHUM");
   assert( ! strcmp( "BOCHUM", mulle_assoc_get( assoc, "VfL")));

   mulle_assoc_remove( assoc, onstack);
   assert( ! mulle_assoc_get( assoc, "VfL"));

   mulle_assoc_destroy( assoc);
}


static void   assoc_reset( void)
{
   struct mulle_assoc                          *assoc;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   mulle_assoc_set( assoc, "VfL", "VFL");
   mulle_assoc_set( assoc, "Bochum", "BOCHUM");
   mulle_assoc_set( assoc, "1848", "1848");

   mulle_assoc_reset( assoc);
   assert( mulle_assoc_get_count( assoc) == 0);

   mulle_assoc_destroy( assoc);
}



static void   assoc_remove( void)
{
   struct mulle_assoc                          *assoc;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   mulle_assoc_set( assoc, "VfL", "VFL");
   mulle_assoc_set( assoc, "Bochum", "BOCHUM");
   mulle_assoc_set( assoc, "1848", "1848");

   mulle_assoc_remove( assoc, "Bochum");
   assert( mulle_assoc_get_count( assoc) == 2);

   mulle_assoc_remove( assoc, "Bochum");
   assert( mulle_assoc_get_count( assoc) == 2);

   mulle_assoc_remove( assoc, "1848");
   assert( mulle_assoc_get_count( assoc) == 1);

   mulle_assoc_remove( assoc, "VfL");
   assert( mulle_assoc_get_count( assoc) == 0);

   mulle_assoc_destroy( assoc);
}


static void   assoc_add_assoc( void)
{
   struct mulle_assoc                          *assoc;
   struct mulle_assoc                          *add;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   mulle_assoc_set( assoc, "VfL", "VFL");
   mulle_assoc_set( assoc, "Bochum", "BOCHUM");
   mulle_assoc_set( assoc, "1848", "1848");

   add = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);
   mulle_assoc_set( add, "Foo", "foo");
   mulle_assoc_set( add, "VfL", "VFL");

   mulle_assoc_add_assoc( assoc, add);

   assert( mulle_assoc_get_count( assoc) == 4);
   assert( mulle_assoc_get_count( add) == 2);

   mulle_assoc_destroy( assoc);
   mulle_assoc_destroy( add);
}


static void   assoc_copy( void)
{
   struct mulle_assoc                          *assoc;
   struct mulle_assoc                          *copy;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   mulle_assoc_set( assoc, "VfL", "VFL");
   mulle_assoc_set( assoc, "Bochum", "BOCHUM");
   mulle_assoc_set( assoc, "1848", "1848");

   copy = mulle_assoc_copy( assoc);

   assert( mulle_assoc_get_count( copy) == 3);

   mulle_assoc_destroy( assoc);
   mulle_assoc_destroy( copy);
}


static void  enumerate( void)
{
   struct mulle_assoc                          *assoc;
   struct mulle_assocenumerator                rover;
   void                                      *key;
   void                                      *value;
   int                                       rval;
   int                                       i;
   int                                       state;
   struct mulle_container_keyvaluecallback   callback;

   callback.keycallback   = mulle_container_keycallback_copied_cstring;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_string_key, NULL);

   mulle_assoc_set( assoc, "VfL", "VFL");
   mulle_assoc_set( assoc, "Bochum", "BOCHUM");
   mulle_assoc_set( assoc, "1848", "1848");

   assert( mulle_assoc_get_count( assoc) == 3);

   state = 0;
   rover = mulle_assoc_enumerate( assoc);
   for( i = 0; i < 3; i++)
   {
      rval  = mulle_assocenumerator_next( &rover, &key, &value);
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

   rval  = mulle_assocenumerator_next( &rover, &key, &value);
   assert( ! rval);
   mulle_assocenumerator_done( &rover);

   assert( mulle_assoc_get_count( assoc) == 3);
   mulle_assoc_remove( assoc, "xxx");
   assert( mulle_assoc_get_count( assoc) == 3);
   mulle_assoc_remove( assoc, "VfL");
   assert( mulle_assoc_get_count( assoc) == 2);
   mulle_assoc_remove( assoc, "Bochum");
   assert( mulle_assoc_get_count( assoc) == 1);
   mulle_assoc_remove( assoc, "1848");
   assert( mulle_assoc_get_count( assoc) == 0);

   mulle_assoc_destroy( assoc);
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
   run_test( assoc_null, "assoc_null");
   run_test( create, "create");
   run_test( simple, "simple");
   run_test( enumerate, "enumerate");
   run_test( assoc_reset, "reset");
   run_test( assoc_remove, "remove");
   run_test( assoc_copy, "copy");
   run_test( assoc_add_assoc, "add_assoc");

   return( 0);
}
