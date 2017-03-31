//
//  mulle_test_allocator.c
//  mulle-container
//
//  Created by Nat! on 04.11.15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
//

#include "mulle_test_allocator.h"

#include <stdio.h>
#include <mulle_thread/mulle_thread.h>
#include <stdlib.h>


#pragma mark -
#pragma mark track allocations

#include "pointer_array.h"

struct _pointer_array    allocations;

static mulle_thread_mutex_t     alloc_lock;

static void  *test_realloc( void *q, size_t size)
{
   void           *p;
   unsigned int   i;

   p = realloc( q, size);
   if( ! p)
      return( p);

   if( mulle_thread_mutex_lock( &alloc_lock))
      abort();

   if( q)
   {
      if( p != q)
      {
         i = _pointer_array_index( &allocations, q);  // analyzer it's ok, just a pointer comparison
         assert( i != (unsigned int) -1);
         _pointer_array_set( &allocations, i, p);
      }
   }
   else
      _pointer_array_add( &allocations, p, realloc);
   mulle_thread_mutex_unlock( &alloc_lock);

   fprintf( stderr, "realloced %p\n", p);

   return( p);
}


static void  *test_calloc( size_t n, size_t size)
{
   void   *p;

   p = calloc( n, size);
   if( ! p)
      return( p);

   if( mulle_thread_mutex_lock( &alloc_lock))
      abort();

   _pointer_array_add( &allocations, p, realloc);
   mulle_thread_mutex_unlock( &alloc_lock);

   fprintf( stderr, "alloced %p\n", p);

   return( p);
}


static void  test_free( void *p)
{
   unsigned int   i;

   if( ! p)
      return;

   if( mulle_thread_mutex_lock( &alloc_lock))
      abort();

   i = _pointer_array_index( &allocations, p);
   assert( i != (unsigned int) -1);  // if assert, this is a double free or not a malloc block
   _pointer_array_set( &allocations, i, NULL);

   mulle_thread_mutex_unlock( &alloc_lock);

   free( p);
}


#pragma mark -
#pragma mark global variable

struct mulle_allocator   mulle_test_allocator =
{
      test_calloc,
      test_realloc,
      test_free
};


#pragma mark -
#pragma mark reset allocator between tests

void   mulle_test_allocator_reset_memory()
{
   struct  _pointer_array_enumerator   rover;
   void                                *p;

   rover = _pointer_array_enumerate( &allocations);
   while( (p = _pointer_array_enumerator_next( &rover)) != (void *) -1)
   {
      if( p)
      {
         fprintf( stderr, "leak %p\n", p);
         abort();
      }
   }
   _pointer_array_enumerator_done( &rover);

   _pointer_array_done( &allocations, free);
   memset( &allocations, 0, sizeof( allocations));
}


__attribute__((constructor))
static void  __load()
{
   int  rval;

   rval = mulle_thread_mutex_init( &alloc_lock);
   assert( ! rval);

   mulle_default_allocator = mulle_test_allocator;  // put test in place
}
