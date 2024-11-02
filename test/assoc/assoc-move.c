//
//  main.c
//  test-assoc
//
//  Created by Nat! on 05.11.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
#ifdef NDEBUG
# undef NDEBUG
#endif
#include <assert.h>

#include <stdio.h>
#include <mulle-container/mulle-container.h>
#include <mulle-testallocator/mulle-testallocator.h>


#define N  5

static void   move_test( struct mulle_range range, intptr_t index)
{
   struct mulle_assoc                        *assoc;
   struct mulle_container_keyvaluecallback   callback;
   struct mulle_pointerpair                  pair;
   intptr_t                                  key;
   char                                      *value;
   char                                      *sep;
   size_t                              i;

   callback.keycallback   = mulle_container_keycallback_intptr;
   callback.valuecallback = mulle_container_valuecallback_copied_cstring;

   assoc = mulle_assoc_create( 0, &callback, _mulle_pointerpair_compare_pointer_key, NULL);

   mulle_assoc_add( assoc, (void *) 10, "10");
   mulle_assoc_add( assoc, (void *) 11, "11");
#if N > 2
   mulle_assoc_add( assoc, (void *) 12, "12");
#endif
#if N > 3
   mulle_assoc_add( assoc, (void *) 13, "13");
#endif
#if N > 4
   mulle_assoc_add( assoc, (void *) 14, "14");
#endif
#if N > 5
   mulle_assoc_add( assoc, (void *) 15, "15");
#endif

   _mulle_assoc_move_intptr_key_range( assoc, range, index);

   printf( "<%td-%td> -> %td : ", range.location, range.length, index);

   sep = "{";
   mulle_assoc_for( assoc, key, value)
   {
      printf( "%s %s", sep, value);
      sep = ",";
   }
   printf( " }\n");

   mulle_assoc_destroy( assoc);
}


static void   move()
{
   struct mulle_range   range;
   intptr_t             index;

   for( range.location = 10; range.location < 10 + N; range.location++)
      for( range.length = 1; range.length <= (10 + N) - range.location; range.length++)
         for( index = 10; index <= 10 + N; index++)
            move_test( range, index);
}


// the mulle_testallocator detects and aborts on leaks
static void   run_test( void (*f)( void), char *name)
{
   mulle_testallocator_reset();
#ifdef DEBUG
   fprintf( stderr, "%s\n", name);
#endif
   (f)();
   mulle_testallocator_reset();
}


int   main( int argc, char * argv[])
{
   run_test( move, "move");

   return( 0);
}
