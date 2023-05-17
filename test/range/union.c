#include <mulle-container/mulle-container.h>

#include <stdio.h>

static void   print_range( struct mulle_range   range)
{
   if( ! range.length || range.length > mulle_range_location_max)
      printf( "[%ld,%ld]",
         (long) range.location, (long) range.length);
   else
      printf( "[%ld,..,%ld]",
         (long) range.location, (long) range.location + (long) range.length - 1);
}


int   main( int argc, char *argv[])
{
   struct mulle_range   range1;
   struct mulle_range   range2;
   struct mulle_range   range3;

   for( range1.length = -1; (long) range1.length < 3; range1.length++)
      for( range1.location = -1; (long) range1.location < 3; range1.location++)
         for( range2.length = -1; (long) range2.length < 3; range2.length++)
            for( range2.location = -1; (long) range2.location < 3; range2.location++)
            {
               if( mulle_range_is_valid( range1) && mulle_range_is_valid( range2))
               {
                  range3 = mulle_range_union( range1, range2);
                  print_range( range1);
                  printf( " union ");
                  print_range( range2);
                  printf( " = ");
                  print_range( range3);
                  printf( "\n");
               }
            }

   return( 0);
}
