//
//  ViewController.swift
//  YUVViewer
//
//  Created by Chen Fang on 2019/4/14.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Cocoa

// https://www.darktable.org/2017/01/rawsamples-ch-replacement/
class ViewController: NSViewController {
    
    @IBOutlet weak var openGLView: NSOpenGLView!
    
    @IBOutlet weak var mWidth: NSTextField!
    @IBOutlet weak var mHeight: NSTextField!
    @IBOutlet weak var mLeft: NSTextField!
    @IBOutlet weak var mTop: NSTextField!
    @IBOutlet weak var mWidthDisplay: NSTextField!
    @IBOutlet weak var mHeightDisplay: NSTextField!
    
    @IBOutlet weak var mRectCheck: NSButton!
    var isRectEnabled : Bool {
        get {
            return mRectCheck.state == NSControl.StateValue.on
        }
        set {
            mRectCheck.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            mTop.isEnabled              = newValue
            mLeft.isEnabled             = newValue
            mWidthDisplay.isEnabled     = newValue
            mHeightDisplay.isEnabled    = newValue
        }
    }
    
    @IBOutlet weak var mPropertyView: NSBox!
    
    // yuv formats
    @IBOutlet weak var mYUVCheck: NSButton!
    @IBOutlet weak var mYUVItems: NSPopUpButton!
    @IBOutlet weak var mUVSwap: NSButton!
    @IBOutlet weak var mUVInterlaced: NSButton!
    
    let YUVs : [ePixelFormat] = [
        kPixelFormatYUV420P,
        kPixelFormatYUV422P,
        kPixelFormatYUV444P,
        kPixelFormatNV12,
        kPixelFormatNV21,
        //kPixelFormatYUYV422,
        kPixelFormatYUV444,
    ]
    
    var isYUV : Bool {
        get {
            return mYUVCheck.state == NSControl.StateValue.on
        }
        set {
            mYUVCheck.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            mYUVItems.isEnabled = newValue
            mUVSwap.isEnabled = newValue
            //mUVInterlaced.isEnabled = newValue
            mUVInterlaced.isHidden = true
        }
    }
    
    var isUVSwap : Bool {
        get {
            return mUVSwap.state == NSControl.StateValue.on
        }
        set {
            mUVSwap.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
        }
    }
    
    var isUVInterlaced : Bool {
        return mUVInterlaced.state == NSControl.StateValue.on
    }
    
    // rgb formats
    @IBOutlet weak var mRGBCheck: NSButton!
    @IBOutlet weak var mRGBItems: NSPopUpButton!
    
    let RGBs = [
        kPixelFormatRGB565,
        kPixelFormatRGB888,
        kPixelFormatRGBA,
        kPixelFormatARGB,
    ]
    
    var isRGB : Bool {
        get {
            return mRGBCheck.state == NSControl.StateValue.on
        }
        set {
            mRGBCheck.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            mRGBItems.isEnabled = newValue
        }
    }
    
    // both yuv & rgb
    @IBOutlet weak var mReverseBytes : NSButton!
    
    var isReverseBytes : Bool {
        get {
            return mReverseBytes.state == NSControl.StateValue.on
        }
        set {
            mReverseBytes.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
        }
    }
    
    var mReader : YUVReader = YUVReader()
    var mMediaOut : MediaOutRef?

    override func viewDidLoad() {
        super.viewDidLoad()

        // setup OpenGL Context
        NSLog("OpenGLContext: %@", openGLView.openGLContext!)
        openGLView.prepareOpenGL()
        
        // set default value
        mReader.setFormat(pixel: kPixelFormatNV12, width: 176, height: 144)
        let imageFormat = mReader.imageFormat
        mWidth.intValue = imageFormat.pointee.width
        mHeight.intValue = imageFormat.pointee.height
        mLeft.intValue = imageFormat.pointee.rect.x
        mTop.intValue = imageFormat.pointee.rect.y
        mWidthDisplay.intValue = imageFormat.pointee.rect.w
        mHeightDisplay.intValue = imageFormat.pointee.rect.h
        
        mYUVItems.removeAllItems()
        for yuv in YUVs {
            let string = GetPixelFormatString(yuv)!
            mYUVItems.addItem(withTitle: String.init(cString: string))
        }
        mRGBItems.removeAllItems()
        for rgb in RGBs {
            let string = GetPixelFormatString(rgb)!
            mRGBItems.addItem(withTitle: String.init(cString: string))
        }

        mPropertyView.isHidden = true
        isYUV = true
        isRGB = false
        isRectEnabled = false
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func drawImage() {
        var pixel = kPixelFormatUnknown;
        if (isYUV) {
            pixel = YUVs[mYUVItems.indexOfSelectedItem]
        } else if (isRGB) {
            pixel = RGBs[mRGBItems.indexOfSelectedItem]
        }
        
        let width = mWidth.intValue
        let height = mHeight.intValue
        validateRect(width: width, height: height)
        
        let x = mLeft.intValue
        let y = mTop.intValue
        let w = mWidthDisplay.intValue
        let h = mHeightDisplay.intValue
        
        NSLog("pixel %s: %d x %d [%d, %d, %d, %d]",
              GetPixelFormatString(pixel),
              width, height, x, y, w, h)
        mReader.setFormat(pixel: pixel, width: width, height: height)
        mReader.setRect(x: x, y: y, w: w, h: h)
        
        let image = mReader.readFrame()
        
        guard image != nil else {
            NSLog("generate image failed")
            return
        }
        
        if (mMediaOut == nil) {
            mMediaOut = MediaOutCreateForImage(mReader.imageFormat, nil)
            assert(mMediaOut != nil)
            // TODO: using ColorConvertor if format is not accept by MediaOut
        }
        
        if (isYUV && isUVSwap) {
            if (ImageFrameSwapUVChroma(image) != kMediaNoError) {
                NSLog("swap UV chroma failed")
                isUVSwap = false
            }
        }
        
        if (isReverseBytes) {
            if (ImageFrameReverseBytes(image) != kMediaNoError) {
                NSLog("reverse bytes failed")
                isReverseBytes = false
            }
        }
        
        MediaOutWrite(mMediaOut, image)
        NSLog("image ref %d", SharedObjectGetRetainCount(image))
        SharedObjectRelease(image)
    }
    
    public func openFile(url : String) {
        NSLog("url = %@", url)
        
        mPropertyView.isHidden = false
        
        if (mReader.open(url: url) == true) {
            drawImage()
        }
    }
    
    func openFile() {
        NSLog("Open File...")
        closeFile()
        
        let url = NSDocumentController.shared.urlsFromRunningOpenPanel()
        if (url != nil) {
            openFile(url: url![0].path)
        } else {
            NSLog("Cancel Open File...")
        }
    }
    
    func closeFile() {
        if (mMediaOut != nil) {
            SharedObjectRelease(mMediaOut)
            mMediaOut = nil
        }
        
        mReader.close()
    }

    @IBAction func open(sender : Any?) {
        close(sender: nil)
        openFile()
    }
    
    @IBAction func close(sender : Any?) {
        
    }
    
    @IBAction func onFormatChanged(_ sender: Any?) {
        NSLog("onFormatChanged")
        // format changed, release MediaOut
        if (mMediaOut != nil) {
            SharedObjectRelease(mMediaOut)
            mMediaOut = nil
        }
        drawImage()
    }
    
    func validateRect(width : Int32, height : Int32) {
        if (isRectEnabled == true) {
            // just valid the values
            if (mTop.intValue > height) {
                mTop.intValue = height
            }
            if (mLeft.intValue > width) {
                mLeft.intValue = width
            }
            if (mWidthDisplay.intValue > width) {
                mWidthDisplay.intValue = width
            }
            if (mHeightDisplay.intValue > height) {
                mHeightDisplay.intValue = height
            }
        } else {
            mTop.intValue   = 0
            mLeft.intValue  = 0
            mWidthDisplay.intValue  = width
            mHeightDisplay.intValue = height
        }
    }
    
    @IBAction func onRectCheck(_ sender: Any) {
        print("onRectCheck <- ", sender)
        isRectEnabled = mRectCheck.state == NSControl.StateValue.on
        onFormatChanged(nil)
    }
    
    @IBAction func onYUVCheck(_ sender: Any) {
        print("onYUVCheck <- ", sender as Any)
        isYUV = mYUVCheck.state == NSControl.StateValue.on
        isRGB = !isYUV
        onFormatChanged(nil)
    }
    
    @IBAction func onRGBCheck(_ sender: Any) {
        print("onRGBCheck <- ", sender as Any)
        isRGB = mRGBCheck.state == NSControl.StateValue.on
        isYUV = !isRGB
        onFormatChanged(nil)
    }
    
    override func mouseDown(with event: NSEvent) {
        
        guard mPropertyView.hitTest(event.locationInWindow) == nil else {
            NSLog("mouse hit property view")
            return
        }
        
        if (mPropertyView.isHidden == true) {
            mPropertyView.isHidden = false
        } else {
            mPropertyView.isHidden = true
        }
    }
}

