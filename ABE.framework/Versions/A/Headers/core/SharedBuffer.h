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

// File:    SharedBuffer.h
// Author:  mtdcy.chen
// Changes:
//          1. 20181112     initial version
//

#ifndef ABE_HEADERS_SHARED_BUFFER_H
#define ABE_HEADERS_SHARED_BUFFER_H
#include <ABE/core/Types.h>
#include <ABE/core/Allocator.h>

__BEGIN_NAMESPACE_ABE

// A COW BUFFER
// NO new or delete in SharedBuffer, always using mAllocator
struct ABE_EXPORT SharedBuffer {
    public:
        /**
         * create a cow buffer and retain it.
         * @param a Allocator reference.
         * @param number bytes to allocate.
         * @return return a reference to new created cow buffer on success, otherwise Nil.
         */
        static SharedBuffer * Create(const sp<Allocator>&, UInt32);

    public:
        /**
         * retain this cow buffer.
         * @note retain is thread safe, can perform without lock.
         */
        SharedBuffer *      RetainBuffer();
    
        /**
         * release this cow buffer
         * @param keep  if keep == False & this is the last ref, backend memory will be released
         *             if keep == True & this is the last ref, backend memory will NOT release, PLEASE delete it explicit
         * @return return remains retain count.
         * @note release is thread safe, can perform without lock.
         * @note if client have to do extra destruction work on this cow buffer, PLEASE set keep to False.
         */
        UInt32              ReleaseBuffer(Bool keep = False);
        void                DeleteBuffer();
    
        /**
         * get this cow buffer's retain count
         * @note thread safe.
         */
        ABE_INLINE UInt32   GetRetainCount() const      { return mRefs.load();      }
    
        /**
         * is this cow buffer shared with others
         * @note thread safe.
         * @note if IsBufferNotShared() == True, it is safe to do anything later
         * @note if IsBufferShared() == True, you may have to test again later
         */
        ABE_INLINE Bool     IsBufferShared() const      { return mRefs.load() > 1;  }
        ABE_INLINE Bool     IsBufferNotShared() const   { return mRefs.load() == 1; }

    public:
        /**
         * perform edit before you write to this cow buffer
         * @param number of bytes to edit, zero means not changed.
         * @note if this cow is not shared, it does NOTHING. otherwise it perform a cow action
         * @note edit with new size always perform a cow action.
         * @note edit with new size will assert on failure
         */
        SharedBuffer *      edit(UInt32 = 0);

    public:
        ABE_INLINE Char *       data()                  { return mData;             }
        ABE_INLINE const Char * data() const            { return mData;             }
        ABE_INLINE UInt32       size() const            { return mSize;             }
    private:
        sp<Allocator>   mAllocator;
        Char *          mData;
        UInt32          mSize;
        mutable Atomic<UInt32> mRefs;
    
        // PLEASE USE Create() TO ALLOC A NEW SHARED BUFFER.
        SharedBuffer() : mAllocator(Nil), mData(Nil), mSize(0), mRefs(0) { }
        ~SharedBuffer();
        DISALLOW_EVILS(SharedBuffer);
};

__END_NAMESPACE_ABE
#endif // ABE_HEADERS_SHARED_BUFFER_H
