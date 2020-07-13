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


// File:    String.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160701     initial version
//


#ifndef ABE_STRING_H
#define ABE_STRING_H

#include <ABE/core/Types.h>
#include <ABE/core/SharedBuffer.h>

__BEGIN_NAMESPACE_ABE

/**
 * a utf8 string object with cow support
 */
class ABE_EXPORT String : public StaticObject {
    public:
        /**
         * format a string
         */
        static String format(const Char *format, ...);
        //static String format(const Char *format, va_list ap);
        
        UInt32 scan(const Char * format, ...);
        //UInt32 scan(const Char * format, va_list ap);

    public:
        /**
         * Null string (empty string)
         * @note access to Null string will assert, String::Null should only be used in compare
         */
        static String Null;
    
    public:
        String();                               ///< empty string
        String(const String& s);                ///< copy string
        String(const Char *s, UInt32 n = 0);    ///< from null-terminated string
        String(const UInt16 *s, UInt32 n = 0);  ///< from utf16 null-terminated string
        explicit String(const Char c);
        explicit String(const UInt8 v);
        explicit String(const Int8 v);
        explicit String(const UInt16 v);
        explicit String(const Int16 v);
        explicit String(const UInt32 v);
        explicit String(const Int32 v);
        explicit String(const UInt64 v);
        explicit String(const Int64 v);
        explicit String(const Float32 v);
        explicit String(const Float64 v);
        explicit String(const void * v);

        ~String();

    public:
        // edit string
        String&     set(const String& s);
        String&     set(const Char * s);
        String&     append(const String &s);
        String&     append(const Char * s);
        String&     insert(UInt32 pos, const String &s);
        String&     insert(UInt32 pos, const Char * s);
        String&     erase(UInt32 pos, UInt32 n);
        String&     replace(const Char * s0, const Char * s1, Bool all = False);

        ABE_INLINE String& replace(const String& s0, const String& s1, Bool all = False)    { return replace(s0.c_str(), s1.c_str(), all);  }
        ABE_INLINE String& replace(const String& s0, const Char * s1, Bool all = False)     { return replace(s0.c_str(), s1, all);          }
        ABE_INLINE String& replace(const Char * s0, const String& s1, Bool all = False)     { return replace(s0, s1.c_str(), all);          }
    
    public:
        String&     trim();
        void        clear();
        String      substring(UInt32 pos, UInt32 n = 0) const;
        String&     swap(String& s);
        String&     lower();
        String&     upper();

    public:
        // operators
        ABE_INLINE String& operator=(const Char * s)    { set(s); return *this;                 }
        ABE_INLINE String& operator=(const String &s)   { set(s); return *this;                 }
        ABE_INLINE String& operator+=(const Char * s)   { append(s); return *this;              }
        ABE_INLINE String& operator+=(const String &s)  { append(s); return *this;              }
        ABE_INLINE String  operator+(const Char * s)    { String a(*this); return a.append(s);  }
        ABE_INLINE String  operator+(const String &s)   { String a(*this); return a.append(s);  }

    public:
        /**
         * get char at position
         * @param index     position, range, [0, size]
         * @return return a Char reference.
         */
        const Char& operator[](UInt32 index) const;
        Char&       operator[](UInt32 index);

    public:
        /**
         * return a pointer to null-terminated c-stype string
         * @note always non-null, even size() == 0, except String::Null
         * @note no non-const version of c_str()
         */
        ABE_INLINE const Char * c_str() const   { return mData->data(); }
        ABE_INLINE UInt32       size() const    { return mSize;         }
        ABE_INLINE Bool         empty() const   { return mSize == 0;    }

    public:
        /**
         * find string
         * param ic     ignore case
         * @return return >= 0 if string exists, otherwise return -1
         */
        Int     indexOf(UInt32 start, const Char * s, Bool ic = False) const;
        Int     lastIndexOf(const Char * s, Bool ic = False) const;

        ABE_INLINE Int indexOf(UInt32 start, const String& s, Bool ic = False) const { return indexOf(start, s.c_str()); }
        ABE_INLINE Int indexOf(const String& s, Bool ic = False) const     { return indexOf(0, s.c_str());     }
        ABE_INLINE Int indexOf(const Char * s, Bool ic = False) const      { return indexOf(0, s);             }
        ABE_INLINE Int lastIndexOf(const String& s, Bool ic = False) const { return lastIndexOf(s.c_str());    }

    public:
        Bool    startsWith(const Char * s, Bool ic = False) const;
        Bool    endsWith(const Char * s,  Bool ic = False) const;
    
        ABE_INLINE Bool startsWith(const String& s, Bool ic = False) const { return startsWith(s.c_str(), ic); }
        ABE_INLINE Bool endsWith(const String& s, Bool ic = False) const   { return endsWith(s.c_str(), ic);   }

    public:
        Int32       toInt32() const;
        Int64       toInt64() const;
        Float32     toFloat32() const;
        Float64     toFloat64() const;

    public:
        String      dirname() const;
        String      basename() const;
    
    public:
        UInt32      hash() const;
    
    public:
        /**
         * compare string
         * @param case compare case of not.
         * @return return -1 if lhs < rhs, return 0 if lhs == rhs, return 1 if lhs > rhs.
         */
        Int         compare(const Char * s, Bool ic = True) const;
        Int         compare(const String& s, Bool ic = True) const;
    
        ABE_INLINE Bool equals(const String& s, Bool ic = True) const { return !compare(s, ic);   }
        ABE_INLINE Bool equals(const Char * s, Bool ic = True) const  { return !compare(s, ic);   }

#define OPERATOR(op) \
ABE_INLINE Bool operator op(const String& rhs) const { return compare(rhs) op 0; }    \
ABE_INLINE Bool operator op(const Char * rhs) const { return compare(rhs) op 0; }
    OPERATOR(==)
    OPERATOR(!=)
    OPERATOR(<)
    OPERATOR(<=)
    OPERATOR(>)
    OPERATOR(>=)
#undef OPERATOR
             
    private:
        SharedBuffer *  mData;
        UInt32          mSize;
};

///////////////////////////////////////////////////////////////////////////

__END_NAMESPACE_ABE
#endif // ABE_STRING_H


