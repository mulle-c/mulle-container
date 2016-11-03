//
//  _mulle_pointerpair.h
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

#include "mulle_container_callback.h"
#include "mulle_container_compiler_attributes.h"


struct mulle_pointerpair
{
   void   *_key;  // suggestively named
   void   *_value;
};



static inline struct mulle_pointerpair   mulle_pointerpair_create( void *key, void *value)
{
   struct mulle_pointerpair   result;
   
   result._key   = key;
   result._value = value;
   return( result);
}


static inline void   *_mulle_pointerpair_get_key( struct mulle_pointerpair *pair)
{
   return( pair->_key);
}


static inline void   *mulle_pointerpair_get_key( struct mulle_pointerpair *pair)
{
   return( pair ? pair->_key : NULL);
}


static inline void   *_mulle_pointerpair_get_value( struct mulle_pointerpair *pair)
{
   return( pair->_value);
}


static inline void   *mulle_pointerpair_get_value( struct mulle_pointerpair *pair)
{
   return( pair ? pair->_value : NULL);
}


static inline void   mulle_pointerpair_set_key( struct mulle_pointerpair *pair, void *key)
{
   assert( pair);
   pair->_key = key;
}


static inline void   mulle_pointerpair_set_value( struct mulle_pointerpair *pair, void * value)
{
   assert( pair);
   pair->_value = value;
}

#endif
