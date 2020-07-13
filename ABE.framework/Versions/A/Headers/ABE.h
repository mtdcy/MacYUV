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


#ifndef ABE_HEADERS_ALL_H
#define ABE_HEADERS_ALL_H

// core [c & c++]
#include <ABE/core/Version.h>
#include <ABE/core/Types.h>

#ifdef LOG_TAG
#include <ABE/core/Log.h>
#endif

#include <ABE/core/System.h>

#ifdef __cplusplus  // only available for c++
#include <ABE/core/Allocator.h>
#include <ABE/core/SharedBuffer.h>
#include <ABE/core/String.h>
#include <ABE/core/Mutex.h>

// object types [SharedObject] [c & c++]
#include <ABE/core/Buffer.h>
#include <ABE/core/Message.h>
#include <ABE/core/Content.h>
#include <ABE/core/Looper.h>

// containers [non-SharedObject]
#include <ABE/stl/TypeHelper.h>
#include <ABE/stl/List.h>
#include <ABE/stl/Vector.h>
#include <ABE/stl/HashTable.h>
#include <ABE/stl/Queue.h>
#include <ABE/stl/Bits.h>

// math [non-SharedObject]
#include <ABE/math/Matrix.h>

#endif //  __cplusplus

// c bindings
BEGIN_DECLS

typedef void (*UserCallback)(void *user);

typedef void *                  SharedObjectRef;
#define NIL                     (SharedObjectRef)0

ABE_EXPORT SharedObjectRef      SharedObjectRetain(SharedObjectRef);
ABE_EXPORT void                 SharedObjectRelease(SharedObjectRef);
ABE_EXPORT UInt32               SharedObjectGetRetainCount(const SharedObjectRef);
#define SharedObjectIsShared(s)     (SharedObjectGetRetainCount(s) > 1)
#define SharedObjectIsNotShared(s)  !SharedObjectIsShared(s)
ABE_EXPORT UInt32               SharedObjectGetID(const SharedObjectRef);

typedef SharedObjectRef         AllocatorRef;
ABE_EXPORT AllocatorRef         AllocatorGetDefault(void);
ABE_EXPORT AllocatorRef         AllocatorGetDefaultAligned(UInt32);
ABE_EXPORT UInt32               AllocatorGetAlignment(AllocatorRef);
ABE_EXPORT void *               AllocatorAllocate(AllocatorRef, UInt32);
ABE_EXPORT void *               AllocatorReallocate(AllocatorRef, void *, UInt32);
ABE_EXPORT void                 AllocatorDeallocate(AllocatorRef, void *);

typedef SharedObjectRef         SharedBufferRef;
ABE_EXPORT SharedBufferRef      SharedBufferCreate(AllocatorRef allocator, UInt32);
ABE_EXPORT SharedBufferRef      SharedBufferRetain(SharedBufferRef);
ABE_EXPORT UInt32               SharedBufferGetRetainCount(const SharedBufferRef);
ABE_EXPORT void                 SharedBufferRelease(SharedBufferRef);
ABE_EXPORT Char *               SharedBufferGetDataPointer(SharedBufferRef);
ABE_EXPORT const Char *         SharedBufferGetConstDataPointer(const SharedBufferRef);
ABE_EXPORT UInt32               SharedBufferGetDataLength(const SharedBufferRef);
ABE_EXPORT SharedBufferRef      SharedBufferEdit(SharedBufferRef);
ABE_EXPORT SharedBufferRef      SharedBufferEditWithSize(SharedBufferRef, UInt32);
ABE_EXPORT UInt32               SharedBufferReleaseWithoutDelete(SharedBufferRef);
ABE_EXPORT void                 SharedBufferDelete(SharedBufferRef);

typedef SharedObjectRef         BufferObjectRef;
ABE_EXPORT BufferObjectRef      BufferObjectCreate(UInt32);                     // sp<Buffer>
ABE_EXPORT BufferObjectRef      BufferObjectCreateWithUrl(const Char * url);    // sp<Content>
ABE_EXPORT Int64                BufferObjectGetCapacity(const BufferObjectRef);
ABE_EXPORT Int64                BufferObjectGetDataLength(const BufferObjectRef);
ABE_EXPORT Int64                BufferObjectGetEmptyLength(const BufferObjectRef);
ABE_EXPORT Int64                BufferObjectGetOffset(const BufferObjectRef);
ABE_EXPORT const Char *         BufferObjectGetDataPointer(const BufferObjectRef);
ABE_EXPORT UInt32               BufferObjectGetData(const BufferObjectRef, Char *, UInt32);
ABE_EXPORT BufferObjectRef      BufferObjectReadBytes(const BufferObjectRef, UInt32);
ABE_EXPORT BufferObjectRef      BufferObjectCloneBytes(const BufferObjectRef);
ABE_EXPORT Int64                BufferObjectSkipBytes(const BufferObjectRef, Int64);
ABE_EXPORT void                 BufferObjectResetBytes(const BufferObjectRef);
ABE_EXPORT UInt32               BufferObjectPutData(BufferObjectRef, const Char *, UInt32);
ABE_EXPORT UInt32               BufferObjectWriteBytes(BufferObjectRef, BufferObjectRef, UInt32);
ABE_EXPORT void                 BufferObjectFlushBytes(BufferObjectRef);
ABE_EXPORT void                 BufferObjectClearBytes(BufferObjectRef);

typedef SharedObjectRef         MessageObjectRef;
ABE_EXPORT MessageObjectRef     MessageObjectCreate();
ABE_EXPORT MessageObjectRef     MessageObjectCopy(MessageObjectRef);
ABE_EXPORT UInt32               MessageObjectGetSize    (const MessageObjectRef);
ABE_EXPORT Bool                 MessageObjectContains   (const MessageObjectRef, UInt32);
ABE_EXPORT Bool                 MessageObjectRemove     (MessageObjectRef, UInt32);
ABE_EXPORT void                 MessageObjectClear      (MessageObjectRef);
ABE_EXPORT void                 MessageObjectPutInt32   (MessageObjectRef, UInt32, Int32);
ABE_EXPORT void                 MessageObjectPutInt64   (MessageObjectRef, UInt32, Int64);
ABE_EXPORT void                 MessageObjectPutFloat   (MessageObjectRef, UInt32, Float32);
ABE_EXPORT void                 MessageObjectPutDouble  (MessageObjectRef, UInt32, Float64);
ABE_EXPORT void                 MessageObjectPutPointer (MessageObjectRef, UInt32, void *);
ABE_EXPORT void                 MessageObjectPutString  (MessageObjectRef, UInt32, const Char *);
ABE_EXPORT void                 MessageObjectPutObject  (MessageObjectRef, UInt32, SharedObjectRef);
ABE_EXPORT Int32                MessageObjectGetInt32   (const MessageObjectRef, UInt32, Int32);
ABE_EXPORT Int64                MessageObjectGetInt64   (const MessageObjectRef, UInt32, Int64);
ABE_EXPORT Float32              MessageObjectGetFloat   (const MessageObjectRef, UInt32, Float32);
ABE_EXPORT Float64              MessageObjectGetDouble  (const MessageObjectRef, UInt32, Float64);
ABE_EXPORT void *               MessageObjectGetPointer (const MessageObjectRef, UInt32, void *);
ABE_EXPORT const Char *         MessageObjectGetString  (const MessageObjectRef, UInt32, const Char *);
ABE_EXPORT SharedObjectRef      MessageObjectGetObject  (const MessageObjectRef, UInt32, SharedObjectRef);

typedef SharedObjectRef         JobObjectRef;
typedef SharedObjectRef         LooperObjectRef;
typedef SharedObjectRef         DispatchQueueRef;

ABE_EXPORT JobObjectRef         JobObjectCreate(UserCallback, void *);
ABE_EXPORT JobObjectRef         JobObjectCreateWithLooper(LooperObjectRef, UserCallback, void *);
ABE_EXPORT JobObjectRef         JobObjectCreateWithDispatchQueue(DispatchQueueRef, UserCallback, void *);
ABE_EXPORT void                 JobObjectDispatch(JobObjectRef, UInt64);
ABE_EXPORT Bool                 JobObjectSync(JobObjectRef, UInt64);
ABE_EXPORT void                 JobObjectCancel(JobObjectRef);

ABE_EXPORT LooperObjectRef      LooperObjectMain();
ABE_EXPORT LooperObjectRef      LooperObjectCurrent();
ABE_EXPORT LooperObjectRef      LooperObjectCreate(const Char * name);
ABE_EXPORT void                 LooperObjectDispatch(LooperObjectRef, JobObjectRef, UInt64);
ABE_EXPORT Bool                 LooperObjectSync(LooperObjectRef, JobObjectRef, UInt64);
ABE_EXPORT Bool                 LooperObjectRemove(LooperObjectRef, JobObjectRef);
ABE_EXPORT Bool                 LooperObjectFind(LooperObjectRef, JobObjectRef);
ABE_EXPORT void                 LooperObjectFlush(LooperObjectRef);

ABE_EXPORT DispatchQueueRef     DispatchQueueCreate(LooperObjectRef);
ABE_EXPORT void                 DispatchQueueDispatch(DispatchQueueRef, JobObjectRef, UInt64);
ABE_EXPORT Bool                 DispatchQueueSync(DispatchQueueRef, JobObjectRef, UInt64);
ABE_EXPORT Bool                 DispatchQueueRemove(DispatchQueueRef, JobObjectRef);
ABE_EXPORT Bool                 DispatchQueueFind(DispatchQueueRef, JobObjectRef);
ABE_EXPORT void                 DispatchQueueFlush(DispatchQueueRef);

END_DECLS

#endif // ABE_HEADERS_ALL_H
