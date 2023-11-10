#define NO_MULLE_THREAD_UNPLEASANT_RACE_YIELD
#include <mulle-container/mulle-container.h>

#include <stdio.h>


static struct mulle_range   randomRange( void)
{
   return( mulle_range_make( rand() % 100000, rand() % 1000));
}


int   main( int argc, char *argv[])
{
   struct mulle__rangeset   set;
   uintptr_t                i;
   struct mulle_range       range;

   _mulle__rangeset_init( &set, 0, NULL);

   for( i = 0; i < 1000000; i++)
   {
      if( i % 100000 == 98765)
         _mulle__rangeset_reset( &set, NULL);

      if( i % 1000 == 999)
         fputc( '.', stderr);
      switch( rand() & 0x7)
      {
      case 0 :
      case 1 :
      case 2 :
         _mulle__rangeset_remove( &set, randomRange(),  NULL);
         break;

      default :
         _mulle__rangeset_insert( &set, randomRange(),  NULL);
      }
   }
   fputc( '\n', stderr);

   _mulle__rangeset_done( &set, NULL);

   return( 0);
}
