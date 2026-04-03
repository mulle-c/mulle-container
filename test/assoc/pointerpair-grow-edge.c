#include "include.h"
#include <stdio.h>

int main( void)
{
   struct mulle__pointerpairarray   array;
   
   printf( "Testing pointerpair edge cases\n");
   
   // Cover line 55: grow with size 0
   _mulle__pointerpairarray_init( &array, 0, NULL);
   _mulle__pointerpairarray_grow( &array, NULL);
   printf( "Grow from size 0: OK\n");
   _mulle__pointerpairarray_done( &array, NULL);
   
   return( 0);
}
