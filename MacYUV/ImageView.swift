//
//  OpenGLView.swift
//  mpx
//
//  Created by Chen Fang on 2019/4/12.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Cocoa
import OpenGL

class ImageView: NSImageView {
    
    @IBOutlet weak var viewController : ViewController!
    
    let NSFilenamesPboardType = NSPasteboard.PasteboardType("NSFilenamesPboardType")

    override func draw(withExpansionFrame contentFrame: NSRect, in view: NSView) {
        super.draw(withExpansionFrame: contentFrame, in: view)
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        
        // Declare and register an array of accepted types
        registerForDraggedTypes([NSPasteboard.PasteboardType(kUTTypeFileURL as String),
                                 NSPasteboard.PasteboardType(kUTTypeItem as String)])
    }
    
    var url : String?
    func checkFile(drag: NSDraggingInfo) -> Bool {
        if let board = drag.draggingPasteboard.propertyList(forType: NSFilenamesPboardType) as? NSArray {
            url = board[0] as? String
            guard url != nil else {
                return false
            }
            
            let type = try? NSDocumentController.shared.typeForContents(of: URL.init(fileURLWithPath: url!))
            guard type != nil else {
                url = nil
                return false
            }
            
            NSLog("type: %@ <= %@", type!, url!)
            return true;

        }
        return false
    }
    
    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation {
        NSLog("draggingEntered")
        if checkFile(drag: sender) {
            return .copy
        } else {
            return []
        }
    }

    override func draggingUpdated(_ sender: NSDraggingInfo) -> NSDragOperation {
        //NSLog("draggingUpdated")
        if url != nil {
            return .copy
        } else {
            return []
        }
    }
    
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        NSLog("performDragOperation")
        guard url != nil else {
            return false
        }
        
        // THIS IS WERE YOU GET THE PATH FOR THE DROPPED FILE
        NSLog("openFile -> %@", url!)
        
        // FIXME: draw immediately will show a black frame, WHY?
        // override draw() won't fix this
        //viewController!.openFile(url: url!)
        DispatchQueue.main.asyncAfter(wallDeadline: DispatchWallTime.now() + 0.05, execute: {
            () -> Void in
            self.viewController!.openFile(url: self.url!)
        })
        return true
    }
    
    // help move window by drag
    override var acceptsFirstResponder: Bool {
        get {
            return true
        }
    }
    
    // not working but still put this
    override var mouseDownCanMoveWindow: Bool {
        get {
            return false
        }
    }
    
    var pixelFormat : ePixelFormat {
        // CGImage using RGBA in byte-order by default.
        // but our color converter support ARGB in word-order well
        return kPixelFormatBGRA
    }
    
    // return empty string on success, otherwise return error string
    public func drawFrame(frame: MediaFrameRef) -> String {
        let imageFormat : UnsafeMutablePointer<ImageFormat> = MediaFrameGetImageFormat(frame)
        let descriptor : UnsafePointer<PixelDescriptor> = GetPixelFormatDescriptor(imageFormat.pointee.format)
        
        var data = MediaFrameGetPlaneData(frame, 0)
//        let bitmap = NSBitmapImageRep.init(bitmapDataPlanes: &data,
//                                           pixelsWide: Int(imageFormat.pointee.width),
//                                           pixelsHigh: Int(imageFormat.pointee.height),
//                                           bitsPerSample: 8,
//                                           samplesPerPixel: 4,
//                                           hasAlpha: true,
//                                           isPlanar: false,
//                                           colorSpaceName: NSColorSpaceName.deviceRGB,
//                                           bytesPerRow: Int(imageFormat.pointee.width * 4),
//                                           bitsPerPixel: descriptor.pointee.bpp)
//        guard bitmap != nil else {
//            self.image = nil
//            return "create bitmap failed."
//        }
//
//        let cgImage = bitmap!.cgImage
        
        // create a ARGB(word-order) CGImage
        let bitmapInfo = CGBitmapInfo.byteOrder32Little.rawValue | CGImageAlphaInfo.premultipliedFirst.rawValue
        let cgContext = CGContext.init(data: data,
                                       width: Int(imageFormat.pointee.width),
                                       height: Int(imageFormat.pointee.height),
                                       bitsPerComponent: 8,
                                       bytesPerRow: Int(imageFormat.pointee.width * 4),
                                       space: CGColorSpace.init(name: CGColorSpace.sRGB)!,
                                       bitmapInfo: bitmapInfo)
        let cgImage = cgContext?.makeImage()
        guard cgImage != nil else {
            self.image = nil
            return "create cgImage failed."
        }

        let image = NSImage.init(cgImage: cgImage!,
                                 size: NSMakeSize(CGFloat(imageFormat.pointee.width), CGFloat(imageFormat.pointee.height)))
        self.image = image
        
        // error: unrecognized selector sent to instance. WHY?
        //imageView.provideImageData(UnsafeMutableRawPointer.init(data!), bytesPerRow: Int(outputFormat.width * 4), origin: Int(outputFormat.rect.x), Int(outputFormat.rect.y), size: size, Int(outputFormat.height), userInfo: self)
                
        SharedObjectRelease(frame)
        NSLog("drawFrame complete")
        return ""
    }
}
