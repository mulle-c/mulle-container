#include <mulle-container/mulle-container.h>

#include <stdio.h>


static uintptr_t   values[6] =
{
   0,
   1,
   2,
   mulle_range_location_max,
   mulle_not_found_e,
   -1
};


static void   print_value( uintptr_t value)
{
   if( value == mulle_range_location_max)
      printf( "mulle_range_location_max");
   else
      if( value == mulle_not_found_e)
         printf( "mulle_not_found_e");
      else
         printf( "%ld", (long) value);
}




int   main( int argc, char *argv[])
{
   struct mulle_range   range1;
   uintptr_t            location;
   unsigned int         i, j;
   unsigned int         k;

   for( k = 0; k < 6; k++)
   {
      for( j= 0; j < 6; j++)
      {
         for( i = 0; i < 6; i++)
         {
            range1.location = values[ i];
            range1.length   = values[ j];
            location        = values[ k];

            if( ! mulle_range_is_valid( range1))
               continue;

            printf( "[");
            print_value( range1.location);
            printf( " - ");
            print_value( range1.length);
            printf( "] contains ");
            print_value( location);
            printf( " : %s\n",
               mulle_range_contains_location( range1, location) ? "YES" : "NO");
         }
      }
      printf( "\n");
   }
   printf( "\n");

   return( 0);
}
