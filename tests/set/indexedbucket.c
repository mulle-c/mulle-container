#include <mulle_standalone_container/mulle_standalone_container.h>
#include <mulle_test_allocator/mulle_test_allocator.h>

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
   char                          key[ 5];
   struct _mulle_indexedbucket   *bucket;
   unsigned int                  i;
   
   bucket = _mulle_indexedbucket_create( 128, 0, CALLBACK, NULL);
   
   printf( "%ld\n", _mulle_indexedbucket_get_count( bucket));

   for( i = 0; i < 100000; i++)
   {
      random_key( key);
      _mulle_indexedbucket_put( bucket, key, CALLBACK, NULL);
   }

   _mulle_indexedbucket_reset( bucket, CALLBACK, NULL);

   printf( "%ld\n", _mulle_indexedbucket_get_count( bucket));
   _mulle_indexedbucket_put( bucket, key, CALLBACK, NULL);
   printf( "%ld\n", _mulle_indexedbucket_get_count( bucket));
   _mulle_indexedbucket_free( bucket, CALLBACK, NULL);
}



int  main()
{
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   simple_fill_test();

   mulle_test_allocator_reset();
}

