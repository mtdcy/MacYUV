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


// File:    Message.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef ABE_HEADERS_MESSAGE_H
#define ABE_HEADERS_MESSAGE_H 

#include <ABE/core/Types.h>
#include <ABE/core/String.h>
#include <ABE/stl/HashTable.h>

__BEGIN_NAMESPACE_ABE

// we prefer FOURCC as name
class ABE_EXPORT Message : public SharedObject {
    public:
        Message();
    
        sp<Message> copy() const;
    
    public:
        enum Type {
            kTypeInt32,
            kTypeInt64,
            kTypeFloat,
            kTypeDouble,
            kTypePointer,
            kTypeString,
            kTypeObject,
        };

    public:
        ABE_INLINE UInt32   size() const    { return mEntries.size();   }
        ABE_INLINE UInt32   empty() const   { return size() == 0;       }

    public:
        void            clear       ();
        Bool            contains    (UInt32 name) const;
        Bool            remove      (UInt32 name);
        Bool            contains    (UInt32 name, Type) const;
        UInt32          name        (UInt32 index, Type&) const;
    
    public:
        // DEBUGGING
        String          string      () const;

    public:
        // core types
        void            setInt32    (UInt32 name, Int32 value);                     // kTypeInt32
        void            setInt64    (UInt32 name, Int64 value);                     // kTypeInt64
        void            setFloat    (UInt32 name, Float32 value);                   // kTypeFloat
        void            setDouble   (UInt32 name, Float64 value);                   // kTypeDouble
        void            setPointer  (UInt32 name, void *value);                     // kTypePointer
        void            setString   (UInt32 name, const Char *s, UInt32 len = 0);   // kTypeString
        void            setObject   (UInt32 name, SharedObject * object);           // kTypeObject

        template <class T> ABE_INLINE void setObject(UInt32 name, const sp<T>& o)
        { setObject(name, static_cast<SharedObject *>(o.get())); }

        Int32           findInt32   (UInt32 name, Int32 def = 0) const;             // kTypeInt32
        Int64           findInt64   (UInt32 name, Int64 def = 0) const;             // kTypeInt64
        Float32         findFloat   (UInt32 name, Float32 def = 0) const;           // kTypeFloat
        Float64         findDouble  (UInt32 name, Float64 def = 0) const;           // kTypeDouble
        void *          findPointer (UInt32 name, void * def = Nil) const;          // kTypePointer
        const Char *    findString  (UInt32 name, const Char * def = Nil) const;    // kTypeString
        SharedObject *  findObject  (UInt32 name, SharedObject * def = Nil) const;  // kTypeObject

        // alias
        ABE_INLINE void setString   (UInt32 name, const String &s)
        { setString(name, s.c_str()); }

    private:
        struct Entry {
            Type                mType;
            union {
                Int32           i32;
                Int64           i64;
                Float32         flt;
                Float64         dbl;
                void *          ptr;
                SharedObject *  obj;
            } u;
        };
        HashTable<UInt32, Entry>  mEntries;
    
        OBJECT_TAIL(Message)
};

__END_NAMESPACE_ABE
#endif  // ABE_HEADERS_MESSAGE_H

