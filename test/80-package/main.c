#include <mulle-data/mulle-data.h>
#include <mulle-container/mulle-container.h>


// happy if it compiles...
int   main( void)
{
   struct mulle_pointerarray  *array;

   array = mulle_pointerarray_create_default();
   mulle_pointerarray_destroy( array);
   return( 0);
}
