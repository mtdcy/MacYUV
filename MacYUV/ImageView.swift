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
        viewController!.openFile(url: url!)
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
}
