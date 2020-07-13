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


// File:    Queue.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160829     initial version
//
#ifndef ABE_STL_QUEUE_H
#define ABE_STL_QUEUE_H
#include <ABE/stl/TypeHelper.h>
__BEGIN_NAMESPACE_ABE_PRIVATE
/**
 * a lock free queue implement
 */
class ABE_EXPORT LockFreeQueue : public SharedObject {
    public:
        LockFreeQueue(UInt32 dataLength, type_destruct_t);

    public:
        UInt32          push1(const void * what, type_copy_t);   // for single producer
        UInt32          pushN(const void * what, type_copy_t);   // for multi producer
        Bool            pop1(void * what, type_move_t);          // for single consumer
        Bool            popN(void * what, type_move_t);          // for multi consumer
        UInt32          size() const;
        void            clear();

    private:
        struct Node;
        Node *          allocateNode();
        void            freeNode(Node *);

        const UInt32        mDataLength;
        volatile UInt32     mLength;
        volatile Node *     mHead;
        volatile Node *     mTail;
        type_destruct_t     mDeletor;

        OBJECT_TAIL(LockFreeQueue);
};
__END_NAMESPACE_ABE_PRIVATE

__BEGIN_NAMESPACE_ABE
namespace LockFree {
    template <class TYPE> class ABE_EXPORT Queue : public StaticObject {
        public:
            ABE_INLINE Queue() : mImpl(new __NAMESPACE_PRIVATE::LockFreeQueue(sizeof(TYPE), type_destruct<TYPE>)) { }
            ABE_INLINE ~Queue() { }

            ABE_INLINE UInt32      size() const        { return mImpl->size();                      }
            ABE_INLINE Bool        empty() const       { return size() == 0;                        }
            ABE_INLINE void        clear()             { mImpl->clear();                            }
            ABE_INLINE UInt32      push(const TYPE& v) { return mImpl->pushN(&v, type_copy<TYPE>);  }
            ABE_INLINE Bool        pop(TYPE& v)        { return mImpl->popN(&v, type_move<TYPE>);   }
        
        private:
            sp<__NAMESPACE_PRIVATE::LockFreeQueue> mImpl;
    };
};
__END_NAMESPACE_ABE
#endif // ABE_STL_QUEUE_H
