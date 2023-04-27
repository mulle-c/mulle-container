//
//  mulle-flexarray.h
//  mulle-container
//
//  Created by Nat! on 22/08/16.
//  Copyright (c) 2022 Nat! - Mulle kybernetiK.
//  Copyright (c) 2022 Codeon GmbH.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef mulle_flexarray__h__
#define mulle_flexarray__h__

#include "include.h"

// use mulle-c11 for that
//#include <stdalign.h>


// A mulle-flexarray is useful in functions, when you prefer to do the work
// in a stack based (auto) array, but must remain flexible to use malloc
// for heavier workloads
//
// e.g. conventional code:
//
// void  foo( int n, int *data)
// {
//    int  copy_data[ 32];
//    int  *copy = copy_data;
//
//    if( n > 32)
//       copy = malloc( n * sizeof( int));
//
//    memcpy( copy, data, n * sizeof( int));
//
//    if( copy != copy_data)
//       free( copy);
// }
//
//
// mulle-flexarray code:
//
// void  foo( int n, int *data)
// {
//    mulle_flexarray_do( copy, int, 32, n);
//    {
//       memcpy( copy, data, n * sizeof( int));
//    }
// }

#define mulle_flexarray( name, type, stackcount)                           \
   type                        name ## __storage[ (stackcount)];           \
   struct mulle__structarray   name ## __array =                           \
                               MULLE__STRUCTARRAY_INIT( name ## __storage, \
                                                        type,              \
                                                        (stackcount))

#define mulle_flexarray_alloc( name, actualcount)                          \
      _mulle__structarray_guarantee( &name ## __array, (actualcount), NULL)

#define mulle_flexarray_realloc( name, count)                              \
   (                                                                       \
      _mulle__structarray_sizeto_length(                                   \
         &name ## __array,                                                 \
         (count) * sizeof( name ## __storage[ 0]),                         \
         NULL),                                                            \
      name ## __array.storage                                              \
   )

#define mulle_flexarray_done( name)                                        \
   do                                                                      \
   {                                                                       \
      _mulle__structarray_done( &name ## __array, NULL);                   \
      name = NULL;                                                         \
   }                                                                       \
   while( 0)

#define mulle_flexarray_define( name, type, stackcount, count)             \
   mulle_flexarray( name, type, stackcount);                               \
   type *name = mulle_flexarray_alloc( name, count)


#define mulle_flexarray_return( name, value)                               \
   do                                                                      \
   {                                                                       \
      __typeof__( *name) name ## __tmp = (value);                          \
      mulle_flexarray_done( name);                                         \
      return( name ## __tmp);                                              \
   }                                                                       \
   while( 0)

#define mulle_flexarray_return_void( name)                                 \
   do                                                                      \
   {                                                                       \
      mulle_flexarray_done( name);                                         \
      return;                                                              \
   }                                                                       \
   while( 0)


//
// we have to keep storage out of the for loop, but we can zero the
// pointer, so that a "too late" access is catchable
//
#define mulle_flexarray_do( name, type, stackcount, count)                   \
   type   name ## __storage[ stackcount];                                    \
   type   *name;                                                             \
   for( struct mulle__structarray                                            \
           name ## __array =                                                 \
              MULLE__STRUCTARRAY_INIT( name ## __storage, type, stackcount), \
           name ## __i =                                                     \
           {                                                                 \
              ( name = mulle_flexarray_alloc( name, count), (void *) 0 )     \
           };                                                                \
        ! name ## __i._storage;                                              \
        name = NULL,                                                         \
        name ## __i._storage =                                               \
        (                                                                    \
           _mulle__structarray_done( &name ## __array, NULL),                \
           (void *) 0x1                                                      \
        )                                                                    \
      )                                                                      \
                                                                             \
      for( int  name ## __j = 0;    /* break protection */                   \
           name ## __j < 1;                                                  \
           name ## __j++)

#endif
