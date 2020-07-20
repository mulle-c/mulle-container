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
#ifndef mulle__pointerset_generic_h__
#define mulle__pointerset_generic_h__

#include "mulle--pointerset-struct.h"


void   **_mulle__pointerset_allocate_storage_generic( size_t n,
                                                      void *notakey,
                                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointerset_reset_generic( struct mulle__pointerset *set,
                                         void *notakey);


MULLE_C_NONNULL_FIRST_THIRD
static inline
void  _mulle__pointerset_set_generic( struct mulle__pointerset *set,
                                      void *p,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   void   *_mulle__pointerset_write_generic( struct mulle__pointerset *set,
                                             void *p,
                                             enum mulle_container_write_mode mode,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator);

   _mulle__pointerset_write_generic( set,
                                     p,
                                     mulle_container_overwrite_e,
                                     callback,
                                     allocator);
}

MULLE_C_NONNULL_FIRST_THIRD
static inline
void    *_mulle__pointerset_insert_generic( struct mulle__pointerset *set,
                                            void *p,
                                            struct mulle_container_keycallback *callback,
                                            struct mulle_allocator *allocator)
{
   void   *_mulle__pointerset_write_generic( struct mulle__pointerset *set,
                                             void *p,
                                             enum mulle_container_write_mode mode,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator);

   return( _mulle__pointerset_write_generic( set,
                                             p,
                                             mulle_container_insert_e,
                                             callback,
                                             allocator));
}


MULLE_C_NONNULL_FIRST_THIRD
void   *_mulle__pointerset_get_generic( struct mulle__pointerset *set,
                                        void *key,
                                        struct mulle_container_keycallback *callback);

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__pointerset_shrink_generic( struct mulle__pointerset *set,
                                           struct mulle_container_keycallback *callback,
                                           struct mulle_allocator *allocator);

int   _mulle__pointerset_remove_generic( struct mulle__pointerset *set,
                                         void *p,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator);

int   _mulle__pointerset_copy_items_generic( struct mulle__pointerset *dst,
                                             struct mulle__pointerset *src,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator);


#pragma mark - enumeration

#define MULLE__GENERICPOINTERSETENUMERATOR_BASE   \
   void     **curr;                                \
   size_t   left;                                  \
   void     *notakey

struct mulle__genericpointersetenumerator
{
   MULLE__GENERICPOINTERSETENUMERATOR_BASE;
};


extern struct mulle__genericpointersetenumerator   mulle__genericpointersetenumerator_empty;


MULLE_C_NONNULL_FIRST
static inline struct mulle__genericpointersetenumerator
   _mulle__pointerset_enumerate_generic( struct mulle__pointerset *set,
                                         struct mulle_container_keycallback *callback)
{
   struct mulle__genericpointersetenumerator   rover;

   rover.left    = set->count;
   rover.curr    = set->storage;
   rover.notakey = callback->notakey;

   return( rover);
}


static inline struct mulle__genericpointersetenumerator
   mulle__pointerset_enumerate_generic( struct mulle__pointerset *set,
                                        struct mulle_container_keycallback *callback)
{
   if( ! set)
      return( mulle__genericpointersetenumerator_empty);

   return( _mulle__pointerset_enumerate_generic( set, callback));
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__genericpointersetenumerator_next( struct mulle__genericpointersetenumerator *rover)
{
   void   *p;

   if( ! rover->left)
      return( rover->notakey);

   for(;;)
   {
      p = *rover->curr++;
      if( p != rover->notakey)
      {
         rover->left--;
         return( p);
      }
   }
}


static inline void   _mulle__genericpointersetenumerator_done( struct mulle__genericpointersetenumerator *rover)
{
}

static inline void   mulle__genericpointersetenumerator_done( struct mulle__genericpointersetenumerator *rover)
{
}

#endif
