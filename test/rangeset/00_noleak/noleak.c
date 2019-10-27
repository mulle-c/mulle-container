#include <mulle-container/mulle-container.h>


int   main( int argc, char *argv[])
{
   struct _mulle_rangeset   set;

   _mulle_rangeset_init( &set, 100, NULL);
   _mulle_rangeset_done( &set, NULL);
   return( 0);
}
