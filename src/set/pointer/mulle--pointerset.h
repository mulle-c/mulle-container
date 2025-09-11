//
//  Copyright (C) 2011-2020 Nat!, Mulle kybernetiK.
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
#ifndef mulle__pointerset_h__
#define mulle__pointerset_h__

#include "mulle--pointerset-struct.h"
#include "mulle--pointerset-generic.h"

#pragma mark - operations built on generic and struct operations


MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerset_reset( struct mulle__pointerset *set)
{
   _mulle__pointerset_reset_generic( set, mulle_not_a_pointer);
}

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerset_set( struct mulle__pointerset *set,
                           void *p,
                           struct mulle_allocator *allocator)
{
   _mulle__pointerset_set_generic( set,
                                   p,
                                   &mulle_container_keycallback_nonowned_pointer_or_null,
                                   allocator);
}


// returns 1 if insert was successful
MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointerset_insert( struct mulle__pointerset *set,
                              void *p,
                              struct mulle_allocator *allocator)
{
   return( _mulle__pointerset_insert_generic( set,
                                              p,
                                              &mulle_container_keycallback_nonowned_pointer_or_null,
                                              allocator));
}


MULLE_C_NONNULL_FIRST
static inline void *
   _mulle__pointerset_register( struct mulle__pointerset *set,
                                void *p,
                                struct mulle_allocator *allocator)
{
   return( _mulle__pointerset_register_generic( set,
                                                p,
                                                &mulle_container_keycallback_nonowned_pointer_or_null,
                                                allocator));
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerset_shrink( struct mulle__pointerset *set,
                                  struct mulle_allocator *allocator)
{
   _mulle__pointerset_shrink_generic( set, &mulle_container_keycallback_nonowned_pointer_or_null, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle__pointerset_shrink_if_needed( struct mulle__pointerset *set,
                                                         struct mulle_allocator *allocator)
{
   if( _mulle__pointerset_is_sparse( set))
      _mulle__pointerset_shrink( set, allocator);
}


MULLE_C_NONNULL_FIRST
static inline
int  _mulle__pointerset_remove( struct mulle__pointerset *set,
                                void *p,
                                struct mulle_allocator *allocator)
{
   return( _mulle__pointerset_remove_generic( set,
                                              p,
                                              &mulle_container_keycallback_nonowned_pointer_or_null,
                                              allocator));
}

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerset_copy_items( struct mulle__pointerset *dst,
                                  struct mulle__pointerset *src,
                                  struct mulle_allocator *allocator)
{
   _mulle__pointerset_copy_items_generic( dst,
                                          src,
                                          &mulle_container_keycallback_nonowned_pointer_or_null,
                                          allocator);
}


struct mulle__pointerset  *
   _mulle__pointerset_copy( struct mulle__pointerset *set,
                            struct mulle_allocator *allocator);

// created by make-container-do.sh mulle--pointerset.c

#define mulle__pointerset_do( name)                              \
   for( struct mulle__pointerset                                 \
           name ## __container = { 0 },                          \
           *name = &name ## __container,                         \
           *name ## __i = NULL;                                  \
        ! name ## __i;                                           \
        name ## __i =                                            \
        (                                                        \
           _mulle__pointerset_done( &name ## __container, NULL), \
           (void *) 0x1                                          \
        )                                                        \
      )                                                          \
      for( int  name ## __j = 0;    /* break protection */       \
           name ## __j < 1;                                      \
           name ## __j++)

// created by make-container-for.sh src/set/pointer/mulle--pointerset.c

#define mulle__pointerset_for( name, item)                                               \
   assert( sizeof( item) == sizeof( void *));                                            \
   for( struct mulle__pointersetenumerator                                               \
           rover__ ## item = mulle__pointerset_enumerate( name),                         \
           *rover__  ## item ## __i = (void *) 0;                                       \
        ! rover__  ## item ## __i;                                                      \
        rover__ ## item ## __i = (_mulle__pointersetenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                          \
      while( _mulle__pointersetenumerator_next( &rover__ ## item, (void **) &item))


#endif
