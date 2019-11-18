#include <mulle-container/mulle-container.h>

#include <stdio.h>

static uintptr_t   values[6] =
{
   0,
   1,
   2,
   mulle_range_max,
   mulle_not_found_e,
   -1
};


static void   print_value( uintptr_t value)
{
   if( value == mulle_range_max)
      printf( "mulle_range_max");
   else
      if( value == mulle_not_found_e)
         printf( "mulle_not_found_e");
      else
         printf( "%ld", (long) value);
}


int   main( int argc, char *argv[])
{
   struct mulle_range   range;
   unsigned int         i, j;

   for( j= 0; j < 6; j++)
      for( i = 0; i < 6; i++)
      {
         range.location = values[ i];
         range.length   = values[ j];

         printf( "[");
         print_value( range.location);
         printf( ",");
         print_value( range.length);
         printf( "] : %s\n",
            mulle_range_is_valid( range) ? "YES" : "NO");
      }
  return( 0);
}
