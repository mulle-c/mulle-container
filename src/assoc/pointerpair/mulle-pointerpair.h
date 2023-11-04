//
//  _mulle-pointerpair.h
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
#ifndef mulle_pointerpair_h__
#define mulle_pointerpair_h__

#include "mulle-container-callback.h"
#include "mulle-qsort.h"

#define mulle_pointerpair_notakey    mulle_not_a_pointer

//
// It is assumed, that the key must not have the value
// mulle_pointerpair_notakey.
//
struct mulle_pointerpair
{
   void   *key;
   void   *value;
};


static inline struct mulle_pointerpair
   mulle_pointerpair_make( void *key, void *value)
{
   struct mulle_pointerpair   result;

   result.key   = key;
   result.value = value;

   return( result);
}


static inline struct mulle_pointerpair   mulle_pointerpair_make_invalid( void)
{
   struct mulle_pointerpair   result;

   result.key   = mulle_pointerpair_notakey;
   result.value = NULL;

   return( result);
}


static inline int   mulle_pointerpair_is_invalid( struct mulle_pointerpair pair)
{
   return( pair.key == mulle_not_a_pointer);
}


MULLE_C_NONNULL_SECOND
static inline struct mulle_pointerpair
   mulle_pointerpair_retain( struct mulle_pointerpair pair,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   new_pair;

   assert( pair.key != callback->keycallback.notakey);

   new_pair.key   = (*callback->keycallback.retain)( &callback->keycallback, pair.key, allocator);
   new_pair.value = (*callback->valuecallback.retain)( &callback->valuecallback, pair.value, allocator);

   return( new_pair);
}



MULLE_C_NONNULL_SECOND
static inline
void  mulle_pointerpair_release( struct mulle_pointerpair pair,
                                 struct mulle_container_keyvaluecallback *callback,
                                 struct mulle_allocator *allocator)
{
   (*callback->keycallback.release)( &callback->keycallback, pair.key, allocator);
   (*callback->valuecallback.release)( &callback->valuecallback, pair.value, allocator);
}


//
// a < b  -1  descending
// a = b  0   equal
// a > 0  1    ascending
//
typedef int    mulle_pointerpair_compare_t( struct mulle_pointerpair *,
                                            struct mulle_pointerpair *,
                                            void *);
//
// for bsearch and sort
//
// Memo: checked with godbolt. If the comparison routine for besarch is
// known the compiler can inline the check. The actual bsearch code is not
// that large.
//
static inline int   _mulle_pointerpair_compare_pointer_key( struct mulle_pointerpair *a,
                                                            struct mulle_pointerpair *b,
                                                            void *userinfo)
{
   char   *s_a = a->key;
   char   *s_b = b->key;

   return( (int) (s_a - s_b));
}


static inline int   _mulle_pointerpair_compare_intptr_key( struct mulle_pointerpair *a,
                                                           struct mulle_pointerpair *b,
                                                           void *userinfo)
{
   intptr_t  v_a = (intptr_t) a->key;
   intptr_t  v_b = (intptr_t) b->key;

   return( (int) (v_a - v_b));
}



static inline int   _mulle_pointerpair_compare_string_key( struct mulle_pointerpair *a,
                                                           struct mulle_pointerpair *b,
                                                           void *userinfo)
{
   char  *a_s = a->key;
   char  *b_s = b->key;

   while( *a_s)
   {
      if (*a_s != *b_s)
         break;
      a_s++;
      b_s++;
   }

   return (int) *(unsigned char*) a_s - (int) *(unsigned char*) b_s;
}


//
// Buf must have been sorted with "compare" before for the bsearch to work.
//
// return -1 if not found, otherwise the index in buf (0-(n-1))
//
static inline int   _mulle_pointerpair_bsearch( struct mulle_pointerpair *buf,
                                                unsigned int n,
                                                struct mulle_pointerpair search,
                                                mulle_pointerpair_compare_t *compare,
                                                void *userinfo)
{
   struct mulle_pointerpair    *p;
   int                         first;
   int                         last;
   int                         middle;
   int                         rc;

   first  = 0;
   last   = n - 1;
   middle = (first + last) / 2;

   while( first <= last)
   {
      p = &buf[ middle];

      rc = (*compare)( p, &search, userinfo);
      if( rc <= 0)
      {
         if( rc == 0)
            return( middle);

         first = middle + 1;
      }
      else
         last = middle - 1;

      middle = (first + last) / 2;
   }

   return( -1);
}



//
// Buf must have been sorted with "compare" before for the bsearch to work.
//
// return -1 if not found, otherwise the index in buf (0-(n-1))
//
static inline int   _mulle_pointerpair_bsearch_or_less( struct mulle_pointerpair *buf,
                                                        unsigned int n,
                                                        struct mulle_pointerpair search,
                                                        mulle_pointerpair_compare_t *compare,
                                                        void *userinfo)
{
   struct mulle_pointerpair    *p;
   int                         first;
   int                         last;
   int                         middle;
   int                         closest;
   int                         rc;

   first   = 0;
   last    = n - 1;
   middle  = (first + last) / 2;
   closest = -1;

   while( first <= last)
   {
      p = &buf[ middle];

      rc = (*compare)( p, &search, userinfo);
      if( rc <= 0)
      {
         if( rc == 0)
            return( middle);
         if( middle > closest)
            closest = middle;
         
         first = middle + 1;
      }
      else
         last = middle - 1;

      middle = (first + last) / 2;
   }

   return( middle);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FOURTH
void   mulle_pointerpair_qsort_r( struct mulle_pointerpair *buf,
                                  unsigned int n,
                                  mulle_pointerpair_compare_t *compare,
                                  void *userinfo);



// callback can be empty, in which case pointer equality is seatched
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t
   _mulle_pointerpair_find_in_range_callback( struct mulle_pointerpair *buf,
                                              struct mulle_pointerpair search,
                                              struct mulle_range range,
                                              struct mulle_container_keyvaluecallback *callback);

uintptr_t
   _mulle_pointerpair_find_in_range_compare( struct mulle_pointerpair *buf,
                                             struct mulle_pointerpair search,
                                             struct mulle_range range,
                                             mulle_pointerpair_compare_t *compare,
                                             void *userinfo);

#endif
