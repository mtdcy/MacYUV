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


// File:    Types.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160701     initial version
//

#ifndef ABE_TYPES_H
#define ABE_TYPES_H

/**
 * @note NO config.h for ABE public headers
 *
 * @note only system/libc/compiler macro allowed, like __APPLE__
 *  https://sourceforge.net/p/predef/wiki/Architectures/
 *  https://sourceforge.net/p/predef/wiki/Compilers/
 *  https://sourceforge.net/p/predef/wiki/OperatingSystems/
 *  https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
 *
 * Compiler:
 *    _MSC_VER & __clang__ & __GNUC__
 * Platform:
 *    __APPLE__ & _WIN32 & _WIN64 & __MINGW32__ & __ANDROID__ & __linux__
 *
 *
 * @note we assume everything in bytes-order
 * @note NO Endian specific operations expect tool designed for this
 **/

#pragma mark Headers
#include <stdint.h>
#include <stdarg.h>         // va_list
#include <inttypes.h>       // for PRId32/PRId64/...
#include <stddef.h>         // size_t, need by some internal apis
#define PRIf32  "f"
#define PRIf64  "lf"

#pragma mark Basic Types
/**
 * Basic Types
 * @note we discourage the use of size_t/Int
 */
// platform-depended width integer type
typedef int             Int;
typedef unsigned int    UInt;
// fixed-width unsigned integer type
typedef uint8_t         UInt8;
typedef uint16_t        UInt16;
typedef uint32_t        UInt32;
typedef uint64_t        UInt64;
// fixed-width signed integer type
typedef int8_t          Int8;
typedef int16_t         Int16;
typedef int32_t         Int32;
typedef int64_t         Int64;
// other types
typedef float           Float32;
typedef double          Float64;;
typedef char            Char;
// alias types
typedef Int             Bool;
typedef UInt32          Status;
// type values
#define True            1
#define False           0
#define Nil             0
#define OK              0   // For Status with no error

#pragma mark Macros
#define FOURCC(x) ((((UInt32)(x) >> 24) & 0xff)         \
        | (((UInt32)(x) >> 8) & 0xff00)         \
        | (((UInt32)(x) << 8) & 0xff0000)       \
        | (((UInt32)(x) << 24) & 0xff000000))

// atomic
// FIXME: understand the memmodel
#ifndef ABE_ATOMIC_MEMMODEL
#define ABE_ATOMIC_MEMMODEL             __ATOMIC_SEQ_CST
#endif

// fence
#define atomic_fence()                  __atomic_thread_fence(ABE_ATOMIC_MEMMODEL)

#if defined(__GNUC__) || defined(__clang__)
// The ‘__atomic’ builtins can be used with any integral scalar or pointer type
// that is 1, 2, 4, or 8 bytes in length. 16-byte integral types are also allowed
// if ‘__int128’ (see __int128) is supported by the architecture.
#define ABE_ATOMIC_STORE(p, val)        __atomic_store_n(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_LOAD(p)              __atomic_load_n(p, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_EXCHANGE(p, val)     __atomic_exchange_n(p, val, ABE_ATOMIC_MEMMODEL)
// if *p0 == *p1: val => *p0 : return True; else: *p0 => *p1 : return False
#define ABE_ATOMIC_CAS(p0, p1, val)     __atomic_compare_exchange_n(p0, p1, val, False, ABE_ATOMIC_MEMMODEL, ABE_ATOMIC_MEMMODEL)    // compare and swap

#define ABE_ATOMIC_ADD(p, val)          __atomic_add_fetch(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_SUB(p, val)          __atomic_sub_fetch(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_AND(p, val)          __atomic_and_fetch(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_OR(p, val)           __atomic_or_fetch(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_XOR(p, val)          __atomic_xor_fetch(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_NAND(p, val)         __atomic_nand_fetch(p, val, ABE_ATOMIC_MEMMODEL)

#define ABE_ATOMIC_FETCH_ADD(p, val)    __atomic_fetch_add(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_FETCH_SUB(p, val)    __atomic_fetch_sub(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_FETCH_AND(p, val)    __atomic_fetch_and(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_FETCH_OR(p, val)     __atomic_fetch_or(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_FETCH_XOR(p, val)    __atomic_fetch_xor(p, val, ABE_ATOMIC_MEMMODEL)
#define ABE_ATOMIC_FETCH_NAND(p, val)   __atomic_fetch_nand(p, val, ABE_ATOMIC_MEMMODEL)
#elif _MSC_VER
#error "TODO: ATOMIC is missing for MSVC"
#endif

// macros
#ifdef __cplusplus
#define BEGIN_DECLS                     extern "C" {
#define END_DECLS                       }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif

#if defined(_MSC_VER)
#define ABE_INLINE                      inline
#ifdef BUILD_ABE_DLL
#define ABE_EXPORT                      __declspec(dllexport)
#else
#define ABE_EXPORT                      __declspec(dllimport)
#endif
#define ABE_DEPRECATED                  __declspec(deprecated("deprecated"))
#define ABE_EXPERIMENTAL                __declspec(deprecated("experimental"))
#else
//#define __ABE_INLINE                    __attribute__ ((__always_inline__))
#define ABE_INLINE                      __attribute__ ((__visibility__("hidden"), __always_inline__)) inline
#define ABE_EXPORT                      __attribute__ ((__visibility__("default")))
#define ABE_DEPRECATED                  __attribute__ ((deprecated("deprecated")))
#define ABE_EXPERIMENTAL                __attribute__ ((deprecated("experimental")))
#endif

#if defined(_MSC_VER)
#define ABE_LIKELY(x)                   (x)
#define ABE_UNLIKELY(x)                 !(x)
#else
#define ABE_LIKELY(x)                   __builtin_expect(x, True)
#define ABE_UNLIKELY(x)                 __builtin_expect(x, False)
#endif

#ifdef __cplusplus
#define __BEGIN_NAMESPACE(x)            namespace x {
#define __END_NAMESPACE(x)              }
#define __USING_NAMESPACE(x)            using namespace x;

#define __NAMESPACE_ABE                 abe
#define __BEGIN_NAMESPACE_ABE           __BEGIN_NAMESPACE(__NAMESPACE_ABE)
#define __END_NAMESPACE_ABE             __END_NAMESPACE(__NAMESPACE_ABE)

#define __NAMESPACE_PRIVATE             _privates
#define __BEGIN_NAMESPACE_ABE_PRIVATE   __BEGIN_NAMESPACE(__NAMESPACE_ABE) __BEGIN_NAMESPACE(__NAMESPACE_PRIVATE)
#define __END_NAMESPACE_ABE_PRIVATE     __END_NAMESPACE(__NAMESPACE_PRIVATE) __END_NAMESPACE(__NAMESPACE_ABE)

#define USING_NAMESPACE_ABE             __USING_NAMESPACE(__NAMESPACE_ABE)

// borrow from Android
// Put this in the private: declarations for a class to be uncopyable.
#define DISALLOW_COPY(TypeName)         TypeName(const TypeName&)

// Put this in the private: declarations for a class to be unassignable.
#define DISALLOW_ASSIGN(TypeName)       void operator=(const TypeName&)

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// In most case, copy constructor and operator= is not neccessary,
// and it may cause problem if you don't declare it or misuse it.
#define DISALLOW_EVILS(TypeName)            \
    private:                                \
    TypeName(const TypeName&);              \
    TypeName& operator=(const TypeName&)

//=============================================================
// sp Type
__BEGIN_NAMESPACE_ABE

// https://stackoverflow.com/questions/6271615/any-way-to-prevent-dynamic-allocation-of-a-class
/**
 * static object that can not be object
 * @note usally for local auxiliary class
 */
#define STATIC_OBJECT(TypeName)             \
    private:                                \
    void *  operator new(size_t);           \
    void    operator delete(void *)
struct ABE_EXPORT StaticObject {
    StaticObject() { }
    ~StaticObject() { }
    STATIC_OBJECT(StaticObject);
};

// FIXME: limit T to basic types
template <typename T>
class Atomic : public StaticObject {
    private:
        volatile T value;

    public:
        ABE_INLINE Atomic()                     { ABE_ATOMIC_STORE(&value, static_cast<T>(0));  }
        ABE_INLINE Atomic(const Atomic& rhs)    { ABE_ATOMIC_STORE(&value, rhs.load());         }
        ABE_INLINE Atomic(T _value)             { ABE_ATOMIC_STORE(&value, _value);             }

        ABE_INLINE void   store(T val)          { ABE_ATOMIC_STORE(&value, val);                }
        ABE_INLINE T      load() const          { return ABE_ATOMIC_LOAD(&value);               }
        ABE_INLINE T      exchange(T val)       { return ABE_ATOMIC_EXCHANGE(&value, val);      }
        ABE_INLINE Bool   cas(T& to, T val)     { return ABE_ATOMIC_CAS(&value, &to, val);      }   // compare and swap

        ABE_INLINE T      operator++()          { return ABE_ATOMIC_ADD(&value, 1);             }   // pre-increment
        ABE_INLINE T      operator++(Int)       { return ABE_ATOMIC_FETCH_ADD(&value, 1);       }   // post_increment
        ABE_INLINE T      operator--()          { return ABE_ATOMIC_SUB(&value, 1);             }
        ABE_INLINE T      operator--(Int)       { return ABE_ATOMIC_FETCH_SUB(&value, 1);       }

        ABE_INLINE T      operator+=(T val)     { return ABE_ATOMIC_ADD(&value, val);           }
        ABE_INLINE T      operator-=(T val)     { return ABE_ATOMIC_SUB(&value, val);           }
        ABE_INLINE T      operator&=(T val)     { return ABE_ATOMIC_AND(&value, val);           }
        ABE_INLINE T      operator|=(T val)     { return ABE_ATOMIC_OR(&value, val);            }
        ABE_INLINE T      operator^=(T val)     { return ABE_ATOMIC_XOR(&value, val);           }

        ABE_INLINE T      fetch_add(T val)      { return ABE_ATOMIC_FETCH_ADD(&value, val);     }
        ABE_INLINE T      fetch_sub(T val)      { return ABE_ATOMIC_FETCH_SUB(&value, val);     }
        ABE_INLINE T      fetch_and(T val)      { return ABE_ATOMIC_FETCH_AND(&value, val);     }
        ABE_INLINE T      fetch_or(T val)       { return ABE_ATOMIC_FETCH_OR(&value, val);      }
        ABE_INLINE T      fetch_xor(T val)      { return ABE_ATOMIC_FETCH_XOR(&value, val);     }
};

/**
 * all members of SharedObject use static member function style start with capital
 * to avoid overload by subclass
 * @note client should NOT keep a pointer to SharedObject without retain, always
 *       retain first, use it later
 * @note NEVER retain to self in constructor.
 * @note 
 */
template<typename T> class sp;
template<typename U> class wp;
struct ABE_EXPORT SharedObject {
    private:
        template<typename T> friend class sp;
        template<typename T> friend class wp;
        struct Refs {
            private:
                SharedObject * const    mObject;
                Atomic<UInt32>          mRefs;
                Atomic<UInt32>          mWeakRefs;

            private:
                friend struct SharedObject;
                Refs(SharedObject *);
                ~Refs();

            public:
                // strong refs
                UInt32  incRefs();
                UInt32  decRefs();

                // weak refs
                UInt32  incWeakRefs();
                UInt32  decWeakRefs();
            
                // try to retain the object by inc strong refs.
                SharedObject * retain();

            public:
                // DEBUGGING
                UInt32  refCount() const;
                UInt32  weakRefCount() const;
        };
    
    private:
        // object id sematics
        //  public class id fourcc start with '?'
        //  backend context id fourcc start with '!'
        UInt32          mID;        ///< ugly RTTI
        UInt32          mPreserved;
        Refs * const    mRefs;
    
    protected:
        // a object pointer help subclass to keep ABI stable and
        // hide member data details.
        // the partner object can help subclass especially
        // public class to keep constant size during upgrade.
        SharedObject *  mPartner;

    protected:
        // ALLOCATE A SharedObject IS MEANINGLESS, SO MAKE IT PROTECTED.
        SharedObject(const UInt32 id = FOURCC('?obj'), SharedObject * partner = Nil) :
        mID(id), mRefs(new Refs(this)), mPartner(partner) { }
    
    protected:
        // MAKE DESTRUCTOR PROTECTED TO AVOID DELETE OBJECT EXPLICITLY.
        virtual ~SharedObject();

    private:
        // OVERLOAD THESE APIS WHEN NECCESARY
        // called when the first strong refs is retained.
        virtual void        onFirstRetain() { }
        // called when the last strong refs is released.
        // @note put code here to avoid 'Pure virtual function called!'.
        virtual void        onLastRetain() { }

    public:
        // APIs FOR EXPORT OBJECT INTO C CODE.
        ABE_INLINE UInt32   GetObjectID() const { return mID; }
    
        /**
         * get this object reference count
         * @return return current reference count
         */
        ABE_INLINE UInt32   GetRetainCount() const { return mRefs->refCount();  }

        /**
         * retain this object by inc strong refs.
         * XXX: did subclass need to overload RetainObject() ?
         */
        SharedObject *      RetainObject()  { mRefs->incRefs(); return this; }

        /**
         * release this object by dec strong refs.
         * if refs reach 0, this object will be deleted.
         * @param keep  whether to keep the memory if this is the last refs.
         * @return return remains retain count.
         * @note keep the memory if inherited need to do extra complex destruction work
         */
        UInt32              ReleaseObject() { return mRefs->decRefs(); }

        // NO API FOR ACQUIRE WEAK REFS AS OBJECT MAYBE GONE WHEN WEAK REFS STILL EXISTS.

        /**
         * is this object object with others
         * @note if object is not object, it is safe to do anything
         *       else either copy this object or lock it to modify its context
         */
        ABE_INLINE Bool    IsObjectShared() const      { return GetRetainCount() > 1; }
        ABE_INLINE Bool    IsObjectNotShared() const   { return !IsObjectShared(); }

        DISALLOW_EVILS(SharedObject);
};

// good practice for object
// 1. make destructor as protected to avoid explicit delete object
// 2. implement onRetainObject & onReleaseObject.
// 3. leave copy constructor & assignment private and empty.
#define OBJECT_TAIL(TYPE)                       \
    public:                                     \
    protected:                                  \
        virtual ~TYPE() { }                     \
        virtual void onFirstRetain();           \
        virtual void onLastRetain();            \
    private:                                    \
        TYPE(const TYPE&);                      \
        TYPE& operator=(const TYPE&);

#define COMPARE(_op_)                                                   \
    ABE_INLINE Bool operator _op_ (const sp<T>& o) const {              \
        return mObject _op_ o.mObject;                                  \
    }                                                                   \
    template<typename U>                                                \
    ABE_INLINE Bool operator _op_ (const sp<U>& o) const {              \
        return mObject _op_ o.mObject;                                  \
    }
/**
 * shared/strong pointer for SharedObject only
 * @note can NOT assign from raw pointer except class derived from SharedPointer
 * @note NO assignment from wp<T>, using wp<T>.retain()
 */
template <class T>
class ABE_EXPORT sp : public StaticObject {
    public:
        // constructors
        ABE_INLINE sp() : mObject(Nil), mRefs(Nil) { }
        ABE_INLINE sp(SharedObject * rhs) : mObject(Nil), mRefs(Nil)                    { set(rhs); }
        ABE_INLINE sp(const sp<T>& rhs) : mObject(Nil), mRefs(Nil)                      { set(rhs); }
        template<typename U> ABE_INLINE sp(const sp<U>& rhs) : mObject(Nil), mRefs(Nil) { set(rhs); }

        // copy assignments
        ABE_INLINE sp& operator=(SharedObject * rhs)                                    { set(rhs); return *this; }
        ABE_INLINE sp& operator=(const sp<T>& rhs)                                      { set(rhs); return *this;  }
        template<typename U> ABE_INLINE sp& operator=(const sp<U>& rhs)                 { set(rhs); return *this;  }

        // destructors
        ABE_INLINE ~sp() { clear(); }

        // clear, release refs explict
        ABE_INLINE void clear();

        // Nil test
        ABE_INLINE Bool isNil() const { return mObject == Nil ; }

        // object ID
        ABE_INLINE UInt32 object() const { return mObject ? mObject->mID : 0; }
    
    public:
        // compare object pointer.
        COMPARE(==);
        COMPARE(!=);

    public:
        // access to object
        ABE_INLINE  T*          operator->()       { return static_cast<T*>(mObject);           }
        ABE_INLINE  const T*    operator->() const { return static_cast<const T*>(mObject);     }

        ABE_INLINE  T&          operator*()        { return *static_cast<T*>(mObject);          }
        ABE_INLINE  const T&    operator*() const  { return *static_cast<const T*>(mObject);    }
    
        // export object to c code.
        ABE_INLINE  T*          get() const        { return static_cast<T *>(mObject);          }

    public:
        // DEBUGGING
        ABE_INLINE UInt32       refsCount() const  { return mRefs ? mRefs->refCount() : 0;      }

    public:
        template<typename U> friend class sp;
        template<typename U> friend class wp;
        ABE_INLINE void set(SharedObject *);
        ABE_INLINE void set(const sp<T>&);
        template<typename U> ABE_INLINE void set(const sp<U>&);
        // for wp<T>::retain()
        static ABE_INLINE sp<T> Retain(SharedObject * object);
        SharedObject *          mObject;
        SharedObject::Refs *    mRefs;
};
#undef COMPARE

/**
 * weak pointer for SharedObject only
 * @note assign from raw pointer SHOULD be careful. only from *this*.
 * @note no object access operation in wp<T>, please retain first then using sp<T>.
 */
template <typename T>
class wp : public StaticObject {
    public:
        // constructors
        ABE_INLINE wp() : mRefs(Nil) { }
        ABE_INLINE wp(SharedObject * rhs) : mRefs(Nil)                      { set(rhs); }
        ABE_INLINE wp(const wp<T>& rhs) : mRefs(Nil)                        { set(rhs); }
        ABE_INLINE wp(const sp<T>& rhs) : mRefs(Nil)                        { set(rhs); }
        template<typename U> ABE_INLINE wp(const wp<U>& rhs) : mRefs(Nil)   { set(rhs); }
        template<typename U> ABE_INLINE wp(const sp<U>& rhs) : mRefs(Nil)   { set(rhs); }
    
        // copy assiments
        ABE_INLINE wp& operator=(SharedObject * rhs)                        { set(rhs); return *this;   }
        ABE_INLINE wp& operator=(const wp<T>& rhs)                          { set(rhs); return *this;   }
        ABE_INLINE wp& operator=(const sp<T>& rhs)                          { set(rhs); return *this;   }
        template<typename U> ABE_INLINE wp& operator=(const wp<U>& rhs)     { set(rhs); return *this;   }
        template<typename U> ABE_INLINE wp& operator=(const sp<U>& rhs)     { set(rhs); return *this;   }

        // destructors
        ABE_INLINE ~wp() { clear(); }

        // clear , release refs explict
        ABE_INLINE void clear();

        // retain, SHOULD with const qualifier ???
        // without const qualifier we can NOT access const wp<T> directly.
        ABE_INLINE sp<T> retain() const;

        // test
        ABE_INLINE Bool isNil() const { return mRefs == Nil; }
    
        // COMPARE WEAK REFS IS MEANINGLESS, SO NO COMPARE OPERATORS HERE.
        // after object is released, we only keep the refs and the object pointer
        // becomes a wild pointer, it is meaningless to compare them.

    public:
        // DEBUGGING
        ABE_INLINE UInt32 refsCount() const { return mRefs ? mRefs->weakRefCount() : 0; }

    private:
        template<typename U> friend class wp;
        ABE_INLINE void set(SharedObject *);
        template<typename U> ABE_INLINE void set(const wp<U>&);
        template<typename U> ABE_INLINE void set(const sp<U>&);
        SharedObject::Refs * mRefs;
};

// local pointer
template <typename TYPE>
class ABE_EXPERIMENTAL lp : public StaticObject {
    public:
        ABE_INLINE lp() : mTarget(Nil), mRefs(0) { }
        ABE_INLINE lp(const lp& rhs) : mTarget(rhs.mTarget), mRefs(rhs.mRefs.load()) { }
    
        ABE_INLINE lp& operator=(const lp& rhs) {
            if (mTarget && (--mRefs) == 0) delete mTarget;
            mTarget = rhs.mTarget;
            if (mTarget) mRefs.store(++rhs.mRefs);
            else mRefs.store(0);
        }
    
    public:
        // access to object
        ABE_INLINE  TYPE*       operator->()       { return mTarget;    }
        ABE_INLINE  const TYPE* operator->() const { return mTarget;    }

        ABE_INLINE  TYPE&       operator*()        { return *mTarget;   }
        ABE_INLINE  const TYPE& operator*() const  { return *mTarget;   }
    
        // export object to c code.
        ABE_INLINE  TYPE *      get() const        { return mTarget;    }
    
    private:
        TYPE *          mTarget;
        Atomic<UInt32>  mRefs;
};

///////////////////////////////////////////////////////////////////////////
template<typename T>
void sp<T>::clear() {
    if (mObject) {
        mRefs->decRefs();
        mRefs = Nil;
        mObject = Nil;
    }
}

template<typename T>
void sp<T>::set(SharedObject * object) {
    clear();
    mObject = object;
    if (mObject) {
        mRefs = mObject->mRefs;
        mRefs->incRefs();
    }
}

template<typename T>
void sp<T>::set(const sp<T>& rhs) {
    clear();
    mObject = rhs.mObject;
    if (mObject) {
        mRefs = mObject->mRefs;
        mRefs->incRefs();
    }
}

template<typename T> template<typename U>
void sp<T>::set(const sp<U>& rhs) {
    clear();
    mObject = rhs.mObject;
    if (mObject) {
        mRefs = mObject->mRefs;
        mRefs->incRefs();
    }
}

template<typename T>
void wp<T>::clear() {
    if (mRefs) {
        mRefs->decWeakRefs();
        mRefs = Nil;
    }
}

template<typename T>
void wp<T>::set(SharedObject * object) {
    clear();
    if (object == Nil) return;
    mRefs = object->mRefs;
    if (mRefs) {
        mRefs->incWeakRefs();
    }
}

template<typename T> template<typename U>
void wp<T>::set(const wp<U>& rhs) {
    clear();
    mRefs = rhs.mRefs;
    if (mRefs) {
        mRefs->incWeakRefs();
    }
}

template<typename T> template<typename U>
void wp<T>::set(const sp<U>& rhs) {
    clear();
    mRefs = rhs.mRefs;
    if (mRefs) {
        mRefs->incWeakRefs();
    }
}

template<typename T>
sp<T> wp<T>::retain() const {
    if (mRefs) {
        SharedObject * object = mRefs->retain();
        if (object) {
            return sp<T>::Retain(object);
        }
    }
    return Nil;
}

template<typename T>
sp<T> sp<T>::Retain(SharedObject * object) {
    sp<T> pointee;
    pointee.mObject = object;
    pointee.mRefs   = object->mRefs;
    return pointee;
}

__END_NAMESPACE_ABE
#endif // __cplusplus

#endif // ABE_TYPES_H

