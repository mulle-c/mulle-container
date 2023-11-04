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


static void  stress1( void)
{
   struct mulle_assoc                        *assoc;
   void                                      *key;
   void                                      *value;
   intptr_t                                  i;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_assocenumerator              rover;

   callback.keycallback   = mulle_container_keycallback_int;
   callback.valuecallback = mulle_container_valuecallback_int;

   assoc = mulle_assoc_create( 0, &callback, NULL, NULL);

   for( i = 1; i <= 10000; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x7)
      {
      default :
         mulle_assoc_set( assoc, (void *) i, (void *) -i);
         break;

      case 6 :
         mulle_assoc_remove( assoc, key);
         break;

      case 7 :
         value = mulle_assoc_get( assoc, (void *) key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   rover = mulle_assoc_enumerate( assoc);
   while( mulle_assocenumerator_next( &rover, &key, &value))
   {
      assert( ! value || (intptr_t) value == - (intptr_t) key);
   }
   mulle_assocenumerator_done( &rover);

   for( i = 1; i <= 10000; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x7)
      {
      case 0 :
      case 1 :
         mulle_assoc_set( assoc, (void *) i, (void *) -i);
         break;

      default :
         mulle_assoc_remove( assoc, key);
         break;

      case 3 :
         value = mulle_assoc_get( assoc, (void *) key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   mulle_assoc_destroy( assoc);
}


static void  stress2( void)
{
   struct mulle_assoc                           *assoc;
   void                                         *key;
   void                                         *value;
   intptr_t                                     i;
   struct mulle_container_keyvaluecallback      callback;

   callback.keycallback   = mulle_container_keycallback_nonowned_pointer;
   callback.valuecallback = mulle_container_valuecallback_nonowned_pointer;

   assoc = mulle_assoc_create( 0, &callback, NULL, NULL);

   for( i = 1; i <= 12345; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x3)
      {
      default :
         mulle_assoc_set( assoc, key, (void *) - (intptr_t) key);
         break;

      case 2 :
         mulle_assoc_remove( assoc, key);
         break;

      case 3 :
         value = mulle_assoc_get( assoc, key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   mulle_assoc_for( assoc, key, value)
   {
      assert( ! value || (intptr_t) value == - (intptr_t) key);
   }

   for( i = 1; i <= 12345; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x3)
      {
      case 2 :
         mulle_assoc_set( assoc, key, (void *) - (intptr_t) key);
         break;

      default :
         mulle_assoc_remove( assoc, key);
         break;

      case 3 :
         value = mulle_assoc_get( assoc, key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   mulle_assoc_destroy( assoc);
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
   run_test( stress2);
   return( 0);
}
