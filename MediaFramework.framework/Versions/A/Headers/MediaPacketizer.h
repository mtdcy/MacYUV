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


// File:    Packetizer.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160701     initial version
//

#ifndef _MEDIA_PACKETIZER_H
#define _MEDIA_PACKETIZER_H

#include <MediaFramework/MediaTypes.h>

#ifdef __cplusplus
__BEGIN_NAMESPACE_MPX

///////////////////////////////////////////////////////////////////////////
struct API_EXPORT MediaPacketizer : public SharedObject {
    MediaPacketizer() {}
    virtual ~MediaPacketizer() {}

    static sp<MediaPacketizer> Create(UInt32 format);     // @see eAudioCodec @see eVideoCodec

    virtual String          string() const = 0;

    /**
     * queue a packet into packetizer
     * @param   in      reference of input packet
     * @return return kMediaNoError on success, return kMediaErrorResourceBusy
     *                if internal buffer is full.
     */
    virtual MediaError      enqueue(const sp<MediaFrame>& in) = 0;
    /**
     * dequeue a packet from packetizer
     * @return return reference to new packet, or Nil if no packet ready
     */
    virtual sp<MediaFrame> dequeue() = 0;

    /**
     * flush packetizer content and reset its state
     */
    virtual void            flush() = 0;
};

__END_NAMESPACE_MPX
#endif

#endif // _MEDIA_PACKETIZER_H

