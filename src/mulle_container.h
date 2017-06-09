//
//  mulle_container.h
//  mulle-container
//
//  Created by Nat! on 02/11/15.
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
#ifndef mulle_container__h__
#define mulle_container__h__

//
// community version is always even
//
#define MULLE_CONTAINER_VERSION  ((0 << 20) | (9 << 8) | 1)

#include <mulle_allocator/mulle_allocator.h>

#include "mulle_container_operation.h"
#include "mulle_container_callback.h"
#include "mulle_container_callback_global.h"

#include "mulle_prime.h"
#include "mulle_hash.h"

#include "_mulle_array.h"
#include "mulle_array.h"

#include "_mulle_set.h"
#include "mulle_set.h"
#include "_mulle_bigset.h"
#include "mulle_bigset.h"

#include "_mulle_map.h"
#include "mulle_map.h"
#include "_mulle_bigmap.h"
#include "mulle_bigmap.h"

#include "_mulle_queue.h"
#include "mulle_pointerset.h"
#include "mulle_pointerarray.h"
#include "mulle_pointerpairarray.h"

#include "mulle_container_callback.h"
#include "mulle_container_operation.h"

#if MULLE_ALLOCATOR_VERSION < ((2 << 20) | (0 << 8) | 0)
# error "mulle_allocator is too old"
#endif

#endif /* mulle_container_h */
