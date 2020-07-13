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


// File:    Vector.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160829     initial version
//


#ifndef ABE_STL_VECTOR_H
#define ABE_STL_VECTOR_H 

#include <ABE/stl/TypeHelper.h>
#include <ABE/core/Allocator.h>
#include <ABE/core/SharedBuffer.h>

__BEGIN_NAMESPACE_ABE_PRIVATE
class ABE_EXPORT Vector : public SharedObject {
    public:
        Vector(const sp<Allocator>& allocator,
               UInt32 capacity, UInt32 dataLength,
               type_destruct_t, type_move_t);

    public:
        void        clear();
        void        shrink();

        ABE_INLINE UInt32   size() const        { return mItemCount; }
        ABE_INLINE UInt32   capacity() const    { return mCapacity; }

    public:
        const void *    access  (UInt32 index) const;
        void *          access  (UInt32 index);

        void *      emplace (UInt32 pos, type_construct_t);
        void        insert  (UInt32 pos, const void * what, type_copy_t);
        void        erase   (UInt32 pos);
        void        erase   (UInt32 first, UInt32 last);

        void        sort    (type_compare_t);

    private:
        void *      grow   (UInt32 pos, UInt32 amount);
        void        remove (UInt32 pos, UInt32 amount);

    private:
        sp<Allocator>   mAllocator;
        const UInt32    mDataLength;
        UInt32          mCapacity;
        UInt32          mItemCount;
        void *          mItems;
        type_destruct_t mDeletor;
        type_move_t     mMover;
    
        OBJECT_TAIL(Vector);
};
__END_NAMESPACE_ABE_PRIVATE

__BEGIN_NAMESPACE_ABE
// Note:
// 1. No interator for Vector because it's random access has constant time.
// 2. No auto memory shrink
template <typename T> class ABE_EXPORT Vector : public StaticObject {
    public:
        ABE_INLINE Vector(UInt32 capacity = 4, const sp<Allocator>& allocator = kAllocatorDefault) :
        mImpl(new __NAMESPACE_PRIVATE::Vector(allocator, capacity, sizeof(T), type_destruct<T>, type_move<T>)) { }

        ABE_INLINE ~Vector() { }

    public:
        ABE_INLINE void   clear()          { mImpl->clear();           }
        ABE_INLINE void   shrink()         { mImpl->shrink();          }
        ABE_INLINE UInt32 size() const     { return mImpl->size();     }
        ABE_INLINE Bool   empty() const    { return size() == 0;       }
        ABE_INLINE UInt32 capacity() const { return mImpl->capacity(); }

    public:
        // stable sort, uses operator<
        ABE_INLINE void sort() { mImpl->sort(type_compare_lt<T>); }

    public:
        // element access with range check which is not like std::vector::operator[]
        ABE_INLINE T&       operator[](UInt32 index)       { return *static_cast<T*>(mImpl->access(index));       }
        ABE_INLINE const T& operator[](UInt32 index) const { return *static_cast<const T*>(mImpl->access(index)); }

    public:
        ABE_INLINE T&       front()       { return *static_cast<T*>(mImpl->access(0));              }
        ABE_INLINE T&       back()        { return *static_cast<T*>(mImpl->access(size()-1));       }
        ABE_INLINE const T& front() const { return *static_cast<const T*>(mImpl->access(0));        }
        ABE_INLINE const T& back() const  { return *static_cast<const T*>(mImpl->access(size()-1)); }

    public:
        ABE_INLINE void push(const T& v) { mImpl->insert(size(), &v, type_copy<T>);                            }
        ABE_INLINE T&   push()           { return *static_cast<T*>(mImpl->emplace(size(), type_construct<T>)); }
        ABE_INLINE void pop()            { mImpl->erase(size()-1);                                             }

    public:
        ABE_INLINE void erase(UInt32 index)              { mImpl->erase(index);       }
        ABE_INLINE void erase(UInt32 first, UInt32 last) { mImpl->erase(first, last); }
        ABE_INLINE void insert(UInt32 index, const T& v) { mImpl->insert(index, &v, type_copy<T>);                            }
        ABE_INLINE T&   insert(UInt32 index)             { return *static_cast<T*>(mImpl->emplace(index, type_construct<T>)); }
        
    private:
        sp<__NAMESPACE_PRIVATE::Vector> mImpl;
};

__END_NAMESPACE_ABE
#endif // ABE_STL_VECTOR_H 
