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
    @IBOutlet weak var mYUV2RGB: NSButton!
    
    let YUVs : [ePixelFormat] = [
        // common pixels
        kPixelFormat420YpCbCrPlanar,
        kPixelFormat422YpCbCrPlanar,
        kPixelFormat444YpCbCrPlanar,
        kPixelFormat422YpCbCr,
        kPixelFormat444YpCbCr,
        kPixelFormat420YpCbCrSemiPlanar,
    ]
    
    var isYUV : Bool {
        get {
            return mYUVCheck.state == NSControl.StateValue.on
        }
        set {
            mYUVCheck.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            mYUVItems.isEnabled = newValue
            mUVSwap.isEnabled = newValue
            mYUV2RGB.isEnabled = newValue
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
    
    var isYUV2RGB : Bool {
        get {
            return mYUV2RGB.state == NSControl.StateValue.on
        }
        set {
            mYUV2RGB.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
        }
    }
    
    // rgb formats
    @IBOutlet weak var mRGBCheck: NSButton!
    @IBOutlet weak var mRGBItems: NSPopUpButton!
    
    let RGBs = [
        kPixelFormatRGB565,
        kPixelFormatBGR565,
        kPixelFormatRGB,
        kPixelFormatARGB,
        kPixelFormatRGBA,
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
    
    var isWordOrder : Bool {
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
    func luckyGuess(size : Int) -> ((String, Int, Int), [ePixelFormat])? {
        // match with current format first
        var plane0 : Int = Int(mWidth.intValue * mHeight.intValue);
        if (isYUV) {
            let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(yuvFormat)
            plane0 = (plane0 * desc.pointee.bpp) / 8
        } else {
            let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(rgbFormat)
            plane0 = (plane0 * desc.pointee.bpp) / 8
        }
        
        if (size % plane0 == 0) {
            return nil
        }
        
        // from large to small
        for res in Resolutions.reversed() {
            let plane0 = res.1 * res.2
            if (plane0 * 2 == size) {   // 16 bpp
                return (res, [kPixelFormat422YpCbCrPlanar, kPixelFormatRGB565])
            } else if (plane0 * 3 == size) {    // 24 bpp
                return (res, [kPixelFormat444YpCbCrPlanar, kPixelFormatRGB])
            } else if (plane0 * 4 == size) {    // 32 bpp
                return (res, [kPixelFormatUnknown, kPixelFormatARGB])
            } else if ((plane0 * 3) / 2 == size) {  // 12 bpp
                return (res, [kPixelFormat420YpCbCrPlanar, kPixelFormatUnknown])
            }
        }
        // return custom
        return nil
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
        mReader.setFormat(pixel: kPixelFormat422YpCbCrPlanar, width: 176, height: 144)
        let imageFormat = mReader.imageFormat
        mWidth.intValue = imageFormat.pointee.width
        mHeight.intValue = imageFormat.pointee.height
        mLeft.intValue = imageFormat.pointee.rect.x
        mTop.intValue = imageFormat.pointee.rect.y
        mWidthDisplay.intValue = imageFormat.pointee.rect.w
        mHeightDisplay.intValue = imageFormat.pointee.rect.h
        
        mYUVItems.removeAllItems()
        for yuv in YUVs {
            let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(yuv)
            mYUVItems.addItem(withTitle: String.init(cString: desc.pointee.name))
        }
        mRGBItems.removeAllItems()
        for rgb in RGBs {
            let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(rgb)
            mRGBItems.addItem(withTitle: String.init(cString: desc.pointee.name))
        }

        isYUV = true
        isRGB = false
        isRectEnabled = false
        isUIHidden = false
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
    
    func pixelName(pixel : ePixelFormat) -> String {
        let desc : UnsafePointer<PixelDescriptor>? = GetPixelFormatDescriptor(pixel)
        guard desc != nil else {
            return "unknown"
        }
        return String.init(cString: desc!.pointee.name)
    }
    
    func pixelIsPlanar(pixel : ePixelFormat) -> Bool {
        let desc : UnsafePointer<PixelDescriptor>? = GetPixelFormatDescriptor(pixel)
        guard desc != nil else {
            return false
        }
        return desc!.pointee.planes > 1
    }
    
    func draw() {
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
        
        // force window aspect ratio
        self.view.window?.contentAspectRatio = NSSize.init(width: Int(width), height: Int(height))
        
        numFrames = mReader.totalBytes / mReader.frameBytes
        
        var info = String.init(format: "%d/%d bytes => pixel %@: %d x %d [%d, %d, %d, %d]",
                            mReader.frameBytes, mReader.totalBytes,
                            pixelName(pixel: pixel), width, height, x, y, w, h)
        
        mInfoText.stringValue = info
        
        let image : MediaFrameRef? = mReader.readFrame()
        guard image != nil else {
            if (mMediaOut != nil) {
                MediaOutFlush(mMediaOut)
            }
            NSLog("read image failed")
            info += "\n >> bad pixel format"
            mInfoText.stringValue = info
            return
        }
        
        let status = drawImage(image: image!)
        
        mInfoText.stringValue += "\n"
        mInfoText.stringValue += status
    }
    
    func drawImage(image : MediaFrameRef) -> String {
        // get a pointer to image format struct
        let imageFormat : UnsafeMutablePointer<ImageFormat> = MediaFrameGetImageFormat(image)
        var status : String = String()
        
        // bytes-order vs word-order
        if (isWordOrder) {
            if (ImageFrameReversePixel(image) == kMediaNoError) {
                status += " >> word-order."
            } else {
                NSLog("reverse bytes failed")
                isWordOrder = false
            }
        }
        
        if (isWordOrder == false) {
            status += " >> byte-roder."
        }
        
        if (isYUV) {
            if (!pixelIsPlanar(pixel: imageFormat.pointee.format)) {
                var planarization = isUVSwap || isYUV2RGB;
                // MediaOut don't support 422 packed, fix later
                if (imageFormat.pointee.format == kPixelFormat422YpCbCr ||
                    imageFormat.pointee.format == kPixelFormat422YpCrCb) {
                    planarization = true
                }
                
                if (planarization) {
                    if (ImageFramePlanarization(image) == kMediaNoError) {
                        status += String.init(format : " >> %@.", pixelName(pixel: imageFormat.pointee.format));
                    } else {
                        NSLog("planarization %@ failed", pixelName(pixel: imageFormat.pointee.format))
                    }
                }
            }
            
            // swap uv chroma
            if (isUVSwap) {
                if (ImageFrameSwapCbCr(image) == kMediaNoError) {
                    status += " >> swap u/v."
                } else {
                    NSLog("swap UV chroma failed")
                    isUVSwap = false
                }
            }
            
            // yuv2rgb
            if (isYUV2RGB) {
                if (ImageFrameToRGB(image) == kMediaNoError) {
                    status += String.init(format : " >> %@.", pixelName(pixel: imageFormat.pointee.format))
                } else {
                    NSLog("yuv2rgb failed")
                    isYUV2RGB = false
                }
            }
        }
        
        if (mMediaOut == nil) {
            mMediaOut = MediaOutCreateForImage(MediaFrameGetImageFormat(image), nil)
        }
        
        guard mMediaOut != nil else {
            NSLog("create MediaOut failed")
            status += " >> open device failed."
            return status;
        }
        
        MediaOutWrite(mMediaOut, image)
        SharedObjectRelease(image)
        
        NSLog("draw ==> %@", status)
        return status
    }
    
    public func openFile(url : String) {
        NSLog("url = %@", url)
        NSDocumentController.shared.noteNewRecentDocumentURL(URL(fileURLWithPath: url))
        
        closeFile()
        
        isUIHidden = false
        
        if (mReader.open(url: url) == true) {
            NSLog("total bytes %d", mReader.totalBytes)
            let lucky = luckyGuess(size: mReader.totalBytes)
            if (lucky != nil) {
                print("lucky => ", lucky!)
                
                mWidth.intValue     = Int32(lucky!.0.1)
                mHeight.intValue    = Int32(lucky!.0.2)
                validateRect(width: mWidth.intValue, height: mHeight.intValue)
                
                // set both yuv & rgb lucky guess
                if (lucky!.1[1] != kPixelFormatUnknown) {
                    rgbFormat = lucky!.1[1]
                }
                if (lucky!.1[0] != kPixelFormatUnknown) {
                    yuvFormat = lucky!.1[0]
                }
            }
            
            draw()
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
        draw()
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
    
    var isUIHidden : Bool {
        get {
            return mPropertyView.isHidden
        }
        set {
            if (newValue == true) {
                guard mMediaOut != nil else {
                    NSLog("hide ui when draw complete")
                    return
                }
            }
            mPropertyView.isHidden = newValue
            mInfoText.isHidden = newValue
        }
    }
    
    override func mouseDown(with event: NSEvent) {
        
        guard mPropertyView.hitTest(event.locationInWindow) == nil else {
            NSLog("mouse hit property view")
            return
        }
        
        isUIHidden = !isUIHidden
    }
    
    @IBOutlet weak var mFrameSlider: NSSlider!
    
    var numFrames : Int {
        get {
            return Int(mFrameSlider.maxValue)
        }
        set {
            mFrameSlider.numberOfTickMarks = newValue
            mFrameSlider.maxValue = Double(newValue)
            mFrameSlider.intValue = 0
            if (newValue <= 1) {
                mFrameSlider.isHidden = true
            } else {
                mFrameSlider.isHidden = false
            }
        }
    }
    
    @IBAction func onFrameSelect(_ sender: Any) {
        NSLog("select frame %d", mFrameSlider.intValue)
        
        guard mMediaOut != nil else {
            NSLog("MediaOut is not ready")
            return
        }
        
        let index = mFrameSlider.intValue
        let image : MediaFrameRef? = mReader.readFrame(index: Int(index))
        guard image != nil else {
            NSLog("read frame failed or eos")
            return
        }
        
        drawImage(image: image!)
    }
    
    override func keyDown(with event: NSEvent) {
        guard mMediaOut != nil else {
            NSLog("MediaOut is not ready")
            return
        }
        
        var index = mFrameSlider.intValue
        let special = event.specialKey;
        if (special != nil) {
            switch (special) {
            case NSEvent.SpecialKey.rightArrow:
                index += 1;
            case NSEvent.SpecialKey.leftArrow:
                index -= 1;
            default:
                break;
            }
        }
        
        mFrameSlider.intValue = index
        
        
        NSLog("to %d", index)
        let image : MediaFrameRef? = mReader.readFrame(index: Int(index))
        guard image != nil else {
            NSLog("read frame failed or eos")
            return
        }
        
        drawImage(image: image!)
    }
}

