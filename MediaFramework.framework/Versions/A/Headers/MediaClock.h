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


/**
 * File:    MediaClock.h
 * Author:  mtdcy.chen
 * Changes:
 *          1. 20181214     initial version
 *
 */

#ifndef MFWK_MEDIA_CLOCK_H
#define MFWK_MEDIA_CLOCK_H

#include <MediaFramework/MediaTypes.h>

__BEGIN_DECLS

enum eClockRole {
    kClockRoleMaster,   ///< master clock
    kClockRoleSlave,    ///< slave clock
};

enum eClockState {
    kClockStateTicking,
    kClockStatePaused,
    kClockStateTimeChanged
};

__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_MFWK

typedef MediaEvent<eClockState> ClockEvent;

/**
 * clock for av sync and speed control. all session share
 * the same SharedClock, multiple slave Clocks can exists at
 * the same time, but only one master Clock. and only the
 * SharedClock itself or master clock can modify the clock
 * context.
 */
class Clock;
class API_EXPORT SharedClock : public SharedObject {
    public:
        SharedClock();

        /**
         * set clock time, without alter clock state
         * @param t     media time.
         */
        void        set(Time t);
        /**
         * start this clock.
         * @note only alter clock state, media time won't update yet.
         */
        void        start();
        /**
         * pause this clock.
         * @note only alter clock state, media time still updating.
         */
        void        pause();
        /**
         * get this clock's state.
         * @return return True if paused.
         */
        Bool        isPaused() const;
        /**
         * set this clock's speed.
         * @param   s   speed of clock in Float64.
         */
        void        setSpeed(Float64 s);
        /**
         * get this clock's speed.
         * @return clock speed in Float64.
         */
        Float64     speed() const;

        /**
         * get clock media time
         * @return  media time, always valid.
         */
        Time        get() const;

    private:
        void        _regListener(const void * who, const sp<ClockEvent>& ce);
        void        _unregListener(const void * who);

        void        notifyListeners_l(eClockState state);

    private:
        // atomic Int
        Atomic<Int>     mGeneration;
        Atomic<Int>     mMasterClock;
        // clock internal context
        mutable Mutex   mLock;          ///< lock for ClockInt
        struct ClockInt {
            ClockInt();
            Time        mMediaTime;
            Time        mSystemTime;
            Bool        mStarted;       ///< clock state, only start() & pause() can alter it
            Bool        mTicking;
            Float64     mSpeed;
        };
        ClockInt        mClockInt;

        friend class Clock;

        void            update(const ClockInt&);
        HashTable<const void *, sp<ClockEvent> > mListeners;

        Time            get_l() const;

        OBJECT_TAIL(SharedClock);
};

/**
 * shadow of SharedClock
 * @see SharedClock
 */
class API_EXPORT Clock : public SharedObject {
    public:
        /**
         * create a shadow of SharedClock
         * @param sc    reference to SharedClock
         * @param role  @see SharedClock::eClockRole
         */
        Clock(const sp<SharedClock>& sc, eClockRole role = kClockRoleSlave);

    public:
        /**
         * get role of this clock
         * @return @see SharedClock::eClockRole
         */
        FORCE_INLINE eClockRole role() const { return mRole; }

        /**
         * set event to this clock
         * @param ce    reference of ClockEvent. if ce = Nil,
         *              clear the listener
         */
        void        setListener(const sp<ClockEvent>& ce);

        /** @see SharedClock::get() */
        Time        get() const;

        /** @see SharedClock::isPaused() */
        Bool        isPaused() const;
        //Bool        isTicking() const;

        /** @see SharedClock::speed() */
        Float64     speed() const;
    
        /**
         * update clock start media time
         * @note start shared clock if not started
         */
        void        start();
    
        /**
         * update clock pause media time
         * @note pause shared clock if not paused
         */
        void        pause();
    
        /**
         * update clock media time
         * @param t media time
         * @note update method is only for master clock
         */
        void        update(Time t);

    private:
        /**
         * shadow SharedClock internal context if generation changed.
         * otherwise do NOTHING.
         */
        void        reload() const;
        Time        getInt() const;

    private:
        // NO lock here
        sp<SharedClock>                 mClock;
        const eClockRole                mRole;
        /**
         * shadow of SharedClock::mGeneration
         */
        mutable Int                     mGeneration;
        /**
         * shadow of SharedClock internal context
         * @see SharedClock::ClockInt
         */
        mutable SharedClock::ClockInt   mClockInt;

        OBJECT_TAIL(Clock);
};


__END_NAMESPACE_MFWK
#endif

#endif
