//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
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

#ifndef mulle_container_operation__h__
#define mulle_container_operation__h__

#include <limits.h>
#include <stdint.h>

// https://stackoverflow.com/questions/1464174/_size-t-vs-uintptr-t
//
// size_t is a type that can hold any array index. So that's what we use
// in container structures to address elements in contigous memory, allocated
// my malloc/calloc which are also size_t based.
//
// The contents of the cells are void * mostly, to distinguish notakey and
// not found conditions we use INPTR_MAX and INTPTR_MIN. These are not
// neccesarily defined in C11 though.
//
// mulle_range location/length are uinptr_t to support mulle_not_found_e
// values.
//



// C11 says intptr_t is optional, but we need it. We use it for finding
// values and returning an error code.
// size_t is the "indexing" value for an array
//
#ifndef INTPTR_MAX
#error "intprt_t and friends are needed for this platform"
#endif


enum mulle_container_write_mode
{
   mulle_container_insert_e,
   mulle_container_overwrite_e
};

// Convenient to be different to "notakey"
// moved to mulle-data! but kept same
// #define mulle_not_found_e     ((uintptr_t) INTPTR_MAX)

// Use this for "notakey" pointer to differentiate valid pointers & NULL
#define mulle_not_a_pointer   ((void *) INTPTR_MIN)

// Use this for "notakey" pointer to differentiate valid int keys
#define mulle_not_an_int      ((void *) INTPTR_MIN)

// Use this for "notakey" pointer to differentiate valid intptr keys
#define mulle_not_an_intptr   ((void *) INTPTR_MIN)



// just to get rid of warnings
static inline void   *mulle_int_as_pointer( int a)
{
   return( (void *) (intptr_t) a);
}


static inline int   mulle_pointer_as_int( void *a)
{
   return( (int) (intptr_t) a);
}

// just to have something for intptr too, cast is easier though
static inline void   *mulle_intptr_as_pointer( intptr_t a)
{
   return( (void *) a);
}


static inline intptr_t   mulle_pointer_as_intptr( void *a)
{
   return( (intptr_t) a);
}


#endif /* mulle_container_operation_h */
