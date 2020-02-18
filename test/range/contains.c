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
   struct mulle_range   range1;
   struct mulle_range   range2;
   unsigned int         i, j;
   unsigned int         k, l;

   for( l = 0; l < 6; l++)
   {
      for( k = 0; k < 6; k++)
      {
         for( j= 0; j < 6; j++)
         {
            for( i = 0; i < 6; i++)
            {
               range1.location = values[ i];
               range1.length   = values[ j];
               range2.location = values[ k];
               range2.length   = values[ l];


               if( ! mulle_range_is_valid( range1))
                  continue;
               if( ! mulle_range_is_valid( range2))
                  continue;

               printf( "[");
               print_value( range1.location);
               printf( " - ");
               print_value( range1.length);
               printf( "] contains [");
               print_value( range2.location);
               printf( " - ");
               print_value( range2.length);
               printf( "] : %s\n",
                  mulle_range_contains( range1, range2) ? "YES" : "NO");
            }
         }
         printf( "\n");
      }
      printf( "\n");
   }
   return( 0);
}
