//
//  main.c
//  test-array
//
//  Created by Nat! on 04.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//

#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


int   char_compare( void *_a, void *_b, void *thunk)
{
   char  *a = _a;
   char  *b = _b;

   if( *a >= *b)
      return( *a != *b);
   return( -1);
}


static void  null()
{
   mulle_qsort_r( NULL, 0, 0, char_compare, NULL);
}


static void  empty()
{
   mulle_qsort_r( "", 0, 0, char_compare, NULL);
}

static void  test_char( char *s)
{
   char   *tmp;

   tmp = mulle_strdup( s);
   mulle_qsort_r( tmp, strlen( tmp), sizeof( char), char_compare, NULL);
   printf( "%s -> %s\n",s , tmp);
   mulle_free( tmp);
}


static void  sorted()
{
   test_char( "abcdefghijklmnopqrstuvwxyz");
}

static void  unsorted()
{
   test_char( "qsdxhgjemznrofpklbitauvwcy");
}



// the mulle_testallocator detects and aborts on leaks
static void  run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
  // printf( "%s\n", name);
   (f)();
   mulle_testallocator_reset();
}


int main(int argc, const char * argv[])
{
   run_test( null, "null");
   run_test( empty, "empty");
   run_test( sorted, "sorted");
   run_test( unsorted, "unsorted");
   return( 0);
}
