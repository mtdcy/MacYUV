//
//  ViewController.swift
//  YUVViewer
//
//  Created by Chen Fang on 2019/4/14.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    
    @IBOutlet weak var openGLView: NSOpenGLView!
    @IBOutlet weak var mItemView: NSPopUpButton!
    
    @IBOutlet weak var mWidth: NSTextField!
    @IBOutlet weak var mHeight: NSTextField!
    @IBOutlet weak var mLeft: NSTextField!
    @IBOutlet weak var mTop: NSTextField!
    @IBOutlet weak var mWidthDisplay: NSTextField!
    @IBOutlet weak var mHeightDisplay: NSTextField!
    
    @IBOutlet weak var mRectCheck: NSButton!
    
    @IBOutlet weak var mPropertyView: NSBox!
    
    let mYUVs : [ePixelFormat] = [
        kPixelFormatYUV420P,
        kPixelFormatYUV422P,
        kPixelFormatYUV444P,
        kPixelFormatNV12,
        kPixelFormatNV21
    ]
    
    var mReader : YUVReader = YUVReader()
    var mMediaOut : MediaOutRef?

    override func viewDidLoad() {
        super.viewDidLoad()

        // setup OpenGL Context
        NSLog("OpenGLContext: %@", openGLView.openGLContext!)
        openGLView.prepareOpenGL()
        
        // set default value
        mReader.setFormat(pixel: kPixelFormatNV12, width: 800, height: 480)
        let imageFormat = mReader.imageFormat
        mWidth.intValue = imageFormat.pointee.width
        mHeight.intValue = imageFormat.pointee.height
        mLeft.intValue = imageFormat.pointee.rect.x
        mTop.intValue = imageFormat.pointee.rect.y
        mWidthDisplay.intValue = imageFormat.pointee.rect.w
        mHeightDisplay.intValue = imageFormat.pointee.rect.h
        
        mItemView.removeAllItems()
        for yuv in mYUVs {
            let string = GetPixelFormatString(yuv)!
            mItemView.addItem(withTitle: String.init(cString: string))
        }
        
        mPropertyView.isHidden = true
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    
    func drawImage() {
        let pixel = mYUVs[mItemView.indexOfSelectedItem]
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
        }
        
        MediaOutWrite(mMediaOut, image)
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
    
    @IBAction func onFormatChanged(_ sender: Any) {
        NSLog("onFormatChanged")
        drawImage()
    }
    
    func validateRect(width : Int32, height : Int32) {
        if (mRectCheck.state == NSControl.StateValue.on) {
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
        print("onRectCheck ", mRectCheck.state)
        
        if (mRectCheck.state == NSControl.StateValue.on) {
            mTop.isEnabled = true
            mLeft.isEnabled = true
            mWidthDisplay.isEnabled = true
            mHeightDisplay.isEnabled = true
        } else {
            mTop.isEnabled = false
            mLeft.isEnabled = false
            mWidthDisplay.isEnabled = false
            mHeightDisplay.isEnabled = false
        }
    }
}

