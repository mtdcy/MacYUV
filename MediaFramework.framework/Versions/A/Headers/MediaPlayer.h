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
 * File:    MediaPlayer.h
 * Author:  mtdcy.chen
 * Changes:
 *          1. 20181214     initial version
 *
 */

#ifndef MFWK_MEDIA_PLAYER_H
#define MFWK_MEDIA_PLAYER_H

#include <MediaFramework/MediaTypes.h>
#include <MediaFramework/MediaClock.h>
#include <MediaFramework/MediaSession.h>

__BEGIN_DECLS

/**
 * player info -> client
 * @note we prefer eInfoType instead of eStateType for client
 */
typedef enum {
    /**
     * player state info
     */
    kInfoPlayerReady,           ///<
    kInfoPlayerPlaying,         ///<
    kInfoPlayerPaused,          ///<
    kInfoPlayerEnd,             ///<
    kInfoPlayerError,           ///< 
    /**
     * player property info
     */
    kInfoOpenGLEnabled,         ///< OpenGL Enabled for video output
    kInfoVideoToolboxEnabled,   ///<
    /**
     *
     */
    kInfoBeginOfFile,           ///< BOS
    kInfoEndOfFile,             ///< EOS
    kInfoClockUpdated,          ///< Clock been updated by master
} ePlayerInfoType;

enum {
    kKeyPlayerInfoEvent     = FOURCC('pinf'),       ///< sp<PlayerInfoEvent>
    kKeyAudioFrameEvent     = FOURCC('afet'),       ///< sp<MediaFrameEvent>
    kKeyVideoFrameEvent     = FOURCC('vfet'),       ///< sp<MediaFrameEvent>
};

__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_MFWK

/**
 * for MediaPlayer streaming MediaFrame to client
 * TODO: remove this one, always render in thread other than main.
 */
typedef MediaEvent<sp<MediaFrame> > MediaFrameEvent;

/**
 * a generic event
 */
typedef MediaEvent2<ePlayerInfoType, sp<Message> > PlayerInfoEvent;

/**
 * add a media to player.
 * about media (per media):
 *  "url"                   - [String]                  - mandatory, url of the media
 *  "VideoFrameEvent"       - [sp<MediaFrameEvent>]     - optional
 *  "AudioFrameEvent"       - [sp<MediaFrameEvent>]     - optional
 *  "StartTime"             - [Float64|seconds]         - optional
 *  "EndTime"               - [Float64|seconds]         - optional
 * about options (global):
 *  "StatusEvent"           - [sp<StatusEvent>]         - optional
 *  "PlayerInfoEvent"       - [sp<PlayerInfoEvent>]     - optional
 * if MediaOut exists, external renderer will be used.
 * @param media option and parameter for this media
 * @return return kMediaNoError on success, otherwise error code
 */
class API_EXPORT IMediaPlayer : public SharedObject {
    public:
        /**
         * create a player with options
         * @param options   option and parameter for player
         */
        static sp<IMediaPlayer> Create(const sp<Message>& media, const sp<Message>& options);

    protected:
        IMediaPlayer(const sp<Looper>& lp);

    public:
        virtual sp<Clock>   clock() const;

        /**
         * prepare this player at given position
         */
        virtual void        prepare(const MediaTime&);

        /**
         * start this player.
         * @note there must be at least one media exists and start success.
         */
        virtual void        start();
        /**
         * pause this player.
         */
        virtual void        pause();
    
    protected:
        struct InitJob;
        struct ReleaseJob;
        struct PrepareJob;
        struct StartJob;
        struct PauseJob;
        virtual void        onInit(const sp<Message>& media, const sp<Message>& options) = 0;
        virtual void        onRelease() = 0;
        virtual void        onPrepare(const MediaTime&) = 0;
        virtual void        onStart() = 0;
        virtual void        onPause() = 0;
        
    protected:
        sp<DispatchQueue>   mDispatch;
        sp<SharedClock>     mClock;
    
        OBJECT_TAIL(IMediaPlayer)
};
__END_NAMESPACE_MFWK
#endif

#endif // MFWK_MEDIA_PLAYER_H

