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
#ifndef mulle__pointerset_struct_h__
#define mulle__pointerset_struct_h__

#include "include.h"

#include "mulle-container-operation.h"
#include "mulle-container-callback-global.h"


#define MULLE__POINTERSET_FILL_SHIFT    2
#define MULLE__POINTERSET_MIN_SIZE      (1 << MULLE__POINTERSET_FILL_SHIFT)
#define MULLE__POINTERSET_INITIAL_SIZE  4


// assume we have
// 0 1 2 3  (sentinel 0)
//
// we want to hash either
// to 0 an 2 for better insert perfomance
//
// 0 2
static inline unsigned int
   mulle__pointerset_hash_for_size( uintptr_t hash, unsigned int size)
{
   assert( size >= 2);
   return( hash & (size - 2));
}


/* set is a primitive growing hashtable, it's like mulle-set but without a
 * callback, therefore no retain/release and two choices hardcoded:
 * notakey is mulle_not_a_pointer
 * hash is mulle_pointer_hash
 */

// fields are considered private
#define MULLE__POINTERSET_BASE  \
   void           **_storage;   \
   unsigned int   _count;       \
   unsigned int   _size

// NSSet/NSMutableSet/NSHashTable

struct mulle__pointerset
{
   MULLE__POINTERSET_BASE;
};


#pragma mark - setup and takedown

struct mulle__pointerset   *_mulle__pointerset_create( unsigned int capacity,
                                                       size_t extra,
                                                       struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void    _mulle__pointerset_destroy( struct mulle__pointerset *set,
                                    struct mulle_allocator *allocator);


// if capacity is 0, this just does a memset 0
MULLE_C_NONNULL_FIRST
void    _mulle__pointerset_init( struct mulle__pointerset *set,
                                 unsigned int capacity,
                                 struct mulle_allocator *allocator);

MULLE_C_NONNULL_FIRST
void    _mulle__pointerset_done( struct mulle__pointerset *set,
                                 struct mulle_allocator *allocator);


#pragma mark - copying

MULLE_C_NONNULL_FIRST
struct mulle__pointerset   *_mulle__pointerset_copy( struct mulle__pointerset *set,
                                                     struct mulle_allocator *allocator);


#pragma mark - petty accessors


// size is the allocated size, it's always >= count
MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__pointerset_get_size( struct mulle__pointerset *set)
{
   return( set->_size);
}


// count is the number of pointers in the set
MULLE_C_NONNULL_FIRST
static inline unsigned int   _mulle__pointerset_get_count( struct mulle__pointerset *set)
{
   return( set->_count);
}


#pragma mark - query

MULLE_C_NONNULL_FIRST
static inline int  _mulle__pointerset_is_full( struct mulle__pointerset *set)
{
   unsigned int    size;

   size = set->_size;
   size = (size - (size >> MULLE__POINTERSET_FILL_SHIFT));  // full when 75% occupied
   return( set->_count >= size);
}


// sparse means the size is too large for the contents, a shrink wouldn't hurt
MULLE_C_NONNULL_FIRST
static inline int  _mulle__pointerset_is_sparse( struct mulle__pointerset *set)
{
   unsigned int    size;

   size = set->_size / 2;
   size = (size - (size >> MULLE__POINTERSET_FILL_SHIFT));
   return( set->_count < size);
}


#pragma mark - operations

MULLE_C_NONNULL_FIRST
void   *_mulle__pointerset_get( struct mulle__pointerset *set,
                                void *p);



#pragma mark - enumeration

#define MULLE__POINTERSETENUMERATOR_BASE  \
   void           **_curr;                \
   unsigned int   _left


struct mulle__pointersetenumerator
{
   MULLE__POINTERSETENUMERATOR_BASE;
};


#define mulle__pointersetenumerator_empty \
   ((struct mulle__pointersetenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct mulle__pointersetenumerator
   _mulle__pointerset_enumerate( struct mulle__pointerset *set)
{
   struct mulle__pointersetenumerator   rover;

   rover._left = set->_count;
   rover._curr = set->_storage;

   return( rover);
}


static inline struct mulle__pointersetenumerator
   mulle__pointerset_enumerate( struct mulle__pointerset *set)
{
   if( ! set)
      return( mulle__pointersetenumerator_empty);

   return( _mulle__pointerset_enumerate( set));
}


MULLE_C_NONNULL_FIRST
static inline int   
   _mulle__pointersetenumerator_next( struct mulle__pointersetenumerator *rover,
                                      void **item)
{
   void   *p;

   if( ! rover->_left)
   {
      if( item)
         *item = mulle_not_a_pointer;
      return( 0);
   }

   for(;;)
   {
      p = *rover->_curr++;
      if( p != mulle_not_a_pointer)
      {
         rover->_left--;
         if( item)
            *item = p;
         return( 1);
      }
   }
}


static inline void   _mulle__pointersetenumerator_done( struct mulle__pointersetenumerator *rover)
{
}

static inline void   mulle__pointersetenumerator_done( struct mulle__pointersetenumerator *rover)
{
}

#endif
