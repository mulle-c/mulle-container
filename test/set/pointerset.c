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
   struct mulle_pointerset   *set;

   set = mulle_pointerset_create( NULL);

   assert( set != NULL);
   assert( mulle_pointerset_get_count( set) == 0);

   mulle_pointerset_destroy( set);
}


static void   set_null( void)
{
   mulle_pointerset_init( NULL, 10, NULL);

   mulle_pointerset_set( NULL, "foo");

   assert( mulle_pointerset_get_count( NULL) == 0);
   assert( mulle_pointerset_get_size( NULL) == 0);

   mulle_pointerset_remove( NULL, "bar");

   mulle_pointerset_done( NULL);
}



static void  simple( void)
{
   char   onstack[] = { 'V', 'f', 'L', 0 };

   assert( mulle_pointerset_get_count( NULL) == 0);

   mulle_pointerset_do( set)
   {
      assert( mulle_pointerset_get_count( set) == 0);
      assert( ! mulle_pointerset_get( set, "VfL"));

      mulle_pointerset_set( set, "VfL");
      assert( ! strcmp( "VfL", mulle_pointerset_get( set, "VfL")));

      mulle_pointerset_set( set, "VfL");
      assert( ! strcmp( "VfL", mulle_pointerset_get( set, "VfL")));

      assert( ! mulle_pointerset_get( set, "Whatever"));

      mulle_pointerset_remove( set, onstack);  // can't match
      assert( ! strcmp( "VfL", mulle_pointerset_get( set, "VfL")));
   }
}

static void   set_remove( void)
{
   struct mulle_pointerset   *set;


   set = mulle_pointerset_create( NULL);

   mulle_pointerset_set( set, "VfL");
   mulle_pointerset_set( set, "Bochum");
   mulle_pointerset_set( set, "1848");

   mulle_pointerset_remove( set, "Bochum");
   assert( mulle_pointerset_get_count( set) == 2);

   mulle_pointerset_remove( set, "Bochum");
   assert( mulle_pointerset_get_count( set) == 2);

   mulle_pointerset_remove( set, "1848");
   assert( mulle_pointerset_get_count( set) == 1);

   mulle_pointerset_remove( set, "VfL");
   assert( mulle_pointerset_get_count( set) == 0);

   mulle_pointerset_destroy( set);
}


static void   set_copy( void)
{
   struct mulle_pointerset   *set;
   struct mulle_pointerset   *copy;
   struct mulle__set         *copy2;

   set = mulle_pointerset_create( NULL);

   // NULL
   copy = mulle_pointerset_copy( NULL);
   mulle_pointerset_destroy( copy);

   // empty
   copy = mulle_pointerset_copy( set);
   mulle_pointerset_destroy( copy);

   mulle_pointerset_set( set, "VfL");
   mulle_pointerset_set( set, "Bochum");
   mulle_pointerset_set( set, "1848");

   copy = mulle_pointerset_copy( set);
   assert( mulle_pointerset_get_count( copy) == 3);

   mulle_pointerset_destroy( copy);
   mulle_pointerset_destroy( set);
}


static void   misc( void)
{
   struct mulle__pointerset  *set;
   struct mulle__pointerset  *copy;

   set = _mulle__pointerset_create( 0, 0, NULL);
   _mulle__pointerset_destroy( set, NULL);

   set   = _mulle__pointerset_create( 1000, 0, NULL);
   assert( _mulle__pointerset_get( set , NULL) == NULL);

   copy = _mulle__pointerset_copy( set, NULL);

   _mulle__pointerset_destroy( copy, NULL);
   _mulle__pointerset_destroy( set, NULL);
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
   run_test( set_remove, "remove");
   run_test( set_copy, "copy");
   run_test( misc, "misc");

   return( 0);
}
