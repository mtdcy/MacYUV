/******************************************************************************
 * Copyright (c) 2016, Chen Fang <mtdcy.chen@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


// File:    Bits.cpp
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef ABE_STL_BITS_H
#define ABE_STL_BITS_H

#include <ABE/core/Types.h>

__BEGIN_NAMESPACE_ABE

template <typename T>
class ABE_EXPORT Bits : public StaticObject {
    public:
        ABE_INLINE Bits() : V(0) { }
        ABE_INLINE Bits(T v) : V(v) { }
        ABE_INLINE Bits(const Bits& rhs) : V(rhs.V) { }
        ABE_INLINE Bits& operator=(const Bits& rhs) { V = rhs.V; return *this;  }
    
    public:
        ABE_INLINE T    set(UInt32 n)           { V |= ((T)1 << n); return V;   }
        ABE_INLINE T    clear(UInt32 n)         { V &= ~((T)1 << n); return V;  }
        ABE_INLINE T    flip(UInt32 n)          { V ^= ((T)1 << n); return V;   }
        ABE_INLINE void clear()                 { V = 0;                        }
        ABE_INLINE T    flip()                  { V = ~V; return V;             }
        ABE_INLINE Bool test(UInt32 n) const    { return V & ((T)1 << n);       }
        ABE_INLINE Bool empty() const           { return V == 0;                }
        ABE_INLINE T    value() const           { return V;                     }
    
    private:
        T V;
};
__END_NAMESPACE_ABE
#endif // ABE_STL_BITS_H 
