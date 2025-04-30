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

//
// the "generic" functions are aware of the callback and use it to add and
// remove stuff from the pointerset. The standard mulle-pointerset user is
// "unaware" that a callback is used during storage I/O.
//
void   **_mulle__pointerset_allocate_storage_generic( size_t n,
                                                      void *notakey,
                                                      struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void   _mulle__pointerset_release_all( struct mulle__pointerset *set,
                                       struct mulle_container_keycallback *callback,
                                       struct mulle_allocator *allocator);


#pragma mark - callback access with mutation check

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT

static inline uintptr_t   _mulle__pointerset_keycallback_hash( struct mulle__pointerset *set,
                                                               struct mulle_container_keycallback *callback,
                                                               void *a)
{
   uintptr_t   memo_set;
   uintptr_t   hash;

   memo_set = set->_n_mutations;
   hash     = (*callback->hash)( callback, a);
   assert( set->_n_mutations == memo_set && "set was modified during hash callback");

   return( hash);
}

static inline int   _mulle__pointerset_keycallback_equal( struct mulle__pointerset *set,
                                                          struct mulle_container_keycallback *callback,
                                                          void *a,
                                                          void *b)
{
   uintptr_t   memo_set;
   int         is_equal;

   memo_set = set->_n_mutations;
   is_equal = (*callback->is_equal)( callback, a, b);
   assert( set->_n_mutations == memo_set && "set was modified during is_equal callback");

   return( is_equal);
}


static inline void   *_mulle__pointerset_keycallback_retain( struct mulle__pointerset *set,
                                                             struct mulle_container_keycallback *callback,
                                                             void *p,
                                                             struct mulle_allocator *allocator)
{
   uintptr_t   memo_set;

   memo_set = set->_n_mutations;
   p        = (*callback->retain)( callback, p, allocator);
   assert( set->_n_mutations == memo_set && "set was modified during retain callback");
   return( p);
}


static inline void   _mulle__pointerset_keycallback_release( struct mulle__pointerset *set,
                                                             struct mulle_container_keycallback *callback,
                                                             void *p,
                                                             struct mulle_allocator *allocator)
{
   uintptr_t   memo_set;

   memo_set = set->_n_mutations;
   (*callback->release)( callback, p, allocator);
   assert( set->_n_mutations == memo_set && "set was modified during release callback");
}


static inline char   *_mulle__pointerset_keycallback_describe( struct mulle__pointerset *set,
                                                               struct mulle_container_keycallback *callback,
                                                               void *p,
                                                               struct mulle_allocator **allocator)
{
   uintptr_t   memo_set;
   char        *s;

   memo_set = set->_n_mutations;
   s        = (*callback->describe)( callback, p, allocator);
   assert( set->_n_mutations == memo_set && "set was modified during describe callback");
   return( s);
}

#else

static inline uintptr_t   _mulle__pointerset_keycallback_hash( struct mulle__pointerset *set,
                                                               struct mulle_container_keycallback *callback,
                                                               void *a)
{
   uintptr_t   hash;

   MULLE_C_UNUSED( set);

   hash = (*callback->hash)( callback, a);
   return( hash);
}


static inline int   _mulle__pointerset_keycallback_equal( struct mulle__pointerset *set,
                                                          struct mulle_container_keycallback *callback,
                                                          void *a,
                                                          void *b)
{
   MULLE_C_UNUSED( set);

   return( (*callback->is_equal)( callback, a, b));
}


static inline void   *_mulle__pointerset_keycallback_retain( struct mulle__pointerset *set,
                                                             struct mulle_container_keycallback *callback,
                                                             void *p,
                                                             struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( set);

   return( (*callback->retain)( callback, p, allocator));
}


static inline void   _mulle__pointerset_keycallback_release( struct mulle__pointerset *set,
                                                             struct mulle_container_keycallback *callback,
                                                             void *p,
                                                             struct mulle_allocator *allocator)
{
   MULLE_C_UNUSED( set);

   (*callback->release)( callback, p, allocator);
}

#endif


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
static inline int
   _mulle__pointerset_insert_generic( struct mulle__pointerset *set,
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
                                             allocator) == NULL);
}


MULLE_C_NONNULL_FIRST_THIRD
static inline void *
   _mulle__pointerset_register_generic( struct mulle__pointerset *set,
                                        void *p,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   void  *old;

   void   *_mulle__pointerset_write_generic( struct mulle__pointerset *set,
                                             void *p,
                                             enum mulle_container_write_mode mode,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator);

   old = _mulle__pointerset_write_generic( set,
                                           p,
                                           mulle_container_insert_e,
                                           callback,
                                           allocator);
   return( old != callback->notakey ? old : p);
}



MULLE_C_NONNULL_FIRST_THIRD
void   *_mulle__pointerset_get_generic( struct mulle__pointerset *set,
                                        void *key,
                                        struct mulle_container_keycallback *callback);


MULLE_C_NONNULL_FIRST_THIRD
static inline int
   _mulle__pointerset_member_generic( struct mulle__pointerset *set,
                                      void *p,
                                      struct mulle_container_keycallback *callback)
{
   return( _mulle__pointerset_get_generic( set, p, callback) != callback->notakey);
}

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__pointerset_shrink_generic( struct mulle__pointerset *set,
                                          struct mulle_container_keycallback *callback,
                                          struct mulle_allocator *allocator);

int   _mulle__pointerset_remove_generic( struct mulle__pointerset *set,
                                         void *p,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator);

void   _mulle__pointerset_copy_items_generic( struct mulle__pointerset *dst,
                                              struct mulle__pointerset *src,
                                              struct mulle_container_keycallback *callback,
                                              struct mulle_allocator *allocator);

//
// dst will be clobbered with the intersection of a and b
// dst can be the same as a,
//
MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__pointerset_intersect_generic( struct mulle__pointerset *dst,
                                             struct mulle__pointerset *a,
                                             struct mulle__pointerset *b,
                                             struct mulle_container_keycallback *callback,
                                             struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST_FOURTH
void   _mulle__pointerset_union_generic( struct mulle__pointerset *dst,
                                         struct mulle__pointerset *a,
                                         struct mulle__pointerset *b,
                                         struct mulle_container_keycallback *callback,
                                         struct mulle_allocator *allocator);

#pragma mark - enumeration

#define MULLE__GENERICPOINTERSETENUMERATOR_BASE \
   void     **_curr;                            \
   size_t   _left;                              \
   void     *_notakey

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
struct mulle__genericpointersetenumerator
{
   MULLE__GENERICPOINTERSETENUMERATOR_BASE;
   struct mulle__pointerset *_set;
   uintptr_t  _n_mutations;
};
#else
struct mulle__genericpointersetenumerator
{
   MULLE__GENERICPOINTERSETENUMERATOR_BASE;
};
#endif

#define mulle__genericpointersetenumerator_empty  \
   ((struct mulle__genericpointersetenumerator) { 0 })

MULLE_C_NONNULL_FIRST_SECOND
static inline struct mulle__genericpointersetenumerator
   _mulle__pointerset_enumerate_generic( struct mulle__pointerset *set,
                                         struct mulle_container_keycallback *callback)
{
   struct mulle__genericpointersetenumerator   rover;

   rover._curr    = set->_storage;
   rover._left    = set->_count;
   rover._notakey = callback->notakey;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   rover._set = set;
   rover._n_mutations = set->_n_mutations;
#endif

   return( rover);
}

MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__genericpointersetenumerator_next( struct mulle__genericpointersetenumerator *rover,
                                             void **item)
{
   void   *p;

   if( ! rover->_left)
   {
      *item = NULL;
      return( 0);
   }

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   assert( rover->_set->_n_mutations == rover->_n_mutations &&
          "set was modified during enumeration");
#endif

   for(;;)
   {
      p = *rover->_curr++;
      if( p != rover->_notakey)
      {
         rover->_left--;
         *item = p;
         return( 1);
      }
   }
}

static inline int
   mulle__genericpointersetenumerator_next( struct mulle__genericpointersetenumerator *rover,
                                            void **item)
{
   void   *p;

   if( ! rover || ! rover->_left)
   {
      if( item)
         *item = NULL;
      return( 0);
   }

   for(;;)
   {
      p = *rover->_curr++;
      if( p != rover->_notakey)
      {
         rover->_left--;
         if( item)
            *item = p;
         return( 1);
      }
   }
}

static inline void   _mulle__genericpointersetenumerator_done( struct mulle__genericpointersetenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}

static inline void   mulle__genericpointersetenumerator_done( struct mulle__genericpointersetenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


#define mulle__genericpointerset_for( set, callback, item)                                                                                            \
   assert( sizeof( item) == sizeof( void *));                                                                                                         \
   for( struct mulle__genericpointersetenumerator rover__ ## item = mulle__pointerset_enumerate_generic( (struct mulle__pointerset *) set, callback); \
        *rover__  ## item ## __i = (void *) 0;                                                                                                        \
        ! rover__  ## item ## __i;                                                                                                                    \
        rover__ ## item ## __i   = (_mulle__genericpointersetenumerator_done( &rover__ ## item), (void *) 1))                                         \
                                                                                                                                                      \
      while( _mulle__genericpointersetenumerator_next( &rover__ ## item, (void **) &item))


#endif
