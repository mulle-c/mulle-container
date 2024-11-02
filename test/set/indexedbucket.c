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


#define CALLBACK   &mulle_container_keycallback_copied_cstring

static void   simple_fill_test()
{
   char                key[ 5];
   struct mulle__set   *set;
   size_t        i;

   set = _mulle__set_create( 128, 0, CALLBACK, NULL);

   printf( "%ld\n", (long) _mulle__set_get_count( set));

   for( i = 0; i < 100000; i++)
   {
      random_key( key);
      _mulle__set_set( set, key, CALLBACK, NULL);
   }

   _mulle__set_reset( set, CALLBACK, NULL);

   printf( "%ld\n", (long) _mulle__set_get_count( set));
   _mulle__set_set( set, key, CALLBACK, NULL);
   printf( "%ld\n", (long) _mulle__set_get_count( set));
   _mulle__set_destroy( set, CALLBACK, NULL);
}



int  main()
{
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   simple_fill_test();

   mulle_testallocator_reset();

   return( 0);
}

