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


// File:    Looper.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef ABE_LOOPER_H
#define ABE_LOOPER_H 

#include <ABE/core/Types.h>
#include <ABE/core/String.h>

#include <pthread.h>

__BEGIN_DECLS
/**
 * thread type
 * combine of nice, priority and sched policy
 */
enum {
    kThreadLowest           = 0,
    kThreadBackgroud        = 16,
    kThreadNormal           = 32,
    kThreadForegroud        = 48,
    // for those who cares about the sched latency.
    // root permission maybe required
    kThreadSystem           = 64,
    kThreadKernel           = 80,
    // for those sched latency is critical.
    // real time sched (SCHED_RR) will be applied if possible.
    // root permission maybe required
    kThreadRealtime         = 96,
    // POSIX.1 requires an implementation to support only a minimum 32
    // distinct priority levels for the real-time policies.
    kThreadHighest          = 128,
    kThreadDefault          = kThreadNormal,
};
typedef UInt32  eThreadType;

// Status for CreateThread.
enum {
    kThreadOK           = OK,
    kThreadXXX,
};

/**
 * create a named thread and start immediately
 * @note the new thread will be in detach state.
 * @note set thread properties using pthread routines or other similar apis in ThreadEntry
 */
ABE_EXPORT Status CreateThread(const char *name,
                               eThreadType,
                               void (*ThreadEntry)(void *),
                               void *);

__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_ABE

class Looper;
class DispatchQueue;
struct JobDelegate;

// A small section of code that can be executed by JobDelegate.
class ABE_EXPORT Job : public SharedObject {
    protected:
        // a job object constructors
        Job();
        Job(const sp<Looper>&);
        Job(const sp<DispatchQueue>&);

    public:
        /**
         * run this job in asynchronized way.
         * @param after time that this job will be executed after.
         * @note if no job delegate exists, after will be ignored.
         */
        virtual void dispatch(Time after = 0);
    
        /**
         * run this job in synchronized way.
         * @param deadline  time that this job will wait until success.
         *                 if deadline == 0, it will wait forever.
         * @return return True on success, otherwise return False on timeout.
         * @note if no job delegate exists, deadline will be ignored.
         */
        virtual Bool sync(Time deadline = 0);

        /**
         * stop this job.
         */
        virtual void cancel();

    protected:
        // abstract interface MUST be implemented by subclass
        virtual void onJob() = 0;

    protected:
        friend struct JobDelegate;
        // run Job directly, for job delegate.
        void execution();   // -> onJob()
    
        OBJECT_TAIL(Job);
};

/**
 * a looper object backed by a job scheduler.
 * @note NO explicit terminate operator for looper other than main, otherwise
 *       related DispatchQueue will stop working after terminate.
 * @note release looper will NOT terminate underlying scheduler immediately.
 *       underlying scheduler will be released after all references are gone.
 * @note jobs may be dropped when scheduler be terminated.
 * @note main looper MUST be terminated explicitly.
 */
class ABE_EXPORT Looper : public SharedObject {
    public:
        /**
         * get 'main looper', prepare one if not exists.
         * @return return reference to 'main looper'
         */
        static sp<Looper> Main();
    
        /**
         * get current looper
         * @return return reference to current looper
         */
        static sp<Looper> Current();

        /**
         * create a looper
         */
        Looper(const String& name, const eThreadType& type = kThreadNormal);

    public:
        /**
         * run a job in asynchronized way.
         * @param a job object refereence.
         * @param after     time that this job will be executed after.
         */
        void    dispatch(const sp<Job>&, Time after = 0);
    
        /**
         * run a job in synchronized way.
         * @param a job object reference.
         * @param deadline time that job will wait until success.
         *                If deadline == 0, it will wait forever.
         * @return return True on success, return False on timeout.
         */
        Bool    sync(const sp<Job>&, Time deadline = 0);

        /**
         * remove a job object from this looper
         * @param a job object reference.
         * @return return True when job exists and be removed successful.
         */
        Bool    remove(const sp<Job>&);

        /**
         * find a job object in this looper
         * @param a job object reference
         * @return return True when job exists, otherwise return False.
         */
        Bool    exists(const sp<Job>&) const;

        /**
         * flush all job objects from this looper
         */
        void    flush();

    public:
        /**
         * start main looper.
         * @note can only be called in main thread.
         * @note it will always block until be terminated.
         */
        void    loop();
    
        /**
         * terminate main looper.
         * @note be careful, terminate main looper will stop related DispatchQueue.
         * @note terminate will drop all existing jobs.
         */
        void    terminate();

    private:
        friend class Job;
        friend class DispatchQueue;
        Looper();
        OBJECT_TAIL(Looper);
};

/**
 * a dispatch queue object backed by looper scheduler.
 * @note multiple dispatch queue can share the same scheduler, but
 *       each queue has a seperate context.
 * @note jobs will be dropped when all references are gone.
 */
class ABE_EXPORT DispatchQueue : public SharedObject {
    public:
        /**
         * create a dispatch queue object on looper
         * @param a looper object reference.
         */
        DispatchQueue(const sp<Looper>&);
                
    public:
        /**
         * run a job in synchronized way.
         * @param a job object reference.
         * @param deadline time that job will wait until success.
         *                If deadline == 0, it will wait forever.
         * @return return True on success, return False on timeout.
         */
        Bool    sync(const sp<Job>&, Time deadline = 0);
    
        /**
         * run a job in asynchronized way.
         * @param a job object reference
         * @param after time that this job will be executed after.
         */
        void    dispatch(const sp<Job>&, Time after = 0);
    
        /**
         * find a job exist in this queue.
         * @return return True if exists, otherwise False.
         */
        Bool    exists(const sp<Job>&) const;
    
        /**
         * remove a job from queue.
         * @param a job object reference.
         * @return return True when job exists and be removed successful.
         * @note
         */
        Bool    remove(const sp<Job>&);
    
        /**
         * flush a jobs in queue.
         */
        void    flush();
        
    private:
        friend class Job;
        OBJECT_TAIL(DispatchQueue);
};

__END_NAMESPACE_ABE
#endif // __cplusplus
#endif // ABE_LOOPER_H
