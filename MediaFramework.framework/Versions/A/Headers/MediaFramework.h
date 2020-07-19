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


// File:    MediaFramework.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#ifndef MFWK_ALL_H
#define MFWK_ALL_H

// MediaFramework
#include <MediaFramework/MediaTypes.h>
#include <MediaFramework/MediaFrame.h>
#include <MediaFramework/MediaUnit.h>
#include <MediaFramework/MediaDevice.h>
#include <MediaFramework/ColorConverter.h>
#include <MediaFramework/MediaClock.h>
#include <MediaFramework/MediaSession.h>
#include <MediaFramework/MediaPlayer.h>
#include <MediaFramework/ImageFile.h>

//#ifndef __cplusplus
__BEGIN_DECLS

// ImageFile
typedef SharedObjectRef         ImageFileRef;
API_EXPORT ImageFileRef         ImageFileOpen(BufferObjectRef);

// MediaFrame
typedef SharedObjectRef         MediaFrameRef;

API_EXPORT MediaFrameRef        MediaFrameCreate(UInt32);
API_EXPORT MediaFrameRef        MediaFrameCreateWithBuffer(BufferObjectRef);
API_EXPORT MediaFrameRef        MediaFrameCreateWithAudioFormat(const AudioFormat *);
API_EXPORT MediaFrameRef        MediaFrameCreateWithImageFormat(const ImageFormat *);
API_EXPORT MediaFrameRef        MediaFrameCreateWithImageBuffer(const ImageFormat *, BufferObjectRef);

API_EXPORT UInt32               MediaFrameGetPlaneCount(const MediaFrameRef);
API_EXPORT UInt32               MediaFrameGetPlaneSize(const MediaFrameRef, UInt32);
API_EXPORT UInt8 *              MediaFrameGetPlaneData(MediaFrameRef, UInt32);

API_EXPORT AudioFormat *        MediaFrameGetAudioFormat(MediaFrameRef);
API_EXPORT ImageFormat *        MediaFrameGetImageFormat(MediaFrameRef);

// Clock, get a clock from MediaPlayerRef, used to update ui
typedef SharedObjectRef         MediaClockRef;
#define MediaClockRelease(x)    SharedObjectRelease((SharedObjectRef)x)

API_EXPORT Int64                MediaClockGetTime(MediaClockRef);

// MediaPlayer
typedef SharedObjectRef         MediaPlayerRef;

API_EXPORT MediaPlayerRef       MediaPlayerCreate(MessageObjectRef, MessageObjectRef);

/**
 * @note remember to release after use
 */
API_EXPORT MediaClockRef        MediaPlayerGetClock(const MediaPlayerRef);

API_EXPORT void                 MediaPlayerPrepare(MediaPlayerRef, Int64);
API_EXPORT void                 MediaPlayerStart(MediaPlayerRef);
API_EXPORT void                 MediaPlayerPause(MediaPlayerRef);

// Events
typedef SharedObjectRef         FrameEventRef;
typedef SharedObjectRef         PlayerInfoEventRef;

typedef void (*FrameCallback)(MediaFrameRef, void *);

API_EXPORT FrameEventRef        FrameEventCreate(LooperObjectRef, void (*Callback)(MediaFrameRef, void *), void *);
#define FrameEventRelease(x)    SharedObjectRelease((SharedObjectRef)x)

typedef void (*PlayerInfoCallback)(ePlayerInfoType, const MessageObjectRef, void *);
API_EXPORT PlayerInfoEventRef   PlayerInfoEventCreate(LooperObjectRef, PlayerInfoCallback, void *);
#define PlayerInfoEventRelease(x) SharedObjectRelease((SharedObjectRef)x)

// MediaDevice
typedef SharedObjectRef         MediaDeviceRef;

API_EXPORT MediaDeviceRef       MediaDeviceCreate(const MessageObjectRef, const MessageObjectRef);
API_EXPORT MessageObjectRef     MediaDeviceFormats(const MediaDeviceRef);
API_EXPORT MediaError           MediaDeviceConfigure(const MediaDeviceRef, const MessageObjectRef);
API_EXPORT MediaError           MediaDevicePush(MediaDeviceRef, const MediaFrameRef);
API_EXPORT MediaFrameRef        MediaDevicePull(MediaDeviceRef);
API_EXPORT MediaError           MediaDeviceReset(MediaDeviceRef);

API_EXPORT MessageObjectRef     MediaDeviceFormats(MediaDeviceRef);
API_EXPORT MediaError           MediaDeviceConfigure(MediaDeviceRef, MessageObjectRef);
API_EXPORT MediaError           MediaDevicePush(MediaDeviceRef, MediaFrameRef);
API_EXPORT MediaFrameRef        MediaDevicePull(MediaDeviceRef);
API_EXPORT MediaError           MediaDeviceReset(MediaDeviceRef);

// Special MediaDevice
API_EXPORT MediaDeviceRef       ColorConverterCreate(const ImageFormat *, const ImageFormat *, MessageObjectRef);

__END_DECLS
//#endif // __cplusplus

#endif // MFWK_ALL_H
