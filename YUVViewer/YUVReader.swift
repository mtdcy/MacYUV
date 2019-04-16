//
//  YUVReader.swift
//  YUV
//
//  Created by Chen Fang on 2019/4/16.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Foundation

class YUVReader : NSObject {
    
    var mContent : ContentObjectRef?
    var mImageFormat = UnsafeMutablePointer<ImageFormat>.allocate(capacity: 1)
    
    override init() {
        
    }
    
    func open(url : String) -> Bool {
        close()
        
        mContent = ContentObjectCreate(url)
        
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
        mImageFormat.pointee.format     = pixel
        mImageFormat.pointee.width      = width
        mImageFormat.pointee.height     = height
        mImageFormat.pointee.rect.x     = 0
        mImageFormat.pointee.rect.y     = 0
        mImageFormat.pointee.rect.w     = width
        mImageFormat.pointee.rect.h     = height
        
        if (mContent != nil) {
            ContentObjectReset(mContent!)
        }
    }
    
    func setRect(x : Int32, y : Int32, w : Int32, h : Int32) -> Void {
        mImageFormat.pointee.rect.x     = x
        mImageFormat.pointee.rect.y     = y
        mImageFormat.pointee.rect.w     = w
        mImageFormat.pointee.rect.h     = h
    }
    
    var frameBytes : Int {
        return GetImageFormatBytes(mImageFormat)
    }
    
    var totalBytes : Int {
        guard mContent != nil else {
            return 0
        }
        return ContentObjectLength(mContent)
    }
    
    var imageFormat : UnsafeMutablePointer<ImageFormat> {
        return mImageFormat
    }
    
    func readFrame() -> MediaFrameRef? {
        guard mContent != nil else {
            return nil
        }
        
        let bytes = frameBytes
        let data = ContentObjectRead(mContent, bytes)
        let image = ImageFrameGenerate(mImageFormat, data)
        return image
    }
}
