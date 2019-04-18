//
//  BaseView.swift
//  mpx
//
//  Created by Chen Fang on 2019/4/12.
//  Copyright © 2019 Chen Fang. All rights reserved.
//

import Cocoa

class BaseView: NSView {

    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        // Drawing code here.
    }
    
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
