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
    
    @IBOutlet weak var imageView: ImageFrameView!
    
    @IBOutlet weak var widthText: NSTextField!
    @IBOutlet weak var heightText: NSTextField!
    @IBOutlet weak var leftText: NSTextField!
    @IBOutlet weak var topText: NSTextField!
    @IBOutlet weak var displayWidthText: NSTextField!
    @IBOutlet weak var displayHeightText: NSTextField!
    
    // default image format value
    var imageFormat : ImageFormat = ImageFormat.init(format: kPixelFormat420YpCbCrPlanar, width: 1920, height: 1080, rect: ImageRect.init(x: 0, y: 0, w: 1920, h: 1080))
    
    @IBOutlet weak var displayRectButton: NSButton!
    var isRectEnabled : Bool {
        get {
            return displayRectButton.state == NSControl.StateValue.on
        }
        set {
            displayRectButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            topText.isEnabled           = newValue
            leftText.isEnabled          = newValue
            displayWidthText.isEnabled  = newValue
            displayHeightText.isEnabled = newValue
        }
    }
    
    @IBOutlet weak var propertyBox: NSBox!
    
    // yuv formats
    @IBOutlet weak var yuvButton: NSButton!
    @IBOutlet weak var yuvItems: NSPopUpButton!
    @IBOutlet weak var uvButton: NSButton!
    @IBOutlet weak var yuv2rgbButton: NSButton!
    
    let YUVs : [ePixelFormat] = [
        // common pixels
        kPixelFormat420YpCbCrPlanar,
        kPixelFormat420YpCrCbPlanar,
        kPixelFormat422YpCbCrPlanar,
        kPixelFormat422YpCrCbPlanar,
        kPixelFormat444YpCbCrPlanar,
        kPixelFormat420YpCbCrSemiPlanar,
        kPixelFormat420YpCrCbSemiPlanar,
        kPixelFormat422YpCbCr,
        kPixelFormat444YpCbCr,
    ]
    
    var isYUV : Bool {
        get {
            return yuvButton.state == NSControl.StateValue.on
        }
        set {
            yuvButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            yuvItems.isEnabled = newValue
            uvButton.isEnabled = newValue
            yuv2rgbButton.isEnabled = newValue
        }
    }
    
    var yuvFormat : ePixelFormat {
        get {
            if (isYUV == false) {
                return kPixelFormatUnknown
            }
            return YUVs[yuvItems.indexOfSelectedItem]
        }
        set {
            guard newValue != kPixelFormatUnknown else {
                return;
            }
            isRGB = false
            isYUV = true
            let index = YUVs.firstIndex(of: newValue)
            yuvItems.selectItem(at: index!)
        }
    }
    
    var isUVSwap : Bool {
        get {
            return uvButton.state == NSControl.StateValue.on
        }
        set {
            uvButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
        }
    }
    
    var isYUV2RGB : Bool {
        get {
            return yuv2rgbButton.state == NSControl.StateValue.on
        }
        set {
            yuv2rgbButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
        }
    }
    
    // rgb formats
    @IBOutlet weak var rgbButton: NSButton!
    @IBOutlet weak var rgbItems: NSPopUpButton!
    
    let RGBs = [
        kPixelFormatRGB565,
        kPixelFormatBGR565,
        kPixelFormatRGB,
        kPixelFormatBGR,
        kPixelFormatARGB,
        kPixelFormatBGRA,
        kPixelFormatABGR,
        kPixelFormatRGBA,
    ]
    
    var isRGB : Bool {
        get {
            return rgbButton.state == NSControl.StateValue.on
        }
        set {
            rgbButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            rgbItems.isEnabled = newValue
        }
    }
    
    var rgbFormat : ePixelFormat {
        get {
            if (isRGB == false) {
                return kPixelFormatUnknown
            }
            return RGBs[rgbItems.indexOfSelectedItem]
        }
        set {
            guard newValue != kPixelFormatUnknown else {
                return;
            }
            isYUV = false
            isRGB = true
            let index = RGBs.firstIndex(of: newValue)
            rgbItems.selectItem(at: index!)
        }
    }
    
    // both yuv & rgb
    @IBOutlet weak var wordOrderButton : NSButton!
    var isWordOrder : Bool {
        get {
            return wordOrderButton.state == NSControl.StateValue.on
        }
        set {
            wordOrderButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
        }
    }
    
    @IBOutlet weak var infoText: NSTextField!
    
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
        var plane0 : Int64 = Int64(widthText.intValue * heightText.intValue);
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
        
        // set background color
        self.view.wantsLayer = true;
        self.view.layer?.backgroundColor = NSColor.black.cgColor
                
        // setup OpenGL Context
        NSLog("OpenGLContext: %@", imageView.openGLContext!)
        imageView.openGLContext?.makeCurrentContext()
        
        // not working, but still put this line
        self.view.window?.isMovableByWindowBackground = true
        
        widthText.formatter = formatter
        heightText.formatter = formatter
        leftText.formatter = formatter
        topText.formatter = formatter
        displayWidthText.formatter = formatter
        displayHeightText.formatter = formatter
        
        // set default value
        widthText.intValue         = imageFormat.width
        heightText.intValue        = imageFormat.height
        leftText.intValue          = imageFormat.rect.x
        topText.intValue           = imageFormat.rect.y
        displayWidthText.intValue  = imageFormat.rect.w
        displayHeightText.intValue = imageFormat.rect.h
        
        yuvItems.removeAllItems()
        for yuv in YUVs {
            let desc = GetPixelFormatDescriptor(yuv)
            yuvItems.addItem(withTitle: String.init(cString: desc!.pointee.name))
        }
        rgbItems.removeAllItems()
        for rgb in RGBs {
            let desc = GetPixelFormatDescriptor(rgb)
            rgbItems.addItem(withTitle: String.init(cString: desc!.pointee.name))
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
    
    func pixelIsPlanar(pixel : ePixelFormat) -> Bool {
        let desc : UnsafePointer<PixelDescriptor>? = GetPixelFormatDescriptor(pixel)
        guard desc != nil else {
            return false
        }
        return desc!.pointee.nb_planes > 1
    }
    
    func openOutputDevice(format: ImageFormat) -> MediaDeviceRef? {
        let formats : MessageObjectRef = MessageObjectCreate()
        MessageObjectPutInt32(formats, kKeyFormat,  Int32(format.format))
        MessageObjectPutInt32(formats, kKeyWidth,   format.width)
        MessageObjectPutInt32(formats, kKeyHeight,  format.height)
        
        let options : MessageObjectRef = MessageObjectCreate()
        let openGLContext = imageView.openGLContext?.cglContextObj
        MessageObjectPutPointer(options, kKeyOpenGLContext, openGLContext)
        
        let device = MediaDeviceCreate(formats, options)
        SharedObjectRelease(formats)
        SharedObjectRelease(options)
        
        return device
    }
    
    func drawImage() {
        if (imageFrame == nil) { return }
        infoText.stringValue = ""
        
        let imageFormat = MediaFrameGetImageFormat(imageFrame)
        // force window aspect ratio
        self.view.window?.contentAspectRatio = NSSize.init(width: Int(imageFormat!.pointee.width), height: Int(imageFormat!.pointee.height))
        
        var status : String = "pixel: "
        
        let origFormat : UnsafeMutablePointer<ImageFormat> = MediaFrameGetImageFormat(imageFrame!)
        var inputFormat     = ImageFormat.init()
        var outputFormat    = ImageFormat.init()
        
        // real input format
        inputFormat.format  = origFormat.pointee.format
        inputFormat.width   = origFormat.pointee.width
        inputFormat.height  = origFormat.pointee.height
        inputFormat.rect    = origFormat.pointee.rect
        
        var descriptor = GetPixelFormatDescriptor(inputFormat.format)
        NSLog("input format: %@", String.init(cString: descriptor!.pointee.name))
        
        // setup output format
        outputFormat        = inputFormat
        outputFormat.format = isYUV2RGB ? kPixelFormatRGB32 : inputFormat.format
        if (isRGB) {
            // output rgb in platform preferred format
            let descriptor = GetPixelFormatDescriptor(origFormat.pointee.format);
            if (descriptor!.pointee.bpp == 16) {
                outputFormat.format = kPixelFormatRGB16
            } else if (descriptor!.pointee.bpp == 24) {
                outputFormat.format = kPixelFormatRGB24
            } else if (descriptor!.pointee.bpp == 32) {
                outputFormat.format = kPixelFormatRGB32
            }
        }
        
        // swap input uv chroma
        if (isYUV && isUVSwap) {
            if (descriptor!.pointee.similar.1 != kPixelFormatUnknown) {
                inputFormat.format = descriptor!.pointee.similar.1
                descriptor = GetPixelFormatDescriptor(inputFormat.format)
                NSLog("input format(swap uv): %@", String.init(cString: descriptor!.pointee.name))
            }
        }
        
        // byte-order -> word-order, only work for packed format
        if (isWordOrder && descriptor!.pointee.nb_planes == 1) {
            if (descriptor!.pointee.similar.2 != kPixelFormatUnknown) {
                inputFormat.format = descriptor!.pointee.similar.2;
                descriptor = GetPixelFormatDescriptor(inputFormat.format)
                NSLog("input format(word order): ", String.init(cString: descriptor!.pointee.name))
            }
        }
        
        status += String.init(cString: descriptor!.pointee.name)
        if (origFormat.pointee.format != inputFormat.format) {
            let descriptor = GetPixelFormatDescriptor(origFormat.pointee.format)
            status += " >> " + String.init(cString: descriptor!.pointee.name)
        }
        
        if (outputFormat.format != origFormat.pointee.format) {
            let descriptor = GetPixelFormatDescriptor(outputFormat.format)
            NSLog("output format: %@", String.init(cString: descriptor!.pointee.name))
            status += " >> " + String.init(cString: descriptor!.pointee.name)
        }
        
        var device = openOutputDevice(format: outputFormat)
        
        // yuv output is not supported ?
        if (device == nil && isYUV) {
            // force output RGB32
            NSLog("output device not support current format, force rgb32")
            outputFormat.format = kPixelFormatRGB32
            device = openOutputDevice(format: outputFormat)
        }
        
        var output = SharedObjectRetain(imageFrame);
        if (inputFormat.format != outputFormat.format) {
            let cc : MediaDeviceRef? = ColorConverterCreate(&inputFormat, &outputFormat, nil)
            guard cc != nil else {
                infoText.stringValue += "create color converter failed."
                return;
            }
            
            MediaDevicePush(cc, output)
            SharedObjectRelease(output)
            output = MediaDevicePull(cc)
            SharedObjectRelease(cc)
            
            guard output != nil else {
                infoText.stringValue += "color convert failed."
                return
            }
        }
        
        guard device != nil else {
            infoText.stringValue += "create media device failed."
            return
        }
        
        MediaDevicePush(device, output)
        SharedObjectRelease(output)
        SharedObjectRelease(device)
        
        NSLog("draw ==> %@", status)
    }
    
    var imageBuffer : BufferObjectRef?
    var imageFrame : MediaFrameRef?
    var imageLength : Int32 = 0
    public func openFile(url : String) -> Void {
        // note recent documents
        NSDocumentController.shared.noteNewRecentDocumentURL(URL(fileURLWithPath: url))
        self.view.window?.title = URL.init(string: url)!.lastPathComponent
        
        closeFile()
        
        isUIHidden = false
        
        imageBuffer = BufferObjectCreateWithUrl(url)
        if (imageBuffer != nil) {
            let lucky = luckyGuess(size: BufferObjectGetDataLength(imageBuffer))
            if (lucky != nil) {
                print("lucky => ", lucky!)
                
                widthText.intValue     = Int32(lucky!.0.1)
                heightText.intValue    = Int32(lucky!.0.2)
                validateRect(width: widthText.intValue, height: heightText.intValue)
                
                // set both yuv & rgb lucky guess
                if (lucky!.1[1] != kPixelFormatUnknown) {
                    rgbFormat = lucky!.1[1]
                }
                if (lucky!.1[0] != kPixelFormatUnknown) {
                    yuvFormat = lucky!.1[0]
                }
            }
            onFormatChanged(nil)
        }
    }
    
    func closeFile() {
        if (imageBuffer != nil) {
            SharedObjectRelease(imageBuffer)
            imageBuffer = nil
        }
        if (imageFrame != nil) {
            SharedObjectRelease(imageFrame)
            imageFrame = nil
        }
        infoText.stringValue = ""
    }
    
    @IBAction func close(sender : Any?) {
        self.view.window?.performClose(nil)
        //NSApplication.shared.terminate(self)
    }
    
    // move window by background
    override func mouseDragged(with event: NSEvent) {
        self.view.window?.performDrag(with: event)
    }
    
    @IBAction func onFormatChanged(_ sender: Any?) {
        NSLog("onFormatChanged")
        
        imageFormat.format     = yuvFormat
        if (isRGB) {
            imageFormat.format = rgbFormat
        }
        imageFormat.width      = widthText.intValue
        imageFormat.height     = heightText.intValue
        validateRect(width: imageFormat.width, height: imageFormat.height)
        if (isRectEnabled) {
            imageFormat.rect.x = leftText.intValue
            imageFormat.rect.y = topText.intValue
            imageFormat.rect.w = displayWidthText.intValue
            imageFormat.rect.h = displayHeightText.intValue
        } else {
            imageFormat.rect.x = 0
            imageFormat.rect.y = 0
            imageFormat.rect.w = imageFormat.width
            imageFormat.rect.h = imageFormat.height
        }
        
        if (imageFrame != nil) {
            SharedObjectRelease(imageFrame)
            imageFrame = nil
        }
        
        guard imageBuffer != nil else {
            NSLog("no media file")
            return
        }
    
        BufferObjectResetBytes(imageBuffer)
        let descriptor = GetPixelFormatDescriptor(imageFormat.format)
        imageLength = (imageFormat.width * imageFormat.height * Int32(descriptor!.pointee.bpp)) / 8
        numFrames = BufferObjectGetDataLength(imageBuffer) / Int64(imageLength)
        
        let data = BufferObjectReadBytes(imageBuffer, Int(imageLength))
        guard data != nil else {
            NSLog("read image frame failed, bad format?")
            return
        }
        
        imageFrame = MediaFrameCreateWithImageBuffer(&imageFormat, data)
        drawImage()
    }
    
    func validateRect(width : Int32, height : Int32) {
        if (isRectEnabled == true) {
            // just valid the values
            if (topText.intValue > height) {
                topText.intValue = height
            }
            if (leftText.intValue > width) {
                leftText.intValue = width
            }
            if (displayWidthText.intValue > width - leftText.intValue) {
                displayWidthText.intValue = width - leftText.intValue
            }
            if (displayHeightText.intValue > height - topText.intValue) {
                displayHeightText.intValue = height - topText.intValue;
            }
        } else {
            topText.intValue   = 0
            leftText.intValue  = 0
            displayWidthText.intValue  = width
            displayHeightText.intValue = height
        }
    }
    
    @IBAction func onRectCheck(_ sender: Any) {
        print("onRectCheck <- ", sender)
        isRectEnabled = displayRectButton.state == NSControl.StateValue.on
        onFormatChanged(nil)
    }
    
    @IBAction func onYUVCheck(_ sender: Any) {
        print("onYUVCheck <- ", sender as Any)
        isYUV = yuvButton.state == NSControl.StateValue.on
        isRGB = !isYUV
        onFormatChanged(nil)
    }
    
    @IBAction func onRGBCheck(_ sender: Any) {
        print("onRGBCheck <- ", sender as Any)
        isRGB = rgbButton.state == NSControl.StateValue.on
        isYUV = !isRGB
        onFormatChanged(nil)
    }
    
    var isUIHidden : Bool {
        get {
            return propertyBox.isHidden
        }
        set {
            self.view.window?.titleVisibility = newValue ? NSWindow.TitleVisibility.hidden : NSWindow.TitleVisibility.visible
            propertyBox.isHidden = newValue
            infoText.isHidden = newValue
            
            if (numFrames <= 1) {
                frameSlider.isHidden = true
            } else {
                frameSlider.isHidden = newValue
            }
        }
    }
    
    @IBOutlet weak var frameSlider: NSSlider!
    
    var numFrames : Int64 {
        get {
            return Int64(frameSlider.maxValue)
        }
        set {
            frameSlider.numberOfTickMarks = Int(newValue)
            frameSlider.minValue = 0
            frameSlider.maxValue = Double(newValue)
            frameSlider.intValue = 0
            if (newValue <= 1) {
                frameSlider.isHidden = true
            } else {
                frameSlider.isHidden = isUIHidden
            }
        }
    }
    
    @IBAction func onFrameSelect(_ sender: Any?) {
        NSLog("select frame %d", frameSlider.intValue)
        
        let index = frameSlider.intValue
        BufferObjectResetBytes(imageBuffer)
        BufferObjectSkipBytes(imageBuffer, Int64(imageLength * (index - 1)))
        
        if (imageFrame != nil) {
            SharedObjectRelease(imageFrame)
            imageFrame = nil
        }
        
        let data = BufferObjectReadBytes(imageBuffer, Int(imageLength))
        guard data != nil else {
            NSLog("read image failed")
            return
        }
        
        imageFrame = MediaFrameCreateWithImageBuffer(&imageFormat, data)
        drawImage()
    }
    
    var eventNumber : Int = 0
    func filterMouseUp(with event: NSEvent) {
        // outdated event
        if (event.eventNumber < eventNumber) {
            return
        }
        NSLog("filterMouseUp %@", event)
        switch event.clickCount {
        case 1:
            NSLog("show or hide UI")
            isUIHidden = !isUIHidden
        case 2:
            NSLog("Enter full screen")
            self.view.window?.toggleFullScreen(self);
            drawImage()
        default:
            break
        }
    }
    
    override func mouseUp(with event: NSEvent) {
        eventNumber = event.eventNumber
        NSLog("mouseUp %@", event)
        // show/hide property box
        guard propertyBox.hitTest(event.locationInWindow) == nil else {
            NSLog("mouse hit property view")
            return
        }
        
        if (event.clickCount > 1) {
            filterMouseUp(with: event)
        } else {
            DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + NSEvent.doubleClickInterval, execute: {
                () -> Void in
                self.filterMouseUp(with: event)
            })
        }
    }
    
    override func keyDown(with event: NSEvent) {
        
        let special = event.specialKey;
        if (special == NSEvent.SpecialKey.rightArrow || special == NSEvent.SpecialKey.leftArrow) {
            var index = frameSlider.intValue
            index += special == NSEvent.SpecialKey.rightArrow ? +1 : -1
            frameSlider.intValue = index
            onFrameSelect(nil)
        }
    }
    
    
}

