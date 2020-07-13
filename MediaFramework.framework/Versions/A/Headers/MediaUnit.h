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


// File:    MediaUnit.h
// Author:  mtdcy.chen
// Changes:
//          1. 20200701     initial version
//

#ifndef _MEDIA_UNIT_H
#define _MEDIA_UNIT_H 

#include <MediaFramework/MediaFrame.h>

__BEGIN_DECLS

enum {
    kMediaUnitProcessInplace            = (1<<0),   // inplace process
    kMediaUnitProcessVariableSamples    = (1<<1),   ///< sample count change
};
typedef UInt32 eMediaUnitFlags;

typedef union MediaFormat {
    UInt32        format;
    AudioFormat     audio;
    ImageFormat     video;
    ImageFormat     image;
} MediaFormat;

typedef void * MediaUnitContext;
typedef MediaError  (*InputCallback)(void *, UInt32 samples, MediaBufferList **);
typedef struct MediaUnit {
    const Char *            name;       ///< hunam readable string, DEBUGGING!
    const eMediaUnitFlags   flags;      ///< media unit flags

    const UInt32 *        iformats;   ///< supported input format list
    const UInt32 *        oformats;   ///< supported output format list

    /**
     * alloc/dealloc a media unit context
     */
    MediaUnitContext    (*alloc)();
    void                (*dealloc)(MediaUnitContext);

    /**
     * init media unit
     * @return return kMediaNoError on success
     * @return return kMediaErrorBadParameters if in/out is bad
     * @note if MediaFormat member is ignored, it MUST set to 0
     */
    MediaError          (*init)(MediaUnitContext, const MediaFormat *, const MediaFormat *);

    /**
     * process audio/video/image with media unit
     * @return return kMediaNoError on success, otherwise MediaError code.
     * @return return kMediaErrorBadParameters if in/out MediaBufferList is bad
     */
    MediaError          (*process)(MediaUnitContext, const MediaBufferList *, MediaBufferList *);
    
    /**
     * complex process function
     * @return return kMediaNoError on success, otherwise MediaError code.
     * @return return kMediaErrorBadParameters if in/out MediaBufferList is bad 
     */
    MediaError          (*pull)(MediaUnitContext, MediaBufferList *, InputCallback icb, void *);

    /**
     * reset media unit
     * @return return kMediaNoError on success, otherwise MediaError code
     */
    MediaError          (*reset)(MediaUnitContext);
} MediaUnit;

__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_MPX


__END_NAMESPACE_MPX
#endif // __cplusplus 

#endif // _MEDIA_UNIT_H 
