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


static void  null( void)
{
   struct mulle_pointerarray                    *array;
   struct mulle_pointerarrayenumerator           rover;
   struct mulle_pointerarrayreverseenumerator    rerover;
   void                                          *item;

   array = NULL;

   mulle_pointerarray_add( array, "VfL");
   mulle_pointerarray_add( array, "Bochum");
   mulle_pointerarray_add( array, "1848");

   assert( mulle_pointerarray_get_count( array) == 0);

   rover = mulle_pointerarray_enumerate( array);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! item);
   mulle_pointerarrayenumerator_done( &rover);

   rerover = mulle_pointerarray_reverseenumerate( array);
   _mulle_pointerarrayreverseenumerator_next( &rerover, &item);
   assert( ! item);
   mulle_pointerarrayreverseenumerator_done( &rerover);

   mulle_pointerarray_destroy( array);
}


static void  simple( void)
{
   struct mulle_pointerarrayenumerator           rover;
   struct mulle_pointerarrayreverseenumerator    rerover;
   void                                          *item;
   char                                          *s;
   int                                           i;

   mulle_pointerarray_do( array)
   {
      s     = "VfL";
      mulle_pointerarray_add( array, s);
      assert( s == _mulle_pointerarray_get( array, 0)); // must have been copied

      mulle_pointerarray_add( array, "Bochum");
      mulle_pointerarray_add( array, "1848");

      assert( mulle_pointerarray_get_count( array) == 3);

      assert( ! strcmp( "VfL", _mulle_pointerarray_get( array, 0)));
      assert( ! strcmp( "Bochum", _mulle_pointerarray_get( array, 1)));
      assert( ! strcmp( "1848", _mulle_pointerarray_get( array, 2)));

      rover = mulle_pointerarray_enumerate( array);
      _mulle_pointerarrayenumerator_next( &rover, &item);
      assert( item);
      assert( ! strcmp( "VfL", item));
      _mulle_pointerarrayenumerator_next( &rover, &item);
      assert( item);
      assert( ! strcmp( "Bochum", item));
      _mulle_pointerarrayenumerator_next( &rover, &item);
      assert( item);
      assert( ! strcmp( "1848", item));
      assert( ! _mulle_pointerarrayenumerator_next( &rover, &item));
      mulle_pointerarrayenumerator_done( &rover);

      // more or less just a syntax check
      i = 0;
      mulle_pointerarray_for( array, item)
      {
         assert( item);
         ++i;
      }
      assert( i == 3);

      rerover = mulle_pointerarray_reverseenumerate( array);
      _mulle_pointerarrayreverseenumerator_next( &rerover, &item);
      assert( item);
      assert( ! strcmp( "1848", item));
      _mulle_pointerarrayreverseenumerator_next( &rerover, &item);
      assert( item);
      assert( ! strcmp( "Bochum", item));
      _mulle_pointerarrayreverseenumerator_next( &rerover, &item);
      assert( item);
      assert( ! strcmp( "VfL", item));
      assert( ! _mulle_pointerarrayreverseenumerator_next( &rerover, &item));
      mulle_pointerarrayreverseenumerator_done( &rerover);


      // more or less just a syntax check
      i = 0;
      mulle_pointerarray_for_reverse( array, item)
      {
         assert( item);
         ++i;
      }
      assert( i == 3);
   }
}


static void  grow( void)
{
   struct mulle_pointerarray   array;
   size_t                      i;

   _mulle_pointerarray_init( &array, 10, NULL);

   for( i = 0; i < 20; i++)
      _mulle_pointerarray_add( &array, "VfL");

   assert( mulle_pointerarray_get_count( &array) == 20);

   _mulle_pointerarray_done( &array);
}


static void  absorb_array( void)
{
   struct mulle_pointerarray             array;
   struct mulle_pointerarray             other;
   struct mulle_pointerarrayenumerator   rover;
   void                                  *item;

   _mulle_pointerarray_init( &array, 0, NULL);

   _mulle_pointerarray_add( &array, "VfL");
   _mulle_pointerarray_add( &array, "Bochum");
   _mulle_pointerarray_add( &array, "1848");

   _mulle_pointerarray_init( &other, 0, NULL);

   _mulle_pointerarray_add( &other, "A");
   _mulle_pointerarray_add( &other, "B");
   _mulle_pointerarray_add( &other, "C");

   mulle_pointerarray_absorb_array( &array, &other);

   assert( mulle_pointerarray_get_count( &array) == 6);
   assert( mulle_pointerarray_get_count( &other) == 0);

   rover = mulle_pointerarray_enumerate( &array);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! strcmp( item, "VfL"));
   _mulle_pointerarrayenumerator_next( &rover, &item);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! strcmp( item, "C"));
   mulle_pointerarrayenumerator_done( &rover);

   mulle_pointerarray_done( &other);
   mulle_pointerarray_done( &array);
}


static void  absorb_array2( void)
{
   struct mulle_pointerarray             array;
   struct mulle_pointerarray             other;
   struct mulle_pointerarrayenumerator   rover;
   void                                  *item;

   _mulle_pointerarray_init( &array, 0, NULL);
   _mulle_pointerarray_init( &other, 0, NULL);

   _mulle_pointerarray_add( &other, "A");
   _mulle_pointerarray_add( &other, "B");
   _mulle_pointerarray_add( &other, "C");

   // for coverage
   _mulle__pointerarray_absorb_array( (struct mulle__pointerarray *) &array,
                                      NULL,
                                      (struct mulle__pointerarray *) &other,
                                      NULL);
   assert( mulle_pointerarray_get_count( &array) == 3);
   assert( mulle_pointerarray_get_count( &other) == 0);

   rover = mulle_pointerarray_enumerate( &array);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! strcmp( item, "A"));
   _mulle_pointerarrayenumerator_next( &rover, &item);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! strcmp( item, "C"));
   mulle_pointerarrayenumerator_done( &rover);

   mulle_pointerarray_done( &other);
   mulle_pointerarray_done( &array);
}


static void  absorb_array3( void)
{
   struct mulle_pointerarray             array;
   struct mulle_pointerarray             other;
   struct mulle_pointerarrayenumerator   rover;
   void                                  *item;

   _mulle_pointerarray_init( &array, 0, NULL);
   _mulle_pointerarray_init( &other, 0, NULL);

   _mulle_pointerarray_add( &array, "A");
   _mulle_pointerarray_add( &array, "B");
   _mulle_pointerarray_add( &array, "C");

   // for coverage
   _mulle__pointerarray_absorb_array( (struct mulle__pointerarray *) &array,
                                      &mulle_default_allocator,
                                      (struct mulle__pointerarray *) &other,
                                      &mulle_default_allocator);

   assert( mulle_pointerarray_get_count( &array) == 3);
   assert( mulle_pointerarray_get_count( &other) == 0);

   rover = mulle_pointerarray_enumerate( &array);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! strcmp( item, "A"));
   _mulle_pointerarrayenumerator_next( &rover, &item);
   _mulle_pointerarrayenumerator_next( &rover, &item);
   assert( ! strcmp( item, "C"));
   mulle_pointerarrayenumerator_done( &rover);

   mulle_pointerarray_done( &other);
   mulle_pointerarray_done( &array);
}


static void  extract_pointers( void)
{
   struct mulle_pointerarray   array;
   struct mulle_pointers       data;

   mulle_pointerarray_init( &array, 0, NULL);


   mulle_pointerarray_add( &array, "VfL");
   mulle_pointerarray_add( &array, "Bochum");
   mulle_pointerarray_add( &array, "1848");

   data = _mulle__pointerarray_extract_pointers( (struct mulle__pointerarray *) &array, NULL);

   assert( mulle_pointerarray_get_count( &array) == 0);
   assert( data.count == 3);

   assert( ! strcmp( data.pointers[ 0], "VfL"));
   assert( ! strcmp( data.pointers[ 2], "1848"));

   mulle_pointerarray_done( &array);

   mulle_pointers_done( data, NULL);
}

static void  extract_pointers2( void)
{
   struct mulle_pointers    data;

   mulle_pointerarray_do_flexible( array, 3)
   {
      mulle_pointerarray_add( array, "VfL");
      mulle_pointerarray_add( array, "Bochum");
      mulle_pointerarray_add( array, "1848");

      data = _mulle__pointerarray_extract_pointers( (struct mulle__pointerarray *) array, NULL);

      assert( mulle_pointerarray_get_count( array) == 0);
      assert( data.count == 3);
   }

   assert( ! strcmp( data.pointers[ 0], "VfL"));
   assert( ! strcmp( data.pointers[ 2], "1848"));

   mulle_pointers_done( data, NULL);
}


static void  extract_pointers0( void)
{
   struct mulle_pointerarray   array;
   struct mulle_pointers       data;

   mulle_pointerarray_init( &array, 0, NULL);

   data = _mulle__pointerarray_extract_pointers( (struct mulle__pointerarray *) &array, NULL);

   assert( mulle_pointerarray_get_count( &array) == 0);
   assert( data.count == 0);

   mulle_pointerarray_done( &array);

   mulle_pointers_done( data, NULL);
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
   run_test( simple, "simple");
   run_test( grow, "grow");
   run_test( absorb_array, "absorb_array");
   run_test( absorb_array2, "absorb_array2");
   run_test( absorb_array3, "absorb_array3");
   run_test( extract_pointers, "extract_pointers");
   run_test( extract_pointers2, "extract_pointers2");
   run_test( extract_pointers0, "extract_pointers0");
   return( 0);
}
