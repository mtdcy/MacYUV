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
        kPixelFormatRGB16,
        kPixelFormatRGB24,
        kPixelFormatRGB32,
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
    func luckyGuess(size : Int64) -> ((String, Int, Int), [ePixelFormat])? {
        // match with current format first
        var plane0 : Int64 = Int64(mWidth.intValue * mHeight.intValue);
        if (isYUV) {
            let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(yuvFormat)
            plane0 = (plane0 * Int64(desc.pointee.bpp)) / 8
        } else {
            let desc : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(rgbFormat)
            plane0 = (plane0 * Int64(desc.pointee.bpp)) / 8
        }
        
        if (size % plane0 == 0) {
            return nil
        }
        
        // from large to small
        for res in Resolutions.reversed() {
            let plane0 = res.1 * res.2
            if (plane0 * 4 == size) {    // 32 bpp
                return (res, [kPixelFormatUnknown, kPixelFormatRGB32])
            } else if (plane0 * 3 == size) {    // 24 bpp
                return (res, [kPixelFormat444YpCbCrPlanar, kPixelFormatRGB24])
            } else if (plane0 * 2 == size) {   // 16 bpp
                return (res, [kPixelFormat422YpCbCrPlanar, kPixelFormatRGB16])
            } else if ((plane0 * 3) / 2 == size) {  // 12 bpp
                return (res, [kPixelFormat420YpCbCrPlanar, kPixelFormatUnknown])
            }
        }
        // return custom
        return nil
    }
    
    var mReader : YUVReader = YUVReader()

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
        let imageFormat         = mReader.imageFormat
        
        mWidth.intValue         = imageFormat.width
        mHeight.intValue        = imageFormat.height
        mLeft.intValue          = imageFormat.rect.x
        mTop.intValue           = imageFormat.rect.y
        mWidthDisplay.intValue  = imageFormat.rect.w
        mHeightDisplay.intValue = imageFormat.rect.h
        
        mYUVItems.removeAllItems()
        for yuv in YUVs {
            let desc = GetPixelFormatDescriptor(yuv)
            mYUVItems.addItem(withTitle: String.init(cString: desc!.pointee.name))
        }
        mRGBItems.removeAllItems()
        for rgb in RGBs {
            let desc = GetPixelFormatDescriptor(rgb)
            mRGBItems.addItem(withTitle: String.init(cString: desc!.pointee.name))
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
        return desc!.pointee.nb_planes > 1
    }
    
    func draw() {
        var pixel = yuvFormat
        if (isRGB) {
            pixel = rgbFormat
        }
        
        let width   = mWidth.intValue
        let height  = mHeight.intValue
        validateRect(width: width, height: height)
        
        let x       = mLeft.intValue
        let y       = mTop.intValue
        let w       = mWidthDisplay.intValue
        let h       = mHeightDisplay.intValue
        
        mReader.setFormat(pixel: pixel, width: width, height: height)
        mReader.setRect(x: x, y: y, w: w, h: h)
        
        // force window aspect ratio
        self.view.window?.contentAspectRatio = NSSize.init(width: Int(w), height: Int(h))
        
        numFrames = mReader.totalBytes / Int64(mReader.frameBytes)
        
        var info = String.init(format: "%d/%d bytes => pixel %@: %d x %d [%d, %d, %d, %d]",
                            mReader.frameBytes, mReader.totalBytes,
                            pixelName(pixel: pixel), width, height, x, y, w, h)
        
        mInfoText.stringValue = info
        
        let image : MediaFrameRef? = mReader.readFrame()
        guard image != nil else {
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
        let origFormat : UnsafeMutablePointer<ImageFormat> = MediaFrameGetImageFormat(image)
        var status : String = "pixel: "
        
        var inputFormat     = ImageFormat.init()
        var outputFormat    = ImageFormat.init()
        
        inputFormat.format  = origFormat.pointee.format
        inputFormat.width   = origFormat.pointee.width
        inputFormat.height  = origFormat.pointee.height
        inputFormat.rect    = origFormat.pointee.rect
        
        outputFormat        = inputFormat
        outputFormat.format = isYUV2RGB ? kPixelFormatRGB32 : inputFormat.format
            
        // swap uv chroma
        if (isYUV && isUVSwap) {
            let descriptor  = GetPixelFormatDescriptor(inputFormat.format)
            if (descriptor!.pointee.similar.1 != kPixelFormatUnknown) {
                inputFormat.format = descriptor!.pointee.similar.1
            }
        }
        
        if (isWordOrder) {
            let descriptor = GetPixelFormatDescriptor(inputFormat.format)
            if (descriptor!.pointee.similar.2 != kPixelFormatUnknown) {
                inputFormat.format = descriptor!.pointee.similar.2;
            }
        }
        
        let descriptor = GetPixelFormatDescriptor(inputFormat.format)
        status += String.init(cString: descriptor!.pointee.name)
        if (origFormat.pointee.format != inputFormat.format) {
            let descriptor = GetPixelFormatDescriptor(origFormat.pointee.format)
            status += " >> " + String.init(cString: descriptor!.pointee.name)
        }
        
        if (outputFormat.format != origFormat.pointee.format) {
            let descriptor = GetPixelFormatDescriptor(outputFormat.format)
            status += " >> " + String.init(cString: descriptor!.pointee.name)
        }
        
        var output = SharedObjectRetain(image);
        if (inputFormat.format != outputFormat.format) {
            let cc : MediaDeviceRef? = ColorConverterCreate(&inputFormat, &outputFormat, nil)
            guard cc != nil else {
                status = "create color converter failed."
                return status;
            }
            
            MediaDevicePush(cc, output)
            SharedObjectRelease(output)
            output = MediaDevicePull(cc)
            SharedObjectRelease(cc)
            
            guard output != nil else {
                status = "color convert failed."
                return status
            }
        }
    
        let formats : MessageObjectRef = MessageObjectCreate()
        MessageObjectPutInt32(formats, kKeyFormat,  Int32(outputFormat.format))
        MessageObjectPutInt32(formats, kKeyWidth,   outputFormat.width)
        MessageObjectPutInt32(formats, kKeyHeight,  outputFormat.height)
        
        let options : MessageObjectRef = MessageObjectCreate()
        let openGLContext = mOpenGLView.openGLContext?.cglContextObj
        MessageObjectPutPointer(options, kKeyOpenGLContext, openGLContext)
        
        let device = MediaDeviceCreate(formats, options)
        SharedObjectRelease(formats)
        SharedObjectRelease(options)
        
        guard device != nil else {
            status = "create media device failed."
            return status;
        }
        
        MediaDevicePush(device, output)
        SharedObjectRelease(output)
        SharedObjectRelease(device)
        
        NSLog("draw ==> %@", status)
        SharedObjectRelease(image)
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
        mReader.close()
        mInfoText.stringValue = ""
    }

    @IBAction func open(sender : Any?) {
        openFile()
    }
    
    @IBAction func close(sender : Any?) {
        //self.view.window?.performClose(nil)
        NSApplication.shared.terminate(self)
    }
    
    // move window by background
    override func mouseDragged(with event: NSEvent) {
        self.view.window?.performDrag(with: event)
    }
    
    @IBAction func onFormatChanged(_ sender: Any?) {
        NSLog("onFormatChanged")
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
            if (mWidthDisplay.intValue > width - mLeft.intValue) {
                mWidthDisplay.intValue = width - mLeft.intValue
            }
            if (mHeightDisplay.intValue > height - mTop.intValue) {
                mHeightDisplay.intValue = height - mTop.intValue;
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
            mPropertyView.isHidden = newValue
            mInfoText.isHidden = newValue
        }
    }
    
    override func mouseDown(with event: NSEvent) {
        if (event.clickCount > 1) {
            self.view.window?.toggleFullScreen(self);
            draw()
            return
        }
        
        guard mPropertyView.hitTest(event.locationInWindow) == nil else {
            NSLog("mouse hit property view")
            return
        }
        
        isUIHidden = !isUIHidden
    }
    
    @IBOutlet weak var mFrameSlider: NSSlider!
    
    var numFrames : Int64 {
        get {
            return Int64(mFrameSlider.maxValue)
        }
        set {
            mFrameSlider.numberOfTickMarks = Int(newValue)
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
        
        let index = mFrameSlider.intValue
        let image : MediaFrameRef? = mReader.readFrame(index: Int(index))
        guard image != nil else {
            NSLog("read frame failed or eos")
            return
        }
        
        drawImage(image: image!)
    }
    
    override func keyDown(with event: NSEvent) {
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

