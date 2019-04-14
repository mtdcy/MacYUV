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
    
    var mContent : ContentObjectRef?
    var mMediaOut : MediaOutRef?

    override func viewDidLoad() {
        super.viewDidLoad()

        // setup OpenGL Context
        NSLog("OpenGLContext: %@", openGLView.openGLContext!)
        openGLView.prepareOpenGL()
        
        
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    func drawFile() {
        guard mMediaOut != nil else {
            NSLog("MediaOut is not ready")
            return
        }
        
        //MediaOutWrite(mMediaOut, mMediaFrame)
    }
    
    public func openFile(url : String) {
        NSLog("url = %@", url)
        
        mContent = ContentObjectCreate(url)
        guard mContent != nil else {
            NSLog("read data failed")
            return
        }
        
        drawFile();
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
            SharedObjectRelease(mMediaOut)
            mMediaOut = nil
        }
        
        if (mContent != nil) {
            SharedObjectRelease(mContent)
            mContent = nil
        }
    }

    @IBAction func open(sender : Any?) {
        close(sender: nil)
        openFile()
    }
    
    @IBAction func close(sender : Any?) {
        
    }
}

