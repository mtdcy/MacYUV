#!/bin/bash

[ $# -lt 1 ] && exit 1

INSTALL_PATH=$PWD
[ -d xcode ] && rm -rf xcode 

mkdir xcode && 
cd xcode
cmake -G Xcode -DWITH_FFMPEG=OFF -DCMAKE_IGNORE_PATH="/usr/local/lib;/usr/local/include" -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH $@
xcodebuild -target install -configuration Debug
