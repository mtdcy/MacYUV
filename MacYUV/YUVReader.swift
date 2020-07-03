//
//  YUVReader.swift
//  YUV
//
//  Created by Chen Fang on 2019/4/16.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Foundation

class YUVReader : NSObject {
    
    var mContent : BufferObjectRef?
    var mImageFormat : ImageFormat = ImageFormat.init()
    
    override init() {
        
    }
    
    func open(url : String) -> Bool {
        close()
        
        mContent = BufferObjectCreateWithUrl(url)
        
        if (mContent == nil) {
            return false
        }
        return true
    }
    
    func close() -> Void {
        if (mContent != nil) {
            SharedObjectRelease(mContent)
        }
    }
    
    func setFormat(pixel : ePixelFormat, width : Int32, height : Int32) -> Void {
        mImageFormat.format     = pixel
        mImageFormat.width      = width
        mImageFormat.height     = height
        mImageFormat.rect.x     = 0
        mImageFormat.rect.y     = 0
        mImageFormat.rect.w     = width
        mImageFormat.rect.h     = height
        
        if (mContent != nil) {
            BufferObjectResetBytes(mContent!)
        }
    }
    
    func setRect(x : Int32, y : Int32, w : Int32, h : Int32) -> Void {
        mImageFormat.rect.x     = x
        mImageFormat.rect.y     = y
        mImageFormat.rect.w     = w
        mImageFormat.rect.h     = h
    }
    
    var frameBytes : Int {
        let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(mImageFormat.format);
        return (Int(mImageFormat.width * mImageFormat.height) * desc.pointee.bpp) / 8;
    }
    
    var totalBytes : Int64 {
        guard mContent != nil else {
            return 0
        }
        return BufferObjectGetCapacity(mContent)
    }
    
    var imageFormat : ImageFormat {
        return mImageFormat
    }
    
    func readFrame() -> MediaFrameRef? {
        guard mContent != nil else {
            return nil
        }
        
        let bytes = frameBytes
        let data = BufferObjectReadBytes(mContent, bytes);
        if (BufferObjectGetDataLength(data) < bytes) {
            NSLog("no enough data")
            return nil
        }
        
        let image = MediaFrameCreateWithImageBuffer(&mImageFormat, data)
        SharedObjectRelease(data)
        return image
    }
    
    func readFrame(index : Int) -> MediaFrameRef? {
        guard mContent != nil else {
            return nil
        }
        
        let bytes = frameBytes
        BufferObjectResetBytes(mContent)
        BufferObjectSkipBytes(mContent, Int64((index - 1) * bytes))
        
        let data = BufferObjectReadBytes(mContent, bytes)
        guard data != nil else {
            NSLog("no more data")
            return nil
        }
        
        if (BufferObjectGetDataLength(data) < bytes) {
            NSLog("no enough data or eos")
            SharedObjectRelease(data)
            return nil
        }
        
        let image = MediaFrameCreateWithImageBuffer(&mImageFormat, data)
        SharedObjectRelease(data)
        return image
    }
}
