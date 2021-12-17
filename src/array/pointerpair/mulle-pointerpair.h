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


static inline struct mulle_pointerpair
   mulle_pointerpair_make_invalid( void)
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

#endif
