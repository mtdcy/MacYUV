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
    
    @IBOutlet weak var imageView: ImageView!
    @IBOutlet weak var widthText: NSTextField!
    @IBOutlet weak var heightText: NSTextField!
    @IBOutlet weak var leftText: NSTextField!
    @IBOutlet weak var topText: NSTextField!
    @IBOutlet weak var displayWidthText: NSTextField!
    @IBOutlet weak var displayHeightText: NSTextField!
    @IBOutlet weak var displayRectButton: NSButton!
    @IBOutlet weak var propertyBox: NSBox!
    @IBOutlet weak var yuvButton: NSButton!
    @IBOutlet weak var yuvItems: NSPopUpButton!
    @IBOutlet weak var colorItems: NSPopUpButton!
    @IBOutlet weak var rgbButton: NSButton!
    @IBOutlet weak var rgbItems: NSPopUpButton!
    @IBOutlet weak var infoText: NSTextField!
    @IBOutlet weak var frameSlider: NSSlider!
    
    var imageBuffer : BufferObjectRef?
    
    var isRectEnabled : Swift.Bool {
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
        kPixelFormat422YpCrCb,
        kPixelFormat422YpCbCrWO,
        kPixelFormat422YpCrCbWO,
    ]
    
    var isYUV : Swift.Bool {
        get {
            return yuvButton.state == NSControl.StateValue.on
        }
        set {
            yuvButton.state = newValue == true ? NSControl.StateValue.on : NSControl.StateValue.off
            yuvItems.isEnabled = newValue
            colorItems.isEnabled = newValue
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
    
    let Matrices : [(eColorMatrix, String)] = [
        (kColorMatrixBT601,     "BT601"),
        (kColorMatrixBT709,     "BT709"),
        (kColorMatrixBT2020,    "BT2020"),
        (kColorMatrixJPEG,      "JPEG"),
    ]
    
    var colorMatrix : eColorMatrix {
        get {
            if isYUV == false {
                return kColorMatrixNull
            }
            return Matrices[colorItems.indexOfSelectedItem].0
        }
        set {
            guard newValue != kColorMatrixNull else {
                return
            }
            isYUV   = true
            for (index, (color, _)) in Matrices.enumerated() {
                if color == newValue {
                    colorItems.selectItem(at: index)
                    break
                }
            }
        }
    }
    
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
    
    var isRGB : Swift.Bool {
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
    
    var statusText : String {
        get {
            return infoText.stringValue
        }
        set {
            if newValue.isEmpty {
                infoText.isHidden = true
            } else {
                infoText.isHidden = false
            }
            infoText.stringValue = newValue
        }
    }
    
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
    func luckyGuess(size : Int64) -> ((String, Swift.Int, Swift.Int), [ePixelFormat])? {
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
        override func isPartialStringValid(_ partialString: String, newEditingString newString: AutoreleasingUnsafeMutablePointer<NSString?>?, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Swift.Bool {
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
        //NSLog("OpenGLContext: %@", imageView.openGLContext!)
        //imageView.openGLContext?.makeCurrentContext()
        
        // not working, but still put this line
        self.view.window?.isMovableByWindowBackground = true
        
        widthText.formatter = formatter
        heightText.formatter = formatter
        leftText.formatter = formatter
        topText.formatter = formatter
        displayWidthText.formatter = formatter
        displayHeightText.formatter = formatter
        
        // set default value
        widthText.intValue         = 1920
        heightText.intValue        = 1080
        leftText.intValue          = 0
        topText.intValue           = 0
        displayWidthText.intValue  = 1920
        displayHeightText.intValue = 1080
        
        yuvItems.removeAllItems()
        for yuv in YUVs {
            let desc = GetPixelFormatDescriptor(yuv)
            yuvItems.addItem(withTitle: String.init(cString: desc!.pointee.name))
        }
        
        colorItems.removeAllItems()
        for (_, name) in Matrices {
            colorItems.addItem(withTitle: name)
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
    
    var imageFormat : ImageFormat {
        get {
            let rect = ImageFormat.__Unnamed_struct_rect.init(x: isRectEnabled ? leftText.intValue : 0,
                                                              y: isRectEnabled ? topText.intValue : 0,
                                                              w: isRectEnabled ? displayWidthText.intValue : widthText.intValue,
                                                              h: isRectEnabled ? displayHeightText.intValue : heightText.intValue)
            return ImageFormat.init(format: isYUV ? yuvFormat : rgbFormat,
                                    matrix: colorMatrix,
                                    width: widthText.intValue,
                                    height: heightText.intValue,
                                    rect: rect)
        }
        set {
            if newValue.width != newValue.rect.w || newValue.height != newValue.rect.h {
                isRectEnabled               = true
            }
            widthText.intValue          = newValue.width
            heightText.intValue         = newValue.height
            leftText.intValue           = newValue.rect.x
            topText.intValue            = newValue.rect.y
            displayWidthText.intValue   = newValue.rect.w
            displayHeightText.intValue  = newValue.rect.h
        }
    }
    
    var imageLength : Int32 {
        let descriptor : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(isYUV ? yuvFormat : rgbFormat)
        return (widthText.intValue * heightText.intValue * Int32(descriptor.pointee.bpp)) / 8
    }
    
    func drawImage(index: Int32) {
        BufferObjectResetBytes(imageBuffer)
        if (index > 0) {
            BufferObjectSkipBytes(imageBuffer, Int64(imageLength * index))
        }
        var data = BufferObjectReadBytes(imageBuffer, UInt32(imageLength))
        guard data != nil else {
            statusText = "read image data failed. bad file ?"
            return
        }
        
        var format = imageFormat
        var image = MediaFrameCreateWithImageBuffer(&format, data)
        SharedObjectRelease(data)
        data = nil
        
        guard image != nil else {
            statusText = "create image frame failed."
            return
        }
        
        // force window aspect ratio
        if self.view.isInFullScreenMode == false {
            let frame = self.view.window?.frame
            if frame != nil {
                // keep width, change height
                let size = NSSize.init(width: frame!.width, height: frame!.width * CGFloat(format.rect.h) / CGFloat(format.rect.w))
                // set aspectRatio will NOT take effect immediately, so setFrame first
                self.view.window?.aspectRatio = size
                let rect = NSRect.init(x: frame!.minX, y: frame!.minY, width: size.width, height: size.height)
                self.view.window?.setFrame(rect, display: true)
            }
        }

        var outputFormat = ImageFormat.init()
        outputFormat.format     = imageView.pixelFormat
        outputFormat.width      = format.rect.w
        outputFormat.height     = format.rect.h
        outputFormat.rect.x     = 0
        outputFormat.rect.y     = 0
        outputFormat.rect.w     = outputFormat.width
        outputFormat.rect.h     = outputFormat.height
        
        var output = SharedObjectRetain(image);
        // do color convertion or cropping
        if (format.format != outputFormat.format ||
            format.rect.x != 0 || format.rect.y != 0) {
            let cc : MediaDeviceRef? = ColorConverterCreate(&format, &outputFormat, nil)
            guard cc != nil else {
                statusText = "create color converter failed."
                return;
            }
            
            MediaDevicePush(cc, output)
            SharedObjectRelease(output)
            
            output = MediaDevicePull(cc)
            SharedObjectRelease(cc)
            
            guard output != nil else {
                statusText = "color convert failed."
                return
            }
        }
        
        statusText = imageView.drawFrame(frame: output!)
        
        // show frame number
        if (numFrames > 1 && statusText == "") {
            statusText = "frames: " + String(index + 1) + "/" + String(numFrames)
        }
        
        SharedObjectRelease(image)
        image = nil
    }
    
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
        statusText = ""
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
        numFrames = BufferObjectGetDataLength(imageBuffer) / Int64(imageLength)
        drawImage(index: frameSlider.intValue)
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
        }
        // DON'T change values if display rectangle is disabled
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
    
    var isUIHidden : Swift.Bool {
        get {
            return propertyBox.isHidden
        }
        set {
            self.view.window?.titleVisibility = newValue ? NSWindow.TitleVisibility.hidden : NSWindow.TitleVisibility.visible
            propertyBox.isHidden = newValue
            
            if (numFrames <= 1) {
                frameSlider.isHidden = true
            } else {
                frameSlider.isHidden = newValue
            }
        }
    }
    
    var numFrames : Int64 {
        get {
            return Int64(frameSlider.maxValue) + 1
        }
        set {
            frameSlider.numberOfTickMarks = Swift.Int(newValue) - 1
            frameSlider.minValue = 0
            frameSlider.maxValue = Double(newValue - 1)
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
        drawImage(index: frameSlider.intValue)
    }
    
    var eventNumber : Int = 0
    func filterMouseUp(with event: NSEvent) {
        // outdated event
        if (event.eventNumber < eventNumber) {
            return
        }
        //NSLog("filterMouseUp %@", event)
        switch event.clickCount {
        case 1:
            NSLog("show or hide UI")
            isUIHidden = !isUIHidden
        case 2:
            NSLog("Enter full screen")
            self.view.window?.toggleFullScreen(self);
            drawImage(index: frameSlider.intValue)
        default:
            break
        }
    }
    
    override func mouseUp(with event: NSEvent) {
        eventNumber = Int(event.eventNumber)
        //NSLog("mouseUp %@", event)
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
    
    func zoomImage(scale : CGFloat, position : CGPoint) -> Void {
        var format = imageFormat
        var w = Int32(CGFloat(format.width) / scale)
        if w > format.width { w = format.width }
        else if w < 32 { w = 32 }   // CGImage min pixels
        else { w = (w / 16) * 16 }  // color converter require row align 16 bytes
        
        // always keep aspect ratio
        let h = (format.height * w) / format.width
        
        var x = Int32(CGFloat(format.rect.w - w) * position.x + CGFloat(format.rect.x))
        if x + w > format.width { x = format.width - w }
        else if x < 0 { x = 0 }
        
        var y = Int32(CGFloat(format.rect.h - h) * position.y + CGFloat(format.rect.y))
        if (y + h > format.height) { y = format.height - h }
        else if y < 0 { y = 0 }
        
        // update property box
        format.rect.x   = Int32(x)
        format.rect.y   = Int32(y)
        format.rect.w   = Int32(w)
        format.rect.h   = Int32(h)
        imageFormat     = format
        
        drawImage(index: frameSlider.intValue)
    }
    
    var scale : CGFloat = 1.0
    override func scrollWheel(with event: NSEvent) {
        NSLog("mouse scroll %@", event)
        
        guard self.view.window != nil else {
            statusText = "window is missing"
            return
        }
        // in frame coordinates with origin at left top
        let frame = self.view.window!.frame
        // in screen coordinates with origin at left bottom
        let mouse = self.view.window!.mouseLocationOutsideOfEventStream
        
        // to percentage
        let position = NSPoint.init(x: mouse.x / frame.width, y: 1 - mouse.y / frame.height)
        
        scale += event.deltaY > 0 ? 0.1 : -0.1
        if scale < 1 { scale = 1 }
        
        zoomImage(scale: scale, position: position)
    }
    
    override func rightMouseUp(with event: NSEvent) {
        // reset zoom
        scale = 1.0
        zoomImage(scale: 1.0, position: NSPoint.init(x: 1.0, y: 1.0))
    }
    
    override func keyDown(with event: NSEvent) {
        let special = event.specialKey;
        if (special == NSEvent.SpecialKey.rightArrow || special == NSEvent.SpecialKey.leftArrow) {
            var index = frameSlider.intValue
            index += special == NSEvent.SpecialKey.rightArrow ? +1 : -1
            guard index >= 0 && index < numFrames else {
                NSLog("out of range")
                return
            }
            frameSlider.intValue = index
            drawImage(index: index)
        }
    }
}

