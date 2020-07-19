/******************************************************************************
 * Copyright (c) 2020, Chen Fang <mtdcy.chen@gmail.com>
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


/**
 * File:    Matrix.h
 * Author:  mtdcy.chen
 * Changes:
 *          1. 20181214     initial version
 *
 */

#ifndef _ABE_MATH_MATRIX_H
#define _ABE_MATH_MATRIX_H

#include <ABE/core/Types.h>

__BEGIN_NAMESPACE_ABE

/**
 * N x N matrix
 * https://en.wikipedia.org/wiki/Matrix_(mathematics)
 */
template <typename TYPE, UInt32 N> struct Matrix : public StaticObject {
    /**
     * 1 x N row vector, tranpose as operator need
     */
    struct Vector {
        TYPE mEntries[N];
        
        Vector() { for (UInt32 i = 0; i < N; ++i) mEntries[i] = 0; }
        Vector(TYPE v) { for (UInt32 i = 0; i < N; ++i) mEntries[i] = v; }
        Vector(const TYPE v[N]) { for (UInt32 i = 0; i < N; ++i) mEntries[i] = v[i]; }
        
        Vector& operator=(TYPE v) { for (UInt32 i = 0; i < N; ++i) mEntries[i] = v; return *this; }
        Vector& operator=(const TYPE v[N]) { for (UInt32 i = 0; i < N; ++i) mEntries[i] = v[i]; return *this; }
        
        ABE_INLINE TYPE& operator[](UInt32 i) { return mEntries[i]; }
        ABE_INLINE const TYPE& operator[](UInt32 i) const { return mEntries[i]; }
        
        ABE_INLINE Bool operator==(const Vector& rhs) const { for (UInt32 i = 0; i < N; ++i) if (mEntries[i] != rhs[i]) return False; return True; }
        ABE_INLINE Bool operator!=(const Vector& rhs) const { return operator==(rhs) == False; }
        
        // per-entry calculation
        ABE_INLINE Vector operator+(const Vector& rhs) const {
            Vector result;
            for (UInt32 i = 0; i < N; ++i) result[i] = mEntries[i] + rhs[i];
            return result;
        }
        
        ABE_INLINE Vector& scale(const Vector& rhs) {
            for (UInt32 i = 0; i < N; ++i) mEntries[i] *= rhs[i];
            return *this;
        }
        
        // matrix multiplication - dot product
        ABE_INLINE TYPE operator*(const Vector& rhs) const {      // dot product
            TYPE sum = 0;
            for (UInt32 i = 0; i < N; ++i) sum += mEntries[i] * rhs[i];
            return sum;
        }
        
        ABE_INLINE Vector operator*(const Matrix& rhs) const {    // dot product
            Vector result;
            for (UInt32 i = 0; i < N; ++i) result[i] = operator*(rhs[i]);
            return result;
        }
    };
    
    Vector mEntries[N];     // rows
    Matrix() { for (UInt32 i = 0; i < N; ++i) mEntries[i][i] = 1; }
    Matrix(const Vector v[N]) { for (UInt32 i = 0; i < N; ++i) mEntries[i] = v[i]; }
    Matrix(const TYPE v[N*N]) { for (UInt32 i = 0; i < N; ++i) for (UInt32 j = 0; j < N; ++j) mEntries[i][j] = v[i * N + j]; }
    
    Matrix& operator=(const Vector v[N]) { for (UInt32 i = 0; i < N; ++i) mEntries[i] = v[i]; return *this; }
    Matrix& operator=(const TYPE v[N*N]) { for (UInt32 i = 0; i < N; ++i) for (UInt32 j = 0; j < N; ++j) mEntries[i][j] = v[i * N + j]; return *this; }
    
    ABE_INLINE Vector& operator[](UInt32 i) { return mEntries[i]; }
    ABE_INLINE const Vector& operator[](UInt32 i) const { return mEntries[i]; }
    
    ABE_INLINE Matrix& transpose() {
        for (UInt32 i = 0; i < N; ++i) {
            for (UInt32 j = i; j < N; ++j) {
                TYPE tmp = mEntries[i][j];
                mEntries[i][j] = mEntries[j][i];
                mEntries[j][i] = tmp;
            }
        }
        return *this;
    }
    
    // per-entry calculation
    ABE_INLINE Matrix operator+(const Matrix& rhs) const {
        Matrix result;
        for (UInt32 i = 0; i < N; ++i) result[i] = mEntries[i] + rhs[i];
        return result;
    }
    
    ABE_INLINE Matrix& scale(const Matrix& rhs) {
        for (UInt32 i = 0; i < N; ++i) mEntries[i].scale(rhs[i]);
        return *this;
    }
    
    // matrix . vector
    ABE_INLINE Vector operator*(const Vector& rhs) const {    // dot product
        Vector result;
        for (UInt32 i = 0; i < N; ++i) result[i] = mEntries[i] * rhs;
        return result;
    }
    
    // matrix . matrix
    ABE_INLINE Matrix operator*(const Matrix& rhs) const {    // dot product
        Matrix result;
        for (UInt32 i = 0; i < N; ++i)          // row
            for (UInt32 j = 0; j < N; ++j)      // column
                result[i][j] = mEntries[i] * rhs[j];
        return result;
    }
};

__END_NAMESPACE_ABE

#endif // _ABE_MATH_MATRIX_H
