//
//  AppDelegate.swift
//  YUVViewer
//
//  Created by Chen Fang on 2019/4/14.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    
    var lastWindow : NSWindow?
    let usingSystemLog = true
    
    override init() {
        super.init()
        NSLog("On Application init")
        if (usingSystemLog) {
            // setup log callback
            // https://originware.com/blog/?p=265
            LogSetCallback { (line : UnsafePointer<Int8>?) in
                // print won't send to syslog
                NSLog(String.init(cString: line!))
            }
        }
    }

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        NSLog("applicationDidFinishLaunching")
        // Insert code here to initialize your application
        if lastWindow == nil {
            open(sender: self)
        }
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        NSLog("applicationWillTerminate")
        // Insert code here to tear down your application
    }

    func application(_ sender: NSApplication, openFile filename: String) -> Bool {
        NSLog("application openFile -> %@", filename)
        openFile(url: filename)
        return true
    }
    
    public func openFile(url: String?) -> Void {
        let storyBoard = NSStoryboard.init(name: NSStoryboard.Name("Main"), bundle: Bundle.main)
        let windowController = storyBoard.instantiateController(withIdentifier:  NSStoryboard.SceneIdentifier("ImageWindowController")) as! NSWindowController
        let viewController = storyBoard.instantiateController(withIdentifier: NSStoryboard.SceneIdentifier("ImageViewController")) as! ViewController
        windowController.contentViewController = viewController
        
        if lastWindow != nil {
            NSLog("set window position")
            let frame = lastWindow!.frame
            // setFrameOrigin/setFrameTopLeftPoint not working
            windowController.window?.setFrame(NSRect.init(x: frame.minX + 32, y: frame.minY - 32, width: frame.width, height: frame.height), display: false)
        }
        windowController.showWindow(self)
        
        lastWindow = windowController.window
        
        guard url != nil else {
            return
        }
        viewController.openFile(url: url!)
            
        // draw input immediately after view open will cause error for OpenGL
//        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 0.5, execute: {
//            () -> Void in
//            viewController.openFile(url: url!)
//        })
    }
    
    @IBAction func open(sender : Any?) {
        let urls = NSDocumentController.shared.urlsFromRunningOpenPanel()
        guard urls != nil else {
            NSLog("Open File Canceled")
            return
        }
        let url = urls![0].path
        NSLog("url = %@", url)
        openFile(url: url)
    }
    
    @IBAction func openHelpWindow(sender : Any?) {
        NSWorkspace.shared.open(URL.init(string: "https://github.com/mtdcy/MacYUV")!)
    }
}

