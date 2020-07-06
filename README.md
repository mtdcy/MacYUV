# MacYUV

A YUV/RGB raw image player powered by [MediaFramework](https://github.com/mtdcy/MediaFramework).

![](app.jpeg?raw=true)

## Features

1. Supported YUVs: 420p/422p/444p/nv12/nv21/...
2. Supported RGBs: RGB565/RGB888/ARGB/RGBA/...
3. Play multiple raw images in the same file.
4. Zoom in/output the raw image.
5. Open a raw image by drag&drop.
6. Open multiple raw images at the same time.
7. More features are comming...


## Notes

### About pixel formats

We use letters to represent pixel format in byte-order instead of word-order both for YUVs and RGBs, which is different from some libraries such as libyuv. Just like 'yuyv' means that Y'0 is the first byte, 'RGBA' means that the red component is the first byte. Therefore you may need to map these formats to your own. 

### About output format 

We always output image frame as RGBA, which uses color converter to achieve it. 

