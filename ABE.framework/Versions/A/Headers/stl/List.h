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


// File:    List.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160829     initial version
//

#ifndef ABE_STL_LIST_H
#define ABE_STL_LIST_H 

#include <ABE/stl/TypeHelper.h>

////////////////////////////////////////////////////////////////////////////// 
// c++ implementation of doubly-linked list container.
__BEGIN_NAMESPACE_ABE_PRIVATE

class ABE_EXPORT List : public SharedObject {
    public:
        class Node {
            friend class List;
            
            protected:
                void *  mData;
                Node *  mPrev;
                Node *  mNext;

            protected:
                ABE_INLINE Node() : mData(Nil), mPrev(Nil), mNext(Nil) { }

            public:
                ABE_INLINE void * data() const { return mData; }
                ABE_INLINE Node * next() const { return mNext; }
                ABE_INLINE Node * prev() const { return mPrev; }

            protected:
                void    unlink();
                Node *  link(Node * next);
                Node *  insert(Node * pos);
                Node *  append(Node * pos);
        };
    
    public:
        List(const sp<Allocator>& allocator, UInt32 size, type_destruct_t dtor);

    public:
        ABE_INLINE UInt32       size() const    { return mListLength;   }
        void                    clear();

    public:
        // list head.
        ABE_INLINE const Node&  head() const    { return mHead;         }
        ABE_INLINE Node&        head()          { return mHead;         }
    
    public:
        // push a node and return it;.
        Node&   push(Node& pos, const void * what, type_copy_t);
        // emplace a node and return it.
        Node&   emplace(Node& pos, type_construct_t);
        // pop a node and return next.
        Node&   pop(Node& pos);

    public:
        // merge sort. [stable]
        void    sort(type_compare_t);

    private:
        Node *  allocateNode();
        void    freeNode(Node* node);

    private:
        sp<Allocator>   mAllocator;
        type_destruct_t mDeletor;
        const UInt32    mDataLength;
        UInt32          mListLength;
        Node            mHead;
    
        OBJECT_TAIL(List)
};
__END_NAMESPACE_ABE_PRIVATE

__BEGIN_NAMESPACE_ABE
// doubly-linked list
template <typename T> class ABE_EXPORT List : public StaticObject {
    protected:
        // iterator for List, bidirection iterator
        template <typename N, typename V> class Iterator {
            public:
                ABE_INLINE Iterator() : mNode(Nil) { }
                ABE_INLINE Iterator(N node) : mNode(node) { }
                ABE_INLINE ~Iterator() { }

            public:
                ABE_INLINE Iterator& operator++()    { mNode = mNode->next(); return *this;           }   // pre-increment
                ABE_INLINE Iterator  operator++(Int) { Iterator old(*this); operator++(); return old; }   // post-increment
                ABE_INLINE Iterator& operator--()    { mNode = mNode->prev(); return *this;           }   // pre-decrement
                ABE_INLINE Iterator  operator--(Int) { Iterator old(*this); operator--(); return old; }   // post-decrement

                ABE_INLINE V&        operator*()     { return *(static_cast<V *>(mNode->data()));     }
                ABE_INLINE V*        operator->()    { return static_cast<V *>(mNode->data());        }

            public:
                ABE_INLINE Bool      operator==(const Iterator& rhs) const { return mNode == rhs.mNode; }
                ABE_INLINE Bool      operator!=(const Iterator& rhs) const { return mNode != rhs.mNode; }

            protected:
                friend class List<V>;
                N    mNode;
        };

    public:
        typedef Iterator<__NAMESPACE_PRIVATE::List::Node *, T> iterator;
        typedef Iterator<const __NAMESPACE_PRIVATE::List::Node *, const T> const_iterator;

    public:
        ABE_INLINE List(const sp<Allocator>& allocator = kAllocatorDefault) :
        mImpl(new __NAMESPACE_PRIVATE::List(allocator, sizeof(T), type_destruct<T>)) { }

        ABE_INLINE ~List() { }

    public:
        ABE_INLINE void    clear()       { return mImpl->clear(); }
        ABE_INLINE UInt32  size() const  { return mImpl->size();  }
        ABE_INLINE Bool    empty() const { return size() == 0;    }

    public:
        // stable sort, uses operator<
        ABE_INLINE void    sort() { mImpl->sort(type_compare_lt<T>); }

    public:
        ABE_INLINE T&       front()       { return *static_cast<T*>(mImpl->head().next()->data());       }
        ABE_INLINE const T& front() const { return *static_cast<const T*>(mImpl->head().next()->data()); }
        ABE_INLINE T&       back()        { return *static_cast<T*>(mImpl->head().prev()->data());       }
        ABE_INLINE const T& back() const  { return *static_cast<const T*>(mImpl->head().prev()->data()); }

    public:
        ABE_INLINE void push(const T& v)       { mImpl->push(mImpl->head(), &v, type_copy<T>); }
        ABE_INLINE void push_back(const T& v)  { push(v);                                      }
        ABE_INLINE void push_front(const T& v) { mImpl->push(*mImpl->head().next(), &v);       }

        ABE_INLINE void pop()                  { mImpl->pop(*mImpl->head().next());            }
        ABE_INLINE void pop_front()            { pop();                                        }
        ABE_INLINE void pop_back()             { mImpl->pop(*mImpl->head().prev());            }

    public:
        // emplace operations
        ABE_INLINE T& push()       { return *static_cast<T*>(mImpl->emplace(mImpl->head(), type_construct<T>).data());         }
        ABE_INLINE T& push_back()  { return push();                                                                            }
        ABE_INLINE T& push_front() { return *static_cast<T*>(mImpl->emplace(*mImpl->head().next(), type_construct<T>).data()); }

    public:
        ABE_INLINE iterator begin()  { return iterator(mImpl->head().next()); }
        ABE_INLINE iterator end()    { return iterator(&mImpl->head());       }
        ABE_INLINE iterator rbegin() { return iterator(mImpl->head().prev()); }
        ABE_INLINE iterator rend()   { return iterator(&mImpl->head());       }

    public:
        ABE_INLINE iterator insert(iterator pos, const T& v) { return iterator(&mImpl->push(*pos.mNode, &v, type_copy<T>)); }
        ABE_INLINE iterator erase(iterator pos)              { return iterator(&mImpl->pop(*pos.mNode));                    }

        ABE_INLINE const_iterator  cbegin() const  { return const_iterator(mImpl->head().next()); }
        ABE_INLINE const_iterator  cend() const    { return const_iterator(&mImpl->head());       }
        ABE_INLINE const_iterator  crbegin() const { return const_iterator(mImpl->head().prev()); }
        ABE_INLINE const_iterator  crend() const   { return const_iterator(&mImpl->head());       }
    
    private:
        sp<__NAMESPACE_PRIVATE::List> mImpl;
};

__END_NAMESPACE_ABE
#endif // ABE_STL_LIST_H 
