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
 * File:    MediaSession.h
 * Author:  mtdcy.chen
 * Changes:
 *          1. 20181214     initial version
 *
 */

#ifndef MFWK_MEDIA_SESSION_H
#define MFWK_MEDIA_SESSION_H

#include <MediaFramework/MediaTypes.h>
#include <MediaFramework/MediaFrame.h>

__BEGIN_DECLS
#pragma mark Keys
enum {
    kKeyURL         = FOURCC('!url'),       ///< string
    kKeyLooper      = FOURCC('lper'),       ///< sp<Looper>
};
__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_MFWK

/**
 * For choose tracks
 */
typedef MediaEvent<UInt32> TrackSelectEvent;

/**
 * For pushing packets to target. when a packet is ready,
 * fire this event, and target will get the packet.
 */
typedef MediaEvent<sp<MediaFrame> > PacketReadyEvent;   // TODO: using FrameReadyEvent

/**
 * For pull packets from packet source.
 */
typedef MediaEvent2<sp<PacketReadyEvent>, MediaTime> PacketRequestEvent;    // TODO: using PacketRequestEvent

/**
 * For pushing frames to target. when a frame is ready,
 * fire this event, and target will receive the frame.
 */
typedef MediaEvent<sp<MediaFrame> > FrameReadyEvent;

/**
 * For pull frames from frame source
 */
typedef MediaEvent2<sp<FrameReadyEvent>, MediaTime> FrameRequestEvent;

typedef enum {
    kSessionInfoReady   = FOURCC('srdy'),
    kSessionInfoEnd     = FOURCC('send'),
    kSessionInfoError   = FOURCC('serr'),
} eSessionInfoType;
typedef MediaEvent2<eSessionInfoType, sp<Message> > SessionInfoEvent;

enum {
    kKeyTrackSelectEvent        = FOURCC('tsel'),   ///< sp<TrackSelectEvent>
    kKeyPacketReadyEvent        = FOURCC('prdy'),   ///< sp<PacketReadyEvent>
    kKeyPacketRequestEvent      = FOURCC('preq'),   ///< sp<PacketRequestEvent>
    kKeyFrameReadyEvent         = FOURCC('frdy'),   ///< sp<FrameReadyEvent>
    kKeyFrameRequestEvent       = FOURCC('freq'),   ///< sp<FrameRequestEvent>
    kKeySessionInfoEvent        = FOURCC('sinf'),   ///< sp<SessionInfoEvent>
    kKeyClock                   = FOURCC('clck'),   ///< sp<Clock>
};

class API_EXPORT IMediaSession : public SharedObject {
    public:
        static sp<IMediaSession> Create(const sp<Message>&, const sp<Message>&);
    
    public:
        IMediaSession(const sp<Looper>& lp) : mDispatch(new DispatchQueue(lp)) { }

    protected:
        // these routine always run inside looper
        struct InitJob;
        struct ReleaseJob;
        virtual void onInit(const sp<Message>&, const sp<Message>&) = 0;
        virtual void onRelease() = 0;

        sp<DispatchQueue> mDispatch;

        OBJECT_TAIL(IMediaSession);
};

__END_NAMESPACE_MFWK
#endif // __cplusplus

#endif // MFWK_MEDIA_SESSION_H
