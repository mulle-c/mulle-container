#define NO_MULLE_THREAD_UNPLEASANT_RACE_YIELD
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


static void  stress1( void)
{
   struct mulle_map                             *map;
   void                                         *key;
   void                                         *value;
   intptr_t                                     i;
   struct mulle_container_keyvaluecallback      callback;
   struct mulle_mapenumerator                   rover;

   callback.keycallback   = mulle_container_keycallback_int;
   callback.valuecallback = mulle_container_valuecallback_int;

   map = mulle_map_create( 0, &callback, NULL);

   for( i = 1; i <= 100000; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x7)
      {
      default :
         mulle_map_set( map, (void *) i, (void *) -i);
         break;

      case 6 :
         mulle_map_remove( map, key);
         break;

      case 7 :
         value = mulle_map_get( map, (void *) key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   rover = mulle_map_enumerate( map);
   while( mulle_mapenumerator_next( &rover, &key, &value))
   {
      assert( ! value || (intptr_t) value == - (intptr_t) key);
   }
   mulle_mapenumerator_done( &rover);

   for( i = 1; i <= 100000; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x7)
      {
      case 0 :
      case 1 :
         mulle_map_set( map, (void *) i, (void *) -i);
         break;

      default :
         mulle_map_remove( map, key);
         break;

      case 3 :
         value = mulle_map_get( map, (void *) key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   mulle_map_destroy( map);
}


static void  stress2( void)
{
   struct mulle_map                             *map;
   void                                         *key;
   void                                         *value;
   intptr_t                                     i;
   struct mulle_container_keyvaluecallback      callback;
   struct mulle_maptinyenumerator               rover;

   callback.keycallback   = mulle_container_keycallback_nonowned_pointer;
   callback.valuecallback = mulle_container_valuecallback_nonowned_pointer;

   map = mulle_map_create( 0, &callback, NULL);

   for( i = 1; i <= 123456; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x3)
      {
      default :
         mulle_map_set( map, key, (void *) - (intptr_t) key);
         break;

      case 2 :
         mulle_map_remove( map, key);
         break;

      case 3 :
         value = mulle_map_get( map, key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

   rover = mulle_map_tinyenumerate_nil( map);
   while( mulle_maptinyenumerator_next( &rover, &key, &value))
   {
      assert( ! value || (intptr_t) value == - (intptr_t) key);
   }
   mulle_maptinyenumerator_done( &rover);

   for( i = 1; i <= 123456; i++)
   {
      key = (void *) (rand() % (i + (i >> 2)) + 1);

      switch( i & 0x3)
      {
      case 2 :
         mulle_map_set( map, key, (void *) - (intptr_t) key);
         break;

      default :
         mulle_map_remove( map, key);
         break;

      case 3 :
         value = mulle_map_get( map, key);
         assert( ! value || (intptr_t) value == - (intptr_t) key);
         break;
      }
   }

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
   run_test( stress1);
   run_test( stress2);
   return( 0);
}
