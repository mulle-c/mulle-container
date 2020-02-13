#include <mulle-container/mulle-container.h>


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;

   _mulle__rangeset_init( &set, 100, NULL);
   _mulle__rangeset_done( &set, NULL);
   return( 0);
}
