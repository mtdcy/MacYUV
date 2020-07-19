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


// File:    MediaFrame.h
// Author:  mtdcy.chen
// Changes:
//          1. 20200630     initial version
//

#ifndef MFWK_MEDIA_FRAME_H
#define MFWK_MEDIA_FRAME_H

#include <MediaFramework/MediaTypes.h>

__BEGIN_DECLS

typedef struct MediaBuffer {
    UInt32          capacity;           ///< max number bytes in data, readonly
    UInt32          size;               ///< number bytes polluted in data, read & write
    UInt8 *         data;               ///< pointer to memory, read & write
#ifdef __cplusplus
    MediaBuffer() : capacity(0), size(0), data(Nil) { }
#endif
} MediaBuffer;

typedef struct MediaBufferList {
    UInt32          count;              ///< number buffer in list
    MediaBuffer     buffers[0];         ///< a variable length array with min length = 1
#ifdef __cplusplus
    MediaBufferList() : count(0) { }
#endif
} MediaBufferList;

__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_MFWK

API_EXPORT String   GetMediaBufferListString(const MediaBufferList&);

/**
 * @note MediaFrame MUST be in decoding order for compressed frame.
 * @note MediaFrame MUST be in presentation order for uncompressed frame.
 * @note NO dts field, the codec just need to decoding frames one by one, dts is useless.
 */
struct API_EXPORT MediaFrame : public SharedObject {
    UInt32            id;             ///< frame id, can be track index or frame index
    UInt32            flags;          ///< frame flags, @see eFrameType
    MediaTime           timecode;       ///< frame presentation time. no dts field.
    MediaTime           duration;       ///< frame duration
    union {
        UInt32        format;
        AudioFormat     audio;          ///< audio format
        ImageFormat     video;          ///< video format
        ImageFormat     image;          ///< image format
    };
    void *              opaque;         ///< invisible, for special purposes
    MediaBufferList     planes;         ///< this SHOULD be the last data member

    /**
     * create a media frame with underlying buffer
     * the underlying buffer is always continues, a single buffer for all planes
     */
    static sp<MediaFrame>   Create(UInt32);                             ///< create a one plane frame with n bytes underlying buffer
    static sp<MediaFrame>   Create(sp<Buffer>&);                        ///< create a one plane frame with Buffer
    static sp<MediaFrame>   Create(const AudioFormat&);                 ///< create a audio frame
    static sp<MediaFrame>   Create(const ImageFormat&);                 ///< create a video/image frame
    static sp<MediaFrame>   Create(const ImageFormat&, sp<Buffer>&);    ///< create a video/image frame with Buffer

    // DEBUGGING: get a human readable string
    virtual String          string() const;

    /** features below is not designed for realtime playback **/

    /**
     * read backend buffer of hwaccel frame
     * @return should return Nil if plane is not exists
     * @note default implementation: read directly from planes
     */
    virtual sp<ABuffer> readPlane(UInt32) const;

    protected:
    MediaFrame();

    OBJECT_TAIL(MediaFrame);
};

__END_NAMESPACE_MFWK
#endif // __cplusplus

#endif // MFWK_MEDIA_FRAME_H
