//
//  main.c
//  test-set
//
//  Created by Nat! on 05.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>


static void   create( void)
{
   struct mulle_set   *set;

   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   assert( set != NULL);
   assert( mulle_set_get_count( set) == 0);

   mulle_set_destroy( set);
}


static void   set_null( void)
{
   mulle_set_init( NULL, 10, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( NULL, "foo");

   assert( mulle_set_get_count( NULL) == 0);
   assert( mulle_set_get_size( NULL) == 0);

   mulle_set_remove( NULL, "bar");

   mulle_set_done( NULL);
}



static void  simple( void)
{
   struct mulle_container_keycallback  callback;
   char                                onstack[] = { 'V', 'f', 'L', 0 };

   callback = mulle_container_keycallback_copied_cstring;

   mulle_set_do( set, &callback)
   {
      assert( mulle_set_get_count( set) == 0);
      assert( ! mulle_set_get( set, "VfL"));

      mulle_set_set( set, "VfL");
      assert( ! strcmp( "VfL", mulle_set_get( set, "VfL")));

      mulle_set_set( set, "VfL");
      assert( ! strcmp( "VfL", mulle_set_get( set, "VfL")));

      mulle_set_remove( set, onstack);
      assert( ! mulle_set_get( set, "VfL"));
   }
}


static void  enumerate( void)
{
   struct mulle_set                        *set;
   struct mulle_setenumerator              rover;
   void                                    *key;
   int                                     rval;
   int                                     i;
   int                                     state;
   struct mulle_container_keycallback      callback;

   callback   = mulle_container_keycallback_copied_cstring;

   set = mulle_set_create( 0, &callback, NULL);

   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   state = 0;
   rover = mulle_set_enumerate( set);
   while( mulle_setenumerator_next( &rover, &key))
   {
      assert( key);
      if( ! strcmp( "VfL", key))
      {
         state |= 0x1;
         continue;
      }
      if( ! strcmp( "Bochum", key))
      {
         state |= 0x2;
         continue;
      }
      if( ! strcmp( "1848", key))
      {
         state |= 0x4;
         continue;
      }
   }
   assert( state == 0x7);

   assert( ! mulle_setenumerator_next( &rover, &key));
   mulle_setenumerator_done( &rover);

   // more or less just a syntax check
   i = 0;
   mulle_set_for( set, key)
   {
      assert( key);
      ++i;
   }
   assert( i == 3);

   assert( mulle_set_get_count( set) == 3);
   mulle_set_remove( set, "xxx");
   assert( mulle_set_get_count( set) == 3);
   mulle_set_remove( set, "VfL");
   assert( mulle_set_get_count( set) == 2);
   mulle_set_remove( set, "Bochum");
   assert( mulle_set_get_count( set) == 1);
   mulle_set_remove( set, "1848");
   assert( mulle_set_get_count( set) == 0);

   mulle_set_destroy( set);
}


static void   set_reset( void)
{
   struct mulle_set   *set;

   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   assert( mulle_set_get_count( set) == 3);
   mulle_set_reset( set);
   assert( mulle_set_get_count( set) == 0);

   mulle_set_destroy( set);
}



static void   set_remove( void)
{
   struct mulle_set   *set;


   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   mulle_set_remove( set, "Bochum");
   assert( mulle_set_get_count( set) == 2);

   mulle_set_remove( set, "Bochum");
   assert( mulle_set_get_count( set) == 2);

   mulle_set_remove( set, "1848");
   assert( mulle_set_get_count( set) == 1);

   mulle_set_remove( set, "VfL");
   assert( mulle_set_get_count( set) == 0);

   mulle_set_destroy( set);
}


static void   set_copy( void)
{
   struct mulle_set                          *set;
   struct mulle_set                          *copy;
   struct mulle__set                         *copy2;
   struct mulle_container_keyvaluecallback   callback;

   set = mulle_set_create( 1000, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( set, "VfL");
   mulle_set_set( set, "Bochum");
   mulle_set_set( set, "1848");

   copy = mulle_set_copy( set);
   assert( mulle_set_get_count( copy) == 3);
   mulle_set_destroy( copy);

   copy2 = _mulle__set_copy( (struct mulle__set *) set,
                             &mulle_container_keycallback_copied_cstring,
                             NULL);
   assert( _mulle__set_get_count( copy2) == 3);
   _mulle__set_destroy( copy2,
                        &mulle_container_keycallback_copied_cstring,
                        NULL);

   mulle_set_shrink_if_needed( set);

   mulle_set_destroy( set);
}


static void   describe( void)
{
   char   *s;

   mulle_set_do( set, &mulle_container_keycallback_copied_cstring)
   {
      s = mulle_set_describe( set);
      printf( "%s\n", s);
      mulle_free( s);

      mulle_set_insert( set, "VfL");
      mulle_set_insert( set, "Bochum");
      s = mulle_set_describe( set);
      printf( "%s\n", s);
      mulle_free( s);
   }
}


static void   set_add( void)
{
   struct mulle_set   *set;
   struct mulle_set   *other;

   set = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( set, "VfL");

   other = mulle_set_create( 0, &mulle_container_keycallback_copied_cstring, NULL);

   mulle_set_set( other, "Bochum");
   mulle_set_set( other, "1848");

   mulle_set_add_set( set, NULL);
   mulle_set_add_set( set, set);

   assert( mulle_set_get_count( set) == 1);
   mulle_set_add_set( set, other);
   assert( mulle_set_get_count( set) == 3);

   mulle_set_destroy( other);
   mulle_set_destroy( set);
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



int main(int argc, const char * argv[])
{
   run_test( set_null, "null");
   run_test( create, "create");
   run_test( simple, "simple");
   run_test( enumerate, "enumerate");
   run_test( set_reset, "reset");
   run_test( set_remove, "remove");
   run_test( set_copy, "copy");
   run_test( describe, "describe");
   run_test( set_add, "set_add");

   return( 0);
}
