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

#ifndef mulle_flexarray_h__
#define mulle_flexarray_h__

#include "include.h"

// use mulle-c11 for that
//#include <stdalign.h>


// The mulle-flexarray was a precursor to mulle-alloca, it remains, because
// there is some code that uses it already.

#define mulle_flexarray( name, type, stackcount)                           \
   type                        name ## __storage[ (stackcount)];           \
   type                        *name;                                      \
   struct mulle__structarray   name ## __array =                           \
                               MULLE__STRUCTARRAY_INIT( name ## __storage, \
                                                        type,              \
                                                        (stackcount))

#define mulle_flexarray_alloc( name, actualcount)                          \
      name = _mulle__structarray_guarantee( &name ## __array,              \
                                            (actualcount),                 \
                                            NULL)

#define mulle_flexarray_realloc( name, count)                              \
   name = (                                                                \
      _mulle__structarray_set_count(                                       \
         &name ## __array,                                                 \
         (count),                                                          \
         NULL),                                                            \
      name ## __array._storage                                             \
   )

#define mulle_flexarray_done( name)                                        \
   do                                                                      \
   {                                                                       \
      _mulle__structarray_done( &name ## __array, NULL);                   \
      name = NULL;                                                         \
   }                                                                       \
   while( 0)


//
// these macros are only useful, if you are not nesting your flexarray_dos
// I just might throw these two macros out again
//
#define _mulle_flexarray_return( name, value)                              \
   do                                                                      \
   {                                                                       \
      __typeof__( value) name ## __tmp = (value);                          \
      mulle_flexarray_done( name);                                         \
      return( name ## __tmp);                                              \
   }                                                                       \
   while( 0)

#define _mulle_flexarray_return_void( name)                                \
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
              ( mulle_flexarray_alloc( name, count), (void *) 0 )            \
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
