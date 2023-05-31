//  mulle-container
//
//  Created by Nat! on 7.09.16
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
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
#include "mulle-container-callback.h"

#include "include.h"


// notakey is mulle_not_an_int
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_int;
// notakey is mulle_not_an_intptr
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_intptr;
// notakey is NULL
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_copied_cstring;
// notakey is NULL
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_nonowned_cstring;
// notakey is NULL
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_owned_cstring;
// notakey is NULL
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer;
// notakey is NULL
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_owned_pointer;
// notakey is mulle_not_a_pointer
MULLE__CONTAINER_GLOBAL struct mulle_container_keycallback   mulle_container_keycallback_nonowned_pointer_or_null;

MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_int;
MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_intptr;
MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_copied_cstring;
MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_owned_cstring;
MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_cstring;
MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_nonowned_pointer;
MULLE__CONTAINER_GLOBAL struct mulle_container_valuecallback   mulle_container_valuecallback_owned_pointer;
