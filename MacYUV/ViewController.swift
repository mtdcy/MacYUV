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
    
    @IBOutlet weak var mOpenGLView: NSOpenGLView!
    
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
        kPixelFormatYUYV422,
        kPixelFormatYVYU422,
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
    
    var yuvFormat : ePixelFormat {
        get {
            if (isYUV == false) {
                return kPixelFormatUnknown
            }
            return YUVs[mYUVItems.indexOfSelectedItem]
        }
        set {
            guard newValue != kPixelFormatUnknown else {
                return;
            }
            isRGB = false
            isYUV = true
            let index = YUVs.firstIndex(of: newValue)
            mYUVItems.selectItem(at: index!)
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
        kPixelFormatBGR565,
        kPixelFormatRGB,
        kPixelFormatBGR,
        kPixelFormatARGB,
        kPixelFormatBGRA,
        kPixelFormatRGBA,
        kPixelFormatABGR,
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
    
    var rgbFormat : ePixelFormat {
        get {
            if (isRGB == false) {
                return kPixelFormatUnknown
            }
            return RGBs[mRGBItems.indexOfSelectedItem]
        }
        set {
            guard newValue != kPixelFormatUnknown else {
                return;
            }
            isYUV = false
            isRGB = true
            let index = RGBs.firstIndex(of: newValue)
            mRGBItems.selectItem(at: index!)
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
    
    @IBOutlet weak var mInfoText: NSTextField!
    
    let Resolutions = [
        ("Custom",  1280,   720),       // default value for custom
        ("QCIF",    176,    144),
        ("QVGA",    320,    240),
        ("HVGA",    480,    320),
        ("VGA",     640,    480),
        ("480p",    720,    480),
        ("576p",    720,    576),
        ("PAL",     768,    576),
        ("WVGA",    800,    480),
        ("SVGA",    800,    600),
        ("XGA",     1024,   768),
        ("720p",    1280,   720),   // HD
        ("WXGA",    1280,   800),
        ("SXGA",    1280,   1024),
        ("1080p",   1920,   1080),  // FHD
        ("WUXGA",   1920,   1200),
        ("2K",      2560,   1440),  // QHD
        ("WQXGA",   2560,   1600),
        ("4K UHD",  3840,   2160),  // UHD
        ("4K",      4096,   2160),
        ("5K",      5120,   2880),
    ]
    
    // -> (RESOLUTION, [YUV, RGB])
    // not always work
    func luckyGuess(size : Int) -> ((String, Int, Int), [ePixelFormat]) {
        // from large to small
        for res in Resolutions.reversed() {
            let plane0 = res.1 * res.2
            if (plane0 * 2 == size) {   // 16 bpp
                return (res, [kPixelFormatYUV422P, kPixelFormatBGR565])
            } else if (plane0 * 3 == size) {    // 24 bpp
                return (res, [kPixelFormatYUV444P, kPixelFormatBGR])
            } else if (plane0 * 4 == size) {    // 32 bpp
                return (res, [kPixelFormatUnknown, kPixelFormatBGRA])
            } else if ((plane0 * 3) / 2 == size) {  // 12 bpp
                return (res, [kPixelFormatYUV420P, kPixelFormatUnknown])
            }
        }
        // return custom
        return (Resolutions.first!, [kPixelFormatUnknown, kPixelFormatUnknown])
    }
    
    var mReader : YUVReader = YUVReader()
    var mMediaOut : MediaOutRef?

    class OnlyNumberFormatter : NumberFormatter {
        override func isPartialStringValid(_ partialString: String, newEditingString newString: AutoreleasingUnsafeMutablePointer<NSString?>?, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
            if partialString.isEmpty {
                return true
            }
            return Int(partialString) != nil
        }
    }
    let formatter = OnlyNumberFormatter()
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // setup OpenGL Context
        NSLog("OpenGLContext: %@", mOpenGLView.openGLContext!)
        mOpenGLView.prepareOpenGL()
        
        mWidth.formatter = formatter
        mHeight.formatter = formatter
        mLeft.formatter = formatter
        mTop.formatter = formatter
        mWidthDisplay.formatter = formatter
        mHeightDisplay.formatter = formatter
        
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
        mInfoText.isHidden = true
        isYUV = true
        isRGB = false
        isRectEnabled = false
    }
    
    override func viewWillDisappear() {
        closeFile()
        
        NSLog("View Will Disappear")
        super.viewWillDisappear()
        
        NSApplication.shared.terminate(self)
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func drawImage() {
        var pixel = yuvFormat
        if (isRGB) {
            pixel = rgbFormat
        }
        
        let width = mWidth.intValue
        let height = mHeight.intValue
        validateRect(width: width, height: height)
        
        let x = mLeft.intValue
        let y = mTop.intValue
        let w = mWidthDisplay.intValue
        let h = mHeightDisplay.intValue
        
        mReader.setFormat(pixel: pixel, width: width, height: height)
        mReader.setRect(x: x, y: y, w: w, h: h)
        
        var desc = String.init(format: "%d/%d bytes => pixel %s: %d x %d [%d, %d, %d, %d]",
                            mReader.frameBytes, mReader.totalBytes,
                            GetPixelFormatString(pixel), width, height, x, y, w, h)
        
        
        let image = mReader.readFrame()
        guard image != nil else {
            NSLog("read image failed")
            desc += "\n >> bad pixel format"
            mInfoText.stringValue = desc
            return
        }
        
        // get a pointer to image format struct
        let imageFormat : UnsafeMutablePointer<ImageFormat> = MediaFrameGetImageFormat(image)
        desc += "\n"
        
        // reverse bytes goes before others
        if (isReverseBytes) {
            if (ImageFrameReversePixel(image) == kMediaNoError) {
                desc += " >> reverse pixel."
            } else {
                NSLog("reverse bytes failed")
                isReverseBytes = false
            }
        }
        
        // always planarization for packed yuv
        // MediaOut not support packed yuv well
        if (isYUV) {
            // planarization
            if (GetPixelFormatIsPlanar(imageFormat.pointee.format) == false && ImageFramePlanarization(image) == kMediaNoError) {
                desc += String.init(format : " >> %s", GetPixelFormatString(imageFormat.pointee.format));
            }
            
            // swap uv chroma
            if (isUVSwap) {
                if (ImageFrameSwapUVChroma(image) == kMediaNoError) {
                    desc += " >> swap u/v."
                } else {
                    NSLog("swap UV chroma failed")
                    isUVSwap = false
                }
            }
            
            // yuv2rgb
            if (ImageFrameToRGB(image) == kMediaNoError) {
                desc += String.init(format : " >> %s.", GetPixelFormatString(imageFormat.pointee.format))
            } else {
                NSLog("yuv2rgb failed")
            }
        }
        
        if (mMediaOut == nil) {
            mMediaOut = MediaOutCreateForImage(MediaFrameGetImageFormat(image), nil)
        }
        
        guard mMediaOut != nil else {
            NSLog("create MediaOut failed")
            desc += " >> open device failed."
            mInfoText.stringValue = desc
            return;
        }
        
        MediaOutWrite(mMediaOut, image)
        SharedObjectRelease(image)
        
        NSLog("draw ==> %@", desc)
        mInfoText.stringValue = desc
    }
    
    public func openFile(url : String) {
        NSLog("url = %@", url)
        closeFile()
        
        mPropertyView.isHidden = false
        mInfoText.isHidden = false
        
        if (mReader.open(url: url) == true) {
            NSLog("total bytes %d", mReader.totalBytes)
            let lucky = luckyGuess(size: mReader.totalBytes)
            
            print("lucky => ", lucky)
            
            mWidth.intValue     = Int32(lucky.0.1)
            mHeight.intValue    = Int32(lucky.0.2)
            validateRect(width: mWidth.intValue, height: mHeight.intValue)
            
            // set both yuv & rgb lucky guess
            if (lucky.1[1] != kPixelFormatUnknown) {
                rgbFormat = lucky.1[1]
            }
            if (lucky.1[0] != kPixelFormatUnknown) {
                yuvFormat = lucky.1[0]
            }
            
            drawImage()
        }
    }
    
    func openFile() {
        NSLog("Open File...")
        
        let url = NSDocumentController.shared.urlsFromRunningOpenPanel()
        if (url != nil) {
            openFile(url: url![0].path)
        } else {
            NSLog("Cancel Open File...")
        }
    }
    
    func closeFile() {
        if (mMediaOut != nil) {
            MediaOutFlush(mMediaOut)
            SharedObjectRelease(mMediaOut)
            mMediaOut = nil
        }
        
        mReader.close()
        mInfoText.stringValue = ""
    }

    @IBAction func open(sender : Any?) {
        openFile()
    }
    
    @IBAction func close(sender : Any?) {
        self.view.window?.performClose(nil)
    }
    
    // move window by background
    override func mouseDragged(with event: NSEvent) {
        self.view.window?.performDrag(with: event)
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
            mInfoText.isHidden = false
        } else {
            mPropertyView.isHidden = true
            mInfoText.isHidden = true
        }
    }
}

