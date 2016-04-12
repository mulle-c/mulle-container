#include <mulle_standalone_container/mulle_standalone_container.h>
#include <mulle_test_allocator/mulle_test_allocator.h>

#include <stdio.h>



static void   simple_fill_test()
{
   struct mulle_buffer   *buffer;
   unsigned int           i;
   size_t                 len;
   
   buffer = mulle_buffer_create( NULL);

   printf( "simple_fill_test: %s\n", mulle_buffer_has_overflown( buffer) ? "YES" : "NO");
   printf( "simple_fill_test: %s\n", mulle_buffer_is_empty( buffer) ? "YES" : "NO");
   printf( "simple_fill_test: %s\n", mulle_buffer_is_void( buffer) ? "YES" : "NO");

   for( i = 0; i < 100000; i++)
      mulle_buffer_add_byte( buffer, 'a' + i % 26);
   printf( "simple_fill_test: %ld\n", mulle_buffer_get_length( buffer));

   mulle_buffer_memset( buffer, 'z', 1000);
   mulle_buffer_zero_last_byte( buffer);
   
   len = strlen( mulle_buffer_get_bytes( buffer));
   printf( "simple_fill_test: %ld\n", len);

   printf( "simple_fill_test: %ld\n", mulle_hash( mulle_buffer_get_bytes( buffer),
                                                  len));

   mulle_buffer_add_string( buffer, "hello");

   printf( "simple_fill_test: %ld\n", mulle_buffer_get_length( buffer));
   
   printf( "simple_fill_test: %ld\n", mulle_buffer_get_static_bytes_length( buffer));


   printf( "simple_fill_test: %s\n", mulle_buffer_has_overflown( buffer) ? "YES" : "NO");
   printf( "simple_fill_test: %s\n", mulle_buffer_is_empty( buffer) ? "YES" : "NO");
   printf( "simple_fill_test: %s\n", mulle_buffer_is_void( buffer) ? "YES" : "NO");

   mulle_buffer_reset( buffer);
   printf( "simple_fill_test: %ld\n", mulle_buffer_get_length( buffer));

   mulle_buffer_free( buffer);
}


static void   simple_static_test()
{
   struct mulle_buffer   buffer;
   
   mulle_buffer_init_inflexable_with_static_bytes( &buffer,
                                                   "hello world",
                                                   12);

   printf( "simple_static_test: %s\n", mulle_buffer_is_full( &buffer) ? "YES" : "NO");
   printf( "simple_static_test: %s\n", mulle_buffer_has_overflown( &buffer) ? "YES" : "NO");
   printf( "simple_static_test: %s\n", mulle_buffer_is_empty( &buffer) ? "YES" : "NO");
   printf( "simple_static_test: %s\n", mulle_buffer_is_void( &buffer) ? "YES" : "NO");

   mulle_buffer_add_byte( &buffer, 'x');

   printf( "simple_static_test: %s\n", mulle_buffer_is_full( &buffer) ? "YES" : "NO");
   printf( "simple_static_test: %s\n", mulle_buffer_has_overflown( &buffer) ? "YES" : "NO");
   printf( "simple_static_test: %s\n", mulle_buffer_is_empty( &buffer) ? "YES" : "NO");
   printf( "simple_static_test: %s\n", mulle_buffer_is_void( &buffer) ? "YES" : "NO");

   mulle_buffer_memset( &buffer, 'y', 3);

   printf( "simple_static_test: %ld\n", mulle_buffer_get_length( &buffer));
   printf( "simple_static_test: %s\n", mulle_buffer_get_bytes( &buffer));
   printf( "simple_static_test: %ld\n", mulle_buffer_get_static_bytes_length( &buffer));

   mulle_buffer_reset( &buffer);
   printf( "simple_static_test: %ld\n", mulle_buffer_get_length( &buffer));

   mulle_buffer_done( &buffer);
}


int  main()
{
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   simple_fill_test();
   mulle_test_allocator_reset();
   simple_static_test();

   mulle_test_allocator_reset();
}

