//
//  mulle-container.h
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

#define MULLE_CONTAINER_VERSION  ((2 << 20) | (0 << 8) | 0)

#include "include.h"

#include "mulle-container-operation.h"
#include "mulle-container-math.h"
#include "mulle-container-callback.h"
#include "mulle-container-callback-global.h"

#include "mulle-prime.h"
#include "mulle-hash.h"

#include "mulle--array.h"
#include "mulle-array.h"

#include "mulle--set.h"
#include "mulle-set.h"

#include "mulle--map.h"
#include "mulle-map.h"

#include "mulle-range.h"
#include "mulle--rangeset.h"

#include "mulle--pointerqueue.h"
#include "mulle-pointerarray.h"
#include "mulle--pointermap.h"
#include "mulle-pointerset.h"
#include "mulle-pointerpairarray.h"

#include "mulle-container-callback.h"
#include "mulle-container-operation.h"

#if MULLE_C11_VERSION < ((3 << 20) | (2 << 8) | 0)
# error "mulle-c11 is too old"
#endif

#if MULLE_ALLOCATOR_VERSION < ((2 << 20) | (0 << 8) | 0)
# error "mulle-allocator is too old"
#endif


#endif /* mulle_container_h */
