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

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }

    func application(_ sender: NSApplication, openFile filename: String) -> Bool {
        let viewController = NSApplication.shared.mainWindow?.contentViewController as? ViewController
        guard viewController != nil else {
            return false
        }
        
        viewController?.openFile(url: filename)
        return true
    }
}

