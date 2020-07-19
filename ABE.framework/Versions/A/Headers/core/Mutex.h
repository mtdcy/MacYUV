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


// File:    Mutex.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef ABE_HEADERS_MUTEX_H
#define ABE_HEADERS_MUTEX_H

#include <ABE/core/Types.h>
#include <ABE/core/System.h>

#if defined(_WIN32)
#include <Windows.h>
/* use light weight mutex/condition variable API for Windows Vista and later */
typedef SRWLOCK                 pthread_mutex_t;
typedef CONDITION_VARIABLE      pthread_cond_t;
#else
#include <pthread.h>
#endif

__BEGIN_NAMESPACE_ABE

class Condition;
class ABE_EXPORT Mutex : public StaticObject {
    public:
        Mutex(Bool recursive = False);
        ~Mutex();

    public:
        void    lock();
        void    unlock();
        Bool    tryLock();      ///< return True on success

    private:
        friend class    Condition;

        pthread_mutex_t mLock;

    DISALLOW_EVILS(Mutex);
};

class ABE_EXPORT AutoLock : public StaticObject {
    public:
        ABE_INLINE AutoLock(Mutex& lock) : mLock(lock)    { mLock.lock();   }
        ABE_INLINE ~AutoLock()                            { mLock.unlock(); }

    private:
        Mutex&  mLock;

    DISALLOW_EVILS(AutoLock);
};

class ABE_EXPORT Condition : public StaticObject {
    public:
        Condition();
        ~Condition();
        void    wait(Mutex& lock);
        /**
         * @param nsecs relative time to wait
         * @return return True on timeout, other wise return False
         */
        Bool    waitRelative(Mutex& lock, Time after);
        void    signal();
        void    broadcast();

    private:
        pthread_cond_t  mWait;

    DISALLOW_EVILS(Condition);
};

#if 0
class ABE_EXPORT RWLock : public StaticObject {
    public:
        RWLock();
        ~RWLock();

        void    lock(Bool write = False);
        void    unlock(Bool write = False);
        Bool    tryLock(Bool write = False);

    private:
        pthread_rwlock_t    mLock;

    DISALLOW_EVILS(RWLock);
};
#endif

__END_NAMESPACE_ABE

#endif // ABE_HEADERS_MUTEX_H
