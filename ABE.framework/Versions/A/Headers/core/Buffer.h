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


// File:    Buffer.h
// Author:  mtdcy.chen
// Changes: 
//          1. 20160701     initial version
//

#include <ABE/core/Types.h>

#ifndef ABE_HEADERS_BUFFER_H
#define ABE_HEADERS_BUFFER_H 

///////////////////////////////////////////////////////////////////////////////
// Memory Management Layer [C++ Implementation]
#include <ABE/core/Allocator.h>
#include <ABE/core/String.h>

__BEGIN_NAMESPACE_ABE

/**
 * base class for FIFO buffer
 * Buffer:
 *                           write pos
 *                           v
 *  |-----------------------------------------------------------|
 *      ^
 *      read pos
 *
 * ABuffer is not thread safe
 */
class ABE_EXPORT ABuffer : public SharedObject {
    public:
        enum eByteOrder { Little, Big };
        virtual ~ABuffer() { }
    
    public:
        /**
         * read buffer properties
         * @note abstract interface for inherited class
         */
        virtual Int64     capacity() const = 0;   ///< return buffer capacity
        virtual Int64     size() const = 0;       ///< return data size in bytes in buffer
        virtual Int64     empty() const = 0;      ///< return empty bytes in buffer
        virtual Int64     offset() const = 0;     ///< return current read position
        virtual const Char* data() const = 0;
    
    public:
        /**
         * read bytes from buffer
         * @note bytes operator will reset reader bits reservoir
         */
        virtual sp<ABuffer> readBytes(UInt32) const = 0;        ///< read n bytes as a new buffer
        virtual UInt32      readBytes(Char *, UInt32) const = 0;///< read n bytes into certain memory
        virtual Int64       skipBytes(Int64) const = 0;       ///< skip read n bytes, [-offset(), size()]
        virtual void        resetBytes() const = 0;             ///< reset bytes reader
        virtual sp<ABuffer> cloneBytes() const = 0;             ///< clone all bytes
        
        /**
         * write bytes into buffer
         * @note bytes writer will flush writer bits reservoir
         */
        virtual UInt32      writeBytes(const Char *, UInt32 n = 0) = 0;         ///< write n bytes into this buffer
        virtual UInt32      writeBytes(const sp<ABuffer>& b, UInt32 n = 0) = 0; ///< write a buffer into this buffer
        virtual UInt32      writeBytes(Int c, UInt32 n) = 0;    ///< write n bytes of c
        virtual void        flushBytes() = 0;                   ///< flush bytes writer
        virtual void        clearBytes() = 0;                   ///< clear bytes in buffer

    protected:
        // basic operators for bit reader & writer
        virtual UInt8       readByte() const = 0;               ///< read one byte
        virtual void        writeByte(UInt8) = 0;             ///< write one byte

    public:
        /**
         * bit reader operators
         * @note client have to do sanity check on bits left for read
         */
        void                skip(UInt32) const;         ///< skip n bits
        void                skip() const;               ///< skip current byte tail bits
        UInt32              read(UInt32) const;         ///< read n bits
        UInt32              show(UInt32) const;         ///< show n bits
        UInt8               r8() const;                 ///< read 8 bits
        UInt16              rl16() const;               ///< read 16 bits as little endian
        UInt16              rb16() const;               ///< read 16 bits as big endian
        UInt32              rl24() const;               ///< read 24 bits as little endian
        UInt32              rb24() const;               ///< read 24 bits as big endian
        UInt32              rl32() const;               ///< read 32 bits as little endian
        UInt32              rb32() const;               ///< read 32 bits as big endian
        UInt64              rl64() const;               ///< read 64 bits as little endian
        UInt64              rb64() const;               ///< read 64 bits as big endian
        String              rs(UInt32) const;           ///< read n Char as string

        /**
         * bit writer operators
         * @note client have to do sanity check on bits left for write
         */
        void                write(UInt32, UInt32);    ///< write value x as n bits
        void                write();                    ///< write bits 0 to byte boundary
        void                w8(UInt8);                ///< write 8 bits
        void                wl16(UInt16);             ///< write 16 bits as little endian
        void                wb16(UInt16);             ///< write 16 bits as big endian
        void                wl24(UInt32);             ///< write 24 bits as little endian
        void                wb24(UInt32);             ///< write 24 bits as big endian
        void                wl32(UInt32);             ///< write 32 bits as little endian
        void                wb32(UInt32);             ///< write 32 bits as big endian
        void                wl64(UInt64);             ///< write 64 bits as little endian
        void                wb64(UInt64);             ///< write 64 bits as big endian
        void                ws(const String&, UInt32 n = 0);    ///< write n byte string

        /**
         * set default byte order and read based on byte order
         * @note values in a structure always has the same byte order
         */
        ABE_INLINE eByteOrder byteOrder() const { return mByteOrder; }
        ABE_INLINE void     setByteOrder(eByteOrder order) const { mByteOrder = order; }
        ABE_INLINE UInt16   r16() const     { return mByteOrder == Big ? rb16() : rl16(); }
        ABE_INLINE UInt32   r24() const     { return mByteOrder == Big ? rb24() : rl24(); }
        ABE_INLINE UInt32   r32() const     { return mByteOrder == Big ? rb32() : rl32(); }
        ABE_INLINE UInt64   r64() const     { return mByteOrder == Big ? rb64() : rl64(); }
        ABE_INLINE void     w16(UInt16 x) { mByteOrder == Big ? wb16(x) : wl16(x); }
        ABE_INLINE void     w24(UInt32 x) { mByteOrder == Big ? wb24(x) : wl24(x); }
        ABE_INLINE void     w32(UInt32 x) { mByteOrder == Big ? wb32(x) : wl32(x); }
        ABE_INLINE void     w64(UInt64 x) { mByteOrder == Big ? wb64(x) : wl64(x); }

    protected:
        ABuffer() : SharedObject(FOURCC('?buf')), mByteOrder(Little) {
            mReadReservoir.mLength = 0;
            mWriteReservoir.mLength = 0;
        }
        
        void    reset() const;  ///< reset reader reservoir
        void    flush();        ///< flush writer reservoir
    
    private:
        mutable eByteOrder mByteOrder;
        struct Reservoir {
            UInt64      mBits;
            UInt32      mLength;
        };
        mutable Reservoir mReadReservoir;
        Reservoir mWriteReservoir;
};

class ABE_EXPORT Buffer : public ABuffer {
    public:
        enum eBufferType {
            Linear,             ///< linear buffer
            Ring,               ///< implement ring buffer, take twice memory as it need
            Default = Linear
        };
    
        /**
         * alloc an empty buffer with given capacity
         * @param capacity  initial capacity of this buffer
         * @param type      type of this buffer, @see eBufferType
         */
        Buffer(UInt32 capacity, const sp<Allocator>& allocator = kAllocatorDefault);
        Buffer(UInt32 capacity, eBufferType type, const sp<Allocator>& allocator = kAllocatorDefault);

        /**
         * alloc a buffer by duplicate a null-terminated string
         * @param data  pointer hold the string
         * @param n     length to duplicate
         * @param type  type of this buffer, @see eBufferType
         */
        Buffer(const Char *, UInt32 n = 0, eBufferType type = Default,
                const sp<Allocator>& allocator = kAllocatorDefault);
    
        /**
         * get flags of this buffer
         * @return return the flags @see eBufferFlags
         */
        eBufferType type() const { return mType; }
    
        /**
         * resize this buffer's backend memory
         * @param cap   new capacity of the backend memory
         * @return return True on success
         */
        Bool                resize(UInt32 cap);

        //! DEBUGGING
        String      string(Bool hex = False) const;
    
    public:
        virtual Int64       capacity() const { return mCapacity; }
        virtual Int64       size() const { return mWritePos - mReadPos; }
        virtual Int64       empty() const;
        virtual Int64       offset() const;
    
    public:
        virtual sp<ABuffer> readBytes(UInt32) const;
        virtual UInt32      readBytes(Char *, UInt32) const;
        virtual Int64       skipBytes(Int64) const;
        virtual void        resetBytes() const;
        virtual sp<ABuffer> cloneBytes() const;
    
        virtual UInt32      writeBytes(const Char *, UInt32 n = 0);
        virtual UInt32      writeBytes(const sp<ABuffer>&, UInt32 n = 0);
        virtual UInt32      writeBytes(Int c, UInt32 n);
        virtual void        flushBytes();
        virtual void        clearBytes();
    
    public:
        // access bytes directly. UNSAFE!
        // MUST avoid using these routines
        const Char *        base() const { return mData->data() + mOffset; }
        virtual const Char* data() const { return base() + mReadPos; }
        Char *              base();
        Char *              data() { return base() + mReadPos; }
        /**
         * move read pointer forword by n bytes
         * @note after write directly @ data()
         */
        UInt32              stepBytes(UInt32 n);
        /**
         * set buffer range, read pos @ offset with n bytes data
         * @note after write directly @ base()
         */
        void                setBytesRange(UInt32 offset, UInt32 n);

    protected:
        virtual UInt8       readByte() const;
        virtual void        writeByte(UInt8);

    private:
        void                edit();
        void                rewind(UInt32);
        SharedBuffer *      alloc();
    
    protected:
        // for creating cow buffer
        Buffer(const Buffer *, UInt32, UInt32);
    
        // backend memory provider
        sp<Allocator>       mAllocator;
        SharedBuffer *      mData;
        UInt32              mOffset;    // for cow buffer
        UInt32              mCapacity;
        const eBufferType   mType;
        mutable UInt32      mReadPos;
        UInt32              mWritePos;
    
        OBJECT_TAIL(Buffer);
};

__END_NAMESPACE_ABE

#endif // ABE_HEADERS_BUFFER_H 


