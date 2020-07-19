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


// File:    Log.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef ABE_HEADERS_LOG_H
#define ABE_HEADERS_LOG_H

#include <ABE/core/Types.h>

BEGIN_DECLS
enum eLogLevel {
    LOG_FATAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
};

/**
 * print log message
 */
ABE_EXPORT void SystemLogPrint(
        const Char *    tag,        ///< @see LOG_TAG
        enum eLogLevel  lvl,        ///< @see eLogLevel
        const Char *    func,       ///< @see __FUNCTION__
        UInt32          line,       ///< @see __LINE__
        const Char *    format,     ///< @see man(3) printf
        ...);

ABE_EXPORT void LogSetCallback(void (*callback)(const Char *));

#ifndef LOG_TAG
#define LOG_TAG   __FILE__
#endif

#ifndef LOG_NDEBUG
#define LOG_NDEBUG 1
#endif

#if LOG_NDEBUG == 0
#define DEBUG(fmt, ...) SystemLogPrint(LOG_TAG, LOG_DEBUG, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...) do { } while(0)
#endif

#define INFO(fmt, ...)  SystemLogPrint(LOG_TAG, LOG_INFO, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...)  SystemLogPrint(LOG_TAG, LOG_WARN, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) SystemLogPrint(LOG_TAG, LOG_ERROR, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) SystemLogPrint(LOG_TAG, LOG_FATAL, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define __LITERAL_TO_STRING0(x) #x
#define __LITERAL_TO_STRING(x) __LITERAL_TO_STRING0(x)
#define __VA_NARGS0(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define __VA_NARGS(...) __VA_NARGS0(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define CHECK_TRUE(expr, ...) do {                                      \
    if (__builtin_expect(!(expr), 0)) {                                 \
        if (__VA_NARGS(__VA_ARGS__) > 0)                                \
        FATAL(" CHECK_TRUE(" #expr ") failed. => " __VA_ARGS__);        \
        else                                                            \
        FATAL(" CHECK_TRUE(" #expr ") failed.");                        \
    }                                                                   \
} while(0)

#define CHECK_FALSE(expr, ...) do {                                     \
    if (__builtin_expect(expr, 0)) {                                    \
        if (__VA_NARGS(__VA_ARGS__) > 0)                                \
        FATAL(" CHECK_FALSE(" #expr ") failed. => " __VA_ARGS__);       \
        else                                                            \
        FATAL(" CHECK_FALSE(" #expr ") failed.");                       \
    }                                                                   \
} while(0)

#define CHECK_NULL(ptr, ...) do {                                       \
    if (__builtin_expect(!(ptr != Nil), 0)) {                          \
        if (__VA_NARGS(__VA_ARGS__) > 0)                                \
        FATAL(" CHECK_NULL(" #ptr ") failed. => " __VA_ARGS__);         \
        else                                                            \
        FATAL(" CHECK_NULL(" #ptr ") failed.");                         \
    }                                                                   \
} while(0)

END_DECLS

#ifdef __cplusplus
#include <ABE/core/String.h>

__BEGIN_NAMESPACE_ABE_PRIVATE

template <class A, class B>
ABE_INLINE String __MAKE_CHECK_STRING(const A& a, const B& b) {
    String res;
    res.append(String(a));
    res.append(" vs ");
    res.append(String(b));
    return res;
}

__END_NAMESPACE_ABE_PRIVATE


#define _CHECK2(a, b, op, ...) do {                                         \
    if (__builtin_expect(!((a) op (b)), 0)) {                               \
        using __NAMESPACE_ABE::String;                                      \
        String __res = __NAMESPACE_PRIVATE::__MAKE_CHECK_STRING(a, b);  \
        if (__VA_NARGS(__VA_ARGS__) > 0)                                    \
        FATAL(__FILE__ ":" __LITERAL_TO_STRING(__LINE__)                      \
                " CHECK(" #a " " #op " " #b ") failed. "                    \
                "%s. => " __VA_ARGS__ , __res.c_str());                     \
        else                                                                \
        FATAL(__FILE__ ":" __LITERAL_TO_STRING(__LINE__)                      \
                " CHECK(" #a " " #op " " #b ") failed. "                    \
                "%s", __res.c_str());                                       \
    }                                                                       \
} while(0)

#else // __cplusplus
// check builtin types, an print result as integer if failed.
// the check always work, but not the print message.
#define _CHECK2(a, b, op, ...) do {                                 \
    if (__builtin_expect(!((a) op (b)), 0)) {                       \
        if (__VA_NARGS(__VA_ARGS__) > 0)                            \
        FATAL(__FILE__ ":" __LITERAL_TO_STRING(__LINE__)              \
                " CHECK(" #a " " #op " " #b ") failed. "            \
                " => " __VA_ARGS__);                                \
        else                                                        \
        FATAL(__FILE__ ":" __LITERAL_TO_STRING(__LINE__)              \
                " CHECK(" #a " " #op " " #b ") failed. ");          \
    }                                                               \
} while(0)
#endif // __cplusplus

#define CHECK_EQ(a, b, ...)     _CHECK2(a, b, ==, __VA_ARGS__)
#define CHECK_NE(a, b, ...)     _CHECK2(a, b, !=, __VA_ARGS__)
#define CHECK_GT(a, b, ...)     _CHECK2(a, b, >, __VA_ARGS__)
#define CHECK_LT(a, b, ...)     _CHECK2(a, b, <, __VA_ARGS__)
#define CHECK_GE(a, b, ...)     _CHECK2(a, b, >=, __VA_ARGS__)
#define CHECK_LE(a, b, ...)     _CHECK2(a, b, <=, __VA_ARGS__)
#endif // ABE_HEADERS_LOG_H

