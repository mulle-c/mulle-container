//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
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
#ifndef mulle__set__h__
#define mulle__set__h__

#include "mulle--pointerset.h"
#include "mulle-container-callback.h"


#define MULLE__SET_BASE   MULLE__POINTERSET_BASE


// NSSet/NSMutableSet/NSHashTable

struct mulle__set
{
   MULLE__SET_BASE;
};


#pragma mark - setup and takedown

MULLE_C_NONNULL_THIRD
struct mulle__set   *_mulle__set_create( size_t capacity,
                                         size_t extra,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator) ;

MULLE_C_NONNULL_FIRST_SECOND
void    _mulle__set_destroy( struct mulle__set *set,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST_THIRD
void    _mulle__set_init( struct mulle__set *set,
                          size_t capacity,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_SECOND
void    _mulle__set_done( struct mulle__set *set,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator);


MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__set_reset( struct mulle__set *set,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator);


#pragma mark - petty accessors


MULLE_C_NONNULL_FIRST
static inline size_t   _mulle__set_get_size( struct mulle__set *set)
{
   return( _mulle__pointerset_get_size( (struct mulle__pointerset *) set));
}


MULLE_C_NONNULL_FIRST
static inline size_t   _mulle__set_get_count( struct mulle__set *set)
{
   return( _mulle__pointerset_get_count( (struct mulle__pointerset *) set));
}


#pragma mark - query

MULLE_C_NONNULL_FIRST
static inline int  _mulle__set_is_full( struct mulle__set *set)
{
   return( _mulle__pointerset_is_full( (struct mulle__pointerset *) set));
}

MULLE_C_NONNULL_FIRST
static inline int  _mulle__set_is_sparse( struct mulle__set *set)
{
   return( _mulle__pointerset_is_sparse( (struct mulle__pointerset *) set));
}


#pragma mark - operations

MULLE_C_NONNULL_FIRST_THIRD
static inline
void   _mulle__set_set( struct mulle__set *set,
                        void *p,
                        struct mulle_container_keycallback *callback,
                        struct mulle_allocator *allocator)
{
   _mulle__pointerset_set_generic( (struct mulle__pointerset *) set,
                                   p,
                                   callback,
                                   allocator);
}

MULLE_C_NONNULL_FIRST_THIRD
static inline
void    *_mulle__set_insert( struct mulle__set *set,
                             void *p,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator)
{
   return( _mulle__pointerset_insert_generic( (struct mulle__pointerset *) set,
                                               p,
                                               callback,
                                               allocator));
}


//
// you are getting the key here! If nothing is found it will be
// callback->notakey, not NULL
//
MULLE_C_NONNULL_FIRST_THIRD
static inline
void   *_mulle__set_get( struct mulle__set *set,
                         void *p,
                         struct mulle_container_keycallback *callback)
{
   return( _mulle__pointerset_get_generic( (struct mulle__pointerset *) set,
                                            p,
                                            callback));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__set_member( struct mulle__set *set,
                       void *p,
                       struct mulle_container_keycallback *callback)
{
   return( _mulle__pointerset_member_generic( (struct mulle__pointerset *) set,
                                              p,
                                              callback));
}


static inline int
   mulle__set_member( struct mulle__set *set,
                      void *p,
                      struct mulle_container_keycallback *callback)
{
   if( ! set)
      return( 0);
   return( _mulle__pointerset_member_generic( (struct mulle__pointerset *) set,
                                               p,
                                               callback));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline
int  _mulle__set_remove( struct mulle__set *set,
                         void *p,
                         struct mulle_container_keycallback *callback,
                         struct mulle_allocator *allocator)
{
   return( _mulle__pointerset_remove_generic( (struct mulle__pointerset *) set,
                                               p,
                                               callback,
                                               allocator));
}

MULLE_C_NONNULL_FIRST_SECOND
static inline void  _mulle__set_shrink_if_needed( struct mulle__set *set,
                                                  struct mulle_container_keycallback *callback,
                                                  struct mulle_allocator *allocator)
{
   if( _mulle__set_is_sparse( set))
      _mulle__pointerset_shrink_generic( (struct mulle__pointerset *) set,
                                         callback,
                                         allocator);
}



#pragma mark - copying

MULLE_C_NONNULL_FIRST_SECOND_THIRD
static inline
int   _mulle__set_copy_items( struct mulle__set *dst,
                              struct mulle__set *src,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator)
{
   return( _mulle__pointerset_copy_items_generic( (struct mulle__pointerset *) dst,
                                                  (struct mulle__pointerset *) src,
                                                  callback,
                                                  allocator));
}


MULLE_C_NONNULL_FIRST_SECOND
struct mulle__set   *_mulle__set_copy( struct mulle__set *set,
                                       struct mulle_container_keycallback *callback,
                                       struct mulle_allocator *allocator);

#pragma mark - debugging

MULLE_C_NONNULL_FIRST_SECOND
char   *_mulle__set_describe( struct mulle__set *set,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator);

#pragma mark - enumeration

#define MULLE__SETENUMERATOR_BASE   MULLE__GENERICPOINTERSETENUMERATOR_BASE


struct mulle__setenumerator
{
   MULLE__SETENUMERATOR_BASE;
};

extern struct mulle__setenumerator   mulle__setenumerator_empty;


MULLE_C_NONNULL_FIRST_SECOND
static inline struct mulle__setenumerator
   _mulle__set_enumerate( struct mulle__set *set,
                          struct mulle_container_keycallback *callback)
{
   struct mulle__setenumerator   rover;

   *((struct mulle__genericpointersetenumerator *) &rover) = _mulle__pointerset_enumerate_generic( (struct mulle__pointerset *) set, callback);

   return( rover);
}


static inline struct mulle__setenumerator
   mulle__set_enumerate( struct mulle__set *set,
                         struct mulle_container_keycallback *callback)
{
   struct mulle__setenumerator   rover;

   if( set)
      return( mulle__setenumerator_empty);
   return( _mulle__set_enumerate( set, callback));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__setenumerator_next( struct mulle__setenumerator *rover, void **item)
{
   return( _mulle__genericpointersetenumerator_next( (struct mulle__genericpointersetenumerator *) rover,
                                                   item));
}

static inline int
   mulle__setenumerator_next( struct mulle__setenumerator *rover, void **item)
{
   return( mulle__genericpointersetenumerator_next( (struct mulle__genericpointersetenumerator *) rover,
                                                   item));
}



MULLE_C_NONNULL_FIRST
static inline void   _mulle__setenumerator_done( struct mulle__setenumerator *rover)
{
}


static inline void   mulle__setenumerator_done( struct mulle__setenumerator *rover)
{
}

#endif
