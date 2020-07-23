#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   random_key( char  key[ 5])
{
   key[ 0] = rand() % 10 + '0';
   key[ 1] = rand() % 10 + '0';
   key[ 2] = rand() % 10 + '0';
   key[ 3] = rand() % 10 + '0';
   key[ 4] = 0;
}


struct mulle_container_keyvaluecallback   _CALLBACK;

#define CALLBACK   &_CALLBACK

static void   simple_fill_test()
{
   char                       key[ 5];
   struct mulle__map          *bucket;
   unsigned int               i;
   struct mulle_pointerpair   pair;

   bucket = _mulle__map_create( 128, 0, CALLBACK, NULL);

   printf( "%ld\n", (long) _mulle__map_get_count( bucket));

   pair.key   = key;
   pair.value = key;

   for( i = 0; i < 100000; i++)
   {
      random_key( key);
      _mulle__map_set( bucket, &pair, CALLBACK, NULL);
   }

   _mulle__map_reset( bucket, CALLBACK, NULL);

   printf( "%ld\n", (long) _mulle__map_get_count( bucket));
   _mulle__map_set( bucket, &pair, CALLBACK, NULL);
   printf( "%ld\n", (long) _mulle__map_get_count( bucket));

   _mulle__map_destroy( bucket, CALLBACK, NULL);
}



int  main()
{
   _CALLBACK.keycallback   = mulle_container_keycallback_copied_cstring;
   _CALLBACK.valuecallback = mulle_container_valuecallback_copied_cstring;

   mulle_testallocator_initialize();

   mulle_default_allocator = mulle_testallocator;

   simple_fill_test();

   mulle_testallocator_reset();

   return( 0);
}

