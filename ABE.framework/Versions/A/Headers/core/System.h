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

// File:    System.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef ABE_SYSTEM_H
#define ABE_SYSTEM_H

#include <ABE/core/Types.h>

BEGIN_DECLS

ABE_EXPORT UInt32 GetCpuCount();

// always return a nul-terminated string
// if env does NOT exists, return a empty string
ABE_EXPORT const Char * GetEnvironmentValue(const Char *);

// save current call stack
ABE_EXPORT UInt32 CallStackGet(UInt64 array[], UInt32 max);

// print this call stack
ABE_EXPORT void CallStackPut(const UInt64 array[], UInt32 size);

// print current call stack
#define CallStackPrint() do {               \
    UInt64  stack[32];                      \
    UInt32 n = CallStackGet(stack, 32);     \
    CallStackPut(stack, n);                 \
} while(0)

/**
 * memory analyzer.
 * @note current only support memory leak detect.
 * @note memory analyzer is in bypass mode by default.
 */
ABE_EXPORT void MemoryAnalyzerPrepare();
ABE_EXPORT void MemoryAnalyzerFinalize();
ABE_EXPORT void MemoryAnalyzerBypass();

END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_ABE

// system time in ns, Epoch or Monotonic(Default)
struct ABE_EXPORT Time : public StaticObject {
    public:
        static Time Now(Bool epoch = False);
    
        ABE_INLINE Time(Int64 time = 0) : mTime(time) { }
        ABE_INLINE Time(const Time& rhs) : mTime(rhs.mTime) { }
    
        ABE_INLINE Int64 value() const { return mTime; }
    
    public:
        static ABE_INLINE Time Seconds(Float64 sec)         { return Time(sec * 1E9);           }
        static ABE_INLINE Time MilliSeconds(Int64 msecs)    { return Time(msecs * 1000000LL);   }
        static ABE_INLINE Time MicroSeconds(Int64 usecs)    { return Time(usecs * 1000LL);      }
    
    public:
        ABE_INLINE Float64 seconds() const                  { return mTime / 1E9;               }
        ABE_INLINE Int64  nseconds() const                  { return mTime;                     }
        ABE_INLINE Int64  useconds() const                  { return mTime / 1000LL;            }
        ABE_INLINE Int64  mseconds() const                  { return mTime / 1000000LL;         }
    
    public:
        ABE_INLINE Time& operator+=(const Time& rhs)        { mTime += rhs.mTime; return *this; }
        ABE_INLINE Time& operator-=(const Time& rhs)        { mTime -= rhs.mTime; return *this; }
    
    public:
        ABE_INLINE Time& operator+=(Float64 sec)            { mTime += sec * 1E9; return *this; }
        ABE_INLINE Time& operator+=(Int64 nsec)             { mTime += nsec; return *this;      }
        ABE_INLINE Time& operator-=(Float64 sec)            { mTime -= sec * 1E9; return *this; }
        ABE_INLINE Time& operator-=(Int64 nsec)             { mTime -= nsec; return *this;      }
        ABE_INLINE Time& operator*=(Float64 multiplier)     { mTime *= multiplier; return *this;}
    
    public:
        ABE_INLINE Time operator+(const Time& rhs) const    { return Time(mTime + rhs.mTime);   }
        ABE_INLINE Time operator-(const Time& rhs) const    { return Time(mTime - rhs.mTime);   }
    
    public:
        ABE_INLINE Time operator+(Float64 sec) const        { return Time(mTime + sec * 1E9 );  }
        ABE_INLINE Time operator+(UInt64 nsec) const        { return Time(mTime + nsec );       }
        ABE_INLINE Time operator-(Float64 sec) const        { return Time(mTime - sec * 1E9 );  }
        ABE_INLINE Time operator-(UInt64 nsec) const        { return Time(mTime - nsec );       }
        ABE_INLINE Time operator*(Float64 multiplier) const { return Time(mTime * multiplier);  }
        
    public:
        ABE_INLINE Time operator-() const                   { return Time(-mTime);              }
            
    public:
        ABE_INLINE Bool operator<(const Time& rhs) const    { return mTime < rhs.mTime;         }
        ABE_INLINE Bool operator>(const Time& rhs) const    { return mTime > rhs.mTime;         }
        ABE_INLINE Bool operator<=(const Time& rhs) const   { return mTime <= rhs.mTime;        }
        ABE_INLINE Bool operator>=(const Time& rhs) const   { return mTime >= rhs.mTime;        }
        ABE_INLINE Bool operator==(const Time& rhs) const   { return mTime == rhs.mTime;        }
        ABE_INLINE Bool operator!=(const Time& rhs) const   { return mTime != rhs.mTime;        }
    
    public:
        // TODO
        // String format(const char *) const;
        
    private:
        // MUST USING SIGNED VALUE, SO Arithmetic won't overflow.
        Int64 mTime;
};

struct ABE_EXPORT Timer : public StaticObject {
    Timer() { }
    
    // return True on success(timeout), otherwise return False.
    Bool sleep(Time interval, Bool interrupt = True);
};

struct ABE_EXPORT CallStack : public StaticObject {
    public:
        ABE_INLINE CallStack()  { mSize = CallStackGet(mStack, 32); }
        ABE_INLINE ~CallStack() { }
    
        ABE_INLINE void print() { CallStackPut(mStack, mSize);      }

    private:
        UInt64  mStack[32];
        UInt32  mSize;
};

struct ABE_EXPORT MemoryAnalyzer : public StaticObject {
    ABE_INLINE MemoryAnalyzer()     { MemoryAnalyzerPrepare();  }
    ABE_INLINE ~MemoryAnalyzer()    { MemoryAnalyzerFinalize;   }
};
__END_NAMESPACE_ABE
#endif

#endif // ABE_SYSTEM_H
