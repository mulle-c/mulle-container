#define NO_MULLE_THREAD_UNPLEASANT_RACE_YIELD
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
# undef NDEBUG
#endif
#include <assert.h>

#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>
#include <assert.h>


static void  stress1( void)
{
   struct mulle_set                        *set;
   void                                    *key;
   void                                    *value;
   intptr_t                                i;
   struct mulle_container_keycallback      callback;
   struct mulle_setenumerator              rover;

   callback = mulle_container_keycallback_int;

   set = mulle_set_create( 0, &callback, NULL);

   for( i = 1; i <= 100000; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x7)
      {
      default :
         mulle_set_set( set, key);
         break;

      case 6 :
         mulle_set_remove( set, key);
         break;

      case 7 :
         mulle_set_get( set, key);
         break;
      }
   }

   rover = mulle_set_enumerate( set);
   while( mulle_setenumerator_next( &rover, &key))
   {
   }
   mulle_setenumerator_done( &rover);

   for( i = 1; i <= 100000; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x7)
      {
      case 0 :
      case 1 :
         mulle_set_set( set, key);
         break;

      default :
         mulle_set_remove( set, key);
         break;

      case 3 :
         mulle_set_get( set, key);
         break;
      }
   }

   mulle_set_destroy( set);
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
   run_test( stress1);
   return( 0);
}
