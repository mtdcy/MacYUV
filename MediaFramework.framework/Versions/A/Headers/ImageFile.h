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


// File:    ImageFile.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160701     initial version
//

#ifndef _MEDIA_MODULES_IMAGE_FILE_H
#define _MEDIA_MODULES_IMAGE_FILE_H

#include <MediaFramework/MediaTypes.h>

#ifdef __cplusplus
__BEGIN_NAMESPACE_MPX

struct API_EXPORT ImageFile : public SharedObject {
    ImageFile() : SharedObject() { }
    virtual ~ImageFile() { }
    
    static sp<ImageFile> Create();
    
    virtual MediaError init(const sp<ABuffer>& pipe, const sp<Message>& options) = 0;
    
    virtual MediaError configure(const sp<Message>& options) { return kMediaErrorNotSupported; }
    
    virtual sp<Message> formats() const = 0;
    
    virtual sp<MediaFrame> read() = 0;
    
    virtual sp<MediaFrame> readImage() = 0;
    
    virtual MediaError write(const sp<MediaFrame>&) = 0;
    
    virtual MediaError writeImage(const sp<MediaFrame>&) = 0;
};

__END_NAMESPACE_MPX
#endif // __cplusplus

#endif // _MEDIA_MODULES_IMAGE_FILE_H
