//
//  mulle-pointerpair.c
//  mulle-container
//
//  Copyright (c) 2023 Nat! - Mulle kybernetiK.
//  All rights reserved.
//
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
#include "mulle-pointerpair.h"


MULLE_C_NEVER_INLINE
static void   _mulle_pointerpair_qsort_compare_pointer_key( struct mulle_pointerpair *buf,
                                                            size_t n,
                                                            void *userinfo)
{
   _mulle_qsort_r_inline( buf,
                          n,
                          sizeof( struct mulle_pointerpair),
                          (int (*)(void *, void *, void *)) _mulle_pointerpair_compare_pointer_key,
                          userinfo);
}


MULLE_C_NEVER_INLINE
static void   _mulle_pointerpair_qsort_compare_string_key( struct mulle_pointerpair *buf,
                                                           size_t n,
                                                           void *userinfo)
{
   _mulle_qsort_r_inline( buf,
                          n,
                          sizeof( struct mulle_pointerpair),
                          (int (*)(void *, void *, void *)) _mulle_pointerpair_compare_string_key,
                          userinfo);
}


void   mulle_pointerpair_qsort_r( struct mulle_pointerpair *buf,
                                  size_t n,
                                  mulle_pointerpair_compare_t *compare,
                                  void *userinfo)
{
   if( ! buf)
      return;

   if( compare == _mulle_pointerpair_compare_pointer_key)
      _mulle_pointerpair_qsort_compare_string_key( buf, n, userinfo);
   else
      if( compare == _mulle_pointerpair_compare_string_key)
         _mulle_pointerpair_qsort_compare_pointer_key( buf, n, userinfo);
      else
         _mulle_qsort_r_inline( buf,
                                n,
                                sizeof( struct mulle_pointerpair),
                                (int (*)(void *, void *, void *)) compare,
                                userinfo);
}


uintptr_t
   _mulle_pointerpair_find_in_range_callback( struct mulle_pointerpair *buf,
                                              struct mulle_pointerpair search,
                                              struct mulle_range range,
                                              struct mulle_container_keyvaluecallback *callback)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   // pointerpair can't use it
   assert( range.length != (uintptr_t) -1);

   if( mulle_pointerpair_is_invalid( search))
      return( mulle_not_found_e);

   p        = &buf[ range.location];
   sentinel = &p[ range.length];

   if( ! callback || _mulle_container_keycallback_isbitequals( &callback->keycallback))
   {
      while( p < sentinel)
      {
         if( p->key == search.key)
            return( p - buf);

         p++;
      }
   }
   else
   {
      while( p < sentinel)
      {
         if( (*callback->keycallback.is_equal)( &callback->keycallback, p->key, search.key))
            return( p - buf);

         p++;
      }
   }
   return( mulle_not_found_e);
}



uintptr_t
   _mulle_pointerpair_find_in_range_compare( struct mulle_pointerpair *buf,
                                             struct mulle_pointerpair search,
                                             struct mulle_range range,
                                             mulle_pointerpair_compare_t *compare,
                                             void *userinfo)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   if( ! buf)
      return( mulle_not_found_e);

   // pointerpair can't use it
   if( range.length == (uintptr_t) -1)
      abort();

   assert( search.key != mulle_pointerpair_notakey);

   p        = &buf[ range.location];
   sentinel = &p[ range.length];

   if( ! compare || compare == _mulle_pointerpair_compare_pointer_key)
   {
      while( p < sentinel)
      {
         if( p->key == search.key)
            return( p - buf);

         p++;
      }
   }
   else
   {
      while( p < sentinel)
      {
         if( (*compare)( p, &search, userinfo) == 0)
            return( p - buf);

         p++;
      }
   }
   return( mulle_not_found_e);
}




