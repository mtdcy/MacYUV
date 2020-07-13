/******************************************************************************
 * Copyright (c) 2016, Chen Fang <mtdcy.chen@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


// File:    MediaDevice.h
// Author:  mtdcy.chen
// Changes:
//          1. 20200630     initial version
//

#ifndef _MEDIA_MODULES_DEVICE_H
#define _MEDIA_MODULES_DEVICE_H

#include <MediaFramework/MediaTypes.h>
#include <MediaFramework/MediaFrame.h>

/**
 * Formats
 *  audio track formats:
 *   kKeyFormat:        eAudioCodec     [*] audio codec
 *   kKeyType:          eCodecType      [ ] codec type, default:kCodecTypeAudio
 *   kKeyChannels:      UInt32        [*] audio channels
 *   kKeySampleRate:    UInt32        [*] audio sample rate
 *   kKeyChannelMap:    UInt32        [ ] audio channel map
 *   kKeyESDS:          sp<Buffer>      [ ] audio magic data
 *
 *  audio sample formats:
 *   kKeyFormat:        eSampleFormat   [*] sample format
 *   kKeyType:          eCodecType      [ ] codec type, default:kCodecTypeAudio
 *   kKeyChannels:      UInt32        [*] audio channels
 *   kKeySampleRate:    UInt32        [*] audio sample rate
 *
 *  video track formats:
 *   kKeyFormat:        eVideoCodec     [*] video codec
 *   kKeyType:          eCodecType      [*] codec type, default:kCodecTypeVideo
 *   kKeyWidth:         UInt32        [*] video width
 *   kKeyHeight:        UInt32        [*] video height
 *   kKeyavcC:          sp<Buffer>      [ ] h264 avcC data
 *   kKeyhvcC:          sp<Buffer>      [ ] hevc hvcC data
 *
 *  video pixel formats:
 *   kKeyFormat:        ePixelFormat    [*] pixel format
 *   kKeyType:          eCodecType      [*] codec type, default:kCodecTypeVideo   
 *   kKeyWidth:         UInt32        [*] video width
 *   kKeyHeight:        UInt32        [*] video height
 *
 *  common track formats:
 *   kKeyBitrate:       UInt32        [ ] bit rate
 */

/**
 * File Device:
 *  input formats:
 *   kKeyContent:       sp<ABuffer>     [*] media content
 *
 *  input options:
 *
 *  output formats:
 *   kKeyFormat:        eFileFormat     [*] file format
 *   kKeyDuration:      Int64         [ ] file duration in us
 *   kKeyBitrate:       UInt32        [ ] bit rate
 *   kKeyMetaData:      sp<Message>     [ ] file meta data
 *   kKeyCount:         UInt32        [ ] track count, default:1
 *   kKeyTrack + i:     sp<Message>     [*] audio/video/subtitle track format
 *
 *  configure options:
 *   kKeySeek:          Int64         [ ] perform seek
 *   kKeyTracks:        UInt32        [ ] perform track select based on track mask
 *
 * Codec Device:
 *  input formats:
 *   ... audio/video track formats
 *   kKeyRequestFormat: UInt32        [ ] track request format
 *
 *  input options:
 *   kKeyMode:          eModeType       [ ] codec mode
 *   kKeyPause:         Bool            [ ] pause/unpause codec, some codec may need this
 *
 *  output formats:
 *   ... sample formats/pixel formats
 *
 * Output Device:
 *  input formats:
 *   ... sample formats/pixel formats
 *
 *  input options:
 *
 *  output formats:
 *   ... sample formats/pixel formats
 *   kKeyLatency:       Int64         [ ] device push latency in us
 *   kKeyMode:          eBlockModeType  [ ] device push mode, default:kModeBlock
 *
 *  configure options:
 *   kKeyPause:         Bool            [ ] pause/unpause device
 */

__BEGIN_DECLS

#pragma mark Common Keys
// common keys for media device
enum {
    // common keys
    kKeyContent         = FOURCC('cont'),       ///< sp<ABuffer>
    kKeyFormat          = FOURCC(' fmt'),       ///< UInt32, @see eFileFormat/eAudioCodec/eVideoCodec/eSampleFormat/ePixelFormat
    kKeyRequestFormat   = FOURCC('!fmt'),       ///< UInt32
    kKeyType            = FOURCC('type'),       ///< UInt32, @see eCodecType
    kKeyMode            = FOURCC('mode'),       ///< UInt32,
    kKeySeek            = FOURCC('seek'),       ///< Int64, us
    kKeyDuration        = FOURCC('dura'),       ///< Int64, us
    kKeyLatency         = FOURCC('late'),       ///< Int64, us
    kKeyChannels        = FOURCC('chan'),       ///< UInt32
    kKeySampleRate      = FOURCC('srat'),       ///< UInt32
    kKeyChannelMap      = FOURCC('cmap'),       ///< UInt32
    kKeyWidth           = FOURCC('widt'),       ///< UInt32
    kKeyHeight          = FOURCC('heig'),       ///< UInt32
    kKeyRotate          = FOURCC('?rot'),       ///< UInt32, eRotate
    kKeyCount           = FOURCC('#cnt'),       ///< UInt32
    kKeyBitrate         = FOURCC('btrt'),       ///< UInt32
    kKeyTracks          = FOURCC('trak'),       ///< Int32, bit mask
    kKeyTrack           = FOURCC('0trk'),       ///< sp<Message>
    kKeyError           = FOURCC('!err'),       ///< Int32, MediaError
    kKeyOpenGLContext   = FOURCC('oglt'),       ///< void *
    kKeyPause           = FOURCC('paus'),       ///< Int32, Bool
    kKeyColorMatrix     = FOURCC('cmat'),       ///< UInt32, @see eColorMatrix
    kKeyDeviceName      = FOURCC('dnam'),       ///< UInt32
    kKeyESDS            = FOURCC('esds'),       ///< sp<Buffer>
    kKeyavcC            = FOURCC('avcC'),       ///< sp<Buffer>
    kKeyhvcC            = FOURCC('hvcC'),       ///< sp<Buffer>
    kKeyCodecSpecData   = FOURCC('#csd'),       ///< sp<Buffer>
    kKeyMetaData        = FOURCC('meta'),       ///< sp<Message>
    kKeyEncoderDelay    = FOURCC('edly'),       ///< Int32
    kKeyEncoderPadding  = FOURCC('epad'),       ///< Int32
    
    // Microsoft codec manager data
    kKeyMicrosoftVCM    = FOURCC('MVCM'),       ///< sp<Buffer>, Microsoft VCM, exists in matroska, @see BITMAPINFOHEADER
    kKeyMicorsoftACM    = FOURCC('MACM'),       ///< sp<Buffer>, Microsoft ACM, exists in matroska, @see WAVEFORMATEX
    
    kKeyMax             = MEDIA_ENUM_MAX
};
typedef UInt32 eKeyType;

#pragma mark Meta Data Keys
// meta data keys, default value type is string
enum {
    kKeyTitle           = FOURCC('0tit'),   ///< titles
    kKeyAlbum           = FOURCC(' alb'),
    kKeyComposer        = FOURCC(' wrt'),
    kKeyGenre           = FOURCC(' gen'),
    kKeyEncoder         = FOURCC(' enc'),
    kKeyArtist          = FOURCC(' art'),
    kKeyAlbumArtist     = FOURCC('aart'),   // what's this
    kKeyPerformer       = FOURCC('perf'),
    kKeyAuthor          = FOURCC('auth'),
    kKeyDate            = FOURCC('date'),
    kKeyYear            = FOURCC('year'),
    kKeyCompilation     = FOURCC('cpil'),
    kKeyComment         = FOURCC('comm'),
    kKeyTrackNum        = FOURCC('trkn'),
    kKeyDiskNum         = FOURCC('disk'),
    kKeyLanguage        = FOURCC('lang'),
    kKeyLocation        = FOURCC('loci'),
    kKeyCopyright       = FOURCC('cprt'),
    kKeyLicense         = FOURCC('lics'),
    kKeyBPM             = FOURCC(' BPM'),
    kKeyCustom          = FOURCC('0000'),   ///< user defined texts
    kKeyAlbumArt        = FOURCC('alma'),   ///< sp<Buffer>

    // special meta data
    kKeyiTunSMPB        = FOURCC('smpb'),   // String, m4a only
    
    kMetaKeyMax         = MEDIA_ENUM_MAX
};

#pragma mark Values
// key - kKeyMode
enum {
    kModeTypeNormal     = 0,                ///< use hardware accel if available
    kModeTypeSoftware,                      ///< no hardware accel.
    kModeTypePreview,
    kModeTypeDefault    = kModeTypeNormal
};
typedef UInt32 eModeType;

// key - kKeyMode
enum {
    kModeBlock      = FOURCC('!blk'),
    kModeNonBlock   = FOURCC('~blk'),
};
typedef UInt32 eBlockModeType;

enum {
    kDeviceOpenGL       = FOURCC('opgl'),
    kDeviceOpenAL       = FOURCC('opal'),
    kDeviceVideoToolbox = FOURCC('vtlx'),
    kDeviceAudioToolbox = FOURCC('atlx'),
    kDeviceLibavformat  = FOURCC('lavf'),
    kDeviceLibavcodec   = FOURCC('lavc'),
};
typedef UInt32 eDeviceName;

__END_DECLS

#ifdef __cplusplus
__BEGIN_NAMESPACE_MPX

class API_EXPORT MediaDevice : public SharedObject {
    public:
        /**
         * create a media device with given formats and options
         * @return return reference to media device on success, or Nil on failure
         */
        static sp<MediaDevice>  create(const sp<Message>&, const sp<Message>&);
        /**
         * get formats of this media device
         * @return return message reference of this media device
         * @note device should not keep reference to this message, as client
         *       may modify its content
         */
        virtual sp<Message>     formats() const             = 0;
        /**
         * configure this media device
         * @param a message reference contains options and parameters
         * @return return kMediaNoError on success
         *         return kMediaErrorNotSupported if option is not supported
         *         return MediaError code on other cases
         */
        virtual MediaError      configure(const sp<Message>&)   = 0;
        /**
         * push a MediaFrame into this media device
         * @param a MediaFrame reference will be pushed in
         * @return return kMediaNoError on success
         *         return kMediaErrorResourceBusy if device is busy, pull and push again.
         *         return kMediaErrorInvalidOperation if push is not available
         *         return MediaError code when push after pull failed.
         * @note push a Nil MediaFrame to notify end of stream.
         * @note push maybe in block or non-block way, block by default
         */
        virtual MediaError      push(const sp<MediaFrame>&) = 0;
        /**
         * pull a MediaFrame from this media device
         * @return return MediaFrame reference on success or Nil
         * @note some devices has delays on output and will return Nil on first few pull.
         * @note pull return Nil on failure too, but next push will return MediaError code.
         */
        virtual sp<MediaFrame>  pull()                      = 0;
        /**
         * reset this media device
         * @return return kMediaNoError on success, otherwise MediaError code
         */
        virtual MediaError      reset()                     = 0;

    protected:
        MediaDevice() : SharedObject() { }
        virtual ~MediaDevice() { }

        DISALLOW_EVILS(MediaDevice);
};

__END_NAMESPACE_MPX
#endif

#endif // _MEDIA_MODULES_DEVICE_H
