#include <mulle-container/mulle-container.h>
#include <stdio.h>

static struct mulle_range random_range(void)
{
   return( mulle_range_make( rand() % 100000, rand() % 1000));
}

static void *random_value(void)
{
   return( (void *) (uintptr_t) (rand() % 1000 + 1));
}

int main(int argc, char *argv[])
{
   struct mulle__rangemap   map;
   uintptr_t               i;
   struct mulle_range      range;
   void                    *value;
   void                    *merged;

   _mulle__rangemap_init( &map, 0, NULL);

   for( i = 0; i < 100000; i++)
   {
      if( i % 10000 == 9999)
         _mulle__rangemap_reset( &map, NULL);

      if( i % 1000 == 999)
         fputc( '.', stderr);

      switch( rand() & 0x7)
      {
      case 0:
      case 1:
      case 2:
         range = random_range();
         __mulle__rangemap_remove( &map, range, NULL, NULL, NULL);
         break;

      default:
         range = random_range();
         value = random_value();
         _mulle__rangemap_insert( &map, range, value, &merged, NULL);
      }
   }
   fputc( '\n', stderr);

   _mulle__rangemap_done( &map, NULL);

   return( 0);
}
