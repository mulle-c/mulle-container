//
//  Copyright (C) 2011 Nat!, Mulle kybernetiK.
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

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>


static inline uintptr_t  mulle_prime_for_depth( int depth)
{
   extern uintptr_t   __mulle_primes[];

   assert( depth >= -32 && depth <= 32);
   return( __mulle_primes[ 32 + depth]);
}


//
// this produces a lot of fast code...
// it looks funny, but rocks on my i7
//
static inline uintptr_t  mulle_prime_hash_for_depth( uintptr_t value, int depth)
{
   switch( depth)
   {
   case  32 : return( value % 2147483659U);
   case  31 : return( value % 1073741827U);
   case  30 : return( value % 536870923U);
   case  29 : return( value % 268435459U);
   case  28 : return( value % 134217757);
   case  27 : return( value % 67108879);
   case  26 : return( value % 33554467);
   case  25 : return( value % 16777259);
   case  24 : return( value % 8388617);
   case  23 : return( value % 4194319);
   case  22 : return( value % 2097169);
   case  21 : return( value % 1048583);
   case  20 : return( value % 524309);
   case  19 : return( value % 262147);
   case  18 : return( value % 131101);
   case  17 : return( value % 65537);

   case  16 : return( value % 32771);
   case  15 : return( value % 16411);
   case  14 : return( value % 8209);
   case  13 : return( value % 4099);
   case  12 : return( value % 2053);
   case  11 : return( value % 1031);
   case  10 : return( value % 521);
   case   9 : return( value % 257);
   case   8 : return( value % 131);
   case   7 : return( value % 67);
   case   6 : return( value % 37);
   case   5 : return( value % 17);
   case   4 : return( value % 11);
   case   3 : return( value % 5);
   case   2 : return( value % 2);
   case   1 : return( value % 1);

   case   0 : return( 0);

   case  -1 : return( value % 1);
   case  -2 : return( value % 2);
   case  -3 : return( value % 5);
   case  -4 : return( value % 11);
   case  -5 : return( value % 17);
   case  -6 : return( value % 37);
   case  -7 : return( value % 67);
   case  -8 : return( value % 131);
   case  -9 : return( value % 257);
   case -10 : return( value % 521);
   case -11 : return( value % 1031);
   case -12 : return( value % 2053);
   case -13 : return( value % 4099);
   case -14 : return( value % 8209);
   case -15 : return( value % 16411);
   case -16 : return( value % 32771);

   case -17 : return( value % 65537);
   case -18 : return( value % 131101);
   case -19 : return( value % 262147);
   case -20 : return( value % 524309);
   case -21 : return( value % 1048583);
   case -22 : return( value % 2097169);
   case -23 : return( value % 4194319);
   case -24 : return( value % 8388617);
   case -25 : return( value % 16777259);
   case -26 : return( value % 33554467);
   case -27 : return( value % 67108879);
   case -28 : return( value % 134217757);
   case -29 : return( value % 268435459U);
   case -30 : return( value % 536870923U);
   case -31 : return( value % 1073741827U);
   case -32 : return( value % 2147483659U);
   }

   abort();
}
