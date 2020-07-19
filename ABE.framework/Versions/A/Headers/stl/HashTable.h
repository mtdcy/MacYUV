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


// File:    HashTable.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160701     initial version
//

#ifndef ABE_STL_HASHTABLE_H
#define ABE_STL_HASHTABLE_H

#include <ABE/stl/TypeHelper.h>

__BEGIN_NAMESPACE_ABE_PRIVATE

class ABE_EXPORT HashTable : public SharedObject {
    public:
        HashTable(const sp<Allocator>& allocator,
                  UInt32 tableLength,
                  UInt32 keyLength,
                  UInt32 dataLength,
                  type_hash_t hash,
                  type_compare_t cmp,
                  type_copy_t copy,
                  type_destruct_t key_dtor,
                  type_destruct_t data_dtor);

    public:
        struct Node {
            public:
                const void * key() const { return mKey; }
                const void * value() const { return mValue; }
                void * value() { return mValue; }
                
            protected:
                friend class HashTable;
                const UInt32    mHash;
                void *          mKey;
                void *          mValue;
                Node *          mNext;

                Node(UInt32);
                ~Node();
        };

    public:
        ABE_INLINE UInt32   size        () const { return mNumElements; }
        ABE_INLINE UInt32   tableLength () const { return mTableLength; }

    public:
        // edit hash table
        void                insert      (const void * k, const void * v, type_copy_t);
        UInt32              erase       (const void * k);
        void                clear       ();

    public:
        // access hash table
        // return Nil if not exists
        void *              find        (const void * k);
        const void *        find        (const void * k) const;
        // assert if not exists
        void *              access      (const void * k);
        const void *        access      (const void * k) const;
        // emplace if not exists
        void *              emplace     (const void * k, type_construct_t);

    public:
        // for iterator
        Node *              next        (const Node *) const;
        Node *              erase       (const Node *);

    private:
        Node *              allocateNode(UInt32);
        void                freeNode    (Node *);
        void                grow        ();
        void                shrink      ();

    private:
        sp<Allocator>       mAllocator;
        const UInt32        mKeyLength;
        const UInt32        mDataLength;
        UInt32              mTableLength;
        UInt32              mNumElements;
        Node **             mBuckets;
        type_hash_t         mHash;
        type_compare_t      mKeyCompare;
        type_copy_t         mKeyCopy;
        type_destruct_t     mKeyDeletor;
        type_destruct_t     mDataDeletor;
    
        OBJECT_TAIL(HashTable);
};

__END_NAMESPACE_ABE_PRIVATE

__BEGIN_NAMESPACE_ABE
//////////////////////////////////////////////////////////////////////////////

template <typename K, typename V>
class ABE_EXPORT HashTable : public StaticObject {
    private:
        // increment only iterator
        template <class TABLE, class VALUE, class NODE> class Iterator {
            public:
                ABE_INLINE Iterator() : mTable(Nil), mNode(Nil) { }
                ABE_INLINE Iterator(TABLE table, const NODE& node) : mTable(table), mNode(node) { }
                ABE_INLINE ~Iterator() { }

                ABE_INLINE Iterator& operator++()    { next(); return *this;                              }   // pre-increment
                ABE_INLINE Iterator  operator++(Int) { Iterator old(*this); next(); return old;           }   // post-increment

                ABE_INLINE Bool      operator == (const Iterator& rhs) const { return mNode == rhs.mNode; }
                ABE_INLINE Bool      operator != (const Iterator& rhs) const { return mNode != rhs.mNode; }

                ABE_INLINE const K&  key() const     { return *static_cast<const K*>(mNode->key());       }
                ABE_INLINE VALUE&    value()         { return *static_cast<VALUE*>(mNode->value());       }

            private:
                ABE_INLINE void      next()          { mNode = mTable->mImpl->next(mNode);                }

            protected:
                TABLE   mTable;
                NODE    mNode;

            private:
                // no decrement
                Iterator&   operator--();
                Iterator    operator--(Int);
        };

    public:
        typedef Iterator<HashTable<K, V> *, V, __NAMESPACE_PRIVATE::HashTable::Node *> iterator;
        typedef Iterator<const HashTable<K, V> *, const V, const __NAMESPACE_PRIVATE::HashTable::Node *> const_iterator;

    public:
        ABE_INLINE HashTable(UInt32 tableLength = 4, const sp<Allocator>& allocator = kAllocatorDefault) :
        mImpl(new __NAMESPACE_PRIVATE::HashTable(allocator, tableLength, sizeof(K), sizeof(V),
                                                 type_hash<K>, type_compare_eq<K>, type_copy<K>,
                                                 type_destruct<K>, type_destruct<V>)) { }

        ABE_INLINE ~HashTable() { }

    public:
        ABE_INLINE UInt32       size() const    { return mImpl->size(); }
        ABE_INLINE Bool         empty() const   { return size() == 0;   }
        ABE_INLINE void         clear()         { mImpl->clear();       }

    public:
        // insert value with key, replace if exists
        ABE_INLINE void         insert(const K& k, const V& v) { mImpl->insert(&k, &v, type_copy<V>);               }
        // erase element with key, return 1 if exists, and 0 otherwise.
        ABE_INLINE UInt32       erase(const K& k)              { return mImpl->erase(&k);                           }
        // return Nil if not exists
        ABE_INLINE V *          find(const K& k)               { return static_cast<V *>(mImpl->find(&k));          }
        ABE_INLINE const V *    find(const K& k) const         { return static_cast<const V *>(mImpl->find(&k));    }
        // emplace if not exists
        ABE_INLINE V&           operator[](const K& k)         { return *static_cast<V *>(mImpl->emplace(&k, type_construct<V>)); }
        // assert if not exists
        ABE_INLINE const V&     operator[](const K& k) const   { return *static_cast<const V *>(mImpl->access(&k)); }

    public:
        // forward iterator
        ABE_INLINE iterator         begin()         { return iterator(this, mImpl->next(Nil));          }
        ABE_INLINE iterator         end()           { return iterator(this, Nil);                       }
        ABE_INLINE const_iterator   cbegin() const  { return const_iterator(this, mImpl->next(Nil));    }
        ABE_INLINE const_iterator   cend() const    { return const_iterator(this, Nil);                 }
    
    private:
        sp<__NAMESPACE_PRIVATE::HashTable> mImpl;
};

__END_NAMESPACE_ABE

#endif // ABE_STL_HASHTABLE_H

