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


// File:    STL.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160829     initial version
//
// This File should only be used by container implementation

#ifndef ABE_HEADERS_STL_H
#define ABE_HEADERS_STL_H 

#include <ABE/stl/Traits.h>
#include <new>
#include <string.h>     // memcpy & memmove
__BEGIN_NAMESPACE_ABE

// TYPE()
typedef void (*type_construct_t)(void * dest, UInt32);
// ~TYPE()
typedef void (*type_destruct_t)(void * dest, UInt32);
// TYPE(const TYPE&);
typedef void (*type_copy_t)(void * dest, const void *, UInt32);
// move TYPE
typedef void (*type_move_t)(void * dest, void * src, UInt32);
// operator< or operator> or operator==
typedef Bool (*type_compare_t)(const void * lhs, const void * rhs);

//////////////////////////////////////////////////////////////////////////////
template <typename TYPE> static ABE_INLINE void type_construct(void * dest, UInt32 n) {
    if (is_trivial_ctor<TYPE>::value) {
        // NOTHING
    } else {
        TYPE * p = static_cast<TYPE *>(dest);
        while (n--) { ::new (p++) TYPE; }
    }
}

//////////////////////////////////////////////////////////////////////////////
template <typename TYPE> static ABE_INLINE void type_destruct(void * dest, UInt32 n) {
    if (is_trivial_dtor<TYPE>::value) {
        // NOTHING
    } else {
        TYPE * p = static_cast<TYPE *>(dest);
        while (n--) { p->~TYPE(); ++p; }
    }
}

//////////////////////////////////////////////////////////////////////////////
template <typename TYPE> static ABE_INLINE void type_copy_trivial(void * dest, const void * src, UInt32 n) {
    memcpy(dest, src, n * sizeof(TYPE));
}

template <typename TYPE> static ABE_INLINE void type_copy(void * dest, const void * src, UInt32 n) {
    if (is_trivial_copy<TYPE>::value) {
        memcpy(dest, src, n * sizeof(TYPE));
    } else {
        TYPE * rhs = static_cast<TYPE *>(dest);
        const TYPE * lhs = static_cast<const TYPE *>(src);
        while (n--) { ::new (rhs++) TYPE(*lhs++); }
    }
}

//////////////////////////////////////////////////////////////////////////////
template <typename TYPE> static ABE_INLINE void type_move_trivial(void * dest, void * src, UInt32 n) {
    memmove(dest, src, n * sizeof(TYPE));
}

template <typename TYPE> static ABE_INLINE void type_move(void * dest, void * src, UInt32 n) {
    TYPE * lhs = (TYPE *)dest;
    TYPE * rhs  = (TYPE *)src;
#define _ABS(x)  ((x) > 0 ? (x) : (-(x)))
    if (is_trivial_move<TYPE>::value) {
        memmove(dest, src, n * sizeof(TYPE));
    } else if (_ABS(lhs - rhs) > n) {
        // no overlap
        if (is_trivial_copy<TYPE>::value) 
            type_copy_trivial<TYPE>(dest, src, n);
        else
            type_copy<TYPE>(dest, src, n);
        type_destruct<TYPE>(src, n);
    } else {
        if (lhs > rhs) {
            lhs += n;
            rhs += n;
            while (n--) {
                lhs -= 1;
                rhs -= 1;
                type_copy<TYPE>(lhs, rhs, 1);
                type_destruct<TYPE>(rhs, 1);
            }
        } else {
            while (n--) {
                type_copy<TYPE>(lhs, rhs, 1);
                type_destruct<TYPE>(rhs, 1);
                lhs += 1;
                rhs += 1;
            }
        }
    }
#undef _ABS
}

//////////////////////////////////////////////////////////////////////////////
// operator<
template <typename TYPE> static ABE_INLINE Bool type_compare_lt(const void* lhs, const void* rhs) {
    return *static_cast<const TYPE*>(lhs) < *static_cast<const TYPE*>(rhs);
}

// operator>
template <typename TYPE> static ABE_INLINE Bool type_compare_gt(const void* lhs, const void* rhs) {
    return *static_cast<const TYPE*>(lhs) > *static_cast<const TYPE*>(rhs);
}

// operator==
template <typename TYPE> static ABE_INLINE Bool type_compare_eq(const void* lhs, const void* rhs) {
    return *static_cast<const TYPE*>(lhs) == *static_cast<const TYPE*>(rhs);
}

//////////////////////////////////////////////////////////////////////////////
// implementation of hash of core types
template <typename TYPE> static ABE_INLINE UInt32 hash(const TYPE& value) {
    return value.hash();
};

#define HASH_BASIC_TYPES32(TYPE)                                                        \
    template <> ABE_INLINE UInt32 hash(const TYPE& v) { return UInt32(v); }
#define HASH_BASIC_TYPES64(TYPE)                                                        \
    template <> ABE_INLINE UInt32 hash(const TYPE& v) { return UInt32((v >> 32) ^ v); }
#define HASH_BASIC_TYPES(TYPE)                                                          \
    template <> ABE_INLINE UInt32 hash(const TYPE& v) {                                 \
        UInt32 x = 0;                                                                   \
        const UInt8 *u8 = (const UInt8*)(&v);                                           \
        for (UInt32 i = 0; i < sizeof(TYPE); ++i) x = x * 31 + u8[i];                   \
        return x;                                                                       \
    };

HASH_BASIC_TYPES32  (Char);
HASH_BASIC_TYPES32  (UInt8);
HASH_BASIC_TYPES32  (Int8);
HASH_BASIC_TYPES32  (UInt16);
HASH_BASIC_TYPES32  (Int16);
HASH_BASIC_TYPES32  (UInt32);
HASH_BASIC_TYPES32  (Int32);
HASH_BASIC_TYPES64  (UInt64);
HASH_BASIC_TYPES64  (Int64);
HASH_BASIC_TYPES    (Float32);
HASH_BASIC_TYPES    (Float64);
#undef HASH_BASIC_TYPES
#undef HASH_BASIC_TYPES32
#undef HASH_BASIC_TYPES64

template <typename TYPE> ABE_INLINE UInt32 hash(TYPE * const& p) {
    return hash<UInt64>(UInt64(p));
};

typedef UInt32 (*type_hash_t)(const void *);

template <typename TYPE>
static ABE_INLINE UInt32 type_hash(const void * k) {
    return hash(*static_cast<const TYPE *>(k));
}


__END_NAMESPACE_ABE

#endif // ABE_HEADERS_STL_H 
