#include <mulle-container/mulle-container.h>

#include <stdio.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;

   _mulle__rangeset_init( &set, 4, NULL);

   _mulle__rangeset_insert( &set, mulle_range_make( 0, 5), NULL);

   printf( "Before shrink_to_fit: size=%lu length=%lu\n",
           (unsigned long) set._size,
           (unsigned long) set._length);

   _mulle__rangeset_shrink_to_fit( &set, NULL);

   printf( "After shrink_to_fit: size=%lu length=%lu\n",
           (unsigned long) set._size,
           (unsigned long) set._length);

   // the container must remain usable after shrinking
   _mulle__rangeset_insert( &set, mulle_range_make( 30, 3), NULL);
   printf( "After re-insert: size=%lu length=%lu\n",
           (unsigned long) set._size,
           (unsigned long) set._length);
   _mulle__rangeset_print( &set);

   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
