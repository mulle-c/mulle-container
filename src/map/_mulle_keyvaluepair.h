//
//  _mulle_keyvaluepair.h
//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright © 2015 Mulle kybernetiK. All rights reserved.
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
#ifndef _mulle_keyvaluepair_h
#define _mulle_keyvaluepair_h

#include "mulle_container_callback.h"
#include "mulle_container_attributes.h"


struct _mulle_keyvaluepair
{
   void   *_key;
   void   *_value;
};


//
// some somewhat obscure facilities for map
//
static inline int  __mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair, void *not_a_key_marker) mulle_nonnull_first;
static inline int  __mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair, void *not_a_key_marker)
{
   return( pair->_value == 0 && pair->_key == not_a_key_marker);
}


static inline int  _mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline int  _mulle_keyvaluepair_is_empty( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback)
{
   return( __mulle_keyvaluepair_is_empty( pair, callback->keycallback.not_a_key_marker));
}


static inline int  __mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                  void *not_a_key_marker) mulle_nonnull_first;
static inline int  __mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                  void *not_a_key_marker)
{
   return( pair->_value != 0 && pair->_key == not_a_key_marker);
}


static inline int  _mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline int  _mulle_keyvaluepair_is_queue( struct _mulle_keyvaluepair *pair,
                                                 struct mulle_container_keyvaluecallback *callback)
{
   return( __mulle_keyvaluepair_is_queue( pair, callback->keycallback.not_a_key_marker));
}


static inline int  __mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                    void *not_a_key_marker) mulle_nonnull_first;
static inline int  __mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                    void *not_a_key_marker)
{
   return( pair->_key != not_a_key_marker);
}


static inline int  _mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                   struct mulle_container_keyvaluecallback *callback) mulle_nonnull_first_second;
static inline int  _mulle_keyvaluepair_is_storage( struct _mulle_keyvaluepair *pair,
                                                   struct mulle_container_keyvaluecallback *callback)
{
   return( __mulle_keyvaluepair_is_storage( pair, callback->keycallback.not_a_key_marker));
}


#endif /* _mulle_keyvaluepair_h */
